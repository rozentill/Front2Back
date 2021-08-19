#include "register.h"

#include <Eigen/Dense>
#include <Eigen/SVD>
#include <Eigen/QR>

#include <igl/random_points_on_mesh.h>
#include <igl/point_mesh_squared_distance.h>
#include <igl/per_face_normals.h>

void closest_rotation(
	const Eigen::Matrix3d & M,
	Eigen::Matrix3d & R)
{

	// My code
	Eigen::JacobiSVD<Eigen::MatrixXd> svd(M, Eigen::ComputeFullU | Eigen::ComputeFullV);
	Eigen::Matrix3d U = svd.matrixU();
	Eigen::Matrix3d V = svd.matrixV();
	Eigen::Matrix3d Omega = Eigen::Matrix3d::Identity();
	Omega(2, 2) = (U*V.transpose()).determinant();
	R = (U * Omega * V.transpose()).transpose();
}


void point_triangle_distance(
	const Eigen::RowVector3d & x,
	const Eigen::RowVector3d & aa,
	const Eigen::RowVector3d & bb,
	const Eigen::RowVector3d & cc,
	double & dd,
	Eigen::RowVector3d & p)
{

	Eigen::RowVector3d B = aa;
	Eigen::RowVector3d E0 = bb - aa;
	Eigen::RowVector3d E1 = cc - aa;
	Eigen::RowVector3d P = x;

	double a = E0.dot(E0);
	double b = E0.dot(E1);
	double c = E1.dot(E1);
	double d = E0.dot(B - P);
	double e = E1.dot(B - P);
	double f = (B - P).dot(B - P);

	double s = b * e - c * d;
	double t = b * d - a * e;
	double det = a * c - b * b;

	if (s + t <= det) {
		if (s < 0) {
			if (t < 0) {
				// ===============
				// region 4
				if (d < 0) {
					t = 0;
					if (-d >= a)
						s = 1;
					else
						s = -d / a;
				}
				else {
					s = 0;
					if (e >= 0)
						t = 0;
					else if (-e >= c)
						t = 1;
					else
						t = -e / c;
				}
				// ===============
			}
			else {
				// ===============
				// region 3
				s = 0;
				if (e >= 0)
					t = 0;
				else if (-e >= c)
					t = 1;
				else
					t = -e / c;
				// ===============
			}
		}
		else if (t < 0) {
			// ===============
			// region 5
			t = 0;
			if (d >= 0)
				s = 0;
			else if (-d >= a)
				s = 1;
			else
				s = -d / a;
			// ===============
		}
		else {
			// ===============
			// region 0
			s /= det;
			t /= det;
			// ===============
		}
	}
	else {
		if (s < 0) {
			// ===============
			// region 2
			double temp0 = b + d;
			double temp1 = c + e;
			if (temp1 > temp0) {
				double num = temp1 - temp0;
				double denom = a - 2 * b + c;
				if (num >= denom)
					s = 1;
				else
					s = num / denom;
				t = 1 - s;
			}
			else {
				s = 0;
				if (temp1 <= 0)
					t = 1;
				else if (e >= 0)
					t = 0;
				else
					t = -e / c;
			}
			// ===============
		}
		else if (t < 0) {
			// ===============
			// region 6
			double temp0 = b + e;
			double temp1 = a + d;
			if (temp1 > temp0) {
				double num = temp1 - temp0;
				double denom = a - 2 * b + c;
				if (num >= denom)
					t = 1;
				else
					t = num / denom;
				s = 1 - t;
			}
			else {
				t = 0;
				if (temp1 <= 0)
					s = 1;
				else if (d >= 0)
					s = 0;
				else
					s = -d / a;
			}
			// ===============
		}
		else {
			// ===============
			// region 1
			double num = (c + e) - (b + d);
			if (num <= 0)
				s = 0;
			else {
				double denom = a - 2 * b + c;
				if (num >= denom)
					s = 1;
				else
					s = num / denom;
			}
			t = 1 - s;
			// ===============
		}
	}
	p = B + s * E0 + t * E1;
	dd = (p - x).norm();
}





void point_to_plane_rigid_matching(
	const Eigen::MatrixXd & X,
	const Eigen::MatrixXd & P,
	const Eigen::MatrixXd & N,
	Eigen::Matrix3d & R,
	Eigen::RowVector3d & t)
{
	R = Eigen::Matrix3d::Identity();
	t = Eigen::RowVector3d::Zero();

	int k = X.rows();
	// construct b
	Eigen::VectorXd RHS, u;
	Eigen::MatrixXd LHS;
	RHS.resize(k);
	LHS.resize(k, 6);
	u.resize(6);
	for (int ii = 0; ii < k; ii++) {
		// assemble RHS
		RHS(ii) = N(ii, 0)*(P(ii, 0) - X(ii, 0)) +
			N(ii, 1)*(P(ii, 1) - X(ii, 1)) +
			N(ii, 2)*(P(ii, 2) - X(ii, 2));
		// assemble A
		LHS.row(ii) << N(ii, 2)*X(ii, 1) - N(ii, 1)*X(ii, 2),
			N(ii, 0)*X(ii, 2) - N(ii, 2)*X(ii, 0),
			N(ii, 1)*X(ii, 0) - N(ii, 0)*X(ii, 1),
			N(ii, 0),
			N(ii, 1),
			N(ii, 2);
	}
	u = LHS.colPivHouseholderQr().solve(RHS);
	Eigen::Matrix3d M;
	M << 1, -u(2), u(1),
		u(2), 1, -u(0),
		-u(1), u(0), 1;
	closest_rotation(M, R);
	t(0) = u(3);
	t(1) = u(4);
	t(2) = u(5);
}

void point_mesh_distance(
	const Eigen::MatrixXd & X,
	const Eigen::MatrixXd & VY,
	const Eigen::MatrixXi & FY,
	Eigen::VectorXd & D,
	Eigen::MatrixXd & P,
	Eigen::MatrixXd & N)
{
	// compute face normals
	Eigen::MatrixXd FN;
	igl::per_face_normals(VY, FY, Eigen::Vector3d(1, 1, 1).normalized(), FN);

	D.resize(X.rows());
	P.resize(X.rows(), 3);
	N.resize(X.rows(), 3);

	Eigen::RowVector3d Xpt, a, b, c, p;
	double dist;
	double minDist;
	int minIdx;
	double pmin_x, pmin_y, pmin_z;
	for (int ii = 0; ii < X.rows(); ii++) { // loop over points X
		minDist = std::numeric_limits<double>::max();

		for (int jj = 0; jj < FY.rows(); jj++) { // loop over triangles
			Xpt = X.row(ii);
			a = VY.row(FY(jj, 0));
			b = VY.row(FY(jj, 1));
			c = VY.row(FY(jj, 2));
			point_triangle_distance(Xpt, a, b, c, dist, p);

			if (dist < minDist) {
				minDist = dist;
				minIdx = jj;
				pmin_x = p(0);
				pmin_y = p(1);
				pmin_z = p(2);
			}
		}
		D(ii) = minDist;
		P.row(ii) << pmin_x, pmin_y, pmin_z;
		N.row(ii) = FN.row(minIdx);
	}
}

double icp_single_iteration(
	const Eigen::MatrixXd & VX,
	const Eigen::MatrixXi & FX,
	const Eigen::MatrixXd & VY,
	const Eigen::MatrixXi & FY,
	const int num_samples,
	Eigen::Matrix3d & R,
	Eigen::RowVector3d & t)
{
	R = Eigen::Matrix3d::Identity();
	t = Eigen::RowVector3d::Zero();

	Eigen::MatrixXd X, P, N;
	Eigen::VectorXd D;

	random_points_on_mesh(num_samples, VX, FX, X);

	point_mesh_distance(X, VY, FY, D, P, N);

	// point-to-plane
	point_to_plane_rigid_matching(X, P, N, R, t);

	return D.sum()/D.rows();
}