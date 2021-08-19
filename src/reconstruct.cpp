#include "reconstruct.h"
#include "param.h"
#include "headers.h"
#include <sstream>
#include <fstream>
#include <iostream>

#include <opencv2\opencv.hpp>


float Reconstruct::cvtValue2Depth(unsigned char value) {
	return (float)value * 1.2 / 255.f - 0.7;
}

float Reconstruct::cvtValue2Width(float value) {//assume window size is 0.5
	return ((float)value + 0.5 - WINDOW_SIZE / 2.) /  WINDOW_RATIO;
}

float Reconstruct::cvtValue2Height(float value) {
	return (WINDOW_SIZE / 2. - (float)value - 0.5) / WINDOW_RATIO;
}

float Reconstruct::cvtValue2Normal(unsigned char value) {
	return ((float)value * 2 / 255.f - 1.);
}

unsigned char Reconstruct::cvtDepth2Value(float z) {
	return (uchar)((z + 0.7) * 255.f / 1.2);
}

unsigned char Reconstruct::cvtWidth2Value(float x) {
	return (int)(x * WINDOW_RATIO  + WINDOW_SIZE / 2.);
}

unsigned char Reconstruct::cvtHeight2Value(float y) {
	return (int)(WINDOW_SIZE / 2. - y * WINDOW_RATIO );
}

void Reconstruct::generate_point_cloud(std::string f_opc, std::string f_normal_front, std::string f_normal_back, std::string f_depth_front, std::string f_depth_back, bool test, std::string f_density, float thre){

	cv::Mat normal_front = cv::imread(f_normal_front);
	cv::Mat normal_back = cv::imread(f_normal_back);
	cv::Mat depth_front = cv::imread(f_depth_front);
	cv::Mat depth_back = cv::imread(f_depth_back);

	cv::Mat density_map(depth_front.rows, depth_front.cols, CV_8UC1, cv::Scalar::all(0));

	float a, b, c, d;
	float ox, oy, oz, k, px, py, pz, sx, sy, sz, vx, vy, vz, nx, ny, nz;

	Eigen::RowVector3d symmetry_normal(1, 0, 0);;

	Eigen::Matrix3d rotate_mat_y, rotate_mat_x;
	thre = 4.;
	
	float x_angle = 30.;
	float y_angle = 150.;

	rotate_mat_x  <<
		1, 0, 0,
		0, cos(atan(1) * 4 * x_angle / 180), sin(atan(1) * 4 * x_angle / 180),
		0, -sin(atan(1) * 4 * x_angle / 180), cos(atan(1) * 4 * x_angle / 180);

	rotate_mat_y <<
		cos(atan(1) * 4 * y_angle / 180), 0, -sin(atan(1) * 4 * y_angle / 180),
		0, 1, 0,
		sin(atan(1) * 4 * y_angle / 180), 0, cos(atan(1) * 4 * y_angle / 180);

	symmetry_normal *= rotate_mat_y;
	symmetry_normal *= rotate_mat_x;
	symmetry_normal.normalize();
	std::cout << symmetry_normal << std::endl;

	int col = depth_front.cols;
	int row = depth_front.rows;
	std::ofstream out(f_opc);
	float off_x_float, off_y_float, off_unit = 0.33;
	float nx_front, ny_front, nz_front, nx_back, ny_back, nz_back;
	int offset = 0;
	int v_ind = 1;
	int center_value, top_value, bottom_value, left_value, right_value, value_sum, value_num, min_value;
	//float thre = 5.;
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				normal_back.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_back.at<cv::Vec3b>(y, x)[0] > 0
				)
			{

				//denoise
						// check density
				value_sum = 0;
				value_num = 0;
				min_value = 300;

				center_value = depth_front.at<cv::Vec3b>(y, x)[0];
				top_value = depth_front.at<cv::Vec3b>(y - 1, x)[0];
				bottom_value = depth_front.at<cv::Vec3b>(y + 1, x)[0];
				left_value = depth_front.at<cv::Vec3b>(y, x - 1)[0];
				right_value = depth_front.at<cv::Vec3b>(y, x + 1)[0];

				if (top_value != 0)
				{
					value_sum += abs(center_value - top_value);
					value_num++;

					if (abs(center_value - top_value) < min_value)
					{
						min_value = abs(center_value - top_value);
					}
				}
				if (bottom_value != 0)
				{
					value_sum += abs(center_value - bottom_value);
					value_num++;

					if (abs(center_value - bottom_value) < min_value)
					{
						min_value = abs(center_value - bottom_value);
					}
				}
				if (left_value != 0)
				{
					value_sum += abs(center_value - left_value);
					value_num++;

					if (abs(center_value - left_value) < min_value)
					{
						min_value = abs(center_value - left_value);
					}
				}
				if (right_value != 0)
				{
					value_sum += abs(center_value - right_value);
					value_num++;

					if (abs(center_value - right_value) < min_value)
					{
						min_value = abs(center_value - right_value);
					}
				}
				if (value_num != 0)
				{


					//judge by min 
					if (min_value > thre)
					{
						continue;
					}
				}
				else
				{
					continue;
				}

				nx_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[2]);
				ny_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[1]);
				nz_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[0]);


				out <<
					"v " <<
					cvtValue2Width(x) << " " // x
					<< cvtValue2Height(y) << " "// y
					<< cvtValue2Depth(depth_front.at<cv::Vec3b>(y, x)[0]) << " "
					<< std::endl << "vn "  // z
					<< nx_front << " "// nx
					<< ny_front << " "// ny
					<< nz_front << " "// nz
					<< std::endl;

			}
		}
	}

	//back
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				normal_back.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_back.at<cv::Vec3b>(y, x)[0] > 0
				)
			{
				for (int off_x = -offset; off_x < offset + 1; off_x++)
				{
					off_x_float = off_x * off_unit;

					for (int off_y = -offset; off_y < offset + 1; off_y++)
					{

						off_y_float = off_y * off_unit;

						//denoise
						// check density
						value_sum = 0;
						value_num = 0;
						min_value = 300;

						center_value = depth_back.at<cv::Vec3b>(y, x)[0];
						top_value = depth_back.at<cv::Vec3b>(y - 1, x)[0];
						bottom_value = depth_back.at<cv::Vec3b>(y + 1, x)[0];
						left_value = depth_back.at<cv::Vec3b>(y, x - 1)[0];
						right_value = depth_back.at<cv::Vec3b>(y, x + 1)[0];

						if (top_value != 0)
						{
							value_sum += abs(center_value - top_value);
							value_num++;

							if (abs(center_value - top_value) < min_value)
							{
								min_value = abs(center_value - top_value);
							}
						}
						if (bottom_value != 0)
						{
							value_sum += abs(center_value - bottom_value);
							value_num++;

							if (abs(center_value - bottom_value) < min_value)
							{
								min_value = abs(center_value - bottom_value);
							}
						}
						if (left_value != 0)
						{
							value_sum += abs(center_value - left_value);
							value_num++;

							if (abs(center_value - left_value) < min_value)
							{
								min_value = abs(center_value - left_value);
							}
						}
						if (right_value != 0)
						{
							value_sum += abs(center_value - right_value);
							value_num++;

							if (abs(center_value - right_value) < min_value)
							{
								min_value = abs(center_value - right_value);
							}
						}
						if (value_num != 0)
						{
							//judge by min 
							if (min_value > thre)
							{
								continue;
							}
						}
						else
						{
							continue;
						}

						nx_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[2]);
						ny_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[1]);
						nz_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[0]);


						out <<
							"v " <<
							cvtValue2Width(x) << " " // x
							<< cvtValue2Height(y) << " "// y
							<< cvtValue2Depth(depth_back.at<cv::Vec3b>(y, x)[0]) << " "
							<< std::endl << "vn " // z
							<< nx_back << " "// nx
							<< ny_back << " "// ny
							<< nz_back << " "// nz
							<< std::endl;
					}
				}
			}
		}
	}
}

void Reconstruct::generate_poisson_input(std::string f_opc, std::string f_normal_front, std::string f_normal_back, std::string f_depth_front, std::string f_depth_back) {

	cv::Mat normal_front = cv::imread(f_normal_front);
	cv::Mat normal_back = cv::imread(f_normal_back);
	cv::Mat depth_front = cv::imread(f_depth_front);
	cv::Mat depth_back = cv::imread(f_depth_back);

	std::fstream file;
	file.open(f_normal_back);
	if (!file)
	{
		return;
	}
	else
	{
		file.close();
	}

	int col = depth_front.cols;
	int row = depth_front.rows;
	int center_value, top_value, bottom_value, left_value, right_value, value_sum, value_num, min_value;
	float thre = 4.;
	std::ofstream out(f_opc);
	float off_x_float, off_y_float, off_unit = 0.33;
	float nx_front, ny_front, nz_front, nx_back, ny_back, nz_back;
	int offset = 0;
	int v_ind = 1;
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				normal_back.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_back.at<cv::Vec3b>(y, x)[0] > 0
				)
			{
				for (int off_x = -offset; off_x < offset + 1; off_x++)
				{
					off_x_float = off_x * off_unit;

					for (int off_y = -offset; off_y < offset + 1; off_y++)
					{
						off_y_float = off_y * off_unit;

						// check density
						value_sum = 0;
						value_num = 0;
						min_value = 300;

						center_value = depth_front.at<cv::Vec3b>(y, x)[0];
						top_value = depth_front.at<cv::Vec3b>(y - 1, x)[0];
						bottom_value = depth_front.at<cv::Vec3b>(y + 1, x)[0];
						left_value = depth_front.at<cv::Vec3b>(y, x - 1)[0];
						right_value = depth_front.at<cv::Vec3b>(y, x + 1)[0];

						if (top_value != 0)
						{
							value_sum += abs(center_value - top_value);
							value_num++;

							if (abs(center_value - top_value) < min_value)
							{
								min_value = abs(center_value - top_value);
							}
						}
						if (bottom_value != 0)
						{
							value_sum += abs(center_value - bottom_value);
							value_num++;

							if (abs(center_value - bottom_value) < min_value)
							{
								min_value = abs(center_value - bottom_value);
							}
						}
						if (left_value != 0)
						{
							value_sum += abs(center_value - left_value);
							value_num++;

							if (abs(center_value - left_value) < min_value)
							{
								min_value = abs(center_value - left_value);
							}
						}
						if (right_value != 0)
						{
							value_sum += abs(center_value - right_value);
							value_num++;

							if (abs(center_value - right_value) < min_value)
							{
								min_value = abs(center_value - right_value);
							}
						}
						if (value_num != 0)
						{

							//judge by min 
							if (min_value > thre)
							{
								depth_front.at<cv::Vec3b>(y, x)[0] = depth_front.at<cv::Vec3b>(y, x)[1] = depth_front.at<cv::Vec3b>(y, x)[2] = 255;
								continue;
							}
						}
						else
						{
							depth_front.at<cv::Vec3b>(y, x)[0] = depth_front.at<cv::Vec3b>(y, x)[1] = depth_front.at<cv::Vec3b>(y, x)[2] = 255;
							continue;
						}


						nx_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[2]);
						ny_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[1]);
						nz_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[0]);

						nx_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[2]);
						ny_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[1]);
						nz_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[0]);

						//front
						out <<
							cvtValue2Width(x + off_x_float) << " " // x
							<< cvtValue2Height(y + off_y_float) << " "// y
							<< cvtValue2Depth(depth_front.at<cv::Vec3b>(y, x)[0]) << " "
							<< nx_front << " "// nx
							<< ny_front << " "// ny
							<< nz_front << " "// nz
							<< std::endl;


						//back test for front
						for (int iy = -1; iy < 2; iy++)
						{
							for (int ix = -1; ix < 2; ix++)
							{

								if (
									depth_back.at<cv::Vec3b>(y + iy, x + ix)[0] >= depth_front.at<cv::Vec3b>(y, x)[0]

									)
								{
									depth_back.at<cv::Vec3b>(y + iy, x + ix)[0] = depth_front.at<cv::Vec3b>(y, x)[0] - 2;
								}


							}
						}

					}
				}
			}
		}
	}


	//back
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				normal_back.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_back.at<cv::Vec3b>(y, x)[0] > 0
				)
			{
				for (int off_x = -offset; off_x < offset + 1; off_x++)
				{
					off_x_float = off_x * off_unit;

					for (int off_y = -offset; off_y < offset + 1; off_y++)
					{

						off_y_float = off_y * off_unit;

						//denoise
						// check density
						value_sum = 0;
						value_num = 0;
						min_value = 300;

						center_value = depth_back.at<cv::Vec3b>(y, x)[0];
						top_value = depth_back.at<cv::Vec3b>(y - 1, x)[0];
						bottom_value = depth_back.at<cv::Vec3b>(y + 1, x)[0];
						left_value = depth_back.at<cv::Vec3b>(y, x - 1)[0];
						right_value = depth_back.at<cv::Vec3b>(y, x + 1)[0];

						if (top_value != 0)
						{
							value_sum += abs(center_value - top_value);
							value_num++;

							if (abs(center_value - top_value) < min_value)
							{
								min_value = abs(center_value - top_value);
							}
						}
						if (bottom_value != 0)
						{
							value_sum += abs(center_value - bottom_value);
							value_num++;

							if (abs(center_value - bottom_value) < min_value)
							{
								min_value = abs(center_value - bottom_value);
							}
						}
						if (left_value != 0)
						{
							value_sum += abs(center_value - left_value);
							value_num++;

							if (abs(center_value - left_value) < min_value)
							{
								min_value = abs(center_value - left_value);
							}
						}
						if (right_value != 0)
						{
							value_sum += abs(center_value - right_value);
							value_num++;

							if (abs(center_value - right_value) < min_value)
							{
								min_value = abs(center_value - right_value);
							}
						}
						if (value_num != 0)
						{
							//judge by min 
							if (min_value > thre)
							{
								continue;
							}
						}
						else
						{
							continue;
						}
						
						if (depth_back.at<cv::Vec3b>(y, x)[0] >= depth_front.at<cv::Vec3b>(y, x)[0]
							)
						{
							depth_back.at<cv::Vec3b>(y, x)[0] = depth_front.at<cv::Vec3b>(y, x)[0] - 2;
						}

						nx_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[2]);
						ny_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[1]);
						nz_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[0]);

						out <<
							cvtValue2Width(x + off_x_float) << " " // x
							<< cvtValue2Height(y + off_y_float) << " "// y
							<< cvtValue2Depth(depth_back.at<cv::Vec3b>(y, x)[0]) << " "
							<< nx_back << " "// nx
							<< ny_back << " "// ny
							<< nz_back << " "// nz
							<< std::endl;


					}
				}
			}
		}
	}
}

void Reconstruct::generate_point_cloud_sym(std::string f_opc, std::string f_normal_front, std::string f_normal_back, std::string f_depth_front, std::string f_depth_back, std::string f_sym) {

	cv::Mat normal_front = cv::imread(f_normal_front);
	cv::Mat normal_back = cv::imread(f_normal_back);
	cv::Mat depth_front = cv::imread(f_depth_front);
	cv::Mat depth_back = cv::imread(f_depth_back);
	cv::Mat s_depth(depth_front.rows, depth_front.cols, CV_8UC3, cv::Scalar::all(255));
	float a, b, c, d;
	float ox, oy, oz, k, px, py, pz, sx, sy, sz, vx, vy, vz, nx, ny, nz;
	double length;
	int center_value, top_value, bottom_value, left_value, right_value, value_sum, value_num, min_value;
	float thre = 4.;
	std::ifstream fin(f_sym);

	fin >> a >> b >> c >> d;

	fin.close();
	std::cout << "The plane is " << a << " " << b << " " << c << " " << d << std::endl;

	std::fstream file;
	file.open(f_normal_back);
	if (!file)
	{
		return;
	}
	else
	{
		file.close();
	}

	int col = depth_front.cols;
	int row = depth_front.rows;
	std::ofstream out(f_opc);
	float off_x_float, off_y_float, off_unit = 0.33;
	float nx_front, ny_front, nz_front, nx_back, ny_back, nz_back;
	int offset = 0;
	int v_ind = 1;
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				normal_back.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_back.at<cv::Vec3b>(y, x)[0] > 0
				)
			{
				for (int off_x = -offset; off_x < offset + 1; off_x++)
				{
					off_x_float = off_x * off_unit;

					for (int off_y = -offset; off_y < offset + 1; off_y++)
					{
						off_y_float = off_y * off_unit;

						//denoise
						// check density
						value_sum = 0;
						value_num = 0;
						min_value = 300;

						center_value = depth_front.at<cv::Vec3b>(y, x)[0];
						top_value = depth_front.at<cv::Vec3b>(y - 1, x)[0];
						bottom_value = depth_front.at<cv::Vec3b>(y + 1, x)[0];
						left_value = depth_front.at<cv::Vec3b>(y, x - 1)[0];
						right_value = depth_front.at<cv::Vec3b>(y, x + 1)[0];

						if (top_value != 0)
						{
							value_sum += abs(center_value - top_value);
							value_num++;

							if (abs(center_value - top_value) < min_value)
							{
								min_value = abs(center_value - top_value);
							}
						}
						if (bottom_value != 0)
						{
							value_sum += abs(center_value - bottom_value);
							value_num++;

							if (abs(center_value - bottom_value) < min_value)
							{
								min_value = abs(center_value - bottom_value);
							}
						}
						if (left_value != 0)
						{
							value_sum += abs(center_value - left_value);
							value_num++;

							if (abs(center_value - left_value) < min_value)
							{
								min_value = abs(center_value - left_value);
							}
						}
						if (right_value != 0)
						{
							value_sum += abs(center_value - right_value);
							value_num++;

							if (abs(center_value - right_value) < min_value)
							{
								min_value = abs(center_value - right_value);
							}
						}
						if (value_num != 0)
						{

							//judge by min 
							if (min_value > thre)
							{
								depth_front.at<cv::Vec3b>(y, x)[0] = depth_front.at<cv::Vec3b>(y, x)[1] = depth_front.at<cv::Vec3b>(y, x)[2] = 255;
								continue;
							}
						}
						else
						{
							depth_front.at<cv::Vec3b>(y, x)[0] = depth_front.at<cv::Vec3b>(y, x)[1] = depth_front.at<cv::Vec3b>(y, x)[2] = 255;
							continue;
						}

						ox = cvtValue2Width(x);
						oy = cvtValue2Height(y);
						oz = cvtValue2Depth(depth_front.at<cv::Vec3b>(y, x)[0]);

						k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

						px = a * k + ox;
						py = b * k + oy;
						pz = c * k + oz;

						sx = 2 * px - ox;
						sy = 2 * py - oy;
						sz = 2 * pz - oz;

						nx_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[2]);
						ny_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[1]);
						nz_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[0]);

						ox = ox + nx_front;
						oy = oy + ny_front;
						oz = oz + nz_front;

						k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

						px = a * k + ox;
						py = b * k + oy;
						pz = c * k + oz;

						nx = 2 * px - ox - sx;
						ny = 2 * py - oy - sy;
						nz = 2 * pz - oz - sz;

						length = sqrt(nx_front*nx_front + ny_front * ny_front + nz_front * nz_front);
						nx_front /= length;
						ny_front /= length;
						nz_front /= length;

						length = sqrt(nx*nx + ny * ny + nz * nz);
						nx /= length;
						ny /= length;
						nz /= length;
						
						////front
						out <<
							"v " <<
							cvtValue2Width(x + off_x_float) << " " // x
							<< cvtValue2Height(y + off_y_float) << " "// y
							<< cvtValue2Depth(depth_front.at<cv::Vec3b>(y, x)[0]) << " "
							<< std::endl << "vn "  // z
							<< nx_front << " "// nx
							<< ny_front << " "// ny
							<< nz_front << " "// nz
							<< std::endl;

						vx = cvtWidth2Value(sx);
						vy = cvtHeight2Value(sy);
						vz = cvtDepth2Value(sz);

						if (vx >= 0 && vx < depth_front.cols && vy >= 0 && vy < depth_front.rows)
						{
							if (
								depth_front.at<cv::Vec3b>(vy, vx)[0] < vz
								)//reflected point is in front of front, not use 
							{
								continue;
							}

							out <<
								"v " <<
								sx << " " // x
								<< sy << " "// y
								<< sz << " "
								<< std::endl << "vn "  // z
								<< nx << " "// nx
								<< ny << " "// ny
								<< nz << " "// nz
								<< std::endl;


							if (s_depth.at<cv::Vec3b>(vy, vx)[0] > vz)
							{
								s_depth.at<cv::Vec3b>(vy, vx)[0] = s_depth.at<cv::Vec3b>(vy, vx)[1] = s_depth.at<cv::Vec3b>(vy, vx)[2] = vz;
							}

							//back test
							nx_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(vy, vx)[2]);
							ny_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(vy, vx)[1]);
							nz_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(vy, vx)[0]);

							Eigen::Vector3d normal_reflect(nx, ny, nz);
							Eigen::Vector3d normal_back(nx_back, ny_back, nz_back);
							normal_reflect.normalize();
							normal_back.normalize();
							if (
								depth_back.at<cv::Vec3b>(vy, vx)[0] >= vz
								)
							{
								depth_back.at<cv::Vec3b>(vy, vx)[0] = vz - 2;
							}
						}


					}
				}
			}
		}
	}

	//back
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				normal_back.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_back.at<cv::Vec3b>(y, x)[0] > 0
					)
			{
				for (int off_x = -offset; off_x < offset + 1; off_x++)
				{
					off_x_float = off_x * off_unit;

					for (int off_y = -offset; off_y < offset + 1; off_y++)
					{

						off_y_float = off_y * off_unit;


						//////denoise
						// check density
						value_sum = 0;
						value_num = 0;
						min_value = 300;

						center_value = depth_back.at<cv::Vec3b>(y, x)[0];
						top_value = depth_back.at<cv::Vec3b>(y - 1, x)[0];
						bottom_value = depth_back.at<cv::Vec3b>(y + 1, x)[0];
						left_value = depth_back.at<cv::Vec3b>(y, x - 1)[0];
						right_value = depth_back.at<cv::Vec3b>(y, x + 1)[0];

						if (top_value != 0)
						{
							value_sum += abs(center_value - top_value);
							value_num++;

							if (abs(center_value - top_value) < min_value)
							{
								min_value = abs(center_value - top_value);
							}
						}
						if (bottom_value != 0)
						{
							value_sum += abs(center_value - bottom_value);
							value_num++;

							if (abs(center_value - bottom_value) < min_value)
							{
								min_value = abs(center_value - bottom_value);
							}
						}
						if (left_value != 0)
						{
							value_sum += abs(center_value - left_value);
							value_num++;

							if (abs(center_value - left_value) < min_value)
							{
								min_value = abs(center_value - left_value);
							}
						}
						if (right_value != 0)
						{
							value_sum += abs(center_value - right_value);
							value_num++;

							if (abs(center_value - right_value) < min_value)
							{
								min_value = abs(center_value - right_value);
							}
						}
						if (value_num != 0)
						{

							//judge by min 
							if (min_value > thre)
							{
								depth_back.at<cv::Vec3b>(y, x)[0] = depth_back.at<cv::Vec3b>(y, x)[1] = depth_back.at<cv::Vec3b>(y, x)[2] = 255;
								continue;
							}
						}
						else
						{
							depth_back.at<cv::Vec3b>(y, x)[0] = depth_back.at<cv::Vec3b>(y, x)[1] = depth_back.at<cv::Vec3b>(y, x)[2] = 255;
							continue;
						}

						//back reflect
						if (
							true
							)
						{
							length = sqrt(nx_back*nx_back + ny_back * ny_back + nz_back * nz_back);
							nx_back /= length;
							ny_back /= length;
							nz_back /= length;


							if (depth_front.at<cv::Vec3b>(y, x)[0] <= depth_back.at<cv::Vec3b>(y, x)[0])
							{
								depth_back.at<cv::Vec3b>(y, x)[0] = depth_front.at<cv::Vec3b>(y, x)[0] - 2;
							}

							out <<
								"v " << cvtValue2Width(x + off_x_float) <<  " " // x
								<< cvtValue2Height(y + off_y_float) << " "// y
								<< cvtValue2Depth(depth_back.at<cv::Vec3b>(y, x)[0]) << " "
								<< std::endl << "vn "  // z
								<< nx_back << " "// nx
								<< ny_back << " "// ny
								<< nz_back << " "// nz
								<< std::endl;
						}
						else
						{
							//depth_back.at<cv::Vec3b>(vy, vx)[0] = depth_back.at<cv::Vec3b>(vy, vx)[1] = depth_back.at<cv::Vec3b>(vy, vx)[2] = s_depth.at<cv::Vec3b>(y, x)[0];
						}
					}
				}
			}
		}
	}

	cv::imwrite("../order_back.png", depth_back);
}

void Reconstruct::generate_poisson_input_sym(std::string f_opc, std::string f_normal_front, std::string f_normal_back, std::string f_depth_front, std::string f_depth_back, std::string f_sym) {

	cv::Mat normal_front = cv::imread(f_normal_front);
	cv::Mat normal_back = cv::imread(f_normal_back);
	cv::Mat depth_front = cv::imread(f_depth_front);
	cv::Mat depth_back = cv::imread(f_depth_back);

	float a, b, c, d;
	float ox, oy, oz, k, px, py, pz, sx, sy, sz, vx, vy, vz, nx, ny, nz;

	int center_value, top_value, bottom_value, left_value, right_value, value_sum, value_num, min_value;
	float thre = 4.;
	std::ifstream fin(f_sym);

	fin >> a >> b >> c >> d;
	fin.close();
	std::cout << "The plane is " << a << " " << b << " " << c << " " << d << std::endl;


	std::fstream file;
	file.open(f_normal_back);
	if (!file)
	{
		return;
	}
	else
	{
		file.close();
	}

	int col = depth_front.cols;
	int row = depth_front.rows;

	std::ofstream out(f_opc);
	float off_x_float, off_y_float, off_unit = 0.25;
	float nx_front, ny_front, nz_front, nx_back, ny_back, nz_back;
	int offset = 0;
	int v_ind = 1;
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				normal_back.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_back.at<cv::Vec3b>(y, x)[0] > 0
				)
			{

				value_sum = 0;
				value_num = 0;
				min_value = 300;

				center_value = depth_front.at<cv::Vec3b>(y, x)[0];
				top_value = depth_front.at<cv::Vec3b>(y - 1, x)[0];
				bottom_value = depth_front.at<cv::Vec3b>(y + 1, x)[0];
				left_value = depth_front.at<cv::Vec3b>(y, x - 1)[0];
				right_value = depth_front.at<cv::Vec3b>(y, x + 1)[0];

				if (top_value != 0)
				{
					value_sum += abs(center_value - top_value);
					value_num++;

					if (abs(center_value - top_value) < min_value)
					{
						min_value = abs(center_value - top_value);
					}
				}
				if (bottom_value != 0)
				{
					value_sum += abs(center_value - bottom_value);
					value_num++;

					if (abs(center_value - bottom_value) < min_value)
					{
						min_value = abs(center_value - bottom_value);
					}
				}
				if (left_value != 0)
				{
					value_sum += abs(center_value - left_value);
					value_num++;

					if (abs(center_value - left_value) < min_value)
					{
						min_value = abs(center_value - left_value);
					}
				}
				if (right_value != 0)
				{
					value_sum += abs(center_value - right_value);
					value_num++;

					if (abs(center_value - right_value) < min_value)
					{
						min_value = abs(center_value - right_value);
					}
				}
				if (value_num != 0)
				{

					//judge by min 
					if (min_value > thre)
					{
						depth_front.at<cv::Vec3b>(y, x)[0] = depth_front.at<cv::Vec3b>(y, x)[1] = depth_front.at<cv::Vec3b>(y, x)[2] = 255;
						continue;
					}
				}
				else
				{
					depth_front.at<cv::Vec3b>(y, x)[0] = depth_front.at<cv::Vec3b>(y, x)[1] = depth_front.at<cv::Vec3b>(y, x)[2] = 255;
					continue;
				}

				ox = cvtValue2Width(x);
				oy = cvtValue2Height(y);
				oz = cvtValue2Depth(depth_front.at<cv::Vec3b>(y, x)[0]);

				k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

				px = a * k + ox;
				py = b * k + oy;
				pz = c * k + oz;

				sx = 2 * px - ox;
				sy = 2 * py - oy;
				sz = 2 * pz - oz;



				nx_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[2]);
				ny_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[1]);
				nz_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[0]);

				ox = ox + nx_front;
				oy = oy + ny_front;
				oz = oz + nz_front;

				k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

				px = a * k + ox;
				py = b * k + oy;
				pz = c * k + oz;

				nx = 2 * px - ox - sx;
				ny = 2 * py - oy - sy;
				nz = 2 * pz - oz - sz;

				//front

				out <<
					cvtValue2Width(x) << " " // x
					<< cvtValue2Height(y) << " "// y
					<< cvtValue2Depth(depth_front.at<cv::Vec3b>(y, x)[0]) << " "
					<< nx_front << " "// nx
					<< ny_front << " "// ny
					<< nz_front << " "// nz
					<< std::endl;

				//back test for front
				for (int iy = -1; iy < 2; iy++)
				{
					for (int ix = -1; ix < 2; ix++)
					{

						if (
							depth_back.at<cv::Vec3b>(y + iy, x + ix)[0] >= depth_front.at<cv::Vec3b>(y, x)[0]
							)
						{
							depth_back.at<cv::Vec3b>(y + iy, x + ix)[0] = depth_front.at<cv::Vec3b>(y, x)[0] - 2;
						}


					}
				}

				vx = cvtWidth2Value(sx);
				vy = cvtHeight2Value(sy);
				vz = cvtDepth2Value(sz);

				if (vx >= 0 && vx < depth_front.cols && vy >= 0 && vy < depth_front.rows)
				{
					if (
						depth_front.at<cv::Vec3b>(vy, vx)[0] < vz
						)//reflected point is in front of front, not use 
					{
						continue;
					}

					//back test for sym
					Eigen::Vector3d normal_reflect(nx, ny, nz);
					for (int iy = -1; iy < 2; iy++)
					{
						for (int ix = -1; ix < 2; ix++)
						{

							nx_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(vy + iy, vx + ix)[2]);
							ny_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(vy + iy, vx + ix)[1]);
							nz_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(vy + iy, vx + ix)[0]);

							Eigen::Vector3d normal_back(nx_back, ny_back, nz_back);
							normal_reflect.normalize();
							normal_back.normalize();
							
							if (
								depth_back.at<cv::Vec3b>(vy + iy, vx + ix)[0] >= vz

								)
							{
								depth_back.at<cv::Vec3b>(vy + iy, vx + ix)[0] = vz - 2;
							}

							
						}
					}

					out <<
						sx << " " // x
						<< sy << " "// y
						<< sz << " "
						<< nx << " "// nx
						<< ny << " "// ny
						<< nz << " "// nz
						<< std::endl;

				}

			}
		}
	}


	//back
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				normal_back.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_back.at<cv::Vec3b>(y, x)[0] > 0
				)
			{

				//denoise
				// check density
				value_sum = 0;
				value_num = 0;
				min_value = 300;

				center_value = depth_back.at<cv::Vec3b>(y, x)[0];
				top_value = depth_back.at<cv::Vec3b>(y - 1, x)[0];
				bottom_value = depth_back.at<cv::Vec3b>(y + 1, x)[0];
				left_value = depth_back.at<cv::Vec3b>(y, x - 1)[0];
				right_value = depth_back.at<cv::Vec3b>(y, x + 1)[0];

				if (top_value != 0)
				{
					value_sum += abs(center_value - top_value);
					value_num++;

					if (abs(center_value - top_value) < min_value)
					{
						min_value = abs(center_value - top_value);
					}
				}
				if (bottom_value != 0)
				{
					value_sum += abs(center_value - bottom_value);
					value_num++;

					if (abs(center_value - bottom_value) < min_value)
					{
						min_value = abs(center_value - bottom_value);
					}
				}
				if (left_value != 0)
				{
					value_sum += abs(center_value - left_value);
					value_num++;

					if (abs(center_value - left_value) < min_value)
					{
						min_value = abs(center_value - left_value);
					}
				}
				if (right_value != 0)
				{
					value_sum += abs(center_value - right_value);
					value_num++;

					if (abs(center_value - right_value) < min_value)
					{
						min_value = abs(center_value - right_value);
					}
				}
				if (value_num != 0)
				{

					//judge by min 
					if (min_value > thre)
					{
						continue;
					}
				}
				else
				{
					continue;
				}

				nx_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[2]);
				ny_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[1]);
				nz_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[0]);


				out <<
					cvtValue2Width(x) << " " // x
					<< cvtValue2Height(y) << " "// y
					<< cvtValue2Depth(depth_back.at<cv::Vec3b>(y, x)[0]) 
					<< " "
					<< nx_back << " "// nx
					<< ny_back << " "// ny
					<< nz_back << " "// nz
					<< std::endl;
			

			}
		}
	}

}



void Reconstruct::smoothing(std::string fout, std::string fin) {

	cv::Mat in = cv::imread(fin);
	cv::Mat out;

	cv::bilateralFilter(in, out, 5, 75, 75);

	cv::imwrite(fout, out);
}

Reconstruct::Reconstruct()
{
}

Reconstruct::~Reconstruct()
{
}


void Reconstruct::generate_point_cloud_single(std::string f_opc, std::string f_normal_front, std::string f_depth_front) {

	cv::Mat normal_front = cv::imread(f_normal_front);
	cv::Mat depth_front = cv::imread(f_depth_front);

	cv::Mat density_map(depth_front.rows, depth_front.cols, CV_8UC1, cv::Scalar::all(0));

	float a, b, c, d;
	float ox, oy, oz, k, px, py, pz, sx, sy, sz, vx, vy, vz, nx, ny, nz;

	Eigen::RowVector3d symmetry_normal(1, 0, 0);;

	Eigen::Matrix3d rotate_mat_y, rotate_mat_x;

	float x_angle = 30.;
	float y_angle = 150.;

	rotate_mat_x <<
		1, 0, 0,
		0, cos(atan(1) * 4 * x_angle / 180), sin(atan(1) * 4 * x_angle / 180),
		0, -sin(atan(1) * 4 * x_angle / 180), cos(atan(1) * 4 * x_angle / 180);

	rotate_mat_y <<
		cos(atan(1) * 4 * y_angle / 180), 0, -sin(atan(1) * 4 * y_angle / 180),
		0, 1, 0,
		sin(atan(1) * 4 * y_angle / 180), 0, cos(atan(1) * 4 * y_angle / 180);

	symmetry_normal *= rotate_mat_y;
	symmetry_normal *= rotate_mat_x;
	symmetry_normal.normalize();
	std::cout << symmetry_normal << std::endl;

	int col = depth_front.cols;
	int row = depth_front.rows;
	std::ofstream out(f_opc);
	float off_x_float, off_y_float, off_unit = 0.33;
	float nx_front, ny_front, nz_front, nx_back, ny_back, nz_back;
	int offset = 0;
	int v_ind = 1;
	int center_value, top_value, bottom_value, left_value, right_value, value_sum, value_num, min_value;

	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0

				)
			{

				nx_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[2]);
				ny_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[1]);
				nz_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[0]);

				out <<
					"v " <<
					cvtValue2Width(x) << " " // x
					<< cvtValue2Height(y) << " "// y
					<< cvtValue2Depth(depth_front.at<cv::Vec3b>(y, x)[0]) << " "
					<< std::endl << "vn "  // z
					<< nx_front << " "// nx
					<< ny_front << " "// ny
					<< nz_front << " "// nz
					<< std::endl;


			}
		}
	}


}

void Reconstruct::generate_poisson_input_sym_double(std::string f_out, std::string f_out_sym, std::string f_normal_front, std::string f_normal_back, std::string f_depth_front, std::string f_depth_back, std::string f_sym) {

	cv::Mat normal_front = cv::imread(f_normal_front);
	cv::Mat normal_back = cv::imread(f_normal_back);
	cv::Mat depth_front = cv::imread(f_depth_front);
	cv::Mat depth_back = cv::imread(f_depth_back);

	float a, b, c, d;
	float ox, oy, oz, k, px, py, pz, sx, sy, sz, vx, vy, vz, nx, ny, nz;

	int center_value, top_value, bottom_value, left_value, right_value, value_sum, value_num, min_value;
	float thre = 4.;
	std::ifstream fin(f_sym);

	fin >> a >> b >> c >> d;

	fin.close();
	std::cout << "The plane is " << a << " " << b << " " << c << " " << d << std::endl;

	int col = depth_front.cols;
	int row = depth_front.rows;

	std::ofstream out(f_out);
	std::ofstream out_sym(f_out_sym);
	float off_x_float, off_y_float, off_unit = 0.25;
	float nx_front, ny_front, nz_front, nx_back, ny_back, nz_back;
	int offset = 0;
	int v_ind = 1;
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				normal_back.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_back.at<cv::Vec3b>(y, x)[0] > 0
				)
			{

				value_sum = 0;
				value_num = 0;
				min_value = 300;

				center_value = depth_front.at<cv::Vec3b>(y, x)[0];
				top_value = depth_front.at<cv::Vec3b>(y - 1, x)[0];
				bottom_value = depth_front.at<cv::Vec3b>(y + 1, x)[0];
				left_value = depth_front.at<cv::Vec3b>(y, x - 1)[0];
				right_value = depth_front.at<cv::Vec3b>(y, x + 1)[0];

				if (top_value != 0)
				{
					value_sum += abs(center_value - top_value);
					value_num++;

					if (abs(center_value - top_value) < min_value)
					{
						min_value = abs(center_value - top_value);
					}
				}
				if (bottom_value != 0)
				{
					value_sum += abs(center_value - bottom_value);
					value_num++;

					if (abs(center_value - bottom_value) < min_value)
					{
						min_value = abs(center_value - bottom_value);
					}
				}
				if (left_value != 0)
				{
					value_sum += abs(center_value - left_value);
					value_num++;

					if (abs(center_value - left_value) < min_value)
					{
						min_value = abs(center_value - left_value);
					}
				}
				if (right_value != 0)
				{
					value_sum += abs(center_value - right_value);
					value_num++;

					if (abs(center_value - right_value) < min_value)
					{
						min_value = abs(center_value - right_value);
					}
				}
				if (value_num != 0)
				{

					//judge by min 
					if (min_value > thre)
					{
						depth_front.at<cv::Vec3b>(y, x)[0] = depth_front.at<cv::Vec3b>(y, x)[1] = depth_front.at<cv::Vec3b>(y, x)[2] = 255;
						continue;
					}
				}
				else
				{
					depth_front.at<cv::Vec3b>(y, x)[0] = depth_front.at<cv::Vec3b>(y, x)[1] = depth_front.at<cv::Vec3b>(y, x)[2] = 255;
					continue;
				}

				ox = cvtValue2Width(x);
				oy = cvtValue2Height(y);
				oz = cvtValue2Depth(depth_front.at<cv::Vec3b>(y, x)[0]);

				k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

				px = a * k + ox;
				py = b * k + oy;
				pz = c * k + oz;

				sx = 2 * px - ox;
				sy = 2 * py - oy;
				sz = 2 * pz - oz;



				nx_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[2]);
				ny_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[1]);
				nz_front = cvtValue2Normal(normal_front.at<cv::Vec3b>(y, x)[0]);

				ox = ox + nx_front;
				oy = oy + ny_front;
				oz = oz + nz_front;

				k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

				px = a * k + ox;
				py = b * k + oy;
				pz = c * k + oz;

				nx = 2 * px - ox - sx;
				ny = 2 * py - oy - sy;
				nz = 2 * pz - oz - sz;

				//front
				out <<
					cvtValue2Width(x) << " " // x
					<< cvtValue2Height(y) << " "// y
					<< cvtValue2Depth(depth_front.at<cv::Vec3b>(y, x)[0]) << " "
					<< 3 * nx_front << " "// nx
					<< 3 * ny_front << " "// ny
					<< 3 * nz_front << " "// nz
					<< std::endl;
				
				//front sym
				out_sym <<
					sx << " " // x
					<< sy << " "// y
					<< sz << " "
					<< nx << " "// nx
					<< ny << " "// ny
					<< nz << " "// nz
					<< std::endl;

				//back test for front
				for (int iy = -1; iy < 2; iy++)
				{
					for (int ix = -1; ix < 2; ix++)
					{

						if (
							depth_back.at<cv::Vec3b>(y + iy, x + ix)[0] >= depth_front.at<cv::Vec3b>(y, x)[0]
							)
						{
							depth_back.at<cv::Vec3b>(y + iy, x + ix)[0] = depth_front.at<cv::Vec3b>(y, x)[0] - 2;
						}


					}
				}

				vx = cvtWidth2Value(sx);
				vy = cvtHeight2Value(sy);
				vz = cvtDepth2Value(sz);

				if (vx >= 0 && vx < depth_front.cols && vy >= 0 && vy < depth_front.rows)
				{
					if (
						depth_front.at<cv::Vec3b>(vy, vx)[0] < vz
						)//reflected point is in front of front, not use 
					{
						continue;
					}

					//back test for sym
					Eigen::Vector3d normal_reflect(nx, ny, nz);
					for (int iy = -1; iy < 2; iy++)
					{
						for (int ix = -1; ix < 2; ix++)
						{

							nx_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(vy + iy, vx + ix)[2]);
							ny_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(vy + iy, vx + ix)[1]);
							nz_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(vy + iy, vx + ix)[0]);

							Eigen::Vector3d normal_back(nx_back, ny_back, nz_back);
							normal_reflect.normalize();
							normal_back.normalize();

							if (
								depth_back.at<cv::Vec3b>(vy + iy, vx + ix)[0] >= vz
								)
							{
								depth_back.at<cv::Vec3b>(vy + iy, vx + ix)[0] = vz - 2;
							}


						}
					}
				}

			}
		}
	}


	//back
	for (int y = 0; y < row; y++)
	{
		for (int x = 0; x < col; x++)
		{
			if (
				normal_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				normal_back.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_front.at<cv::Vec3b>(y, x)[0] > 0
				&&
				depth_back.at<cv::Vec3b>(y, x)[0] > 0
				)
			{

				//denoise
				// check density
				value_sum = 0;
				value_num = 0;
				min_value = 300;

				center_value = depth_back.at<cv::Vec3b>(y, x)[0];
				top_value = depth_back.at<cv::Vec3b>(y - 1, x)[0];
				bottom_value = depth_back.at<cv::Vec3b>(y + 1, x)[0];
				left_value = depth_back.at<cv::Vec3b>(y, x - 1)[0];
				right_value = depth_back.at<cv::Vec3b>(y, x + 1)[0];

				if (top_value != 0)
				{
					value_sum += abs(center_value - top_value);
					value_num++;

					if (abs(center_value - top_value) < min_value)
					{
						min_value = abs(center_value - top_value);
					}
				}
				if (bottom_value != 0)
				{
					value_sum += abs(center_value - bottom_value);
					value_num++;

					if (abs(center_value - bottom_value) < min_value)
					{
						min_value = abs(center_value - bottom_value);
					}
				}
				if (left_value != 0)
				{
					value_sum += abs(center_value - left_value);
					value_num++;

					if (abs(center_value - left_value) < min_value)
					{
						min_value = abs(center_value - left_value);
					}
				}
				if (right_value != 0)
				{
					value_sum += abs(center_value - right_value);
					value_num++;

					if (abs(center_value - right_value) < min_value)
					{
						min_value = abs(center_value - right_value);
					}
				}
				if (value_num != 0)
				{

					//judge by min 
					if (min_value > thre)
					{
						continue;
					}
				}
				else
				{
					continue;
				}

				nx_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[2]);
				ny_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[1]);
				nz_back = cvtValue2Normal(normal_back.at<cv::Vec3b>(y, x)[0]);


				ox = cvtValue2Width(x);
				oy = cvtValue2Height(y);
				oz = cvtValue2Depth(depth_back.at<cv::Vec3b>(y, x)[0]);

				k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

				px = a * k + ox;
				py = b * k + oy;
				pz = c * k + oz;

				sx = 2 * px - ox;
				sy = 2 * py - oy;
				sz = 2 * pz - oz;

				ox = ox + nx_back;
				oy = oy + ny_back;
				oz = oz + nz_back;

				k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

				px = a * k + ox;
				py = b * k + oy;
				pz = c * k + oz;

				nx = 2 * px - ox - sx;
				ny = 2 * py - oy - sy;
				nz = 2 * pz - oz - sz;


				out <<
					cvtValue2Width(x) << " " // x
					<< cvtValue2Height(y) << " "// y
					<< cvtValue2Depth(depth_back.at<cv::Vec3b>(y, x)[0]) << " "
					<< nx_back << " "// nx
					<< ny_back << " "// ny
					<< nz_back << " "// nz
					<< std::endl;

				//front sym
				out_sym <<
					sx << " " // x
					<< sy << " "// y
					<< sz << " "
					<< nx << " "// nx
					<< ny << " "// ny
					<< nz << " "// nz
					<< std::endl;

			}
		}
	}

}