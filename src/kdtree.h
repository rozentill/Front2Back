#pragma once

#include <vector>
#include <Eigen\Dense>
#include <algorithm>
#include <iostream>

#define FLT_MAX 3.402823466e+38F  

using namespace std;

class BBox
{
public:

	Eigen::Vector3d min;
	Eigen::Vector3d max;

	BBox();
	~BBox();

	BBox BB_trim_left(int curr_channel, Eigen::Vector3d point);
	BBox BB_trim_right(int curr_channel, Eigen::Vector3d point);

private:

};


class KDNode
{
public:
	Eigen::Vector3d data;

	KDNode *left;
	KDNode *right;

	bool used;

	int ind = 0;

	KDNode(Eigen::Vector3d x);
	~KDNode();

private:

};


class KDTree
{
public:

	Eigen::Vector3d best_point;
	float best_dist;
	BBox bb;
	KDNode *root;
	Eigen::MatrixXd points;
	vector<Eigen::Vector3d> points_array, query_results;
	vector<KDNode> node_list;
	int node_number;
	KDTree();
	~KDTree();

	void load_points(Eigen::MatrixXd input);
	void load_points_from_vector(vector<Eigen::Vector3d> input);
	KDNode* insert(Eigen::Vector3d x, KDNode *node, int curr_channel);
	void search_next(int curr_channel, Eigen::MatrixXd input);
	KDNode* build_tree(int curr_channel, std::vector < Eigen::Vector3d>::iterator start, std::vector < Eigen::Vector3d>::iterator end);
	void construct_tree();
	float dist_bb(Eigen::Vector3d query, BBox bb);
	void NN(Eigen::Vector3d query, KDNode* node, int curr_channel, BBox bb);
	void NN_range(Eigen::Vector3d query, KDNode* node, int curr_channel, double range);
	Eigen::Vector3d find_NN(Eigen::Vector3d query, float &dist);
	std::vector<Eigen::Vector3d> find_NN_range(Eigen::Vector3d query, double range);
private:

};

