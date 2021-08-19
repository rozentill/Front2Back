#pragma once

#include <string>
#include <vector>

#include <Eigen/SVD>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/Geometry>
#include <opencv2/opencv.hpp>

#include "headers.h"
#include "reconstruct.h"

#include "utils.h"
#include "nanoflann.hpp"

using namespace nanoflann;

typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<double, PointCloud<double>>, PointCloud<double>, 3> my_kd_tree_t;

typedef struct {
	Eigen::Vector4d plane; // plane - d, a, b, c, d; transform - dist, x, y, z
	Eigen::Vector3d normal, transform;
	int index_pos, index_neg; // the pair for this vote
} Vote;

typedef struct {
	std::vector<Vote> votes;
	Eigen::Vector3d center;

	float distance, density;
	int spread;
} Cluster;

typedef struct {
	int index_src, index_dst;
	float dist;
	float sx, sy, sz, ox, oy, oz, x, y, z;
} Correspondence;

typedef struct {
	float x;
	float y;
	float z;

	float nx;
	float ny;
	float nz;
} Point;

typedef struct {//store the indices of the vertices
	int a;
	int b;
	int c;
} Face;


class Symmetry
{
public:
	Symmetry();
	
	void input_maps(std::string f_depth = "", std::string f_normal = "");
	void set_sample(int sample_size=-1);
	void vote_symmetry(float criteria = 10);
	void mean_shift_cluster_all(float h, float stop);
	void mean_shift_cluster_speed(float h, float stop);
	void visualize_best_plane(std::string f_vis_plane, Eigen::Vector4d plane);
	float verify_plane(Eigen::Vector4d plane, float &outside_ratio, float &depth_ratio, float &white_ratio);
	bool find_best(char *prefix);
	void optimize_plane(int max_iter=20);
	void affine_reflection(const Eigen::Vector4d &plane);
	void normalize_plane(Eigen::Vector4d &plane);
	void generate_reflect_map(std::string o_depth, std::string o_normal, bool found=true);

	float compute_closest_point(std::vector<Eigen::Vector3d> &src, std::vector<Eigen::Vector3d> &dst, std::vector<Eigen::Vector3d> &ori, float thre = 2e-2);
	float compute_closest_point_kdtree(std::vector<Eigen::Vector3d> &src, std::vector<Eigen::Vector3d> &dst, std::vector<Eigen::Vector3d> &ori, float thre = 4e-3);
	
	Correspondence find_min_corr(int index_src, bool * ifUsed, float criteria = 40.0);

	float error_optimize(Eigen::Vector4d sym_plane, std::vector<Eigen::Vector3d> ori, std::vector<Eigen::Vector3d> dst);

	std::vector<Eigen::Vector4d> optimize_planes;

	Eigen::Vector4d get_best_plane() {
		return best_plane;
	}

	std::vector<Cluster> get_cluster_list() {
		return cluster;
	}

	static Eigen::Vector4d custom_inverse_transform(Eigen::Vector3d vote);
private:

	std::vector<Eigen::Vector3d> sample;
	std::vector<Eigen::Vector3d> point;
	std::vector<Eigen::Vector3d> normal;
	std::vector<Eigen::Vector3d> votes;

	std::vector<Eigen::Vector3d> votes_list;
	std::vector<Cluster> cluster;
	std::vector < std::vector<Cluster>> cluster_ransac_list;
	Eigen::Vector4d best_plane; //for visualization
	Eigen::MatrixXd indice;

	Eigen::MatrixXd ori_pointCloud;
	Eigen::MatrixXd sym_pointCloud;
	Eigen::MatrixXd ori_orientation;
	Eigen::MatrixXd sym_orientation;

	Eigen::Matrix4d transform;

	cv::Mat edge_map;
	cv::Mat sym_depth_front;
	cv::Mat sym_depth_back;
	cv::Mat sym_normal_front;
	cv::Mat sym_normal_back;
	cv::Mat sample_map;
	//cv::Mat s_depth;

	Cluster max_cluster; // for visualization

	Reconstruct reconstruct;

	cv::Mat front_depth, front_normal;

	float height, width;

	bool plane_set;

	PointCloud<double> ori_cloud;
	my_kd_tree_t *index;
};



