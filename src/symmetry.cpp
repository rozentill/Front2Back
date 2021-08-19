#include "symmetry.h"
#include "param.h"
#include "kdtree.h"
#include <ios>
#include <stdlib.h>
#include <time.h>  
#include <vector>
#include <iostream>
#include <algorithm>

bool operator<(const Correspondence& x, const Correspondence& y) {
	return (x.dist < y.dist);
}

bool operator<(const Cluster& x, const Cluster& y) {
	return (x.density < y.density);
}

float kernelEpanechnikov(float dist) {
	return 3.f*(1 - abs(dist)) / 4.f;
}

// assume a2+b2+c2 = 1
Eigen::Vector3d custom_transform(Eigen::Vector4d plane) {

	double x = plane(0);
	double y = plane(1);
	double z = plane(2);
	double d = plane(3);

	double phi = atan(y / x);
	double theta = acos(z);

	return Eigen::Vector3d(phi/PI, theta/ PI, d/(0.6*sqrt(2)));

}

Eigen::Vector4d Symmetry::custom_inverse_transform(Eigen::Vector3d vote) {

	double phi = vote(0)*PI;
	double theta = vote(1)*PI;
	double d = vote(2)*(0.6*sqrt(2));

	double a = sin(theta)*cos(phi);
	double b = sin(theta)*sin(phi);
	double c = cos(theta);

	return Eigen::Vector4d(a, b, c, d);

}

Symmetry::Symmetry() {
	height = -1;
	width = -1;

	plane_set = false;
}

float cosSimilarity(Eigen::Vector3d a, Eigen::Vector3d b) {

	float ra, rb;
	ra = sqrt(a(0)*a(0) + a(1)*a(1) + a(2)*a(2));
	rb = sqrt(b(0)*b(0) + b(1)*b(1) + b(2)*b(2));
	return 	(a(0)*b(0) + a(1)*b(1) + a(2)*b(2)) / (ra*rb);

}

void Symmetry::input_maps(std::string f_depth, std::string f_normal) {

	if (f_depth != "")
	{
		std::cout << "[IO] Read front depth map: " << f_depth << std::endl;
		front_depth = cv::imread(f_depth);
		
		height = front_depth.rows;
		width = front_depth.cols;

	}

	if (f_normal != "")
	{
		std::cout << "[IO] Read front normal map: " << f_normal << std::endl;
		front_normal = cv::imread(f_normal);

		height = front_normal.rows;
		width = front_normal.cols;
	}

	if (height!=-1)
	{
		indice = Eigen::MatrixXd::Random(height, width);

	}
}


void Symmetry::set_sample(int sample_size) {

	sample.clear();
	normal.clear();
	point.clear();

	int index = 0;
	bool silhouette_flag = false;
	
	sample_map = front_depth.clone();
	
	Eigen::Vector3d tmp_point, tmp_sample, tmp_normal;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{

			if (front_depth.at<cv::Vec3b>(y, x)[0] > 0)
			{
				tmp_point(0) = reconstruct.cvtValue2Width(x);
				tmp_point(1) = reconstruct.cvtValue2Height(y);
				tmp_point(2) = reconstruct.cvtValue2Depth(front_depth.at<cv::Vec3b>(y, x)[0]);
				point.push_back(tmp_point);

				tmp_normal(0) = reconstruct.cvtValue2Normal(front_normal.at<cv::Vec3b>(y, x)[2]);//nx
				tmp_normal(1) = reconstruct.cvtValue2Normal(front_normal.at<cv::Vec3b>(y, x)[1]);//ny
				tmp_normal(2) = reconstruct.cvtValue2Normal(front_normal.at<cv::Vec3b>(y, x)[0]);//nz
				
				//std::cout << (float)front_normal.at<cv::Vec3b>(y, x)[2] << " " << (float)front_normal.at<cv::Vec3b>(y, x)[1] << " " << (float)front_normal.at<cv::Vec3b>(y, x)[0] << " and exact is " << tmp_normal << " and norm is " << tmp_normal.norm() << std::endl;
				double tmp_norm = tmp_normal.norm();
				tmp_normal(0) /= tmp_norm;
				tmp_normal(1) /= tmp_norm;
				tmp_normal(2) /= tmp_norm;
				//std::cout << (float)front_normal.at<cv::Vec3b>(y, x)[2] << " " << (float)front_normal.at<cv::Vec3b>(y, x)[1] << " " << (float)front_normal.at<cv::Vec3b>(y, x)[0] << " and exact is " << tmp_normal << std::endl;
				normal.push_back(tmp_normal);
				indice(y, x) = index;
				index++;
			}
		}
	}

	//only for sample
	if (sample_size == -1)
	{
		sample_size = (int)sqrt(point.size() / 350.);
	}
	if (sample_size <=0)
	{
		sample_size = 1;
	}
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{

			if (front_depth.at<cv::Vec3b>(y, x)[0] > 0)
			{

				if (y%sample_size == 0 && x%sample_size == 0)//sample
				{
					tmp_sample(0) = x;
					tmp_sample(1) = y;
					tmp_sample(2) = reconstruct.cvtValue2Depth(front_depth.at<cv::Vec3b>(y, x)[0]);
					sample.push_back(tmp_sample);
				}
			}
		}
	}

	ori_pointCloud.resize(point.size(), 4);
	sym_pointCloud.resize(point.size(), 4);
	ori_orientation.resize(point.size(), 4);
	sym_orientation.resize(point.size(), 4);

	for (int i = 0; i < point.size(); i++)
	{
		ori_pointCloud(i, 0) = point[i](0);
		ori_pointCloud(i, 1) = point[i](1);
		ori_pointCloud(i, 2) = point[i](2);
		ori_pointCloud(i, 3) = 1;

		ori_orientation(i, 0) = point[i](0) + normal[i](0);
		ori_orientation(i, 1) = point[i](1) + normal[i](1);
		ori_orientation(i, 2) = point[i](2) + normal[i](2);
		ori_orientation(i, 3) = 1;
	}

	std::cout << "[SAMPLE] Finished sampling with sample size " << sample_size << ", the number of sampled points are :" << sample.size() << " and the number of points is " << point.size() << ".\n";

}

void Symmetry::vote_symmetry(float criteria) {

	votes.clear();

	for (int i = 0; i < sample.size(); i++)
	{
		for (int j = i + 1; j < sample.size(); j++)
		{
			Eigen::Vector3d pi = sample[i];
			Eigen::Vector3d pj = sample[j];
			Eigen::Vector3d ni, nj;

			int iy = pi(1), ix = pi(0), jy = pj(1), jx = pj(0);

			ni = normal[(int)indice(iy, ix)];
			nj = normal[(int)indice(jy, jx)];

			pi(0) = reconstruct.cvtValue2Width(pi(0));
			pj(0) = reconstruct.cvtValue2Width(pj(0));
			pi(1) = reconstruct.cvtValue2Height(pi(1));
			pj(1) = reconstruct.cvtValue2Height(pj(1));

			Eigen::Vector3d middle = (pi + pj) / 2;//x0,y0,z0
			Eigen::Vector3d direction = pi - pj;//a,b,c
			Eigen::Vector3d direction_normal = pi + ni - pj - nj;
			Eigen::Vector3d middle_normal = (pi + ni + pj + nj) / 2;

			float similarity = abs(acos(cosSimilarity(direction, ni)) - acos(cosSimilarity(nj, -direction)));
			float direction_similarity = abs(acos(cosSimilarity(direction, direction_normal)));
			similarity = similarity * 180 / PI;
			direction_similarity = direction_similarity * 180 / PI;
			
			if (
				similarity < criteria && direction_similarity < criteria
				)
			{
				Eigen::Vector4d tmp_plane;
				Eigen::Vector3d tmp_normal, tmp_vote;

				float a = direction(0);
				float b = direction(1);
				float c = direction(2);
				float d = -a * middle(0) - b * middle(1) - c * middle(2);
				float sq_sum = sqrt(a * a + b * b + c * c);

				if (-a - b - c + d > 0)
				{
					a *= -1;
					b *= -1;
					c *= -1;
					d *= -1;
				}

				if (sq_sum == 0)
				{

					tmp_plane(0) = 0;
					tmp_plane(1) = 0;
					tmp_plane(2) = 0;
					tmp_plane(3) = 0;

					std::cout << "[ERROR] Now the sum of plane weights is 0." << std::endl;
				}
				else {

					tmp_plane(3) = d / sq_sum; //dist
					tmp_plane(0) = a / sq_sum;
					tmp_plane(1) = b / sq_sum;
					tmp_plane(2) = c / sq_sum;
					
					tmp_vote = custom_transform(tmp_plane);

					votes_list.push_back(tmp_vote);
				}
			}
		}
	}

	std::cout << "Finished voting, the number of votes are :" << votes_list.size() << " .\n";

}


void Symmetry::mean_shift_cluster_all(float h, float stop) {
	
	cluster.clear(); 
	
	Eigen::Vector3d shift(0, 0, 0);
	
	int sample_step = 1, spread;
	float tmp_dist, tmp_step, density;
	
	//get clusters
	for (int v = 0; v < votes_list.size(); v += sample_step)
	{
		Eigen::Vector3d center = votes_list[v];
		
		int iter = 0;
		do
		{
			iter++;
			center += shift;// compute the next shift

			shift.setZero();
			
			spread = 0;
			density = 0;

			//
			for (int i = 0; i < votes_list.size(); i+= 1)
			{
				Eigen::Vector3d tmp = votes_list[i];

				tmp_dist = (tmp - center).squaredNorm();
				
				if (tmp_dist < h)
				{
					shift += kernelEpanechnikov(tmp_dist / h)*(votes_list[i] - center);
					density += kernelEpanechnikov(tmp_dist / h);
				}

			}
			if (density != 0)
			{
				shift /= density;
			}
			tmp_step = shift.squaredNorm();

		} while (tmp_step > stop);


		///check if the cluster has similar
		bool already_exist = false;
		for (int i = 0; i < cluster.size(); i++)
		{
			Eigen::Vector3d tmp_center = cluster[i].center;
			if ((tmp_center-center).norm() < 2e-2)
			{
				//std::cout << "Already exist " << std::endl;
				already_exist = true;
				cluster[i].center = (tmp_center + center) / 2.;
				cluster[i].spread++;
				break;
			}
		}

		if (!already_exist)
		{
			Cluster  tmp_cluster;
			tmp_cluster.votes.clear();
			tmp_cluster.center = center;
			tmp_cluster.spread=1;
			cluster.push_back(tmp_cluster);
		}

	}


	//find biggest cluster
	std::sort(cluster.begin(), cluster.end());

	std::cout << "There are total " << cluster.size() << " grouped clusters.\n";
}

void Symmetry::mean_shift_cluster_speed(float h, float stop) {

	cluster.clear();
	cluster_ransac_list.clear();
	Eigen::Vector3d shift(0, 0, 0);
	Eigen::Vector3d tmp;
	int sample_step = 2, spread;
	float tmp_dist, tmp_step, density;
	int sample_num = 10;
	int sub_num = 4e3 < votes_list.size() ? 4e3 : votes_list.size();
	std::set<unsigned int> indices;
	std::vector<Eigen::Vector3d> sub_votes;
	std::srand(std::time(nullptr));
	std::set<unsigned int>::iterator start;
	std::vector<Eigen::Vector3d> nearest_points;
	cluster_ransac_list.resize(sample_num);
	for (int r = 0; r < sample_num; r++)
	{
		cluster_ransac_list[r].clear();
		indices.clear();
		while (indices.size() < sub_num)
			indices.insert(rand() % votes_list.size());
		sub_votes.clear();
		start = indices.begin();
		while (start != indices.end())
		{
			sub_votes.push_back(votes_list[*start]);
			start++;
		}

		//for speed up
		
		KDTree *tree = new KDTree();
		tree->load_points_from_vector(sub_votes);
		tree->construct_tree();

		//iterate each point for shifting
		for (int v = 0; v < sub_votes.size(); v += 1)
		{
			Eigen::Vector3d center = sub_votes[v];
			int iter = 0;
			do
			{
				iter++;
				center += shift;// compute the next shift

				shift.setZero();

				spread = 0;
				density = 0;

				//speed
				nearest_points.clear();
				nearest_points = tree->find_NN_range(center, h);
				for (int i = 0; i < nearest_points.size(); i++)
				{
					tmp = nearest_points[i];
					tmp_dist = (tmp - center).squaredNorm();
					shift += kernelEpanechnikov(tmp_dist / (h*h))*(tmp - center);
					density += kernelEpanechnikov(tmp_dist / (h*h));
				}

				if (density != 0)
				{
					shift /= density;
				}
				tmp_step = shift.norm();

			} while (tmp_step > stop);


			///check if the cluster has similar
			bool already_exist = false;
			for (int i = 0; i < cluster_ransac_list[r].size(); i++)
			{
				Eigen::Vector3d tmp_center = cluster_ransac_list[r][i].center;
				if ((tmp_center - center).norm() < 1e-2)
				{
					//std::cout << "Already exist " << std::endl;
					already_exist = true;
					cluster_ransac_list[r][i].center = (tmp_center + center) / 2.;
					cluster_ransac_list[r][i].spread++;
					break;
				}
			}

			if (!already_exist)
			{
				Cluster  tmp_cluster;
				tmp_cluster.votes.clear();
				tmp_cluster.center = center;
				tmp_cluster.spread = 1;
				tmp_cluster.density = 0;
				cluster_ransac_list[r].push_back(tmp_cluster);
			}

		}

		delete tree;

		//traverse all the points
		double thre_cluster = h;
		for (int i = 0; i < votes_list.size(); i++)
		{
			double dist_cluster_best = 100, dist_cluster;
			int closest_cluster;
			for (int j = 0; j < cluster_ransac_list[r].size(); j++)
			{
				dist_cluster = (votes_list[i] - cluster_ransac_list[r][j].center).squaredNorm();

				if (dist_cluster < h*h)
				{
					cluster_ransac_list[r][j].density++;
				}
			}
		}


		for (int i = 0; i < cluster_ransac_list[r].size(); i++)
		{
			cluster.push_back(cluster_ransac_list[r][i]);
		}
	}

	//find biggest cluster
	std::sort(cluster.begin(), cluster.end());

	std::cout << "There are total " << cluster.size() << " grouped clusters.\n";
}

float Symmetry::verify_plane(Eigen::Vector4d plane, float &outside_ratio, float &depth_ratio, float &white_ratio) {
	
	int total_num = point.size(), outside_num = 0;
	int depth_thre = 10, depth_vio_num = 0;;
	normalize_plane(plane);
	int expand_gap = 5;
	cv::Mat s_depth = front_depth.clone();
	cv::Mat silhou = front_depth.clone();
	cv::Mat silhou_expand;
	////initial with mask
	for (int i = 0; i < front_depth.rows; i++)
	{
		for (int j = 0; j < front_depth.cols; j++)
		{
			if (front_depth.at<cv::Vec3b>(i, j)[0] > 0)
			{
				s_depth.at<cv::Vec3b>(i, j)[0] = s_depth.at<cv::Vec3b>(i, j)[1] = s_depth.at<cv::Vec3b>(i, j)[2] = 255;
				silhou.at<cv::Vec3b>(i, j)[0] = silhou.at<cv::Vec3b>(i, j)[1] = silhou.at<cv::Vec3b>(i, j)[2] = 255;
			}
		}
	}

	//extend visual hull
	silhou_expand = silhou.clone();
	for (int iter = 0; iter < 5; iter++)
	{
		for (int y = 0; y < s_depth.rows; y++)
		{
			for (int x = 0; x < s_depth.cols; x++)
			{
				if (
					silhou.at<cv::Vec3b>(y, x)[0]==255
					)
				{
					if (silhou.at<cv::Vec3b>(y - 1, x)[0] == 0)
					{
						silhou_expand.at<cv::Vec3b>(y - 1, x)[0] = silhou_expand.at<cv::Vec3b>(y - 1, x)[1] = silhou_expand.at<cv::Vec3b>(y - 1, x)[2] = 255;
					}

					if (silhou.at<cv::Vec3b>(y + 1, x)[0] == 0)
					{
						silhou_expand.at<cv::Vec3b>(y + 1, x)[0] = silhou_expand.at<cv::Vec3b>(y + 1, x)[1] = silhou_expand.at<cv::Vec3b>(y + 1, x)[2] = 255;
					}

					if (silhou.at<cv::Vec3b>(y , x - 1)[0] == 0)
					{
						silhou_expand.at<cv::Vec3b>(y , x - 1)[0] = silhou_expand.at<cv::Vec3b>(y, x - 1)[1] = silhou_expand.at<cv::Vec3b>(y, x - 1)[2] = 255;
					}

					if (silhou.at<cv::Vec3b>(y, x + 1)[0] == 0)
					{
						silhou_expand.at<cv::Vec3b>(y, x + 1)[0] = silhou_expand.at<cv::Vec3b>(y, x + 1)[1] = silhou_expand.at<cv::Vec3b>(y, x + 1)[2] = 255;
					}
				}
			}
		}

		silhou = silhou_expand.clone();
	}

	float a = plane(0);
	float b = plane(1);
	float c = plane(2);
	float d = plane(3);

	float ox, oy, oz, k, px, py, pz, sx, sy, sz, vx, vy, vz, nx, ny, nz;

	//check outside
	for (int y = 0; y < front_depth.rows; y++)
	{
		for (int x = 0; x < front_depth.cols; x++)
		{
			if (front_depth.at<cv::Vec3b>(y, x)[0] == 0)
			{
				continue;
			}

			ox = reconstruct.cvtValue2Width(x);
			oy = reconstruct.cvtValue2Height(y);
			oz = reconstruct.cvtValue2Depth(front_depth.at<cv::Vec3b>(y, x)[0]);

			k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

			px = a * k + ox;
			py = b * k + oy;
			pz = c * k + oz;

			sx = 2 * px - ox;
			sy = 2 * py - oy;
			sz = 2 * pz - oz;

			vx = reconstruct.cvtWidth2Value(sx);
			vy = reconstruct.cvtHeight2Value(sy);
			vz = reconstruct.cvtDepth2Value(sz);

			if (vx >= front_depth.cols || vx < 0 || vy >= front_depth.rows || vy < 0)
			{
				outside_num++;
			}
			else {
				if (silhou.at<cv::Vec3b>(vy, vx)[0] == 0)
				{
					outside_num++;
				}
				else if(front_depth.at<cv::Vec3b>(vy, vx)[0] != 0)//inside, check depth
				{
					if ((vz - (int)front_depth.at<cv::Vec3b>(vy, vx)[0]) > depth_thre)
					{
						depth_vio_num++;
					}

					s_depth.at<cv::Vec3b>(vy, vx)[0] = vz;
				}
			}
		}
	}
	
	int white_num = 0;
	for (int i = 0; i < s_depth.rows; i++)
	{
		for (int j = 0; j < s_depth.cols; j++)
		{
			if (s_depth.at<cv::Vec3b>(i, j)[0] == 255)
			{
				white_num++;
			}
		}
	}

	outside_ratio = (float)outside_num / total_num;
	depth_ratio = (float)depth_vio_num / total_num;
	white_ratio = (float)white_num / total_num;
	cv::imwrite("test.png", silhou);
	return (float)outside_num / total_num;
}

bool Symmetry::find_best(char *prefix) {

	float depth_ratio, outside_ratio, best_depth = 1., best_outside = 1., white_ratio = 0., best_white = 1.;
	float depth_thre = 0.1, outside_thre = 0.4, white_thre = 0.8;
	char f_plane[256];
	bool found = false;
	int num_plane = 0, best_ind;
	Eigen::Vector4d curr_plane;
	vector<Eigen::Vector4d> plane_list;
	plane_list.clear();
	bool exist = false;
	// iterate first three
	for (int j = 0; j < cluster.size(); j++)
	{
		//std::cout << "Number " << j << " plane test..\n";
		curr_plane = custom_inverse_transform(get_cluster_list()[get_cluster_list().size() - 1 - j].center);
		
		//test if this plane already appear
		exist = false;
		for (int p = 0; p < plane_list.size(); p++)
		{
			if ((curr_plane - plane_list[p]).squaredNorm() <= 2e-2)
			{
				exist = true;
				break;
			}
		}
		if (exist)
		{
			continue;
		}
		else
		{
			plane_list.push_back(curr_plane);
		}
		num_plane++;
		verify_plane(curr_plane, outside_ratio, depth_ratio, white_ratio);
		std::cout << "Number " << num_plane << "outside ratio is " << outside_ratio << "  and depth ratio is " << depth_ratio << " and white ratio is " << white_ratio << std::endl;
		std::sprintf(f_plane, "%s_%d.png", prefix, num_plane);
		if (outside_ratio < outside_thre && depth_ratio < depth_thre && white_ratio < white_thre)
		{
			
			found = true;
			
			if (
				(depth_ratio < best_depth + 0.01 && outside_ratio < best_outside - 0.1)
				||
				(outside_ratio < best_outside * 3 / 4  && depth_ratio < depth_thre)
				||
				(depth_ratio < best_depth  && outside_ratio < best_outside)
				||
				(outside_ratio < best_outside * 4 / 3 && depth_ratio < best_depth*3./4. && best_outside < 0.2)
				||
				(outside_ratio < best_outside * 2 && depth_ratio < best_depth / 2 && outside_ratio < 0.1)

				)
			{
				best_white = white_ratio;
				best_depth = depth_ratio;
				best_outside = outside_ratio;
				best_plane = curr_plane;
				best_ind = num_plane;
			}

			
		}
		if (num_plane == 50)
		{
			break;
		}
	}
	std::sprintf(f_plane, "%s_best.png", prefix, num_plane);
	visualize_best_plane(f_plane, best_plane);
	std::cout << "The best plane number is "<< best_ind <<" and the best ratios are white: " << best_white << " depth: " << best_depth << " and best outside " << best_outside << std::endl;
	if(!found)
	{
		std::cout << "[ERROR] Not found good plane.\n" << std::endl;
	}

	return found;
}

void Symmetry::visualize_best_plane(std::string f_vis_plane, Eigen::Vector4d plane) {

	cv::Mat symmetry = front_depth.clone();
	normalize_plane(plane);
	float a = plane(0);
	float b = plane(1);
	float c = plane(2);
	float d = plane(3);
		
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Eigen::Vector3d tmp_point;

			//if valid, set to the sample
			if (symmetry.at<cv::Vec3b>(y, x)[0] > 0)
			{
				tmp_point(0) = reconstruct.cvtValue2Width(x);
				tmp_point(1) = reconstruct.cvtValue2Height(y);
				tmp_point(2) = reconstruct.cvtValue2Depth(symmetry.at<cv::Vec3b>(y, x)[0]);

				if (a*tmp_point(0) + b * tmp_point(1) + c * tmp_point(2) + d > 0) // along the normal, set to blue
				{

					symmetry.at<cv::Vec3b>(y, x)[2] = 255;
				}
				else
				{

					symmetry.at<cv::Vec3b>(y, x)[0] = 255;
				}
			}
		}
	}

	cv::imwrite(f_vis_plane, symmetry);
}

void Symmetry::optimize_plane(int max_iter) {
	
	ori_cloud.pts.resize(ori_pointCloud.rows());
	for (int i = 0; i < ori_pointCloud.rows(); i++)
	{
		ori_cloud.pts[i].x = ori_pointCloud(i, 0);
		ori_cloud.pts[i].y = ori_pointCloud(i, 1);
		ori_cloud.pts[i].z = ori_pointCloud(i, 2);
	}
	index = new my_kd_tree_t(3 /*dim*/, ori_cloud, KDTreeSingleIndexAdaptorParams(20 /* max leaf */));
	index->buildIndex();

	optimize_planes.clear();

	std::vector<Eigen::Vector3d> src, dst, ori, tmp_src, tmp_dst, tmp_ori;

	float err_old, err_new, err2;
	float da, db, dc, dd;//derivatives of the plane
	float df;
	float a, b, c, d;

	Eigen::Vector4d curr_plane = best_plane;
	Eigen::Vector4d better_plane = curr_plane;
	Eigen::Vector4d gradient, direction;
	std::cout << "before optimize : " << curr_plane << std::endl;
	float cc = 0.00025, rho = 0.95, e = 0.001, t = 0.005;

	for (int i = 0; i < max_iter; i++)//10 iters for compute correspondences
	{

		affine_reflection(better_plane);

		err_old = compute_closest_point_kdtree(src, dst, ori);
		
		a = better_plane(0);
		b = better_plane(1);
		c = better_plane(2);
		d = better_plane(3);

		da = 0;
		db = 0;
		dc = 0;
		dd = 0;

		//compute gradient
		for (int num = 0; num < src.size(); num++)
		{

			float xi = ori[num](0), yi = ori[num](1), zi = ori[num](2);
			float xip = src[num](0), yip = src[num](1), zip = src[num](2);
			float xj = dst[num](0), yj = dst[num](1), zj = dst[num](2);

			da += (-4 * a*xi - 2 * b * yi - 2 * c * zi - 2 * d)*(xip - xj)
				+ (-2 * b*xi)*(yip - yj)
				+ (-2 * c*xi)*(zip - zj);

			db += (-2 * a*yi)*(xip - xj)
				+ (-2 * a*xi - 4 * b*yi - 2 * c*zi - 2 * d)*(yip - yj)
				+ (-2 * c*yi)*(zip - zj);

			dc += (-2 * a*zi)*(xip - xj)
				+ (-2 * b*zi)*(yip - yj)
				+ (-2 * a*xi - 2 * b*yi - 4 * c*zi - 2 * d)*(zip - zj);

			dd += (-2 * a)*(xip - xj)
				+ (-2 * b)*(yip - yj)
				+ (-2 * c)*(zip - zj);

		}

		gradient = Eigen::Vector4d(da, db, dc, dd);

		direction = -gradient;
		direction /= direction.norm();

		
		curr_plane = better_plane + t * direction;
		normalize_plane(curr_plane);

		err_new = error_optimize(curr_plane, ori, dst);

		int iter = 0;
		int max_step_iter = 20;
		bool found = false;

		for (int iter = 0; iter < max_step_iter; iter++)
		{
			if (err_new - err_old < 0)
			{
				if (iter == 0)
				{
					t -= 0.01*t;
				}
				found = true;
				break;
			}
			t *= rho;
			curr_plane = better_plane + t * direction;
			normalize_plane(curr_plane);
			err_new = error_optimize(curr_plane, ori, dst);
		}

		if (found == true)
		{
			better_plane = curr_plane;
			optimize_planes.push_back(better_plane);
		}
		else
		{
			break;
		}

	}
	std::cout << " after optimize " << better_plane << std::endl;
	//write final corresponding points
	ofstream fout;
	fout.open("../ori_pc.obj");
	for (int i = 0; i < ori_pointCloud.rows(); i++)
	{
		fout << "v " << ori_pointCloud(i, 0) << " " << ori_pointCloud(i, 1) << " " << ori_pointCloud(i, 2) << " " << std::endl;
	}
	fout.close();

	fout.open("../reflect_pc.obj");
	for (int i = 0; i < sym_pointCloud.rows(); i++)
	{
		fout << "v " << sym_pointCloud(i, 0) << " " << sym_pointCloud(i, 1) << " " << sym_pointCloud(i, 2) << " " << std::endl;
	}
	fout.close();

	fout.open("../corr.obj");
	for (int i = 0; i < src.size(); i++)
	{
		fout << "v " << src[i](0) << " " << src[i](1) << " " << src[i](2) << " " << std::endl;
		fout << "v " << dst[i](0) << " " << dst[i](1) << " " << dst[i](2) << " " << std::endl;

		fout << "l " << i * 2 + 1 << " " << i * 2 + 2 << std::endl;
	}
	fout.close();
	best_plane = better_plane;

	delete index;
}

void Symmetry::affine_reflection(const Eigen::Vector4d &sym_plane) {

	float a, b, c, d;
	Eigen::Matrix4d affine;

	a = sym_plane(0);
	b = sym_plane(1);
	c = sym_plane(2);
	d = sym_plane(3);

	affine << 1 - 2 * a*a, -2 * a*b, -2 * a*c, 0,
		-2 * a*b, 1 - 2 * b*b, -2 * b*c, 0,
		-2 * a*c, -2 * b*c, 1 - 2 * c*c, 0,
		-2 * a*d, -2 * b*d, -2 * c*d, 1;

	sym_pointCloud = ori_pointCloud * affine;
	sym_orientation = ori_orientation * affine;

	transform = affine;
}

void Symmetry::normalize_plane(Eigen::Vector4d &plane) {
	float normal_length = sqrt(pow(plane(0), 2) + pow(plane(1), 2) + pow(plane(2), 2));
	plane(0) /= normal_length;
	plane(1) /= normal_length;
	plane(2) /= normal_length;	
	plane(3) /= normal_length;
}

float Symmetry::compute_closest_point_kdtree(std::vector<Eigen::Vector3d> &src, std::vector<Eigen::Vector3d> &dst, std::vector<Eigen::Vector3d> &ori, float thre) {

	float min_dist = 100, tmp_dist, loss = 0;
	Eigen::Vector3d point;
	Eigen::Vector3d ori_normal, sym_normal;
	int num_valid = 0, min_index;
	src.clear();
	dst.clear();
	ori.clear();

	double query_pt[3];
	int num_results = 1;
	std::vector<size_t> ret_index(num_results);
	std::vector<double> out_dist_sqr(num_results);

	for (int i = 0; i < sym_pointCloud.rows(); i++)
	{

		query_pt[0] = sym_pointCloud(i, 0);
		query_pt[1] = sym_pointCloud(i, 1);
		query_pt[2] = sym_pointCloud(i, 2);

		
		index->knnSearch(&query_pt[0], num_results, &ret_index[0], &out_dist_sqr[0]);
		
		ret_index.resize(num_results);
		out_dist_sqr.resize(num_results);

		ori_normal = ori_orientation.row(ret_index[0]) - ori_pointCloud.row(ret_index[0]);
		sym_normal = sym_orientation.row(i) - sym_pointCloud.row(i);
		
		ori_normal.normalize();
		sym_normal.normalize();

		if (acos(ori_normal.dot(sym_normal)) > PI / 3.)
		{
			continue;
		}

		if (out_dist_sqr[0] < thre)
		{
			src.push_back(sym_pointCloud.row(i));
			dst.push_back(ori_pointCloud.row(ret_index[0]));
			ori.push_back(ori_pointCloud.row(i));

			loss += out_dist_sqr[0];
			num_valid++;
		}
	}

	return loss / num_valid;
}

float Symmetry::compute_closest_point(std::vector<Eigen::Vector3d> &src, std::vector<Eigen::Vector3d> &dst, std::vector<Eigen::Vector3d> &ori, float thre) {

	float min_dist = 100, tmp_dist, loss = 0;;
	float sx, sy, sz, ox, oy, oz, min_x, min_y, min_z;
	int num_valid = 0, min_index;
	bool *ifUsed = new bool[point.size()];

	src.clear();
	dst.clear();
	ori.clear();
	
	Correspondence tmp_corr;
	memset(ifUsed, false, sizeof(bool)*point.size());

	std::vector<Correspondence> corr_list;

	corr_list.clear();

	//initialize corr list
	for (int i = 0; i < point.size(); i++)
	{
		tmp_corr = find_min_corr(i, ifUsed);

		if (tmp_corr.dist < thre)
		{
			corr_list.push_back(tmp_corr);
		}
	}

	std::sort(corr_list.begin(), corr_list.end());
	int num = 0;
	bool confict = false;
	while (num<corr_list.size())
	{
		confict = false;
		for (; num < corr_list.size(); num++)
		{
			int index_dst = corr_list[num].index_dst;

			if (!ifUsed[index_dst])
			{
				src.push_back(Eigen::Vector3d(corr_list[num].sx, corr_list[num].sy, corr_list[num].sz));
				dst.push_back(Eigen::Vector3d(corr_list[num].ox, corr_list[num].oy, corr_list[num].oz));
				ori.push_back(Eigen::Vector3d(corr_list[num].x, corr_list[num].y, corr_list[num].z));

				num_valid++;
				loss += corr_list[num].dist;
				ifUsed[index_dst] = true;
			}
			else
			{
				confict = true;
				break;
			}
		}
		if (confict)
		{
			tmp_corr = find_min_corr(corr_list[num].index_src, ifUsed);
			if (tmp_corr.dist < thre)
			{
				corr_list.push_back(tmp_corr);
			}
			num++;
		}
	}



	delete ifUsed;

	return loss / num_valid;
}

Correspondence Symmetry::find_min_corr(int index_src, bool * ifUsed, float criteria) {
	
	float sx, sy, sz, ox, oy, oz, nx, ny, nz, snx, sny, snz, min_x, min_y, min_z;
	float tmp_dist, min_dist = 100;
	int min_index;
	
	Eigen::Vector3d ni, nj, direction;
	
	Correspondence min_corr;

	sx = sym_pointCloud(index_src, 0);
	sy = sym_pointCloud(index_src, 1);
	sz = sym_pointCloud(index_src, 2);

	snx = sym_orientation(index_src, 0);
	sny = sym_orientation(index_src, 1);
	snz = sym_orientation(index_src, 2);

	ni = Eigen::Vector3d(snx - sx, sny - sy, snz - sz);

	for (int j = 0; j < point.size(); j++)
	{
		if (ifUsed[j])
		{
			continue;
		}
		ox = ori_pointCloud(j, 0);
		oy = ori_pointCloud(j, 1);
		oz = ori_pointCloud(j, 2);

		nx = ori_orientation(j, 0);
		ny = ori_orientation(j, 1);
		nz = ori_orientation(j, 2);

		nj = Eigen::Vector3d(nx - ox, ny - oy, nz - oz);

		direction = Eigen::Vector3d(sx - ox, sy - oy, sz - oz);

		tmp_dist = pow((sx - ox), 2) + pow((sy - oy), 2) + pow((sz - oz), 2);

		float similarity = acos(cosSimilarity(direction, ni)) - acos(cosSimilarity(-direction, nj));
		similarity = similarity * 180 / PI;

		if (
			tmp_dist < min_dist
			&&
			abs(similarity) < criteria
			)
		{
			min_dist = tmp_dist;
			min_x = ox;
			min_y = oy;
			min_z = oz;
			min_index = j;
		}
	}

	min_corr.dist = min_dist;

	min_corr.index_dst = min_index;
	min_corr.index_src = index_src;

	min_corr.ox = min_x;
	min_corr.oy = min_y;
	min_corr.oz = min_z;

	min_corr.sx = sx;
	min_corr.sy = sy;
	min_corr.sz = sz;

	min_corr.x = ori_pointCloud(index_src, 0);
	min_corr.y = ori_pointCloud(index_src, 1);
	min_corr.z = ori_pointCloud(index_src, 2);

	return min_corr;
}


float Symmetry::error_optimize(Eigen::Vector4d sym_plane, std::vector<Eigen::Vector3d> ori, std::vector<Eigen::Vector3d> dst) {

	int num = ori.size();
	float err = 0;
	Eigen::Vector4d vec_ori, vec_dst;

	float a, b, c, d;
	Eigen::Matrix4d affine;

	a = sym_plane(0);
	b = sym_plane(1);
	c = sym_plane(2);
	d = sym_plane(3);

	affine << 1 - 2 * a*a, -2 * a*b, -2 * a*c, 0,
		-2 * a*b, 1 - 2 * b*b, -2 * b*c, 0,
		-2 * a*c, -2 * b*c, 1 - 2 * c*c, 0,
		-2 * a*d, -2 * b*d, -2 * c*d, 1;

#pragma omp parallel for
	for (int i = 0; i < num; i++)
	{
		Eigen::Vector4d vec_ori = Eigen::Vector4d(ori[i](0), ori[i](1), ori[i](2), 1);
		Eigen::Vector4d vec_dst = Eigen::Vector4d(dst[i](0), dst[i](1), dst[i](2), 1);
		vec_ori = (vec_ori.transpose() * affine).transpose();

		err += (pow(vec_dst(0) - vec_ori(0), 2) + pow(vec_dst(1) - vec_ori(1), 2) + pow(vec_dst(2) - vec_ori(2), 2));

	}
	return err / num;
}

void Symmetry::generate_reflect_map(std::string o_depth, std::string o_normal, bool found) {
	cv::Mat s_depth(front_depth.rows, front_depth.cols, CV_8UC3, cv::Scalar::all(0));
	cv::Mat s_normal(front_normal.rows, front_normal.cols, CV_8UC3, cv::Scalar::all(0));

	//initial with mask
	for (int i = 0; i < front_depth.rows; i++)
	{
		for (int j = 0; j < front_depth.cols; j++)
		{
			if (front_depth.at<cv::Vec3b>(i, j)[0] > 0)
			{
				s_depth.at<cv::Vec3b>(i, j)[0] = s_depth.at<cv::Vec3b>(i, j)[1] = s_depth.at<cv::Vec3b>(i, j)[2] = 255;
				s_normal.at<cv::Vec3b>(i, j)[0] = s_normal.at<cv::Vec3b>(i, j)[1] = s_normal.at<cv::Vec3b>(i, j)[2] = 255;
			}
		}
	}

	Eigen::Vector3d tmp_normal;
	float a, b, c, d;

	a = best_plane(0);
	b = best_plane(1);
	c = best_plane(2);
	d = best_plane(3);

	float ox, oy, oz, k, px, py, pz, sx, sy, sz, vx, vy, vz, nx, ny, nz;
	if (found)
	{


		for (int y = 0; y < front_depth.rows; y++)
		{
			for (int x = 0; x < front_depth.cols; x++)
			{
				if (front_depth.at<cv::Vec3b>(y, x)[0] == 0)
				{
					continue;
				}

				ox = reconstruct.cvtValue2Width(x);
				oy = reconstruct.cvtValue2Height(y);
				oz = reconstruct.cvtValue2Depth(front_depth.at<cv::Vec3b>(y, x)[0]);

				k = -(a*ox + b * oy + c * oz + d) / (a*a + b * b + c * c);

				px = a * k + ox;
				py = b * k + oy;
				pz = c * k + oz;

				sx = 2 * px - ox;
				sy = 2 * py - oy;
				sz = 2 * pz - oz;


				vx = reconstruct.cvtWidth2Value(sx);
				vy = reconstruct.cvtHeight2Value(sy);
				vz = reconstruct.cvtDepth2Value(sz);

				if (vx < 0 || vx>front_depth.cols || vy < 0 || vy>front_depth.rows || vz < 0 || vz>255)
				{
					std::cout << "Border error!" << std::endl;
					continue;
				}

				if (s_depth.at<cv::Vec3b>(vy, vx)[0] > 0 && s_depth.at<cv::Vec3b>(vy, vx)[0] < vz)
				{
					continue;
				}

				s_depth.at<cv::Vec3b>(vy, vx)[0] = s_depth.at<cv::Vec3b>(vy, vx)[1] = s_depth.at<cv::Vec3b>(vy, vx)[2] = vz;

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

				tmp_normal(0) = nx;
				tmp_normal(1) = ny;
				tmp_normal(2) = nz;

				tmp_normal.normalize();

				s_normal.at<cv::Vec3b>(vy, vx)[0] = (uchar)((nz + 1)*255.f / 2.f);
				s_normal.at<cv::Vec3b>(vy, vx)[1] = (uchar)((ny + 1)*255.f / 2.f);
				s_normal.at<cv::Vec3b>(vy, vx)[2] = (uchar)((nx + 1)*255.f / 2.f);
			}
		}
	}

	cv::imwrite(o_depth, s_depth);
	cv::imwrite(o_normal, s_normal);
}
