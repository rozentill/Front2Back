//#include "register.h"
#include "eval.h"
#include "kdtree.h"
#include <time.h>  
#include <iostream>
#include <numeric>
#include <igl/random_points_on_mesh.h>
#include <igl/point_mesh_squared_distance.h>


int binarySearch(
	const Eigen::VectorXd &CDF,
	int startIdx,
	int endIdx,
	double searchVal) {

	if (startIdx == endIdx)
		return CDF(startIdx) <= searchVal ? startIdx : -1;

	int midIdx = startIdx + (endIdx - startIdx) / 2;
	if (searchVal <= CDF(midIdx))
		return binarySearch(CDF, startIdx, midIdx, searchVal);

	int returnIdx = binarySearch(CDF, midIdx + 1, endIdx, searchVal);
	return returnIdx == -1 ? midIdx : returnIdx;
}

int searchIdx(
	const Eigen::VectorXd &CDF,
	int startIdx,
	int endIdx,
	double searchVal) {
	int faceIdx = binarySearch(CDF, startIdx, endIdx, searchVal);
	// clamp result
	if (faceIdx < 0)
		faceIdx = 0;
	if (faceIdx > (CDF.size() - 1))
		faceIdx = CDF.size() - 1;
	return faceIdx;
}


void random_points_on_mesh(
	const int n,
	const Eigen::MatrixXd & V,
	const Eigen::MatrixXi & F,
	Eigen::MatrixXd & X)
{
	srand(time(NULL));
	X.resize(n, 3);

	Eigen::VectorXd FA, CDF;
	igl::doublearea(V, F, FA);
	igl::cumsum(FA, 1, CDF);
	CDF /= CDF.maxCoeff(); // make CDF min(CDF) ~ 1

	double randNum, u, v, w;
	int FIdx;
	Eigen::Vector3d V0, V1, V2;
	for (int ii = 0; ii < X.rows(); ii++) {
		randNum = ((double)rand() / (RAND_MAX));
		FIdx = searchIdx(CDF, 0, CDF.size() - 1, randNum);

		// barycentric sample per triangle
		u = ((double)rand() / (RAND_MAX));
		v = ((double)rand() / (RAND_MAX));
		w = 1.0 - u - v;
		V0 = V.row(F(FIdx, 0));
		V1 = V.row(F(FIdx, 1));
		V2 = V.row(F(FIdx, 2));
		X.row(ii) = u * V0 + v * V1 + w * V2;
	}
}


double Eval::chamfer_distance_igl(Eigen::MatrixXd src, Eigen::MatrixXd dst, Eigen::MatrixXi F_src, Eigen::MatrixXi F_dst) {
	
	int sample_num = 10000;
	float total = 0;
	
	Eigen::VectorXi FI;
	Eigen::SparseMatrix<double> B;

	Eigen::MatrixXd X_src, X_dst;

	random_points_on_mesh(sample_num, src, F_src, X_src);
	random_points_on_mesh(sample_num, dst, F_dst, X_dst);

	//assume dst is the gt
	Eigen::Vector3d max_range = dst.colwise().maxCoeff();
	Eigen::Vector3d min_range = dst.colwise().minCoeff();

	float max_dist = max(max(max_range(0) - min_range(0), max_range(1) - min_range(1)), max_range(2) - min_range(2));

	Eigen::VectorXd sqrD;
	Eigen::VectorXi I;
	Eigen::MatrixXd C;
	
	igl::point_mesh_squared_distance(X_src, dst, F_dst, sqrD, I, C);

	total += sqrD.sum();

	igl::point_mesh_squared_distance(X_dst, src, F_src, sqrD, I, C);

	total += sqrD.sum();

	return total / (2 * sample_num * max_dist * max_dist);


}

double Eval::cd(Eigen::MatrixXd src, Eigen::MatrixXd dst) {

	int ind, sample_large, sample_small;
	float dist = FLT_MAX, total = 0;

	std::cout << "Number of src is " << src.rows() << " and number of dst is " << dst.rows();

	//assume dst is the gt
	Eigen::Vector3d max_range = dst.colwise().maxCoeff();
	Eigen::Vector3d min_range = src.colwise().minCoeff();

	//float max_dist = max(max(max_range(0) - min_range(0), max_range(1) - min_range(1)), max_range(2) - min_range(2));
	float max_dist = 1;//assume bounding box is unit length(scaled)
	//build kdtree
	KDTree *tree_src = new KDTree();
	tree_src->load_points(src);
	tree_src->construct_tree();

	KDTree *tree_dst = new KDTree();
	tree_dst->load_points(dst);
	tree_dst->construct_tree();

	Eigen::Vector3d src_v, dst_v;
	////sample 1024 from src and find closest in dst
	for (int i = 0; i < src.rows(); i++)
	{
		dist = FLT_MAX;
		ind = i;
		src_v = src.row(ind);
		tree_dst->find_NN(src_v, dist);

		total += dist / max_dist;
	}

	//sample 1024 from dst and find closest in src
	for (int i = 0; i < dst.rows(); i++)
	{
		dist = FLT_MAX;
		ind = i;
		dst_v = dst.row(ind);
		tree_src->find_NN(dst_v, dist);

		total += dist / max_dist;
	}


	total /= (src.rows()+dst.rows());

	delete tree_dst;
	delete tree_src;

	return total;
}


Eval::Eval()
{
}

Eval::~Eval()
{
}