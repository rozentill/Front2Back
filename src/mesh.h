#pragma once
#include "headers.h"
#include <igl/doublearea.h>
#include <math.h>
void project_orthographic(int resolution, Eigen::Vector3d vec_world, Eigen::Vector3d &vec_raster) {
	
	// 0-x, 1-y, 2-z
	double x = vec_world(0);
	double y = vec_world(1);
	double z = vec_world(2);//useless for orthographic view

	double w = resolution, h = resolution;
	double l_h = tan(40. / 360. * igl::PI) * (2.2);
	double l_w = l_h;

	double x_ras = (x + l_w) * (w) / (l_w * 2);
	double y_ras = (l_h - y) * (h) / (l_h * 2);

	vec_raster(0) = x_ras;
	vec_raster(1) = y_ras;
	vec_raster(2) = 0;

	//std::cout << "Projecting " << vec_world << " to " << vec_raster << std::endl;
}

double edgeFunction(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c)
{
	return ((c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]))/2;
}

double areaPolygon(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c, const Eigen::Vector3d &d) 
{
	return abs((a(0)*b(1) - a(1)*b(0)) + (b(0)*c(1) - b(1)*c(0)) + (c(0)*d(1) - c(1)*d(0)) + (d(0)*a(1) - d(1)*a(0))) / 2;
}

double areaTriangle(const Eigen::Vector3d &a, const Eigen::Vector3d &b, const Eigen::Vector3d &c) {
	return ((b - a).cross(c - a)).norm() / 2;
}

void compute_visible_percentage() {

	char f_in[256], f_out[256], shapenet[1000];

	double bbox_x_min, bbox_x_max, bbox_y_min, bbox_y_max;
	double tmp_area, curr_depth;// temporal variables

	//constants
	double camera_center = -2.2;
	int resolution = 2048;
	int width = resolution, height = resolution;

	std::sprintf(shapenet, "D:/Data/ShapeNetCore.v2/ShapeNetCore.v2/03001627");

	std::vector<std::string> items;
	items = get_directories(shapenet);

	Eigen::MatrixXd V;
	Eigen::MatrixXi F;

	Eigen::Vector3d vert_0_world, vert_1_world, vert_2_world, vert_0_raster, vert_1_raster, vert_2_raster;
	Eigen::Vector3d pixel_0_world, pixel_1_world, pixel_2_world, pixel_3_world;
	Eigen::Vector3d tmp_pixel;

	Eigen::MatrixXd area_buffer, depth_buffer;
	Eigen::VectorXd area_mesh;

	Render render(resolution, true);

	int total_model = 0;
	double total_percentage = 0., front_percet, back_percent;
	cv::Mat test_depth(cv::Size(width, height), CV_8UC1);

	for (int i = 0; i < 10; i++)
	{
		std::sprintf(f_in, "%s/%s/models/model_normalized.obj", shapenet, items[i].c_str());

		//std::sprintf(f_in, "../test/cube.obj");

		std::cout << "Now working on data " << items[i].c_str() << std::endl;
		for (int v = 0; v < 5; v++)
		{

			igl::readOBJ(f_in, V, F);

			//front side or back side
			for (int s = 0; s < 2; s++)
			{

				if (s == 0)//front side, generate rotation
				{
					render.view_generation(30, 360, (int)(2 * ((v + i) % 2 - 0.5f)));
					V *= render.get_rotation_y();
					V *= render.get_rotation_x();

				}
				else
				{
					V *= render.get_rotation_back();

				}
				test_depth.setTo(0);
				area_buffer.resize(height, width);
				depth_buffer.resize(height, width);
				depth_buffer.setZero();
				area_buffer.setZero();

				//iterate each triangle
				for (int f = 0; f < F.rows(); f++)
				{
					vert_0_world = V.row(F(f, 0));
					vert_1_world = V.row(F(f, 1));
					vert_2_world = V.row(F(f, 2));

					project_orthographic(resolution, vert_0_world, vert_0_raster);
					project_orthographic(resolution, vert_1_world, vert_1_raster);
					project_orthographic(resolution, vert_2_world, vert_2_raster);

					//iterate bbox pixels
					bbox_x_min = std::min(std::min(vert_0_raster(0), vert_1_raster(0)), vert_2_raster(0));
					bbox_x_max = std::max(std::max(vert_0_raster(0), vert_1_raster(0)), vert_2_raster(0));
					bbox_y_min = std::min(std::min(vert_0_raster(1), vert_1_raster(1)), vert_2_raster(1));
					bbox_y_max = std::max(std::max(vert_0_raster(1), vert_1_raster(1)), vert_2_raster(1));

					//if bbox out of screen
					if (bbox_x_min < 0 || bbox_x_max >= width || bbox_y_max >= height || bbox_y_min < 0)
						continue;

					//the whole area of the triangle
					tmp_area = edgeFunction(vert_0_raster, vert_1_raster, vert_2_raster);

					for (int by = (int)bbox_y_min; by <= (int)bbox_y_max; by++)
					{
						for (int bx = (int)bbox_x_min; bx <= (int)bbox_x_max; bx++)
						{
							//check if the pixel is inside the triangle
							tmp_pixel(0) = bx + 0.5;
							tmp_pixel(1) = by + 0.5;
							tmp_pixel(2) = 0;

							double w0 = edgeFunction(vert_1_raster, vert_2_raster, tmp_pixel);
							double w1 = edgeFunction(vert_2_raster, vert_0_raster, tmp_pixel);
							double w2 = edgeFunction(vert_0_raster, vert_1_raster, tmp_pixel);
							
							w0 /= tmp_area;
							w1 /= tmp_area;
							w2 /= tmp_area;

							if (w0 >= 0 && w1 >= 0 && w2 >= 0) { // in the triangle
								
								curr_depth = vert_0_world(2) * w0 + vert_1_world(2) * w1 + vert_2_world(2) * w2;

								curr_depth -= camera_center;

								curr_depth = 1 / curr_depth;

								if (curr_depth > depth_buffer(by, bx))//set this pixel's values
								{
									depth_buffer(by, bx) = curr_depth;
									area_buffer(by, bx) = 1. * areaTriangle(vert_0_world, vert_1_world, vert_2_world) / abs(tmp_area);
									test_depth.at<uchar>(by, bx) = 255 * (curr_depth - 1 / 3.2) / (1 / 1.2 - 1 / 3.2);
								}
							}
						}
					}
				}

				// sum up area and divide by total area
				igl::doublearea(V, F, area_mesh);
				area_mesh = area_mesh.array() / 2.;
				std::cout << " The percentage of side " << s << " of view " << v << " 's visible area is " << area_buffer.sum() / area_mesh.sum() << std::endl;
				if (s == 1)
				{
					total_model++;
					back_percent = area_buffer.sum() / area_mesh.sum();
					std::cout << "Finish this sample, front+back is " << front_percet + back_percent << std::endl;
				}
				else
				{
					front_percet = area_buffer.sum() / area_mesh.sum();
				}
				
				total_percentage += area_buffer.sum() / area_mesh.sum();
				sprintf(f_out, "../test/test_depth_data%d_view%d_side%d.png", i, v, s);
				cv::imwrite(f_out, test_depth);
			}
		}
	}

	std::cout << "[FINAL RESULT] Average percentage is " << total_percentage / total_model << std::endl;
}