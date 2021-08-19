#pragma once
#include <Eigen/Core>
// Given a 3×3 matrix `M`, find the closest rotation matrix `R`.
//
// Inputs:
//   M  3x3 matrix
// Outputs:
//   R  3x3 rotation matrix
//
void closest_rotation(
	const Eigen::Matrix3d & M,
	Eigen::Matrix3d & R);

void random_points_on_mesh(
	const int n,
	const Eigen::MatrixXd & V,
	const Eigen::MatrixXi & F,
	Eigen::MatrixXd & X);

// Conduct a single iteration of the iterative closest point method align
// (`VX`,`FX`) to (`VY`,`FY`) by finding the rigid transformation
// (`R`,`t`)minimizing the matching energy:
//
// \\[
//    ∫_X ‖ Rx+t - P_Y( Rx +t ) ‖² dA
// \\]
//
//
// Inputs:
//   VX  #VX by 3 list of mesh vertex positions
//   FX  #FX by 3 list of triangle mesh indices into VX
//   VY  #VY by 3 list of mesh vertex positions
//   FY  #FY by 3 list of triangle mesh indices into VY
//   num_samples  number of random samples to use (larger --> more accurate)
//   method  method of linearization to use, one ot ICP_METHOD_POINT_TO_POINT
//     or ICP_METHOD_POINT_TO_PLANE
// Outputs:
//   R  3 by 3 rotation matrix
//   t  3d translation vector 
double icp_single_iteration(
	const Eigen::MatrixXd & VX,
	const Eigen::MatrixXi & FX,
	const Eigen::MatrixXd & VY,
	const Eigen::MatrixXi & FY,
	const int num_samples,
	Eigen::Matrix3d & R,
	Eigen::RowVector3d & t);
