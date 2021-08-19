#include <iostream>
//#include <igl/readOFF.h>
//#include <igl/viewer/Viewer.h>
//#include <igl/per_vertex_normals.h>
//#include <igl/per_face_normals.h>
//#include <igl/per_corner_normals.h>
//#include <iostream>
//#include "tutorial_shared_path.h"
//#include <igl/cotmatrix.h>
//#include <igl/png/writePNG.h>
//#include <igl/png/readPNG.h>
//#include <igl/readPLY.h>
//
//#include "Interpolation.h"
//#include "Refine.h"
//#include <vector>
//#include <Windows.h>
//#include <filesystem>
//#include <iostream>
//#include <fstream>
//
//namespace fs = std::experimental::filesystem;
//
//Eigen::MatrixXd V;
//Eigen::MatrixXi F;
//
//Eigen::MatrixXd N_vertices;
//Eigen::MatrixXd N_faces;
//Eigen::MatrixXd N_corners;
//Eigen::MatrixXd Color;
//Eigen::MatrixXd m1;
//
//// This function is called every time a keyboard button is pressed
//bool key_down(igl::viewer::Viewer& viewer, unsigned char key, int modifier)
//{
//	if (key == '1')
//	{
//		// Allocate temporary buffers
//		Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> R(1280, 800);
//		Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> G(1280, 800);
//		Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> B(1280, 800);
//		Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> A(1280, 800);
//
//		// Draw the scene in the buffers
//		viewer.core.draw_buffer(viewer.data, viewer.opengl, false, R, G, B, A);
//
//		// Save it to a PNG
//		igl::png::writePNG(R, G, B, A, "out.png");
//	}
//}
//
//void generate_normal_map(char* f_in, char*f_out, int i)
//{
//
//	//std::string path_prefix = "data/shapenet/chair/test/model/";
//	//char fname[256];
//
//	// Allocate temporary buffers
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> R(256, 256);
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> G(256, 256);
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> B(256, 256);
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> A(256, 256);
//
//	Eigen::MatrixXd rotateY_80(3, 3), rotateY_45(3, 3), rotateY_55(3, 3), rotateX_25(3, 3), rotateX_15(3, 3), rotate_back(3, 3), rotate_back2(3, 3);
//	Eigen::MatrixXd rotateY_30(3, 3), rotateX_inv15(3, 3), rotateX_inv30(3, 3), rotateX_30(3, 3), rotateX_45(3, 3);
//	rotateY_80 <<
//		0.173648, 0, 0.9848,
//		0, 1, 0,
//		-0.9848, 0, 0.173648;
//
//	rotateY_45 <<
//		0.7, 0, 0.7,
//		0, 1, 0,
//		-0.7, 0, 0.7;
//
//	rotateY_55 <<
//		0.5736, 0, 0.819,
//		0, 1, 0,
//		-0.819, 0, 0.5736;
//
//	rotateY_30 <<
//		0.866, 0, 0.5,
//		0, 1, 0,
//		-0.5, 0, 0.866;
//
//	rotateX_15 <<
//		1, 0, 0,
//		0, 0.966, 0.25882,
//		0, -0.25882, 0.966;
//
//	rotateX_25 <<
//		1, 0, 0,
//		0, 0.91, 0.42262,
//		0, -0.42262, 0.91;
//
//	rotateX_30 <<
//		1, 0, 0,
//		0, 0.866, 0.5,
//		0, -0.5, 0.866;
//
//	rotateX_45 <<
//		1, 0, 0,
//		0, 0.7, 0.7,
//		0, -0.7, 0.7;
//
//	rotateX_inv15 <<
//		1, 0, 0,
//		0, 0.966, -0.25882,
//		0, 0.25882, 0.966;
//
//	rotateX_inv30 <<
//		1, 0, 0,
//		0, 0.866, -0.5,
//		0, 0.5, 0.866;
//
//	rotate_back <<
//		-1, 0, 0,
//		0, 1, 0,
//		0, 0, -1;
//
//	rotate_back2 <<
//		-1, 0, 0,
//		0, 1, 0,
//		0, 0, 1;
//
//	std::vector<Eigen::MatrixXd> rotate_Y, rotate_X;
//
//	rotate_Y.push_back(rotateY_45);
//	rotate_Y.push_back(rotateY_55);
//	rotate_X.push_back(rotateX_15);
//	rotate_X.push_back(rotateX_30);
//	rotate_X.push_back(rotateX_45);
//	rotate_X.push_back(rotateX_inv15);
//	rotate_X.push_back(rotateX_inv30);
//
//	//for (int i = 1; i < 1318; i++)
//	//{e
//	int index_x = i % 5;
//	int index_y = i % 12;
//	//index_x = 0; index_y = 0;
//
//	// Load a mesh in OFF format
//	//sprintf(fname, "%d.obj", i);
//	igl::readOBJ(f_in, V, F);
//
//	// Plot the mesh
//	igl::viewer::Viewer viewer;
//
//	viewer.core.show_lines = false;
//	for (int i = 0; i < index_y; i++)
//	{
//		V = V * rotateY_30;
//	}
//
//	V = V * rotateY_80;
//	V = V * rotateY_80;
//	V = V * rotate_X[index_x];
//
//	igl::per_corner_normals(V, F, 20, N_corners);
//
//	viewer.data.set_mesh(V, F);
//
//	m1 = Eigen::MatrixXd::Ones(N_corners.rows(), N_corners.cols());
//	Color = (N_corners + m1) * 0.5;
//	viewer.data.set_colors(Color);
//	viewer.launch_init();
//
//	viewer.draw();
//
//	glfwSwapBuffers(viewer.window);
//
//	// Draw the scene in the buffers
//	viewer.core.draw_buffer(viewer.data, viewer.opengl, false, R, G, B, A);
//
//	// Save it to a PNG
//	igl::png::writePNG(R, G, B, A, f_out);
//
//	viewer.launch_shut();
//
//	/*******************
//	- Back Normal Map -
//	*********************/
//	/*for back map generation only*/
//	//V = V*rotate_back;
//	//V = V*rotate_back2;
//
//	//viewer.data.set_mesh(V, F);
//
//	////viewer.launch();
//	//viewer.launch_init();
//
//	//viewer.draw();
//
//	//glfwSwapBuffers(viewer.window);
//
//	//// Draw the scene in the buffers
//	//viewer.core.draw_buffer(viewer.data, viewer.opengl, false, R, G, B, A);
//
//	//// Save it to a PNG
//	//sprintf(fname, "data/shapenet/chair/val/front_normal/%d.png", i);
//	//igl::png::writePNG(R, G, B, A, fname);
//
//	//viewer.launch_shut();
//
//
//
//}
//
//void make_dataset() {
//
//	char fname[256];
//
//	for (int i = 0; i < 800; i++)
//	{
//		int num = 19000 + i;
//		sprintf(fname, "data/frontmap_edge_original/%d.png", num);
//		Mat A = imread(fname);
//
//		sprintf(fname, "data/backmap_edge_original/%d.png", num);
//		Mat B = imread(fname);
//
//		Mat A_crop = A(Rect(240, 0, 800, 800));
//		Mat B_crop = B(Rect(240, 0, 800, 800));
//
//		Mat A_resize, B_resize;
//
//		resize(A_crop, A_resize, Size(256, 256), INTER_CUBIC);
//		resize(B_crop, B_resize, Size(256, 256), INTER_CUBIC);
//
//		sprintf(fname, "data/frontmap_edge_original/train/%d.png", num);
//		imwrite(fname, A_resize);
//
//		sprintf(fname, "data/backmap_geometry_original/train/%d.png", num);
//		imwrite(fname, B_resize);
//
//	}
//
//	for (int i = 800; i < 900; i++)
//	{
//		int num = 19000 + i;
//		sprintf(fname, "data/frontmap_edge_original/%d.png", num);
//		Mat A = imread(fname);
//
//		sprintf(fname, "data/backmap_geometry_original/%d.png", num);
//		Mat B = imread(fname);
//
//		Mat A_crop = A(Rect(240, 0, 800, 800));
//		Mat B_crop = B(Rect(240, 0, 800, 800));
//
//		Mat A_resize, B_resize;
//
//		resize(A_crop, A_resize, Size(256, 256), INTER_CUBIC);
//		resize(B_crop, B_resize, Size(256, 256), INTER_CUBIC);
//
//		sprintf(fname, "data/frontmap_edge_original/val/%d.png", num);
//		imwrite(fname, A_resize);
//
//		sprintf(fname, "data/backmap_geometry_original/val/%d.png", num);
//		imwrite(fname, B_resize);
//
//	}
//
//	for (int i = 900; i < 1000; i++)
//	{
//		int num = 19000 + i;
//		sprintf(fname, "data/frontmap_edge_original/%d.png", num);
//		Mat A = imread(fname);
//
//		sprintf(fname, "data/backmap_geometry_original/%d.png", num);
//		Mat B = imread(fname);
//
//		Mat A_crop = A(Rect(240, 0, 800, 800));
//		Mat B_crop = B(Rect(240, 0, 800, 800));
//
//		Mat A_resize, B_resize;
//
//		resize(A_crop, A_resize, Size(256, 256), INTER_CUBIC);
//		resize(B_crop, B_resize, Size(256, 256), INTER_CUBIC);
//
//
//		sprintf(fname, "data/frontmap_edge_original/test/%d.png", num);
//		imwrite(fname, A_resize);
//
//		sprintf(fname, "data/backmap_geometry_original/test/%d.png", num);
//		imwrite(fname, B_resize);
//
//	}
//
//}
//
//void generate_rotate_model(char *f_in, char *f_rot, int i) {
//
//	Eigen::MatrixXd rotateY_80(3, 3), rotateY_45(3, 3), rotateY_55(3, 3), rotateX_25(3, 3), rotateX_15(3, 3), rotate_back(3, 3), rotate_back2(3, 3);
//	Eigen::MatrixXd rotateY_30(3, 3), rotateX_inv15(3, 3), rotateX_inv30(3, 3), rotateX_30(3, 3), rotateX_45(3, 3);
//
//	rotateY_80 <<
//		0.173648, 0, 0.9848,
//		0, 1, 0,
//		-0.9848, 0, 0.173648;
//
//	rotateY_45 <<
//		0.7, 0, 0.7,
//		0, 1, 0,
//		-0.7, 0, 0.7;
//
//	rotateY_55 <<
//		0.5736, 0, 0.819,
//		0, 1, 0,
//		-0.819, 0, 0.5736;
//
//	rotateY_30 <<
//		0.866, 0, 0.5,
//		0, 1, 0,
//		-0.5, 0, 0.866;
//
//	rotateX_15 <<
//		1, 0, 0,
//		0, 0.966, 0.25882,
//		0, -0.25882, 0.966;
//
//	rotateX_25 <<
//		1, 0, 0,
//		0, 0.91, 0.42262,
//		0, -0.42262, 0.91;
//
//	rotateX_30 <<
//		1, 0, 0,
//		0, 0.866, 0.5,
//		0, -0.5, 0.866;
//
//	rotateX_45 <<
//		1, 0, 0,
//		0, 0.7, 0.7,
//		0, -0.7, 0.7;
//
//	rotateX_inv15 <<
//		1, 0, 0,
//		0, 0.966, -0.25882,
//		0, 0.25882, 0.966;
//
//	rotateX_inv30 <<
//		1, 0, 0,
//		0, 0.866, -0.5,
//		0, 0.5, 0.866;
//
//	rotate_back <<
//		-1, 0, 0,
//		0, 1, 0,
//		0, 0, -1;
//
//	rotate_back2 <<
//		-1, 0, 0,
//		0, 1, 0,
//		0, 0, 1;
//
//	std::vector<Eigen::MatrixXd> rotate_Y, rotate_X;
//
//	rotate_Y.push_back(rotateY_45);
//	rotate_Y.push_back(rotateY_55);
//	rotate_X.push_back(rotateX_15);
//	rotate_X.push_back(rotateX_30);
//	rotate_X.push_back(rotateX_45);
//	rotate_X.push_back(rotateX_inv15);
//	rotate_X.push_back(rotateX_inv30);
//
//	int index_x = i % 5;
//	int index_y = i % 12;
//
//	// Load a mesh in OFF format
//	igl::readOBJ(f_in, V, F);
//
//	for (int j = 0; j < index_y; j++)
//	{
//		V = V * rotateY_30;
//	}
//
//	V = V * rotateY_80;
//	V = V * rotateY_80;
//	V = V * rotate_X[index_x];
//
//	igl::writeOBJ(f_rot, V, F);
//
//
//}
//
//void generate_all_map(char* f_in, char*f_out, int i, char* set) {
//
//	char fname[256];
//
//	// Allocate temporary buffers
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> R(256, 256);
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> G(256, 256);
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> B(256, 256);
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> A(256, 256);
//
//	Eigen::MatrixXd rotateY_80(3, 3), rotateY_45(3, 3), rotateY_55(3, 3), rotateX_25(3, 3), rotateX_15(3, 3), rotate_back(3, 3), rotate_back2(3, 3);
//	Eigen::MatrixXd rotateY_30(3, 3), rotateX_inv15(3, 3), rotateX_inv30(3, 3), rotateX_30(3, 3), rotateX_45(3, 3);
//
//	rotateY_80 <<
//		0.173648, 0, 0.9848,
//		0, 1, 0,
//		-0.9848, 0, 0.173648;
//
//	rotateY_45 <<
//		0.7, 0, 0.7,
//		0, 1, 0,
//		-0.7, 0, 0.7;
//
//	rotateY_55 <<
//		0.5736, 0, 0.819,
//		0, 1, 0,
//		-0.819, 0, 0.5736;
//
//	rotateY_30 <<
//		0.866, 0, 0.5,
//		0, 1, 0,
//		-0.5, 0, 0.866;
//
//	rotateX_15 <<
//		1, 0, 0,
//		0, 0.966, 0.25882,
//		0, -0.25882, 0.966;
//
//	rotateX_25 <<
//		1, 0, 0,
//		0, 0.91, 0.42262,
//		0, -0.42262, 0.91;
//
//	rotateX_30 <<
//		1, 0, 0,
//		0, 0.866, 0.5,
//		0, -0.5, 0.866;
//
//	rotateX_45 <<
//		1, 0, 0,
//		0, 0.7, 0.7,
//		0, -0.7, 0.7;
//
//	rotateX_inv15 <<
//		1, 0, 0,
//		0, 0.966, -0.25882,
//		0, 0.25882, 0.966;
//
//	rotateX_inv30 <<
//		1, 0, 0,
//		0, 0.866, -0.5,
//		0, 0.5, 0.866;
//
//	rotate_back <<
//		-1, 0, 0,
//		0, 1, 0,
//		0, 0, -1;
//
//	rotate_back2 <<
//		-1, 0, 0,
//		0, 1, 0,
//		0, 0, 1;
//
//	std::vector<Eigen::MatrixXd> rotate_Y, rotate_X;
//
//	rotate_Y.push_back(rotateY_45);
//	rotate_Y.push_back(rotateY_55);
//	rotate_X.push_back(rotateX_15);
//	rotate_X.push_back(rotateX_30);
//	rotate_X.push_back(rotateX_45);
//	rotate_X.push_back(rotateX_inv15);
//	rotate_X.push_back(rotateX_inv30);
//
//	int index_x = i % 5;
//	int index_y = i % 12;
//
//	// Load a mesh in OFF format
//	igl::readOBJ(f_in, V, F);
//
//	for (int j = 0; j < index_y; j++)
//	{
//		V = V * rotateY_30;
//	}
//
//	V = V * rotateY_80;
//	V = V * rotateY_80;
//	V = V * rotate_X[index_x];
//
//	// Plot the mesh
//	igl::viewer::Viewer viewer;
//
//	viewer.core.show_lines = false;
//
//	igl::per_corner_normals(V, F, 20, N_corners);
//	m1 = Eigen::MatrixXd::Ones(N_corners.rows(), N_corners.cols());
//
//	viewer.data.set_mesh(V, F);
//
//	/***********front normal************/
//
//	Color = (N_corners + m1) * 0.5;
//	viewer.data.set_colors(Color);
//	viewer.launch_init();
//
//	viewer.draw();
//
//	glfwSwapBuffers(viewer.window);
//
//	// Draw the scene in the buffers
//	viewer.core.draw_buffer(viewer.data, viewer.opengl, false, R, G, B, A);
//
//	sprintf(fname, "%s/front_normal/%s/%d.png", f_out, set, i);
//
//	// Save it to a PNG
//	igl::png::writePNG(R, G, B, A, fname);
//
//	viewer.launch_shut();
//
//	/***********front depth************/
//
//	Color = V;
//
//	float z;
//
//	for (int y = 0; y < V.rows(); y++)
//	{
//		z = Color.row(y)[2];
//		z = (z + 0.7) / 1.2; // the scale of depth
//
//		Color.row(y)[0] = Color.row(y)[1] = Color.row(y)[2] = z;
//	}
//
//	viewer.data.set_colors(Color);
//	//viewer.launch();
//	viewer.launch_init();
//
//	viewer.draw();
//	glfwSwapBuffers(viewer.window);
//
//	// Draw the scene in the buffers
//	viewer.core.draw_buffer(viewer.data, viewer.opengl, false, R, G, B, A);
//
//	sprintf(fname, "%s/front_depth/%s/%d.png", f_out, set, i);
//
//	// Save it to a PNG
//	igl::png::writePNG(R, G, B, A, fname);
//
//	viewer.launch_shut();
//
//
//	///*******************
//	//- Back Map -
//	//*********************/
//	V = V * rotate_back;
//	V = V * rotate_back2;
//
//	viewer.data.set_mesh(V, F);
//
//	//////back depth
//
//	viewer.data.set_colors(Color);
//
//	//viewer.launch();
//	viewer.launch_init();
//
//	viewer.draw();
//
//	glfwSwapBuffers(viewer.window);
//
//	// Draw the scene in the buffers
//	viewer.core.draw_buffer(viewer.data, viewer.opengl, false, R, G, B, A);
//
//	// Save it to a PNG
//	sprintf(fname, "%s/back_depth/%s/%d.png", f_out, set, i);
//
//	igl::png::writePNG(R, G, B, A, fname);
//
//	viewer.launch_shut();
//
//	//back normal
//	//Color = (N_corners + m1) * 0.5;
//	//viewer.data.set_colors(Color);
//
//	//viewer.launch_init();
//
//	//viewer.draw();
//
//	//glfwSwapBuffers(viewer.window);
//
//	// Draw the scene in the buffers
//	//viewer.core.draw_buffer(viewer.data, viewer.opengl, false, R, G, B, A);
//
//	//// Save it to a PNG
//	//sprintf(fname, "%s/back_normal/%s/%d.png", f_out, set, i);
//
//	//igl::png::writePNG(R, G, B, A, fname);
//
//	//viewer.launch_shut();
//
//}
//
//void generate_geometry_map(char* f_in, char*f_out, int i)
//{
//
//	//std::string path_prefix = "data/shapenet/chair/test/model/";
//	//char fname[256];
//
//	// Allocate temporary buffers
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> R(256, 256);
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> G(256, 256);
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> B(256, 256);
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> A(256, 256);
//
//	Eigen::MatrixXd rotateY_80(3, 3), rotateY_45(3, 3), rotateY_55(3, 3), rotateX_25(3, 3), rotateX_15(3, 3), rotate_back(3, 3), rotate_back2(3, 3);
//	Eigen::MatrixXd rotateY_30(3, 3), rotateX_inv15(3, 3), rotateX_inv30(3, 3), rotateX_30(3, 3), rotateX_45(3, 3);
//
//	rotateY_80 <<
//		0.173648, 0, 0.9848,
//		0, 1, 0,
//		-0.9848, 0, 0.173648;
//
//	rotateY_45 <<
//		0.7, 0, 0.7,
//		0, 1, 0,
//		-0.7, 0, 0.7;
//
//	rotateY_55 <<
//		0.5736, 0, 0.819,
//		0, 1, 0,
//		-0.819, 0, 0.5736;
//
//	rotateY_30 <<
//		0.866, 0, 0.5,
//		0, 1, 0,
//		-0.5, 0, 0.866;
//
//	rotateX_15 <<
//		1, 0, 0,
//		0, 0.966, 0.25882,
//		0, -0.25882, 0.966;
//
//	rotateX_25 <<
//		1, 0, 0,
//		0, 0.91, 0.42262,
//		0, -0.42262, 0.91;
//
//	rotateX_30 <<
//		1, 0, 0,
//		0, 0.866, 0.5,
//		0, -0.5, 0.866;
//
//	rotateX_45 <<
//		1, 0, 0,
//		0, 0.7, 0.7,
//		0, -0.7, 0.7;
//
//	rotateX_inv15 <<
//		1, 0, 0,
//		0, 0.966, -0.25882,
//		0, 0.25882, 0.966;
//
//	rotateX_inv30 <<
//		1, 0, 0,
//		0, 0.866, -0.5,
//		0, 0.5, 0.866;
//
//	rotate_back <<
//		-1, 0, 0,
//		0, 1, 0,
//		0, 0, -1;
//
//	rotate_back2 <<
//		-1, 0, 0,
//		0, 1, 0,
//		0, 0, 1;
//
//	std::vector<Eigen::MatrixXd> rotate_Y, rotate_X;
//
//	rotate_Y.push_back(rotateY_45);
//	rotate_Y.push_back(rotateY_55);
//	rotate_X.push_back(rotateX_15);
//	rotate_X.push_back(rotateX_30);
//	rotate_X.push_back(rotateX_45);
//	rotate_X.push_back(rotateX_inv15);
//	rotate_X.push_back(rotateX_inv30);
//
//	//for (int i = 1; i < 1318; i++)
//	//{e
//	int index_x = i % 5;
//	int index_y = i % 12;
//	//index_x = 0; index_y = 0;
//
//	// Load a mesh in OFF format
//	//sprintf(fname, "%d.obj", i);
//	igl::readOBJ(f_in, V, F);
//
//	for (int j = 0; j < index_y; j++)
//	{
//		V = V * rotateY_30;
//	}
//
//	V = V * rotateY_80;
//	V = V * rotateY_80;
//	V = V * rotate_X[index_x];
//	////for (int i = 1; i < 1318; i++)
//	////{
//	//int index_x = i % 2;
//	//int index_y = (i % 4) / 2;
//
//	// Plot the mesh
//	igl::viewer::Viewer viewer;
//
//	viewer.core.show_lines = false;
//
//	igl::per_corner_normals(V, F, 20, N_corners);
//
//	viewer.data.set_mesh(V, F);
//
//	m1 = Eigen::MatrixXd::Ones(V.rows(), V.cols());
//	Color = V;
//
//	float z;
//	for (int y = 0; y < V.rows(); y++)
//	{
//		z = Color.row(y)[2];
//		z = (z + 0.7) / 1.2; // the scale of depth
//
//		Color.row(y)[0] = Color.row(y)[1] = Color.row(y)[2] = z;
//	}
//
//	viewer.data.set_colors(Color);
//	//viewer.launch();
//	viewer.launch_init();
//
//	viewer.draw();
//	glfwSwapBuffers(viewer.window);
//
//	// Draw the scene in the buffers
//	viewer.core.draw_buffer(viewer.data, viewer.opengl, false, R, G, B, A);
//
//	// Save it to a PNG
//	//sprintf(fname, "data/shapenet/chair/test/front_depth_rescale/%d.png", i);
//	igl::png::writePNG(R, G, B, A, f_out);
//
//	viewer.launch_shut();
//
//	/*******************
//	- Back Normal Map -
//	*********************/
//	///*for back map generation only*/
//	//V = V*rotate_back;
//	//V = V*rotate_back2;
//
//	//viewer.data.set_mesh(V, F);
//
//	//viewer.data.set_colors(Color);
//
//	////viewer.launch();
//	//viewer.launch_init();
//
//	//viewer.draw();
//
//	//glfwSwapBuffers(viewer.window);
//
//	//// Draw the scene in the buffers
//	//viewer.core.draw_buffer(viewer.data, viewer.opengl, false, R, G, B, A);
//
//	//// Save it to a PNG
//	//sprintf(fname, "data/shapenet/chair/test/back_depth_rescale/%d.png", i);
//	//igl::png::writePNG(R, G, B, A, fname);
//
//	//viewer.launch_shut();
//	//}
//
//
//}
//
//void compute_poisson(Mat normal, Mat depth) {
//
//	int col = normal.cols;
//	int row = normal.rows;
//
//	float nx, ny, nz, dx, dy, dz;
//	float sum = 0, tmp = 0;
//	for (int y = 0; y < row; y++)
//	{
//		for (int x = 0; x < col; x++)
//		{
//			sum = 0;
//
//			if (normal.at<Vec3b>(y, x)[0] > 0 && normal.at<Vec3b>(y, x)[1] > 0 && normal.at<Vec3b>(y, x)[2] > 0)
//			{
//
//				if (y >= 1 && x >= 1 && (y + 1) < normal.rows && (x + 1) < normal.cols
//					&&
//					normal.at<Vec3b>(y - 1, x)[0] > 0 && normal.at<Vec3b>(y - 1, x)[1] > 0 && normal.at<Vec3b>(y - 1, x)[2] > 0
//					&&
//					normal.at<Vec3b>(y + 1, x)[0] > 0 && normal.at<Vec3b>(y + 1, x)[1] > 0 && normal.at<Vec3b>(y + 1, x)[2] > 0
//					&&
//					normal.at<Vec3b>(y, x - 1)[0] > 0 && normal.at<Vec3b>(y, x - 1)[1] > 0 && normal.at<Vec3b>(y, x - 1)[2] > 0
//					&&
//					normal.at<Vec3b>(y, x + 1)[0] > 0 && normal.at<Vec3b>(y, x + 1)[1] > 0 && normal.at<Vec3b>(y, x + 1)[2] > 0
//					)
//				{
//					//nx = (float)(normal.at<Vec3b>(y, x)[2])*2.0 / 255.0 - 1;
//					//ny = (float)(normal.at<Vec3b>(y, x)[1])*2.0 / 255.0 - 1;
//					//nz = (float)(normal.at<Vec3b>(y, x)[0])*2.0 / 255.0 - 1;
//
//					//std::cout << "the length of this normal is :" << sqrt(nx*nx + ny*ny + nz*nz) << " .\n";
//
//					//left 
//					if (x - 1>0)
//					{
//						nx = ((float)(normal.at<Vec3b>(y, x)[2])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x - 1)[2])*2.0 / 255.0 - 1) / 2;
//						ny = ((float)(normal.at<Vec3b>(y, x)[1])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x - 1)[1])*2.0 / 255.0 - 1) / 2;
//						nz = ((float)(normal.at<Vec3b>(y, x)[0])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x - 1)[0])*2.0 / 255.0 - 1) / 2;
//
//
//
//						dx = 1;
//						dy = 0;
//						dz = ((float)(depth.at<Vec3b>(y, x)[0])*6.0 / 255.0 - (float)(depth.at<Vec3b>(y, x - 1)[0])*6.0 / 255.0);
//
//						tmp = (nx*dx + ny * dy + nz * dz) / sqrt((nx*nx + ny * ny + nz * nz)*(dx*dx + dy * dy + dz * dz));
//
//						tmp *= tmp;
//
//						std::cout << "left product is :" << tmp << " .\n";
//						std::cout << "and the nx is " << nx << " , ny is " << ny << " , nz is " << nz << " , dz is " << dz << " .\n";
//
//					}
//
//					//right
//					if (x + 1 < col)
//					{
//						nx = ((float)(normal.at<Vec3b>(y, x)[2])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x + 1)[2])*2.0 / 255.0 - 1) / 2;
//						ny = ((float)(normal.at<Vec3b>(y, x)[1])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x + 1)[1])*2.0 / 255.0 - 1) / 2;
//						nz = ((float)(normal.at<Vec3b>(y, x)[0])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x + 1)[0])*2.0 / 255.0 - 1) / 2;
//
//						dx = -1;
//						dy = 0;
//						dz = ((float)(depth.at<Vec3b>(y, x)[0])*6.0 / 255.0 - (float)(depth.at<Vec3b>(y, x + 1)[0])*6.0 / 255.0);
//
//						tmp = (nx*dx + ny * dy + nz * dz) / sqrt((nx*nx + ny * ny + nz * nz)*(dx*dx + dy * dy + dz * dz));
//
//						tmp *= tmp;
//
//						std::cout << "right product is :" << tmp << " .\n";
//						std::cout << "and the nx is " << nx << " , ny is " << ny << " , nz is " << nz << " , dz is " << dz << " .\n";
//
//
//					}
//
//					//top 
//					if (y - 1>0)
//					{
//						nx = ((float)(normal.at<Vec3b>(y, x)[2])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y - 1, x)[2])*2.0 / 255.0 - 1) / 2;
//						ny = ((float)(normal.at<Vec3b>(y, x)[1])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y - 1, x)[1])*2.0 / 255.0 - 1) / 2;
//						nz = ((float)(normal.at<Vec3b>(y, x)[0])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y - 1, x)[0])*2.0 / 255.0 - 1) / 2;
//
//						dx = 0;
//						dy = 1;
//						dz = ((float)(depth.at<Vec3b>(y, x)[0])*6.0 / 255.0 - (float)(depth.at<Vec3b>(y - 1, x)[0])*6.0 / 255.0);
//
//						tmp = (nx*dx + ny * dy + nz * dz) / sqrt((nx*nx + ny * ny + nz * nz)*(dx*dx + dy * dy + dz * dz));
//
//						tmp *= tmp;
//
//						std::cout << "top product is :" << tmp << " .\n";
//						std::cout << "and the nx is " << nx << " , ny is " << ny << " , nz is " << nz << " , dz is " << dz << " .\n";
//
//
//					}
//
//					//down 
//					if (y + 1<row)
//					{
//						nx = ((float)(normal.at<Vec3b>(y, x)[2])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y + 1, x)[2])*2.0 / 255.0 - 1) / 2;
//						ny = ((float)(normal.at<Vec3b>(y, x)[1])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y + 1, x)[1])*2.0 / 255.0 - 1) / 2;
//						nz = ((float)(normal.at<Vec3b>(y, x)[0])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y + 1, x)[0])*2.0 / 255.0 - 1) / 2;
//
//						dx = 0;
//						dy = -1;
//						dz = ((float)(depth.at<Vec3b>(y, x)[0])*6.0 / 255.0 - (float)(depth.at<Vec3b>(y + 1, x)[0])*6.0 / 255.0);
//
//						tmp = (nx*dx + ny * dy + nz * dz) / sqrt((nx*nx + ny * ny + nz * nz)*(dx*dx + dy * dy + dz * dz));
//
//						tmp *= tmp;
//
//						std::cout << "down product is :" << tmp << " .\n";
//						std::cout << "and the nx is " << nx << " , ny is " << ny << " , nz is " << nz << " , dz is " << dz << " .\n";
//
//
//					}
//
//
//				}
//			}
//
//			////left 
//			//if (x-1>0)
//			//{
//			//	nx = ((float)(normal.at<Vec3b>(y, x)[2])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x - 1)[2])*2.0 / 255.0 - 1) / 2;
//			//	ny = ((float)(normal.at<Vec3b>(y, x)[1])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x - 1)[1])*2.0 / 255.0 - 1) / 2;
//			//	nz = ((float)(normal.at<Vec3b>(y, x)[0])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x - 1)[0])*2.0 / 255.0 - 1) / 2;
//
//			//	
//
//			//	dx = 1;
//			//	dy = 0;
//			//	dz = ((float)(depth.at<Vec3b>(y, x)[0])*6.0 / 255.0 - (float)(depth.at<Vec3b>(y, x - 1)[0])*6.0 / 255.0);
//
//			//	tmp = (nx*dx + ny*dy + nz*dz) / sqrt((nx*nx + ny*ny + nz*nz)*(dx*dx + dy*dy + dz*dz));
//
//			//	tmp *= tmp;
//
//			//	std::cout << "left product is :" << tmp << " .\n";
//			//	std::cout << "and the nx is " << nx << " , ny is " << ny << " , nz is " << nz << " , dz is " << dz << " .\n";
//
//			//}
//
//			////right
//			//if (x + 1 < col)
//			//{
//			//	nx = ((float)(normal.at<Vec3b>(y, x)[2])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x + 1)[2])*2.0 / 255.0 - 1) / 2;
//			//	ny = ((float)(normal.at<Vec3b>(y, x)[1])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x + 1)[1])*2.0 / 255.0 - 1) / 2;
//			//	nz = ((float)(normal.at<Vec3b>(y, x)[0])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y, x + 1)[0])*2.0 / 255.0 - 1) / 2;
//
//			//	dx = -1;
//			//	dy = 0;
//			//	dz = ((float)(depth.at<Vec3b>(y, x)[0])*6.0 / 255.0 - (float)(depth.at<Vec3b>(y, x + 1)[0])*6.0 / 255.0);
//
//			//	tmp = (nx*dx + ny*dy + nz*dz) / sqrt((nx*nx + ny*ny + nz*nz)*(dx*dx + dy*dy + dz*dz));
//
//			//	tmp *= tmp;
//
//			//	std::cout << "right product is :" << tmp << " .\n";
//			//	std::cout << "and the nx is " << nx << " , ny is " << ny << " , nz is " << nz << " , dz is " << dz << " .\n";
//
//
//			//}
//
//			////top 
//			//if (y - 1>0)
//			//{
//			//	nx = ((float)(normal.at<Vec3b>(y, x)[2])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y - 1, x)[2])*2.0 / 255.0 - 1) / 2;
//			//	ny = ((float)(normal.at<Vec3b>(y, x)[1])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y - 1, x)[1])*2.0 / 255.0 - 1) / 2;
//			//	nz = ((float)(normal.at<Vec3b>(y, x)[0])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y - 1, x)[0])*2.0 / 255.0 - 1) / 2;
//
//			//	dx = 0;
//			//	dy = 1;
//			//	dz = ((float)(depth.at<Vec3b>(y, x)[0])*6.0 / 255.0 - (float)(depth.at<Vec3b>(y - 1, x)[0])*6.0 / 255.0);
//
//			//	tmp = (nx*dx + ny*dy + nz*dz) / sqrt((nx*nx + ny*ny + nz*nz)*(dx*dx + dy*dy + dz*dz));
//
//			//	tmp *= tmp;
//
//			//	std::cout << "top product is :" << tmp << " .\n";
//			//	std::cout << "and the nx is " << nx << " , ny is " << ny << " , nz is " << nz << " , dz is " << dz << " .\n";
//
//
//			//}
//
//			////down 
//			//if (y + 1>0)
//			//{
//			//	nx = ((float)(normal.at<Vec3b>(y, x)[2])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y + 1, x)[2])*2.0 / 255.0 - 1) / 2;
//			//	ny = ((float)(normal.at<Vec3b>(y, x)[1])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y + 1, x)[1])*2.0 / 255.0 - 1) / 2;
//			//	nz = ((float)(normal.at<Vec3b>(y, x)[0])*2.0 / 255.0 - 1 + (float)(normal.at<Vec3b>(y + 1, x)[0])*2.0 / 255.0 - 1) / 2;
//
//			//	dx = 0;
//			//	dy = -1;
//			//	dz = ((float)(depth.at<Vec3b>(y, x)[0])*6.0 / 255.0 - (float)(depth.at<Vec3b>(y + 1, x)[0])*6.0 / 255.0);
//
//			//	tmp = (nx*dx + ny*dy + nz*dz) / sqrt((nx*nx + ny*ny + nz*nz)*(dx*dx + dy*dy + dz*dz));
//
//			//	tmp *= tmp;
//
//			//	std::cout << "down product is :" << tmp << " .\n";
//			//	std::cout << "and the nx is " << nx << " , ny is " << ny << " , nz is " << nz << " , dz is " << dz << " .\n";
//
//
//			//}
//			//
//
//
//
//		}
//	}
//
//}
//
////convert normal map map without black(-1,-1,-1)
//void convert_to_common(Mat& input) {
//
//	char fname[256];
//
//	for (int i = 19000; i < 20000; i++)
//	{
//
//		// Load a mesh in OFF format
//		sprintf(fname, "data/frontmap_diff_refine/%d.png", i);
//
//		for (int y = 0; y < input.rows; y++)
//		{
//			for (int x = 0; x < input.cols; x++)
//			{
//				if (
//					input.at<Vec3b>(y, x)[0] == 0 &&
//					input.at<Vec3b>(y, x)[1] == 0 &&
//					input.at<Vec3b>(y, x)[2] == 0
//
//					)
//				{
//					input.at<Vec3b>(y, x)[0] = input.at<Vec3b>(y, x)[1] = input.at<Vec3b>(y, x)[2] = 128;
//
//				}
//			}
//		}
//	}
//}
//
//void visualization_libigl(std::string filename) {
//
//	char fname[256];
//	igl::readOFF(filename, V, F);
//
//	// Plot the mesh
//	igl::viewer::Viewer viewer;
//	viewer.data.set_mesh(V, F);
//	viewer.core.show_lines = false;
//	viewer.launch();
//	//Color = V;
//
//	//float z;
//	//for (int y = 0; y < V.rows(); y++)
//	//{
//	//	z = Color.row(y)[2];
//	//	z = (z + 2) / 4;
//
//	//	Color.row(y)[0] = Color.row(y)[1] = Color.row(y)[2] = z;
//	//}
//
//	//viewer.data.set_colors(Color);
//
//	//viewer.launch_init();
//
//	//viewer.draw();
//
//	//glfwSwapBuffers(viewer.window);
//
//	//// Allocate temporary buffers
//	//Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> R(1280, 800);
//	//Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> G(1280, 800);
//	//Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> B(1280, 800);
//	//Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> A(1280, 800);
//
//	//// Draw the scene in the buffers
//	//viewer.core.draw_buffer(viewer.data, viewer.opengl, false, R, G, B, A);
//
//	//// Save it to a PNG
//	//sprintf(fname, "data/test_mesh/test.png");
//	//igl::png::writePNG(R, G, B, A, fname);
//
//	//viewer.launch_shut();
//
//}
//
//void edgeDetection(std::string f_depth, std::string f_normal, std::string f_edge) {
//
//	cv::Mat depth = cv::imread(f_depth);
//	cv::Mat normal = cv::imread(f_normal);
//	cv::Mat poisson_x = depth.clone(), poisson_y = depth.clone();
//
//	int col = depth.cols;
//	int row = depth.rows;
//
//	int scale = 1;
//	int delta = 0;
//	int ddepth = CV_16S;
//
//
//	cv::Mat grad_x, grad_y, grad_normal, grad_depth, grad;
//	cv::Mat abs_grad_x, abs_grad_y;
//	cv::Mat normal_gray, depth_gray;
//
//	//cv::GaussianBlur(normal, normal, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
//	cv::cvtColor(normal, normal_gray, CV_RGB2GRAY);
//
//	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
//	cv::Sobel(normal_gray, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
//	cv::convertScaleAbs(grad_x, abs_grad_x);
//
//	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
//	cv::Sobel(normal_gray, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
//	cv::convertScaleAbs(grad_y, abs_grad_y);
//
//	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad_normal);
//
//	//cv::GaussianBlur(depth, depth, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
//	cv::cvtColor(depth, depth_gray, CV_RGB2GRAY);
//
//	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
//	cv::Sobel(depth_gray, grad_x, ddepth, 1, 0, -1, scale, delta, cv::BORDER_DEFAULT);
//	cv::convertScaleAbs(grad_x, abs_grad_x);
//
//	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
//	cv::Sobel(depth_gray, grad_y, ddepth, 0, 1, -1, scale, delta, cv::BORDER_DEFAULT);
//	cv::convertScaleAbs(grad_y, abs_grad_y);
//
//	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad_depth);
//
//	for (int y = 0; y < grad_normal.rows; y++)
//	{
//		for (int x = 0; x < grad_normal.cols; x++)
//		{
//			if (grad_normal.at<uchar>(y, x) > 70)
//			{
//				grad_normal.at<uchar>(y, x) = 255;
//			}
//			else
//			{
//				grad_normal.at<uchar>(y, x) = 0;
//			}
//		}
//	}
//
//	for (int y = 0; y < grad_depth.rows; y++)
//	{
//		for (int x = 0; x < grad_depth.cols; x++)
//		{
//			if (grad_depth.at<uchar>(y, x) > 80)
//			{
//				grad_depth.at<uchar>(y, x) = 255;
//			}
//			else
//			{
//				grad_depth.at<uchar>(y, x) = 0;
//			}
//		}
//	}
//
//	addWeighted(grad_depth, 0.5, grad_normal, 0.5, 0, grad);
//
//	for (int y = 0; y < grad.rows; y++)
//	{
//		for (int x = 0; x < grad.cols; x++)
//		{
//			if (grad.at<uchar>(y, x) > 10)
//			{
//				grad.at<uchar>(y, x) = 255;
//			}
//		}
//	}
//
//	cv::imwrite(f_edge, grad);
//
//}
//
//void generateEdgeMap() {
//
//	char f_edge[256], f_depth[256], f_normal[256];
//
//	for (int i = 19000; i < 20000; i++)
//	{
//		sprintf(f_normal, "data/frontmap_normal_256/%d.png", i);
//		sprintf(f_depth, "data/frontmap_geometry_256/%d.png", i);
//		sprintf(f_edge, "data/frontmap_edge_256/%d.png", i);
//
//		edgeDetection(f_depth, f_normal, f_edge);
//
//		sprintf(f_normal, "data/backmap_normal_256/%d.png", i);
//		sprintf(f_depth, "data/backmap_geometry_256/%d.png", i);
//		sprintf(f_edge, "data/backmap_edge_256/%d.png", i);
//
//		edgeDetection(f_depth, f_normal, f_edge);
//
//	}
//}
//
//void render_result(std::string file) {
//	igl::readPLY(file, V, F);
//	igl::viewer::Viewer viewer;
//	viewer.data.set_mesh(V, F);
//	viewer.launch();
//
//}
//
//int main_test(int argc, char *argv[]) {
//
//	//char test[256];
//	//sprintf(test, "data/test.ply");
//
//	//render_result(test);
//
//	//visualization_libigl(argv[1]);
//
//	//generateEdgeMap();
//
//	//char fname[256];
//
//	//for (int i = 0; i < 10; i++)
//	//{
//	//	int num = 19000 + i;
//	//	sprintf(fname, "data/frontmap_sameview/%d.png", num);
//	//	Mat input = imread(fname);
//	//	Mat res = interpolation(input);
//
//	//	sprintf(fname, "result/interpolation/%d_back_interpolation.png", num);
//	//	imwrite(fname, res);
//	//}
//
//
//	//imwrite("data/test/inter.png", res);
//
//	char f_in[256], f_out[256], set[256], f_rot[256];
//
//	for (int i = 23; i < 27; i++)
//	{
//		sprintf(set, "test");
//
//		sprintf(f_in, "data/shapenet/chair/%s/model/%d.obj", set, 23);
//
//		sprintf(f_out, "data/shapenet/chair/23_multiview");
//
//		//sprintf(f_rot, "data/shapenet/chair/%s/rotate_model/23_multi/%d.obj", set, i);
//
//		generate_all_map(f_in, f_out, i, set);
//
//		//generate_rotate_model(f_in, f_rot, i);
//
//		//generate_normal_map(f_in, f_out, i);
//
//		//sprintf(f_out, "data/shapenet/chair/train/front_depth/%d.png", i);
//
//		//generate_geometry_map(f_in, f_out, i);
//
//	}
//
//	//generate_normal_map(f_in, f_out);
//
//	////make_dataset();
//
//	//generate_geometry_map(f_in, f_out);
//
//
//	//Mat depth = imread("data/frontmap_diff_refine_geometry/19000.png");
//	//convert_to_common();
//
//	//compute_poisson(normal, depth);
//
//	//create list of chair
//	//int num = 0;
//	//std::ofstream fout;
//	//fout.open("shapenet/chair.txt");
//	//std::string path = "shapenet/chair";
//	//for (auto & p : fs::directory_iterator(path)) {
//	//	fout << p << std::endl;
//	//	num++;
//	//}
//	//fout.close();
//	//std::cout << "There are " << num << " models in the list.\n";
//
//
//	//generate maps
//	//std::cout << "begin to refine \n";
//	//sharpen("data/refine/airplane.ply", "data/refine/19901_refine.ply");
//	//
//	return 0;
//}
