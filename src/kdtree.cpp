
#include "kdtree.h"

BBox::BBox()
{
}

BBox::~BBox()
{
}

BBox BBox::BB_trim_left(int curr_channel, Eigen::Vector3d point) {
	BBox left;
	for (int c = 0; c < 3; c++)
	{
		left.min(c) = this->min(c);
		left.max(c) = this->max(c);
	}
	left.max(curr_channel) = point(curr_channel);

	return left;
}

BBox BBox::BB_trim_right(int curr_channel, Eigen::Vector3d point) {

	BBox right;

	for (int c = 0; c < 3; c++)
	{
		right.min(c) = this->min(c);
		right.max(c) = this->max(c);
	}
	right.min(curr_channel) = point(curr_channel);

	return right;
}

KDNode::KDNode(Eigen::Vector3d x = Eigen::Vector3d(0, 0, 0))
{
	left = NULL;
	right = NULL;

	used = false;

	data = x;

	ind = 0;
}

KDNode::~KDNode()
{
}

void freeResource(KDNode* node) {
	if (node == NULL) { return; }
	freeResource(node->left);
	freeResource(node->right);
	delete node;
	return;
}

KDTree::KDTree()
{
	best_point = Eigen::Vector3d(0, 0, 0);
	best_dist = FLT_MAX;
	//bb = NULL;
	root = NULL;
	node_number = 0;
}

KDTree::~KDTree()
{
	freeResource(root);
}

void KDTree::load_points(Eigen::MatrixXd input) {

	points = input;
	//bb = new BBox();
	for (int c = 0; c < 3; c++)
	{
		bb.max[c] = input.col(c).maxCoeff();
		bb.min[c] = input.col(c).minCoeff();
	}
}

void KDTree::load_points_from_vector(vector<Eigen::Vector3d> input) {
	points_array.clear();
	points_array = input;
}

KDNode* KDTree::insert(Eigen::Vector3d x, KDNode *node, int curr_channel) {

	if (node == NULL)
	{
		node = new KDNode(x);
		node_number++;
	}
	else if (x == node->data)
	{
		cout << "[KDTree] Found duplicate data." << endl;
		return NULL;
	}
	else if (x(curr_channel) < node->data(curr_channel))
	{
		node->left = insert(x, node->left, (curr_channel + 1) % 3);
	}
	else
	{
		node->right = insert(x, node->right, (curr_channel + 1) % 3);
	}
	return node;
}

void KDTree::search_next(int curr_channel, Eigen::MatrixXd input) {

	int num = input.rows();
	float mid_value = 0;
	int mid_ind = 0;
	if (num == 0)
	{
		return;
	}
	Eigen::VectorXd col = input.col(curr_channel);
	std::sort(col.data(), col.data() + col.size());
	mid_value = col(num / 2);
	for (int r = 0; r < num; r++)
	{
		if (input(r, curr_channel) == mid_value)
		{
			mid_ind = r;
			break;
		}
	}

	Eigen::Vector3d mid_point = input.row(mid_ind);
	if (num == points.rows())
	{
		root = insert(mid_point, root, 0);
	}
	else
	{
		insert(mid_point, root, 0);
	}
	search_next((curr_channel + 1) % 3, input.block(0, 0, mid_ind, 3));
	search_next((curr_channel + 1) % 3, input.block(mid_ind + 1, 0, num - mid_ind - 1, 3));

}

KDNode* KDTree::build_tree(int curr_channel, std::vector < Eigen::Vector3d>::iterator start, std::vector < Eigen::Vector3d>::iterator end) {
	if (start >= end)
	{
		return NULL;
	}

	auto cmp = [curr_channel](const Eigen::Vector3d& p1, const Eigen::Vector3d& p2) {
		return p1(curr_channel) < p2(curr_channel);
	};

	std::size_t len = end - start;
	auto mid = start + len / 2;

	std::nth_element(start, mid, end, cmp);

	while (mid > start && (*(mid - 1))(curr_channel) == (*mid)(curr_channel)) {
        --mid;
    }

	KDNode* newNode = new KDNode((*mid));
	
	newNode->left = build_tree((curr_channel + 1) % 3, start, mid);
	newNode->right = build_tree((curr_channel + 1) % 3, mid + 1, end);

	return newNode;
}

void KDTree::construct_tree() {
	root = build_tree(0, points_array.begin(), points_array.end());
}

float KDTree::dist_bb(Eigen::Vector3d query, BBox bb) {
	float d = 0;
	float d_c, d_min, d_max;
	for (int c = 0; c < 3; c++)
	{
		d_max = query(c) - bb.max(c);
		d_min = bb.min(c) - query(c);

		// max ( d_max, 0, d_min )
		d_c = d_max > 0 ? d_max : 0;
		d_c = d_c > d_min ? d_c : d_min;

		d += d_c * d_c;

	}
	return sqrt(d);
}

void KDTree::NN(Eigen::Vector3d query, KDNode* node, int curr_channel, BBox bb) {

	if ((node == NULL) || (dist_bb(query, bb) > best_dist))
	{
		return;
	}

	float dist = (query - node->data).norm();

	if (dist < best_dist)
	{
		best_dist = dist;
		best_point = node->data;
	}

	if (query(curr_channel) < node->data(curr_channel))
	{
		NN(query, node->left, (curr_channel + 1) % 3, bb.BB_trim_left(curr_channel, node->data));
		NN(query, node->right, (curr_channel + 1) % 3, bb.BB_trim_right(curr_channel, node->data));
	}
	else
	{
		NN(query, node->right, (curr_channel + 1) % 3, bb.BB_trim_right(curr_channel, node->data));
		NN(query, node->left, (curr_channel + 1) % 3, bb.BB_trim_left(curr_channel, node->data));

	}
}

Eigen::Vector3d KDTree::find_NN(Eigen::Vector3d query, float &dist) {

	best_dist = FLT_MAX;
	best_point = Eigen::Vector3d(0, 0, 0);
	NN(query, root, 0, bb);

	dist = best_dist;
	return best_point;
}

void KDTree::NN_range(Eigen::Vector3d query, KDNode* node, int curr_channel, double range) {

	if (node == NULL)
	{
		return;
	}

	float dist = (query - node->data).squaredNorm();

	if (dist < range*range)
	{
		query_results.push_back(node->data);
	}

	// if the plane intersects with sphere
	if (
		(node->data(curr_channel) < query(curr_channel) + range)
		&&
		(node->data(curr_channel) > query(curr_channel) - range)

		)
	{
		NN_range(query, node->left, (curr_channel + 1) % 3, range);
		NN_range(query, node->right, (curr_channel + 1) % 3, range);
	}
	// else, determine which side
	else {
		if (query(curr_channel) < node->data(curr_channel))
		{
			NN_range(query, node->left, (curr_channel + 1) % 3, range);
		}
		else
		{
			NN_range(query, node->right, (curr_channel + 1) % 3, range);

		}
	}
}

std::vector<Eigen::Vector3d> KDTree::find_NN_range(Eigen::Vector3d query, double range) {

	query_results.clear();

	NN_range(query, root, 0, range);

	return query_results;

}
