#pragma once

#include <iostream>
#include <Eigen\Core>
class Eval
{
public:
	
	Eval();
	~Eval();

	double chamfer_distance(Eigen::MatrixXd src, Eigen::MatrixXd dst);//input two point clouds
	double chamfer_distance_igl(Eigen::MatrixXd src, Eigen::MatrixXd dst, Eigen::MatrixXi F_src, Eigen::MatrixXi F_dst);
	double cd(Eigen::MatrixXd src, Eigen::MatrixXd dst);
private:

};

