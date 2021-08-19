#pragma once

#include <igl/opengl/glfw/Viewer.h>
#include <igl/readObj.h>

#include <string>

class Render
{
public:

	Render(float res, bool orthographic, bool high_res=false, float cam_pos=2.2);
	~Render();

	void rotate_model(std::string model, std::string output);
	void rotate_shapehd_model();
	void set_model(Eigen::MatrixXd V_in, Eigen::MatrixXi F_in);
	bool input_model(std::string model, double scale);
	void render_normal(std::string model, std::string output, float side=1, bool random=true);//side: 1 - front, -1 - back
	void render_depth(std::string model, std::string output, float side=1, bool random=true, bool mode=1);
	void render_silhouette(std::string model, std::string output, float side=1, bool random=true, bool mode=1);
	void render_sym(std::string out_depth, std::string out_normal, std::string in_depth, std::string in_normal);

	void set_corner_color(const Eigen::MatrixXd & C);
	void set_view(float x_angle, float y_angle);
	void set_camera(float x, float y, float z);
	void view_generation(float x_range, float y_range, float up_down=0);//y-axis can do both sides, x-axis only down
	void translation_generation();
	void set_transformation(Eigen::MatrixXd rot_x, Eigen::MatrixXd rot_y, Eigen::RowVector3d t);
	void set_translation(double x, double y, double z);
	void compute_angle_from_param(float in_y, float in_x, float &y_rot, float &x_rot);
	void visualize(std::string fin, std::string fout, int side);
	double transform_to_occ(Eigen::MatrixXd ref_V, std::string fout);
	void rotate_back();
	void reflect_model(std::string out);
	//int check_boundary();
	Eigen::MatrixXd rotate_Y_90(Eigen::MatrixXd V);
	Eigen::MatrixXd rotate_Z_90(Eigen::MatrixXd V);
	Eigen::MatrixXd rotate_X_i90(Eigen::MatrixXd V);

	Eigen::MatrixXd get_rotation_x();
	Eigen::MatrixXd get_rotation_y();
	Eigen::RowVector3d get_translation();
	Eigen::Matrix3d get_rotation_back();

	float rand_angle_x, rand_angle_y, rand_tran_x, rand_tran_y;

	Eigen::MatrixXd get_vertices();

private:

	igl::opengl::glfw::Viewer viewer;

	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> R;
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> G;
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> B;
	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> A;

	Eigen::MatrixXd V, NV;
	Eigen::MatrixXi F;
	Eigen::MatrixXd N_corners, N_vertices, N_faces;
	Eigen::MatrixXd C_corners, C_vertices, C_faces;

	Eigen::Matrix3d rotate_mat_x, rotate_mat_y;
	Eigen::RowVector3d tran;

	Eigen::RowVector3d symmetry_normal, init_sym;

	int resolution;
	float custom_window_ratio;
};