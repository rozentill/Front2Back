#include "render.h"
#include "param.h"

#include <igl/per_corner_normals.h>
#include <igl/per_vertex_normals.h>
#include <igl/per_face_normals.h>
#include <igl/png/writePNG.h>
#include <igl/embree/reorient_facets_raycast.h>
#include <igl/orientable_patches.h>
//#include <igl/boundary_loop.h>

#include <math.h>
#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>

/*******IMPORTANT NOTE********/
/******************************
Need to modify draw_buffer in 
ViewerCore.cpp to get rid of 
multisample for antialiasing.
******************************/

Render::Render(float res, bool orthographic, bool high_res, float cam_pos)
{
	resolution = res;
	if (orthographic)
	{
		viewer.core.orthographic = true;

	}
	else
	{
		viewer.core.orthographic = false;

	}
	
	viewer.data().show_lines = false;
	viewer.core.lighting_factor = 0.0;
	viewer.core.viewport(2) = resolution;
	viewer.core.viewport(3) = resolution;
	R.resize(resolution, resolution);
	G.resize(resolution, resolution);
	B.resize(resolution, resolution);
	A.resize(resolution, resolution);

	if (high_res)
	{
		viewer.core.camera_eye << 0., 0., 2.2;
		viewer.core.camera_view_angle = 30.;

		custom_window_ratio = 256.f/( 1.5 * 2 * tan(PI * 30 / 360.));
	}
	else
	{
		//viewer.core.camera_dnear = 1.0;
		viewer.core.camera_eye << 0., 0., cam_pos;
		viewer.core.camera_view_angle = 49.1343426412;
		custom_window_ratio = (137.f / (1.4 * 2 * tan(PI * viewer.core.camera_view_angle / 360.)));
	}

	viewer.launch_init();
	viewer.draw();
	glfwSwapBuffers(viewer.window);

	//initialize transformation matrices
	tran = Eigen::RowVector3d(0, 0, 0);
	rotate_mat_x = Eigen::MatrixXd::Identity(3, 3);
	rotate_mat_y = Eigen::MatrixXd::Identity(3, 3);

	rand_angle_x = 0.;
	rand_angle_y = 0.;
	rand_tran_x = 0.;
	rand_tran_y = 0.;

	//symmetry 
	init_sym = Eigen::RowVector3d(0, 0, 1);
	symmetry_normal = init_sym;
}

Render::~Render()
{
	viewer.launch_shut();
}

void Render::rotate_model(std::string model, std::string output) {
	viewer.data().clear();


	NV = V;
	NV *= rotate_mat_y;
	NV *= rotate_mat_x;

	NV = NV.rowwise() + tran;

	viewer.data().set_mesh(NV, F);

	igl::writeOBJ(output, NV, F);
}

double Render::transform_to_occ(Eigen::MatrixXd ref_V, std::string fout) {

	Eigen::Vector3d max_range = ref_V.colwise().maxCoeff();
	Eigen::Vector3d min_range = ref_V.colwise().minCoeff();
	
	double dist_0 = max_range(0) - min_range(0);
	double dist_1 = max_range(1) - min_range(1);
	double dist_2 = max_range(2) - min_range(2);
	double max_dist = dist_0 > dist_1 ? dist_0 : dist_1;
	max_dist = max_dist > dist_2 ? max_dist : dist_2;
	std::cout << "max dist is " << max_dist << std::endl;
	Eigen::RowVector3d center = (max_range + min_range) / 2;

	V = V.rowwise() - center;

	V /= max_dist;

	igl::writeOBJ(fout, V, F);

	return max_dist;
}
void Render::rotate_back() {
	V *= rotate_mat_x.inverse();
	V *= rotate_mat_y.inverse();
}
void Render::rotate_shapehd_model() {
	viewer.core.camera_eye(2) =  1.;
	V = rotate_Z_90(V);
	V = rotate_X_i90(V);
}

void Render::set_model(Eigen::MatrixXd V_in, Eigen::MatrixXi F_in) {
	V = V_in;
	F = F_in;
}

bool Render::input_model(std::string model, double scale) {
	
	bool success = igl::readOBJ(model, V, F);
	V *= scale;
	symmetry_normal = init_sym;
	return success;

}

void Render::render_normal(std::string model, std::string output, float side, bool random) {
	
	viewer.data().clear();

	Eigen::MatrixXd NV;


	NV = V;
	if (random)
	{
		NV *= rotate_mat_y;
		NV *= rotate_mat_x;
	}
	

	igl::per_face_normals(NV, F, N_faces);

	//check if the normal is pointing back
	for (int i = 0; i < N_faces.rows(); i++)
	{
		if (N_faces(i, 2)*side < 0)
		{
			N_faces(i, 2) *= -1;
			N_faces(i, 1) *= -1;
			N_faces(i, 0) *= -1;
		}
	}

	C_faces = (N_faces + Eigen::MatrixXd::Ones(N_faces.rows(), N_faces.cols())) * 0.5;

	if (side == -1)//if back, change vertex but preserve color
	{
		Eigen::AngleAxisd rollAngle(0.0, Eigen::Vector3d::UnitZ());
		Eigen::AngleAxisd yawAngle(atan(1) * 4, Eigen::Vector3d::UnitY());
		Eigen::AngleAxisd pitchAngle(0.0, Eigen::Vector3d::UnitX());

		Eigen::Quaternion<double> q = rollAngle * yawAngle * pitchAngle;

		Eigen::Matrix3d rotationMatrix = q.matrix();
		Eigen::Matrix3d flipMatrix;
		flipMatrix << 
			-1, 0, 0,
			0, 1, 0,
			0, 0, 1;
		NV = NV * rotationMatrix;
		NV = NV * flipMatrix;

	}
	if (random)
	{
		NV = NV.rowwise() + tran;
	}
	viewer.data().set_mesh(NV, F);
	viewer.data().set_colors(C_faces);
	viewer.data().F_material_ambient = 0.0*viewer.data().F_material_diffuse;
	viewer.data().F_material_specular = 0.0*viewer.data().F_material_diffuse;
	viewer.data().F_material_ambient.col(3) = viewer.data().F_material_diffuse.col(3);
	viewer.data().F_material_specular.col(3) = viewer.data().F_material_diffuse.col(3);

	// Draw the scene in the buffers
	viewer.core.draw_buffer(viewer.data(), false, R, G, B, A);

	// Save it to a PNG
	igl::png::writePNG(R, G, B, A, output);
}

void Render::render_depth(std::string model, std::string output, float side, bool random, bool mode) {

	viewer.data().clear();


	Eigen::MatrixXd NV;
	NV = V;
	if (random)
	{
		NV *= rotate_mat_y;
		NV *= rotate_mat_x;
	}
	
	C_vertices = NV;
	
	float z;

	if (mode)// exactly value
	{
		for (int i = 0; i < NV.rows(); i++)
		{
			z = C_vertices.row(i)[2];
			z = (z + depth_offset) / (0.5 + depth_offset);

			C_vertices.row(i)[0] = C_vertices.row(i)[1] = C_vertices.row(i)[2] = z;
		}
	}
	else // min max normalization
	{
		double z_min = C_vertices.col(2).minCoeff();
		double z_max = C_vertices.col(2).maxCoeff();
		for (int i = 0; i < NV.rows(); i++)
		{
			z = C_vertices.row(i)[2];
			z = (z - z_min) / (z_max - z_min);

			C_vertices.row(i)[0] = C_vertices.row(i)[1] = C_vertices.row(i)[2] = z;
		}
	}


	if (side == -1) //if back, change vertex but preserve color
	{

		Eigen::AngleAxisd rollAngle(0.0, Eigen::Vector3d::UnitZ());
		Eigen::AngleAxisd yawAngle(atan(1) * 4, Eigen::Vector3d::UnitY());
		Eigen::AngleAxisd pitchAngle(0.0, Eigen::Vector3d::UnitX());

		Eigen::Quaternion<double> q = rollAngle * yawAngle * pitchAngle;

		Eigen::Matrix3d rotationMatrix = q.matrix();
		Eigen::Matrix3d flipMatrix;
		flipMatrix << -1, 0, 0,
						0, 1, 0,
						0, 0, 1;
		NV = NV * rotationMatrix;
		NV = NV * flipMatrix;
	}

	if (random)
	{
		NV = NV.rowwise() + tran;
	}
	viewer.data().set_mesh(NV, F);

	viewer.data().set_colors(C_vertices);
	viewer.data().V_material_ambient = 0.0 * viewer.data().V_material_diffuse;
	viewer.data().V_material_specular = 0.0 * viewer.data().V_material_diffuse;
	viewer.data().V_material_ambient.col(3) = viewer.data().V_material_diffuse.col(3);
	viewer.data().V_material_specular.col(3) = viewer.data().V_material_diffuse.col(3);

	// Draw the scene in the buffers
	viewer.core.draw_buffer(viewer.data(), false, R, G, B, A);

	// Save it to a PNG
	igl::png::writePNG(R, G, B, A, output);
}

void Render::render_silhouette(std::string model, std::string output, float side, bool random, bool mode) {

	viewer.data().clear();


	Eigen::MatrixXd NV;
	NV = V;
	if (random)
	{
		NV *= rotate_mat_y;
		NV *= rotate_mat_x;
	}

	if (side == -1) //if back, change vertex but preserve color
	{

		Eigen::AngleAxisd rollAngle(0.0, Eigen::Vector3d::UnitZ());
		Eigen::AngleAxisd yawAngle(atan(1) * 4, Eigen::Vector3d::UnitY());
		Eigen::AngleAxisd pitchAngle(0.0, Eigen::Vector3d::UnitX());

		Eigen::Quaternion<double> q = rollAngle * yawAngle * pitchAngle;

		Eigen::Matrix3d rotationMatrix = q.matrix();
		Eigen::Matrix3d flipMatrix;
		flipMatrix << -1, 0, 0,
			0, 1, 0,
			0, 0, 1;
		NV = NV * rotationMatrix;
		NV = NV * flipMatrix;

	}

	if (random)
	{
		NV = NV.rowwise() + tran;
	}
	viewer.data().set_mesh(NV, F);

	viewer.data().set_colors(Eigen::RowVector3d(1,1,1));
	viewer.data().V_material_ambient = 0.0 * viewer.data().V_material_diffuse;
	viewer.data().V_material_specular = 0.0 * viewer.data().V_material_diffuse;
	viewer.data().V_material_ambient.col(3) = viewer.data().V_material_diffuse.col(3);
	viewer.data().V_material_specular.col(3) = viewer.data().V_material_diffuse.col(3);

	// Draw the scene in the buffers
	viewer.core.draw_buffer(viewer.data(), false, R, G, B, A);

	// Save it to a PNG
	igl::png::writePNG(R, G, B, A, output);
}

void Render::render_sym(std::string out_depth, std::string out_normal, std::string in_depth, std::string in_normal) {

	//input reference front maps
	cv::Mat front_depth = cv::imread(in_depth);
	cv::Mat front_normal = cv::imread(in_normal);

	//first rotate vertices and symmetry plane
	symmetry_normal *= rotate_mat_y;
	symmetry_normal *= rotate_mat_x;
	symmetry_normal.normalize();
	float a = symmetry_normal(0);
	float b = symmetry_normal(1);
	float c = symmetry_normal(2);
	float d = 0;

	float ox, oy, oz, k, px, py, pz, sx, sy, sz, vx, vy, vz, nx, ny, nz;
	float tmp;

	cv::Mat s_depth(resolution, resolution, CV_8UC3, cv::Scalar::all(0));
	cv::Mat s_normal(resolution, resolution, CV_8UC3, cv::Scalar::all(0));
	
	std::ofstream fout;
	fout.open("../test_sym.obj");

	//initial with mask
	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
		{
			if (front_depth.at<cv::Vec3b>(i, j)[0] > 0)
			{
				s_depth.at<cv::Vec3b>(i, j)[0] = s_depth.at<cv::Vec3b>(i, j)[1] = s_depth.at<cv::Vec3b>(i, j)[2] = 255;
				s_normal.at<cv::Vec3b>(i, j)[0] = s_normal.at<cv::Vec3b>(i, j)[1] = s_normal.at<cv::Vec3b>(i, j)[2] = 255;
			}
		}
	}

	for (int y = 0; y < front_depth.rows; y++)
	{
		for (int x = 0; x < front_depth.cols; x++)
		{
			if (front_depth.at<cv::Vec3b>(y, x)[0] == 0)
			{
				continue;
			}
			ox = ((float)x+0.5 - WINDOW_SIZE / 2.) / WINDOW_RATIO;
			oy = (WINDOW_SIZE / 2. - (float)y-0.5) / WINDOW_RATIO;
			oz = (float)(front_depth.at<cv::Vec3b>(y, x)[0]) * 1.2 / 255.f - 0.7; 


			k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

			px = a * k + ox;
			py = b * k + oy;
			pz = c * k + oz;

			sx = 2 * px - ox;
			sy = 2 * py - oy;
			sz = 2 * pz - oz;

			fout << "v " << ox << " " << oy << " " << oz << std::endl;
			fout << "v " << sx << " " << sy << " " << sz << std::endl;

			vx = (unsigned char)(sx * WINDOW_RATIO + WINDOW_SIZE / 2.);
			vy = (unsigned char)(WINDOW_SIZE / 2. - sy * WINDOW_RATIO);
			vz = (unsigned char)((sz + 0.7) * 255.f / 1.2);

			if (vx < 0 || vx>255 || vy < 0 || vy>255 || vz < 0 || vz>255)
			{
				std::cout << "Border error!" << std::endl;
			}

			if (s_depth.at<cv::Vec3b>(vy, vx)[0] > 0 && s_depth.at<cv::Vec3b>(vy, vx)[0] < vz)
			{
				continue;
			}
			s_depth.at<cv::Vec3b>(vy, vx)[0] = s_depth.at<cv::Vec3b>(vy, vx)[1] = s_depth.at<cv::Vec3b>(vy, vx)[2] = vz;

			// render normal 
			nx = front_normal.at<cv::Vec3b>(y, x)[2];
			ny = front_normal.at<cv::Vec3b>(y, x)[1];
			nz = front_normal.at<cv::Vec3b>(y, x)[0];

			nx = nx * 2.f / 255.f - 1;
			ny = ny * 2.f / 255.f - 1;
			nz = nz * 2.f / 255.f - 1;

			ox = ox + nx;
			oy = oy + ny;
			oz = oz + nz;

			k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

			px = a * k + ox;
			py = b * k + oy;
			pz = c * k + oz;

			nx = 2 * px - ox - sx;
			ny = 2 * py - oy - sy;
			nz = 2 * pz - oz - sz;

			tmp = sqrt(nx*nx + ny * ny + nz * nz);
			nx /= tmp;
			ny /= tmp;
			nz /= tmp;

			s_normal.at<cv::Vec3b>(vy, vx)[0] = (uchar)((nz + 1)*255.f / 2.f);
			s_normal.at<cv::Vec3b>(vy, vx)[1] = (uchar)((ny + 1)*255.f / 2.f);
			s_normal.at<cv::Vec3b>(vy, vx)[2] = (uchar)((nx + 1)*255.f / 2.f);
		}
	}
	fout.close();

	cv::imwrite(out_depth, s_depth);
	cv::imwrite(out_normal, s_normal);
}

void Render::set_corner_color(const Eigen::MatrixXd & C) {

	using namespace Eigen;
	// Ambient color should be darker color
	const auto ambient = [](const MatrixXd & C)->MatrixXd
	{
		MatrixXd T = 0.1*C;
		T.col(3) = C.col(3);
		return T;
	};
	// Specular color should be a less saturated and darker color: dampened
	// highlights
	const auto specular = [](const MatrixXd & C)->MatrixXd
	{
		const double grey = 0.3;
		MatrixXd T = grey + 0.1*(C.array() - grey);
		T.col(3) = C.col(3);
		return T;
	};

	if (C.rows() == viewer.data().F.rows()*3)
	{
		viewer.data().set_face_based(true);
		viewer.data().F_material_diffuse.resize(C.rows(), 4);
		viewer.data().F_material_ambient.resize(C.rows(), 4);
		viewer.data().F_material_specular.resize(C.rows(), 4);

		for (unsigned i = 0; i<viewer.data().F_material_diffuse.rows(); ++i)
		{
			if (C.cols() == 3)
				viewer.data().F_material_diffuse.row(i) << C.row(i), 1;
			else if (C.cols() == 4)
				viewer.data().F_material_diffuse.row(i) << C.row(i);
		}
		viewer.data().F_material_ambient = ambient(viewer.data().F_material_diffuse);
		viewer.data().F_material_specular = specular(viewer.data().F_material_diffuse);
	}
	else
		std::cerr << "ERROR (set_corner_colors): Please provide a color per corner." << std::endl;
}

Eigen::MatrixXd Render::rotate_Y_90(Eigen::MatrixXd V) {

	Eigen::MatrixXd rotate_mat(3, 3);
	rotate_mat <<
		cos(atan(1) * 4 * 90 / 180), 0, sin(atan(1) * 4 * 90 / 180),
		0, 1, 0,
		-sin(atan(1) * 4 * 90 / 180), 0, cos(atan(1) * 4 * 90 / 180);

	return V * rotate_mat;
}

Eigen::MatrixXd Render::rotate_Z_90(Eigen::MatrixXd V) {

	Eigen::MatrixXd rotate_mat(3, 3);
	rotate_mat <<
		cos(atan(1) * 4 * 90 / 180), sin(atan(1) * 4 * 90 / 180), 0,
		-sin(atan(1) * 4 * 90 / 180), cos(atan(1) * 4 * 90 / 180), 0,
		0, 0, 1;

	return V * rotate_mat;
}


Eigen::MatrixXd Render::rotate_X_i90(Eigen::MatrixXd V) {

	Eigen::MatrixXd rotate_mat(3, 3);
	rotate_mat <<
		1, 0, 0,
		0, cos(atan(1) * 4 * (-90) / 180), sin(atan(1) * 4 * (-90) / 180),
		0, -sin(atan(1) * 4 * (-90) / 180), cos(atan(1) * 4 * (-90) / 180);

	return V * rotate_mat;
}

void Render::set_view(float x_angle, float y_angle) {
	
	rand_angle_x = x_angle;
	rand_angle_y = y_angle;

	rotate_mat_x <<
		1, 0, 0,
		0, cos(atan(1) * 4 * x_angle / 180), sin(atan(1) * 4 * x_angle / 180),
		0, -sin(atan(1) * 4 * x_angle / 180), cos(atan(1) * 4 * x_angle / 180);

	rotate_mat_y <<
		cos(atan(1) * 4 * y_angle / 180), 0, -sin(atan(1) * 4 * y_angle / 180),
		0, 1, 0,
		sin(atan(1) * 4 * y_angle / 180), 0, cos(atan(1) * 4 * y_angle / 180);

}

void Render::set_camera(float x, float y, float z) {
	viewer.core.camera_eye << x, y, z;
}

void Render::view_generation(float x_range, float y_range, float up_down) {
	
	rand_angle_x = float(rand()) / float(RAND_MAX);
	rand_angle_x *= x_range;	

	rand_angle_y = float(rand()) / float(RAND_MAX);
	rand_angle_y -= 0.5;
	rand_angle_y *= y_range;

	rotate_mat_x <<
		1, 0, 0,
		0, cos(atan(1) * 4 * rand_angle_x / 180), sin(atan(1) * 4 * rand_angle_x / 180),
		0, -sin(atan(1) * 4 * rand_angle_x / 180), cos(atan(1) * 4 * rand_angle_x / 180);

	rotate_mat_y <<
		cos(atan(1) * 4 * rand_angle_y / 180), 0, -sin(atan(1) * 4 * rand_angle_y / 180),
		0, 1, 0,
		sin(atan(1) * 4 * rand_angle_y / 180), 0, cos(atan(1) * 4 * rand_angle_y / 180);

}

void Render::translation_generation() {

	tran = Eigen::RowVector3d(0, 0, 0);

	double x_range = 0.4, y_range = 0.4;

	rand_tran_x = float(rand()) / float(RAND_MAX);
	rand_tran_x -= 0.5;
	rand_tran_x *= x_range;

	rand_tran_y = float(rand()) / float(RAND_MAX);
	rand_tran_y -= 0.5;
	rand_tran_y *= y_range;

	tran(0) = rand_tran_x;
	tran(1) = rand_tran_y;

}

void Render::set_transformation(Eigen::MatrixXd rot_x, Eigen::MatrixXd rot_y, Eigen::RowVector3d t) {
	rotate_mat_x = rot_x;
	rotate_mat_y = rot_y;
	tran = t;
}


void Render::visualize(std::string fin, std::string fout, int side) {

	viewer.data().clear();

	Eigen::MatrixXd NV;

	NV = V;
	Eigen::MatrixXd color;
	color.resize(NV.rows(), NV.cols());
	color.setConstant(1.0);
	viewer.data().set_mesh(NV, F);
	if (side == -1)//if back, change vertex but preserve color
	{
		Eigen::AngleAxisd rollAngle(0.0, Eigen::Vector3d::UnitZ());
		Eigen::AngleAxisd yawAngle(atan(1) * 4, Eigen::Vector3d::UnitY());
		Eigen::AngleAxisd pitchAngle(0.0, Eigen::Vector3d::UnitX());

		Eigen::Quaternion<double> q = rollAngle * yawAngle * pitchAngle;

		Eigen::Matrix3d rotationMatrix = q.matrix();
		Eigen::Matrix3d flipMatrix;
		flipMatrix << -1, 0, 0,
			0, 1, 0,
			0, 0, 1;
		NV = NV * rotationMatrix;
		NV = NV * flipMatrix;

		viewer.data().set_mesh(NV, F);
	}
	viewer.data().set_colors(color);


	viewer.data().F_material_ambient = 0.0*viewer.data().F_material_diffuse;
	viewer.data().F_material_specular = 0.0*viewer.data().F_material_diffuse;
	viewer.data().F_material_ambient.col(3) = viewer.data().F_material_diffuse.col(3);
	viewer.data().F_material_specular.col(3) = viewer.data().F_material_diffuse.col(3);

	// Draw the scene in the buffers
	viewer.core.draw_buffer(viewer.data(), false, R, G, B, A);

	// Save it to a PNG
	igl::png::writePNG(R, G, B, A, fout);
}


Eigen::MatrixXd Render::get_rotation_x() {
	return rotate_mat_x;
}
Eigen::MatrixXd Render::get_rotation_y() {
	return rotate_mat_y;
}
Eigen::RowVector3d Render::get_translation() {
	return tran;
}

Eigen::Matrix3d Render::get_rotation_back() {

	Eigen::AngleAxisd rollAngle(0.0, Eigen::Vector3d::UnitZ());
	Eigen::AngleAxisd yawAngle(atan(1) * 4, Eigen::Vector3d::UnitY());
	Eigen::AngleAxisd pitchAngle(0.0, Eigen::Vector3d::UnitX());

	Eigen::Quaternion<double> q = rollAngle * yawAngle * pitchAngle;

	Eigen::Matrix3d rotationMatrix = q.matrix();

	return rotationMatrix;
}

void Render::compute_angle_from_param(float in_y, float in_x, float &y_rot, float &x_rot) {
	//follow DISN's computation, dont know why

	float theta, phi;
	float cam_x, cam_y, cam_z;

	theta = in_y * PI / 180;
	phi = in_x * PI / 180;
	cam_z = sin(theta)*cos(phi);
	cam_x = cos(theta)*cos(phi);
	cam_y = sin(phi);
	Eigen::Vector3f proj_cam_y(cam_x, 0., cam_z);
	Eigen::Vector3f proj_cam_x(0, cam_y, cam_z);
	Eigen::Vector3f ori_cam(0, 0., 1.);
	Eigen::Vector3f normal(0., 1., 0.);

	proj_cam_y.normalize();
	y_rot = acos((proj_cam_y).dot(ori_cam));
	Eigen::Vector3f cross = ori_cam.cross(proj_cam_y);
	if (normal.dot(cross) < 0) { // Or > 0
		y_rot = -y_rot;
	}
	y_rot = y_rot * 180. / PI;

	proj_cam_x.normalize();
	x_rot = acos((proj_cam_x).dot(ori_cam));
	cross = ori_cam.cross(proj_cam_x);
	if (normal.dot(cross) < 0) { // Or > 0
		x_rot = -x_rot;
	}
	x_rot = -x_rot * 180. / PI;

	x_rot = in_x;
}

void Render::set_translation(double x, double y, double z) {
	tran(0) = x;
	tran(1) = y;
	tran(2) = z;
}

Eigen::MatrixXd Render::get_vertices() {
	return V;
}

void Render::reflect_model(std::string out) {

	symmetry_normal *= rotate_mat_y;
	symmetry_normal *= rotate_mat_x;
	symmetry_normal.normalize();
	float a = symmetry_normal(0);
	float b = symmetry_normal(1);
	float c = symmetry_normal(2);
	float d = 0;
	std::cout << a << " " << b << " " << c << " " << d << std::endl;
	float ox, oy, oz, k, px, py, pz, sx, sy, sz, vx, vy, vz, nx, ny, nz;
	float tmp;

	NV = V;
	for (int y = 0; y < V.rows(); y++)
	{
		ox = V(y, 0);
		oy = V(y, 1);
		oz = V(y, 2);

		k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

		px = a * k + ox;
		py = b * k + oy;
		pz = c * k + oz;

		sx = 2 * px - ox;
		sy = 2 * py - oy;
		sz = 2 * pz - oz;

		NV(y, 0) = sx;
		NV(y, 1) = sy;
		NV(y, 2) = sz;
	}


	igl::writeOBJ(out, NV, F);
}