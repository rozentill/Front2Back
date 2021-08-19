// my headers
#include "render.h"
#include "reconstruct.h"
#include "symmetry.h"
#include "eval.h"
#include "register.h"
#include "io.h"
#include "headers.h"
#include "mesh.h"
#include "render_xml.h"
#include "kdtree.h"

//3rd party
#include "xml_writer.hh"
#include "KDTreeVectorOfVectorsAdaptor.h"

#include <sstream>
#include <string>

using namespace std;

void reconstruct() {

	Reconstruct reconstruct;
	int num = 1306;
	char f_n_f[256], f_n_b[256], f_d_f[256], f_d_b[256], f_out[256], set[256], prefix[256], prefix_nosym[256], f_sym[256], s_num[256];
	char f_den[256];

	std::sprintf(prefix, "../learn/results/03001627_137_sym/test_latest/images");
	std::sprintf(prefix_nosym, "../learn/results/03001627_137/test_latest/images");

	for (int i = 0; i < num; i++)
	{

		std::sprintf(s_num, "%d", i);
		//std::sprintf(s_num, "da6cbcac794d377df9111ef49c078dbe");

		//gt test
		//std::sprintf(f_n_f, "%s/front_normal/test/%s.png", prefix, s_num);
		//std::sprintf(f_n_b, "%s/back_normal/test/%s.png", prefix, s_num);
		//std::sprintf(f_d_b, "%s/back_depth/test/%s.png", prefix, s_num);
		//std::sprintf(f_d_f, "%s/front_depth/test/%s.png", prefix, s_num);

		std::sprintf(f_out, "../res/03001627_137_test/pi/%d.npts", i);
		//std::sprintf(f_den, "../res/f2b_gt_sym/pi/%d_density.png", i);
		//std::sprintf(f_sym, "D:/Data/ShapeNetRendering_test_res/03001627/sym/%s.txt", s_num);
		std::sprintf(f_sym, "D:/Data/03001627/sym_old/%s.txt", s_num);

		//reconstruct.generate_poisson_input(f_out, f_n_f, f_n_b, f_d_f, f_d_b);

		std::ifstream fin(f_sym);
		float test;
		fin >> test;
		if (test==0)
		{
			std::sprintf(f_n_f, "%s/%s_real_A.png", prefix_nosym, s_num);
			std::sprintf(f_n_b, "%s/%s_fake_B.png", prefix_nosym, s_num);
			std::sprintf(f_d_b, "%s/%s_fake_B_depth.png", prefix_nosym, s_num);
			std::sprintf(f_d_f, "%s/%s_real_A_depth.png", prefix_nosym, s_num);
			reconstruct.generate_poisson_input(f_out, f_n_f, f_n_b, f_d_f, f_d_b);
		}
		else
		{
			std::sprintf(f_n_f, "%s/%s_real_A.png", prefix, s_num);
			std::sprintf(f_n_b, "%s/%s_fake_B.png", prefix, s_num);
			std::sprintf(f_d_b, "%s/%s_fake_B_depth.png", prefix, s_num);
			std::sprintf(f_d_f, "%s/%s_real_A_depth.png", prefix, s_num);
			reconstruct.generate_poisson_input_sym(f_out, f_n_f, f_n_b, f_d_f, f_d_b, f_sym);
		}
		fin.close();
	}
}

void reconstruct_gt() {

	Reconstruct reconstruct;

	char f_n_f[256], f_n_b[256], f_d_f[256], f_d_b[256], f_out[256], set[256], prefix[256], f_sym[256], s_num[256];

	// for car and multi view
	std::sprintf(prefix, "D:/Data/Front2Back/03001627_test");

	for (int i = 0; i < 200; i++)
	{
		//std::sprintf(s_num, "%d", i);

		std::sprintf(f_n_f, "%s/%d_normal.png", prefix, i);
		std::sprintf(f_n_b, "%s/%d_normal_back.png", prefix, i);
		std::sprintf(f_d_b, "%s/%d_depth_back.png", prefix, i);
		std::sprintf(f_d_f, "%s/%d_depth.png", prefix, i);

		std::sprintf(f_out, "../res/f2b_test_gt/pc/%d_back.obj", i);

		//reconstruct.generate_poisson_input(f_out, f_n_f, f_n_b, f_d_f, f_d_b);
		reconstruct.generate_point_cloud(f_out, f_n_f, f_n_b, f_d_f, f_d_b);
	}
}


void reconstruct_batch() {

	Reconstruct reconstruct;

	char f_n_f[256], f_n_b[256], f_d_f[256], f_d_b[256], f_out[256],f_out_sym[256], set[256], prefix[256], prefix_nosym[256], f_sym[256], s_num[256];
	char f_den[256], cls_name[256], sym_prefix[256], res_dir[256], ref_prefix[256], ref_dir[256];
	char f_item[256];
	// for car and multi view
	
	std::sprintf(res_dir, "D:/Data/ShapeNetRendering_test_res");
	std::sprintf(ref_prefix, "D:/Data/ShapeNet_test/input");
	//std::sprintf()
	string cls_set[13] = {
		"02691156",
		"02828884",
		"02958343",
		"03001627",
		"02933112",
		"03211117",
		"03636649",
		"03691459",
		"04090263",
		"04256520",
		"04379243",
		"04401088",
		"04530566"
	};
	//std::vector<std::string> cls_set;
	//cls_set = get_directories(res_dir);

	//std::vector<std::string> v;
	for (int c = 0; c < 13; c++)
	{
		std::sprintf(cls_name, "%s", cls_set[c].c_str());
		std::cout << cls_name << std::endl;
		//std::sprintf(cls_name, "03001627");
		std::sprintf(sym_prefix, "%s/03001627_137_%s/sym", res_dir, cls_name);

		//std::sprintf(prefix, "../learn/results/%s_256_sym/test_latest/images", cls_name);
		//std::sprintf(prefix_nosym, "../learn/results/%s_256/test_latest/images", cls_name);

		std::sprintf(prefix, "../learn/results/03001627_137_sym/test_latest/images_%s_class", cls_name);
		std::sprintf(prefix_nosym, "../learn/results/03001627_137/test_latest/images_%s_class", cls_name);
		std::sprintf(ref_dir, "%s/%s", ref_prefix, cls_name);
		std::vector<std::string> items;
		items = get_all_files(ref_dir);
		std::vector<std::string> v;
		for (int i = 0; i < items.size(); i++)
		{

			Symmetry symmetry;
			SplitString(items[i], v, "_");
			sprintf(f_item, "%s", v[0].c_str());
			//sprintf(f_item, "%d", i);

			std::sprintf(f_out, "../res/03001627_137_%s/pi/%s.npts", cls_name, f_item);

			std::sprintf(f_sym, "%s/%s.txt", sym_prefix, f_item);
			if (!std::experimental::filesystem::exists(f_sym))
			{
				continue;
			}
			std::ifstream fin(f_sym);
			float test;
			fin >> test;
			if (test == 0)//no sym, now skip
			{
				//continue;
				std::sprintf(f_n_f, "%s/%s_real_A.png", prefix_nosym, f_item);
				std::sprintf(f_n_b, "%s/%s_fake_B.png", prefix_nosym, f_item);
				std::sprintf(f_d_b, "%s/%s_fake_B_depth.png", prefix_nosym, f_item);
				std::sprintf(f_d_f, "%s/%s_real_A_depth.png", prefix_nosym, f_item);
				reconstruct.generate_poisson_input(f_out, f_n_f, f_n_b, f_d_f, f_d_b);
			}
			else
			{
				//front only
				std::sprintf(f_n_f, "%s/%s_real_A.png", prefix, f_item);
				std::sprintf(f_n_b, "%s/%s_fake_B.png", prefix, f_item);
				std::sprintf(f_d_b, "%s/%s_fake_B_depth.png", prefix, f_item);
				std::sprintf(f_d_f, "%s/%s_real_A_depth.png", prefix, f_item);
				
				//std::sprintf(f_out, "../res/%s_137/pi/%s_sym.npts", cls_name, f_item);
				reconstruct.generate_poisson_input_sym(f_out, f_n_f, f_n_b, f_d_f, f_d_b, f_sym);

				//std::sprintf(f_n_f, "%s/%s_real_A.png", prefix_nosym, f_item);
				//std::sprintf(f_n_b, "%s/%s_fake_B.png", prefix_nosym, f_item);
				//std::sprintf(f_d_b, "%s/%s_fake_B_depth.png", prefix_nosym, f_item);
				//std::sprintf(f_d_f, "%s/%s_real_A_depth.png", prefix_nosym, f_item);
				//std::sprintf(f_out, "../res/%s_137/pi/%s_nosym.npts", cls_name, f_item);
				//reconstruct.generate_poisson_input(f_out, f_n_f, f_n_b, f_d_f, f_d_b);

				//std::sprintf(f_n_f, "%s/%s_real_A.png", prefix, v[0].c_str());
				//std::sprintf(f_n_b, "%s/%s_fake_B.png", prefix, v[0].c_str());
				//std::sprintf(f_d_b, "%s/%s_fake_B_depth.png", prefix, v[0].c_str());
				//std::sprintf(f_d_f, "%s/%s_real_A_depth.png", prefix, v[0].c_str());

				//std::sprintf(f_out, "../res/%s_137/pi/%s_sym_predict.npts", cls_name, v[0].c_str());
				//reconstruct.generate_poisson_input(f_out, f_n_f, f_n_b, f_d_f, f_d_b);

				//std::sprintf(f_n_f, "%s/%s_real_A.png", prefix, v[0].c_str());
				//std::sprintf(f_n_b, "%s/%s_fake_B.png", prefix, v[0].c_str());
				//std::sprintf(f_d_b, "%s/%s_fake_B_depth.png", prefix, v[0].c_str());
				//std::sprintf(f_d_f, "%s/%s_real_A_depth.png", prefix, v[0].c_str());
				////std::sprintf(f_out, "../res/f2b_real2inter_test_small/pc/%d_shift_back.obj", i);
				////reconstruct.generate_point_cloud_sym(f_out, f_n_f, f_n_b, f_d_f, f_d_b, f_sym);
				////std::sprintf(f_out, "../res/f2b_real2inter_test_small/pc/%d.obj", i);
				//std::sprintf(f_out, "../res/%s_137/pi/%s_sym_recon.npts", cls_name, v[0].c_str());
				//std::sprintf(f_out_sym, "../res/%s_137/pi/%s_sym_recon_sym.npts", cls_name, v[0].c_str());
				//reconstruct.generate_poisson_input_sym_double(f_out, f_out_sym, f_n_f, f_n_b, f_d_f, f_d_b, f_sym);

				//std::sprintf(f_n_f, "%s/%s_real_A.png", prefix_nosym, v[0].c_str());
				//std::sprintf(f_n_b, "%s/%s_fake_B.png", prefix_nosym, v[0].c_str());
				//std::sprintf(f_d_b, "%s/%s_fake_B_depth.png", prefix_nosym, v[0].c_str());
				//std::sprintf(f_d_f, "%s/%s_real_A_depth.png", prefix_nosym, v[0].c_str());
				////std::sprintf(f_out, "../res/f2b_real2inter_test_small/pc/%d_shift_back.obj", i);
				////reconstruct.generate_point_cloud_sym(f_out, f_n_f, f_n_b, f_d_f, f_d_b, f_sym);
				////std::sprintf(f_out, "../res/f2b_real2inter_test_small/pc/%d.obj", i);
				//std::sprintf(f_out, "../res/%s_137/pi/%s_nosym_predict.npts", cls_name, v[0].c_str());
				//std::sprintf(f_out_sym, "../res/%s_137/pi/%s_nosym_predict_sym.npts", cls_name, v[0].c_str());
				//reconstruct.generate_poisson_input_sym_double(f_out, f_out_sym,f_n_f, f_n_b, f_d_f, f_d_b, f_sym);

			}
			fin.close();
		}
	}
}

// every time render, need to take care of :
// ViewerCore:
//	- camera position
// Render.cpp :
//  - offset
//	- normalization
// main:
//	- input path
//	- output paths
//	- whether use rotation or not(true or false)
void render_image() {

	srand(static_cast<unsigned int>(clock()));//for random view generation

	float resolution = 137;

	char f_in[256], f_out[256], set[256], f_rot[256], input_prefix[256], output_prefix[256];
	char f_in_1[256], f_in_2[256], f_out_1[256], f_out_2[256];

	std::sprintf(input_prefix, "../data/model/model_chair");
	std::sprintf(output_prefix, "../data/render/ortho_chair_sym_small");

	Render render(resolution, true, false, 1.4);
	float rand_scale = 0;
	for (int i = 1318; i < 5930; i++)
	{
		std::sprintf(set, "train");

		std::sprintf(f_in, "%s/%s/%d.obj", input_prefix, set, i);

		for (int j = 0; j < 5; j++)
		{

			render.view_generation(60, 360, (int)(2*((j+i)%2-0.5f)));
			//render.translation_generation();

			///rotate original model
			//std::sprintf(f_out, "../data/rot_model/rot_model_bench/%s/%d.obj", set, i);
			//render.rotate_model(f_in, f_out);

			///render normal and depth map
			rand_scale = 0.65 + 0.35*float(rand()) / float(RAND_MAX);
			rand_scale = 0.8/ rand_scale;
			render.input_model(f_in, rand_scale);

			////front
			std::sprintf(f_out, "%s/front_normal/%s/%d_%d.png", output_prefix, set, i,j);
			render.render_normal(f_in, f_out, 1, true);//false if using rotated model input

			//back
			std::sprintf(f_out, "%s/back_normal/%s/%d_%d.png", output_prefix, set, i,j);
			render.render_normal(f_in, f_out, -1, true);

			//front
			std::sprintf(f_out, "%s/front_depth/%s/%d_%d.png", output_prefix, set, i,j);
			render.render_depth(f_in, f_out, 1, true);

			//back
			std::sprintf(f_out, "%s/back_depth/%s/%d_%d.png", output_prefix, set, i,j);
			render.render_depth(f_in, f_out, -1, true);

			//sym
			std::sprintf(f_out_1, "%s/sym_depth/%s/%d_%d.png", output_prefix, set, i, j);
			std::sprintf(f_out_2, "%s/sym_normal/%s/%d_%d.png", output_prefix, set, i, j);
			std::sprintf(f_in_1, "%s/front_depth/%s/%d_%d.png", output_prefix, set, i, j);
			std::sprintf(f_in_2, "%s/front_normal/%s/%d_%d.png", output_prefix, set, i, j);
			render.render_sym(f_out_1, f_out_2, f_in_1, f_in_2);
		}

	}

}

void render_perspective() {

	srand(static_cast<unsigned int>(clock()));//for random view generation

	float resolution = 256;

	char f_in[256], f_out[256], set[256], f_rot[256], input_prefix[256], output_prefix[256];

	std::sprintf(input_prefix, "../data/model/model_chair");
	std::sprintf(output_prefix, "../data/render/persp_ortho_sil");

	Render render_persp(resolution, false);
	Render render_ortho(resolution, true);

	Eigen::MatrixXd rot_x, rot_y;
	Eigen::RowVector3d tran;

	double scale_persp = 1.0, scale_ortho = 1.0;
	for (int i = 1318; i < 5930; i++)
	{
		std::sprintf(set, "train");

		std::sprintf(f_in, "%s/%s/%d.obj", input_prefix, set, i);

		for (int j = 0; j < 5; j++)
		{

			render_persp.view_generation(30, 360, (int)(2*((j+i)%2-0.5f)));
			render_persp.translation_generation();

			//record transformation
			rot_x = render_persp.get_rotation_x();
			rot_y = render_persp.get_rotation_y();
			tran = render_persp.get_translation();

			///render normal and depth map
			render_persp.input_model(f_in, scale_persp);

			////front
			std::sprintf(f_out, "%s/persp/front_normal/%s/%d_%d.png", output_prefix, set, i, j);
			render_persp.render_normal(f_in, f_out, 1, true);//false if using rotated model input

			//front
			std::sprintf(f_out, "%s/persp/front_depth/%s/%d_%d.png", output_prefix, set, i, j);
			render_persp.render_depth(f_in, f_out, 1, true, 0);

			std::sprintf(f_out, "%s/persp/front_silhouette/%s/%d_%d.png", output_prefix, set, i, j);
			render_persp.render_silhouette(f_in, f_out, 1, true);
			///* For orthographic */


			render_ortho.set_transformation(rot_x, rot_y, tran);
			//render_ortho.view_generation(30, 360, (int)(2 * ((j + i) % 2 - 0.5f)));
			//render_ortho.translation_generation();

			render_ortho.input_model(f_in, scale_ortho);

			////front
			std::sprintf(f_out, "%s/ortho/front_normal/%s/%d_%d.png", output_prefix, set, i, j);
			render_ortho.render_normal(f_in, f_out, 1, true);//false if using rotated model input

			//std::sprintf(f_out, "%s/ortho/back_normal/%s/%d_%d.png", output_prefix, set, i, j);
			//render_ortho.render_normal(f_in, f_out, -1, true);
															 //front
			std::sprintf(f_out, "%s/ortho/front_depth/%s/%d_%d.png", output_prefix, set, i, j);
			render_ortho.render_depth(f_in, f_out, 1, true);

			std::sprintf(f_out, "%s/ortho/front_silhouette/%s/%d_%d.png", output_prefix, set, i, j);
			render_ortho.render_silhouette(f_in, f_out, 1, true);
			//////back
			//std::sprintf(f_out, "%s/ortho/back_depth/%s/%d_%d.png", output_prefix, set, i, j);
			//render_ortho.render_depth(f_in, f_out, -1, true);


		}

	}

}

void render_compare() {

	float resolution = 256;

	char f_in[256], f_out[256], set[256], f_rot[256], input_prefix[256], output_prefix[256];

	Render render_ours(256, true);
	Render render_theirs(480, true);
	
	Eigen::MatrixXd rot_x, rot_y;
	Eigen::RowVector3d tran;

	char shapenet_dir[1000], shapehd_dir[1000], src[1000], tar[1000], out_dir[1000], rot[256];
	char cls[256];
	std::sprintf(cls, "02958343");
	std::sprintf(shapenet_dir, "D:/Data/ShapeNetCore.v2/ShapeNetCore.v2/%s", cls);
	std::sprintf(shapehd_dir, "D:/Project/GenRe-ShapeHD/downloads/data/shapenet/%s", cls);
	std::sprintf(output_prefix, "../data/compare/%s", cls);

	std::vector<std::string> items;

	items = get_directories(shapenet_dir);

	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 1; j++)
		{

			//std::cout << items[i] << std::endl;
			std::sprintf(src, "%s/%s/models/model_normalized.obj", shapenet_dir, items[i].c_str());
			//std::cout << src << std::endl;

			render_ours.view_generation(30, 360, (int)(2 * ((i + j) % 2 - 0.5f)));
			render_ours.translation_generation();
			//record transformation
			rot_x = render_ours.get_rotation_x();
			rot_y = render_ours.get_rotation_y();
			tran = render_ours.get_translation();


			render_ours.input_model(src, 1.0);
			////front
			std::sprintf(f_out, "%s/ours/front_normal/test/%d.png", output_prefix, i);
			render_ours.render_normal(f_in, f_out, 1, true);//false if using rotated model input

													   //back
			std::sprintf(f_out, "%s/ours/back_normal/test/%d.png", output_prefix, i);
			render_ours.render_normal(f_in, f_out, -1, true);

			//front
			std::sprintf(f_out, "%s/ours/front_depth/test/%d.png", output_prefix, i);
			render_ours.render_depth(f_in, f_out, 1, true);

			//back
			std::sprintf(f_out, "%s/ours/back_depth/test/%d.png", output_prefix, i);
			render_ours.render_depth(f_in, f_out, -1, true);

			// rot model
			std::sprintf(rot, "%s/ours/rot_model/%d.obj", output_prefix, i);
			render_ours.rotate_model(src, rot);

			/*theirs*/

			render_theirs.set_transformation(rot_x, rot_y, tran);

			render_theirs.input_model(src, 1.0);

			std::sprintf(f_out, "%s/theirs/%d_normal.png", output_prefix, i);
			render_theirs.render_normal(f_in, f_out, 1, true);//false if using rotated model input

			std::sprintf(f_out, "%s/theirs/%d_depth.png", output_prefix, i);
			render_theirs.render_depth(f_in, f_out, 1, true);

			std::sprintf(f_out, "%s/theirs/%d_silhouette.png", output_prefix, i);
			render_theirs.render_silhouette(f_in, f_out, 1, true);

			std::sprintf(src, "%s/%s/%s_%s_voxel_normalized_128.mat", shapehd_dir, items[i].c_str(), cls,  items[i].c_str());
			std::sprintf(tar, "%s/theirs/%d_voxel_normalized_128.mat", output_prefix, i);

			std::experimental::filesystem::copy_file(src, tar, std::experimental::filesystem::copy_options::skip_existing);

		}
	}
}

void sym_detection_single_class(int argc, char **argv) {

	char f_n_f[256], f_n_b[256], f_d_f[256], f_d_b[256];
	char f_out[256], set[256], in_prefix[256], out_prefix[256], f_plane[256];
	char o_depth[256], o_normal[256];
	char f_view[256];
	char ref_dir[256], cls_name[256], res_dir[256], item_name[256], ref_prefix[256];

	clock_t begin_time, end_time;

	sprintf(res_dir, "D:/Data/ShapeNetRendering_test_res");
	sprintf(ref_prefix, "D:/Data/ShapeNet_test/input");
	//sprintf(res_dir, "%s", argv[1]);
	//sprintf(ref_prefix, "%s", argv[2]);

	//std::vector<std::string> cls_set;
	//cls_set = get_directories(res_dir);

	std::vector<std::string> v;

	//std::cout << "Now working on "
	std::sprintf(cls_name, "02828884");
	std::sprintf(in_prefix, "%s/03001627_137_%s/processed", res_dir, cls_name);
	std::sprintf(out_prefix, "%s/03001627_137_%s/sym", res_dir, cls_name);
	bool if_found;
	int not_found_num = 0;

	std::sprintf(ref_dir, "%s/%s", ref_prefix, cls_name);
	std::vector<std::string> items;
	items = get_all_files(ref_dir);

	for (int i = 0; i < items.size(); i++)
	{
		Symmetry symmetry;
		SplitString(items[i], v, "_");
		std::sprintf(f_n_f, "%s/%d_normal.png", in_prefix, i);

		//std::sprintf(set, "front_depth");
		std::sprintf(f_d_f, "%s/%d_depth.png", in_prefix, i);

		// input 
		begin_time = clock();

		symmetry.input_maps(f_d_f, f_n_f);
		symmetry.set_sample(1);

		end_time = clock();
		std::cout << "[TIME COST] Input maps and sampling cost " << (double)((end_time - begin_time) / CLOCKS_PER_SEC) << " seconds.\n";

		// initial guess
		begin_time = clock();

		symmetry.vote_symmetry(10);//angle of possible sample pair
		symmetry.mean_shift_cluster_speed(4e-2, 1e-4);

		end_time = clock();
		std::cout << "[TIME COST] Vote and clustering cost " << (double)((end_time - begin_time) / CLOCKS_PER_SEC) << " seconds.\n";

		// visualize and optimization
		begin_time = clock();

		sprintf(f_plane, "%s/%d_initial", out_prefix, i);
		if_found = symmetry.find_best(f_plane);

		if (if_found)
		{
			//float 
			symmetry.optimize_plane(400);
			//symmetry.verify_plane(symmetry.get_best_plane(), );
			std::sprintf(f_plane, "%s/%d_final.png", out_prefix, i);
			symmetry.visualize_best_plane(f_plane, symmetry.get_best_plane());

			end_time = clock();
			std::cout << "[TIME COST] Visualize and optimization cost " << (double)((end_time - begin_time) / CLOCKS_PER_SEC) << " seconds.\n";

		}
		float depth_ratio, outside_ratio, white_ratio;
		symmetry.verify_plane(symmetry.get_best_plane(), outside_ratio, depth_ratio, white_ratio);
		std::cout << "The best ratio is depth: " << depth_ratio << " and outside : " << outside_ratio << " and white: " << white_ratio << std::endl;
		if (depth_ratio > 0.15 || outside_ratio > 0.05 || white_ratio > 0.8)
		{
			if_found = false;
		}


		std::ofstream fout;
		std::sprintf(f_out, "%s/%d.txt", out_prefix, i);
		fout.open(f_out);
		if (if_found)
		{
			fout << symmetry.get_best_plane()(0) << " " << symmetry.get_best_plane()(1) << " " << symmetry.get_best_plane()(2) << " " << symmetry.get_best_plane()(3);
		}
		else
		{
			not_found_num++;
			fout << 0;
		}
		fout.close();

		// generate reflection maps
		begin_time = clock();

		std::sprintf(o_depth, "%s/%d_depth.png", out_prefix, i);
		std::sprintf(o_normal, "%s/%d_normal.png", out_prefix, i);

		symmetry.generate_reflect_map(o_depth, o_normal, if_found);

		end_time = clock();
		std::cout << "[TIME COST] Generation of reflection maps cost " << (double)((end_time - begin_time) / CLOCKS_PER_SEC) << " seconds.\n";

		std::cout << "[TIME COST] Total process cost " << (double)((end_time - begin_time) / CLOCKS_PER_SEC) << " seconds.\n";

	}

	std::cout << "[END] Totally " << not_found_num << " case no good symmetry plane.\n" << std::endl;
	
}



void sym_detection_test(int argc, char **argv) {

	char f_n_f[256], f_n_b[256], f_d_f[256], f_d_b[256];
	char f_out[256], set[256], in_prefix[256], out_prefix[256], f_plane[256];
	char o_depth[256], o_normal[256];
	char f_view[256];
	char ref_dir[256], cls_name[256], res_dir[256], item_name[256], ref_prefix[256];

	clock_t begin_time, end_time;

	sprintf(res_dir, "D:/Data/ShapeNetRendering_test_res");
	sprintf(ref_prefix, "D:/Data/ShapeNet_test/input");
	//sprintf(res_dir, "%s", argv[1]);
	//sprintf(ref_prefix, "%s", argv[2]);
	
	std::vector<std::string> cls_set;
	cls_set = get_directories(res_dir);
	
	std::vector<std::string> v;
	for (int c = 0; c < 1; c++)
	{
		//std::cout << "Now working on " << cls_set[c] << std::endl;
		std::sprintf(cls_name, "04256520");
		std::sprintf(in_prefix, "%s/02828884_137_%s/processed", res_dir, cls_name);
		std::sprintf(out_prefix, "%s/02828884_137_%s/sym", res_dir, cls_name);
		if (!std::experimental::filesystem::exists(in_prefix)) {
			continue;
		}
		bool if_found;
		int not_found_num = 0;

		std::sprintf(ref_dir, "%s/%s", ref_prefix, cls_name);
		std::vector<std::string> items;
		items = get_all_files(ref_dir);

		for (int i = 0; i < items.size(); i++)
		{
			SplitString(items[i], v, "_");
			//v[0] = std::to_string(i);
			std::sprintf(o_depth, "%s/%s_depth.png", out_prefix, v[0].c_str());
			std::sprintf(o_normal, "%s/%s_normal.png", out_prefix, v[0].c_str());
			if (std::experimental::filesystem::exists(o_normal)) {
				continue;
			}

			std::sprintf(f_n_f, "%s/%s_normal.png", in_prefix, v[0].c_str());
			std::sprintf(f_d_f, "%s/%s_depth.png", in_prefix, v[0].c_str());

			if (!std::experimental::filesystem::exists(f_n_f)) {
				continue;
			}

			Symmetry symmetry;
			std::cout << f_n_f << " and " << f_d_f << std::endl;
			// input 
			begin_time = clock();

			symmetry.input_maps(f_d_f, f_n_f);
			symmetry.set_sample(1);

			end_time = clock();
			std::cout << "[TIME COST] Input maps and sampling cost " << (double)((end_time - begin_time) / CLOCKS_PER_SEC) << " seconds.\n";

			// initial guess
			begin_time = clock();

			symmetry.vote_symmetry(10);//angle of possible sample pair
			symmetry.mean_shift_cluster_speed(4e-2, 1e-4);

			end_time = clock();
			std::cout << "[TIME COST] Vote and clustering cost " << (double)((end_time - begin_time) / CLOCKS_PER_SEC) << " seconds.\n";

			// visualize and optimization
			begin_time = clock();

			sprintf(f_plane, "%s/%s_initial", out_prefix, v[0].c_str());
			if_found = symmetry.find_best(f_plane);

			if (if_found)
			{
				//float 
				symmetry.optimize_plane(400);
				//symmetry.verify_plane(symmetry.get_best_plane(), );
				std::sprintf(f_plane, "%s/%s_final.png", out_prefix, v[0].c_str());
				symmetry.visualize_best_plane(f_plane, symmetry.get_best_plane());

				end_time = clock();
				std::cout << "[TIME COST] Visualize and optimization cost " << (double)((end_time - begin_time) / CLOCKS_PER_SEC) << " seconds.\n";

			}	
			float depth_ratio, outside_ratio, white_ratio;
			symmetry.verify_plane(symmetry.get_best_plane(), outside_ratio, depth_ratio, white_ratio);
			std::cout << "The best ratio is depth: " << depth_ratio << " and outside : " << outside_ratio << " and white: " << white_ratio << std::endl;
			if (depth_ratio > 0.15 || outside_ratio > 0.05 || white_ratio > 0.8)
			{
				if_found = false;
			}


			std::ofstream fout;
			std::sprintf(f_out, "%s/%s.txt", out_prefix, v[0].c_str());
			fout.open(f_out);
			if (if_found)
			{
				fout << symmetry.get_best_plane()(0) << " " << symmetry.get_best_plane()(1) << " " << symmetry.get_best_plane()(2) << " " << symmetry.get_best_plane()(3);
			}
			else
			{
				not_found_num++;
				fout << 0;
			}
			fout.close();

			// generate reflection maps
			begin_time = clock();

			

			symmetry.generate_reflect_map(o_depth, o_normal, if_found);

			end_time = clock();
			std::cout << "[TIME COST] Generation of reflection maps cost " << (double)((end_time - begin_time) / CLOCKS_PER_SEC) << " seconds.\n";

			std::cout << "[TIME COST] Total process cost " << (double)((end_time - begin_time) / CLOCKS_PER_SEC) << " seconds.\n";

		}

		std::cout << "[END] Totally " << not_found_num << " case no good symmetry plane.\n" << std::endl;
	}
}


void evaluate() {
	
	char f_src[256], f_dst[256], src_prefix[256], dst_prefix[256], no[256];
	//std::sprintf(src_prefix, "D:/Project/GenRe-ShapeHD/output/test_03001627_shapehd");
	//std::sprintf(dst_prefix, "D:/Project/GenRe-ShapeHD/compare/theirs");
	std::sprintf(src_prefix, "D:/Project/3DFront2Back/3rdparty/poisson/f2b_test");
	std::sprintf(dst_prefix, "D:/Data/Front2Back/03001627_test");
	Eval eval;

	float total = 0.0, total_num = 0, curr =0;

	Eigen::MatrixXd V_src, V_dst;
	Eigen::MatrixXi F_src, F_dst;
	int batch_num = 0;
	for (int i = 0; i < 200; i++)
	{
		batch_num = i;
		if (i/10 == 0)
		{
			batch_num = i * 11;
		}
		else
		{
			batch_num = (i / 10 - 1) * 11 + i % 10 + 1;
		}
		//std::sprintf(no, "%d%d", batch_num / 10, batch_num % 10);

		//std::sprintf(f_src, "%s/batch%04d/%04d_12_pred_voxel.obj", src_prefix, batch_num, batch_num);
		std::sprintf(f_src, "%s/%d_7_1.obj", src_prefix, batch_num);
		std::sprintf(f_dst, "%s/%d_gt.obj", dst_prefix, i);

		//std::cout << f_src << std::endl << f_dst << std::endl;	

		if (!igl::readOBJ(f_src, V_src, F_src))
		{
			continue;
		}
		if (!igl::readOBJ(f_dst, V_dst, F_dst))
		{
			continue;
		}
		

		curr = eval.chamfer_distance_igl(V_src, V_dst, F_src, F_dst);

		//if (curr>=0.002)
		//{
		//	continue;
		//}
		if (curr>1)
		{
			continue;
		}
		total += curr;
		total_num++;

		std::cout << "Current score is " << curr << "and index is " << i << std::endl;
		std::cout << "Final score is " << total/total_num << " and num is " << total_num << std::endl;

	}

	total /= total_num;
	std::cout << "Final score is " << total << " and num is " << total_num << std::endl;
}


void evaluate_test() {

	char f_src[256], f_dst[256], src_prefix[256], dst_prefix[256], src_prefix_poisson[256], no[256];

	std::sprintf(src_prefix, "../compare/chair_test/ours_gt");
	std::sprintf(src_prefix_poisson, "../compare/chair_test/ours_res_new");
	std::sprintf(dst_prefix, "../compare/chair_test/onet");
	Eval eval;

	float total = 0.0, total_num = 0;
	//, curr = 0;
	float s_0, s_1, s_2, s_3, s_4, s_5;
	Eigen::MatrixXd V_src, V_dst;
	Eigen::MatrixXi F_src, F_dst;

	ofstream fout;

	std::sprintf(f_dst, "%s/eval.txt", src_prefix);
	fout.open(f_dst);

	for (int i = 1; i < 101; i++)
	{

		std::cout << "Number " << i << " case.\n";

		
		std::sprintf(f_dst, "%s/%d_gt.obj", dst_prefix, i);
		if (!igl::readOBJ(f_dst, V_dst, F_dst))
		{
			continue;
		}

		std::sprintf(f_src, "%s/%d.png.off", dst_prefix, i);
		if (!igl::readOFF(f_src, V_src, F_src))
		{
			continue;
		}
		s_0 = eval.chamfer_distance_igl(V_src, V_dst, F_src, F_dst);
		std::cout << "Onet CD score is " << s_0 << std::endl;

		std::sprintf(f_dst, "%s/%d.obj", src_prefix, i);
		if (!igl::readOBJ(f_dst, V_dst, F_dst))
		{
			continue;
		}

		std::sprintf(f_src, "%s/%d.obj", src_prefix_poisson, i);
		if (!igl::readOBJ(f_src, V_src, F_src))
		{
			continue;
		}
		s_1 = eval.chamfer_distance_igl(V_src, V_dst, F_src, F_dst);
		std::cout << "Ours CD score is " << s_1 << std::endl;

		fout << i << " " << s_0 << " " << s_1 << " " << s_1 - s_0 << std::endl;
	}
	fout.close();
}



void eval_reconstruct() {//on depth images

	char f_src[256], f_dst[256], src_prefix[256], dst_prefix[256], no[256];

	std::sprintf(src_prefix, "../learn/results/pix2pix_chair_nosym_100/test_latest/images");
	
	float px, py, pz;
	float total = 0.0, total_num = 0, curr = 0;
	int sample_num = 100;
	Eigen::MatrixXd V_fake, V_real, X_fake, X_real;
	X_fake.resize(sample_num, 3);
	X_real.resize(sample_num, 3);
	Reconstruct recon;
	Eval ev;

	Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> perm;

	int ind_rand;
	for (int i = 3; i < 103; i++)
	{
		std::sprintf(f_src, "%s/%d_fake_B_depth.png", src_prefix, i);
		std::sprintf(f_dst, "%s/%d_real_B_depth.png", src_prefix, i);

		cv::Mat depth_fake = cv::imread(f_src);
		cv::Mat depth_real = cv::imread(f_dst);

		V_fake.resize(0, 3);
		V_real.resize(0, 3);

		for (int y = 0; y < depth_fake.rows; y++)
		{
			for (int x = 0; x < depth_fake.cols; x++)
			{
				if (depth_fake.at<cv::Vec3b>(y, x)[0] > 0 && depth_real.at<cv::Vec3b>(y, x)[0] > 0)
				{
					px = recon.cvtValue2Width(x);
					py = recon.cvtValue2Width(y);
					pz = recon.cvtValue2Width(depth_fake.at<cv::Vec3b>(y, x)[0]);

					V_fake.conservativeResize(V_fake.rows() + 1, V_fake.cols());
					V_fake.row(V_fake.rows() - 1) = Eigen::RowVector3d(px, py, pz);

					pz = recon.cvtValue2Width(depth_real.at<cv::Vec3b>(y, x)[0]);

					V_real.conservativeResize(V_real.rows() + 1, V_real.cols());
					V_real.row(V_real.rows() - 1) = Eigen::RowVector3d(px, py, pz);
				}
			}
		}

		std::cout << "Number of points in " << i << " case : " << V_fake.rows() << std::endl;
		perm.resize(V_fake.rows());
		perm.setIdentity();
		std::random_shuffle(perm.indices().data(), perm.indices().data() + perm.indices().size());

		X_fake = (perm * V_fake).block(0, 0, sample_num, 3);

		perm.resize(V_real.rows());
		perm.setIdentity();
		std::random_shuffle(perm.indices().data(), perm.indices().data() + perm.indices().size());

		X_real = (perm * V_real).block(0, 0, sample_num, 3);

		curr = ev.cd(X_fake, X_real);

		total += curr;
		total_num++;

		std::cout << "Current score is " << curr << "and total num is " << total_num << std::endl;

	}
	total /= total_num;
	std::cout << "Final score is " << total << " and num is " << total_num << std::endl;
}

void visualize() {

	Render render(256, true);

	char f_in[256], f_out[256], set[256], f_rot[256], input_prefix[256], output_prefix[256];
	int start = 1, end = 1308;
	std::sprintf(input_prefix, "../3rdparty/poisson/sym_100/obj");
	std::sprintf(output_prefix, "../3rdparty/poisson/sym_100/img");
	//std::sprintf(input_prefix, "../../GenRe-ShapeHD/output/compare/theirs");
	//std::sprintf(output_prefix, "../../GenRe-ShapeHD/output/compare/theirs_img");
	//std::sprintf(input_prefix, "../data/rot_model_chair/test");
	//std::sprintf(output_prefix, "../data/rot_model_chair/img");
	for (int i = start; i < end+1; i++)
	{
		// for ours
		std::sprintf(f_in, "%s/%d_7_1.obj", input_prefix, i);

		if (!render.input_model(f_in, 1.0)) {
			continue;
		}

		std::sprintf(f_out, "%s/%d_front.png", output_prefix, i);
		render.visualize(f_in, f_out, 1);
		
		std::sprintf(f_out, "%s/%d_back.png", output_prefix, i);
		render.visualize(f_in, f_out, -1);

		// for genre
		//std::sprintf(f_in, "%s/batch00%d/00%d_12_pred_voxel.obj", input_prefix, i, i);

		//if (!render.input_model(f_in)) {
		//	continue;
		//}

		//std::sprintf(f_out, "%s/%d_front.png", output_prefix, i);
		//render.visualize(f_in, f_out, 1);

		//std::sprintf(f_out, "%s/%d_back.png", output_prefix, i);
		//render.visualize(f_in, f_out, -1);
	}
}

void register_shapehd() {
	
	Eigen::MatrixXd SV, TV;
	Eigen::MatrixXi SF, TF;
	Eigen::MatrixXd NV;
	Eigen::MatrixXd VX, VY;
	Eigen::MatrixXi FX, FY;

	std::vector<Eigen::MatrixX3d> R_list;
	std::vector<Eigen::RowVector3d> t_list;

	char src_dir[1000], tar_dir[1000], src[1000], tar[1000], out[1000];
	
	std::sprintf(src_dir, "D:/Data/ShapeNetCore.v2/ShapeNetCore.v2/03001627");
	std::sprintf(tar_dir, "D:/Project/GenRe-ShapeHD/downloads/data/shapenet/03001627");
	
	std::vector<std::string> items;
	
	items = get_directories(tar_dir);
	
	for (int i = 0; i < items.size(); i++)
	{
		std::cout << items[i] << std::endl;
		std::sprintf(src, "%s/%s/models/model_normalized.obj", src_dir, items[i].c_str());
		std::cout << src << std::endl;

		for (int v = 0; v < 5; v++)
		{

			std::sprintf(tar, "%s/%s/03001627_%s_view00%d_gt_rotvox_samescale_128.obj", tar_dir, items[i].c_str(), items[i].c_str(), v);
			std::sprintf(out, "%s/%s/03001627_%s_view00%d_gt_original.obj", tar_dir, items[i].c_str(), items[i].c_str(), v);

			igl::readOBJ(src, SV, SF);
			igl::readOBJ(tar, TV, TF);

			int iter = 0;
			int num_samples = 50;
			double err;
			bool not_found=false;
			Eigen::Matrix3d R;
			Eigen::RowVector3d t;

			R_list.clear();
			t_list.clear();

			VX = TV;
			FX = TF;
			VY = SV;
			FY = SF;

			while (true)
			{
				iter++;
				
				err = icp_single_iteration(VX, FX, VY, FY, num_samples, R, t);
				//std::cout << R << std::endl << t << std::endl;
				if (iter % 100 == 0)
				{

					std::cout << "Now " << iter << " iteration.\n";
					std::cout << "Error is " << err << std::endl;
					std::cout << R << std::endl;

				}
				if (iter == 2000)//skip for next
				{
					not_found = true;
					break;
				}
				if (
					abs(R(0, 0) - 1) <= 0.00000001
					&&
					abs(R(1, 1) - 1) <= 0.00000001
					&&
					abs(R(2, 2) - 1) <= 0.00000001
					&&
					err <= 0.008
					)
				{
					break;
				}
				else
				{

					R_list.push_back(R);
					t_list.push_back(t);
					VX = ((VX*R).rowwise() + t);
				}
			}
			if (not_found)
			{
				std::cout << " Not converged, skip " << out << std::endl;
				continue;
			}
			for (int i = R_list.size() - 1; i >= 0; i--)
			{
				SV = SV.rowwise() - t_list[i];
				SV = SV * R_list[i].inverse();
			}

			std::cout << "Finished " << out << std::endl;
			igl::writeOBJ(out, SV, SF);
		}
	}
}

void render_shapehd() {

	float resolution = 256;

	char f_in[256], f_out[256], set[256], f_rot[256], input_prefix[256], output_prefix[256];

	char src_dir[1000], tar_dir[1000], ori_dir[1000], src[1000], tar[1000], out[1000];

	std::sprintf(src_dir, "D:/Data/ShapeNetCore.v2/ShapeNetCore.v2/02958343");
	std::sprintf(tar_dir, "D:/Project/GenRe-ShapeHD/downloads/data/shapenet/02958344");
	std::sprintf(ori_dir, "D:/Project/GenRe-ShapeHD/downloads/data/shapenet/02958343");

	std::vector<std::string> items;

	items = get_directories(ori_dir);

	Render render(resolution, true);

	cv::Mat normal_map(cv::Size(256,256), CV_8UC3), silhou_map(cv::Size(256, 256), CV_8UC3);
	for (int i = 0; i < items.size(); i++)
	{
		std::sprintf(src, "%s/%s/models/model_normalized.obj", src_dir, items[i].c_str());
		std::sprintf(tar, "%s/%s", tar_dir, items[i].c_str());
		mkdir(tar);

		for (int j = 0; j < 20; j++)
		{

			render.view_generation(30, 360, (int)(2 * ((j + i) % 2 - 0.5f)));
			render.translation_generation();

			render.input_model(src, 1.0);

			////front
			std::sprintf(f_out, "%s/02958343_%s_view0%d%d_normal.png", tar, items[i].c_str(), j/10, j%10);
			render.render_normal(src, f_out, 1, true);//false if using rotated model input
			
			std::sprintf(f_out, "%s/02958343_%s_view0%d%d_silhouette.png", tar, items[i].c_str(), j / 10, j % 10);
			render.render_silhouette(src, f_out, 1, true);
			
			std::sprintf(f_out, "%s/02958343_%s_view0%d%d_depth.png", tar, items[i].c_str(), j / 10, j % 10);
			render.render_depth(src, f_out, 1, true);
			
		}
		std::sprintf(f_in, "%s/%s/02958343_%s_voxel_normalized_128.mat",  ori_dir, items[i].c_str(), items[i].c_str());
		std::sprintf(f_out, "%s/%s/02958343_%s_voxel_normalized_128.mat", tar_dir, items[i].c_str(), items[i].c_str());

		std::experimental::filesystem::copy_file(f_in, f_out, std::experimental::filesystem::copy_options::skip_existing);
	}
}

void render_front2back() {
	float resolution = 137;
	float in_x_rot, in_y_rot, our_x_rot, our_y_rot, dist, tmp;
	char shapenet_data[1000], shapehd_data[1000], f2b_data[1000], sun_data[1000], src[1000], tar[1000];
	char f_out[1000], f_env[1000], f_in[1000];
	char cls[256], obj[256];
	char data_3dr2n2[256], metadata[256];
	//traverse model in genre-shapehd dataset and find original model in shapenet
	sprintf(cls, "02691156");
	std::sprintf(shapehd_data, "D:/Project/GenRe-ShapeHD/downloads/data/shapenet/%s", cls);
	std::sprintf(data_3dr2n2, "D:/Data/shapenetrendering_compressed/ShapeNetRendering/ShapeNetRendering/%s", cls);
	std::sprintf(shapenet_data, "D:/Data/ShapeNetCore.v1/%s", cls);
	std::sprintf(f2b_data, "D:/Data/Front2Back/%s", cls);
	//std::sprintf(sun_data, "D:/Data/SUN_bg_Shapenet/chair"); 

	std::vector<std::string> items, bg_items;
	items = get_directories(data_3dr2n2);
	//bg_items = get_all_files(sun_data);

	ofstream fout;
	ifstream fin;
	//sprintf(obj, "1a6f615e8b1b5ae4dbbc9440457e303e");
	Render render(resolution, true, false, 1.4);
	for (int i = 0; i < items.size(); i++)
	{
		std::cout << "Current model is " << items[i].c_str() << std::endl;
		sprintf(obj, "%s", items[i].c_str());
		std::sprintf(src, "%s/%s/model.obj", shapenet_data, obj);
		std::sprintf(tar, "%s/%s", f2b_data, obj);
		std::sprintf(metadata, "%s/%s/rendering/rendering_metadata.txt", data_3dr2n2, obj);
		if (!std::experimental::filesystem::exists(metadata)) {
			continue;
		}
		mkdir(tar);
		fin.open(metadata);
		for (int j = 0; j < 24; j++)
		{

			//std::sprintf(f_out, "%s/%s_%s_view0%d%d_depth_back.png", tar, cls, obj, j / 10, j % 10);
			//if (std::experimental::filesystem::exists(f_out))
			//{
			//	continue;
			//}

			fin >> in_y_rot;
			fin >> in_x_rot;
			fin >> tmp;
			fin >> dist;
			fin >> tmp;

			render.compute_angle_from_param(in_y_rot, in_x_rot, our_y_rot, our_x_rot);
			render.set_view(our_x_rot, -our_y_rot);
			render.input_model(src, 0.8 / dist);

			// 2.5D
			std::sprintf(f_out, "%s/%s_%s_view0%d%d_normal_front.png", tar, cls, obj, j / 10, j % 10);
			render.render_normal(src, f_out, 1, true);//false if using rotated model input


			std::sprintf(f_out, "%s/%s_%s_view0%d%d_normal_back.png", tar, cls, obj, j / 10, j % 10);
			render.render_normal(src, f_out, -1, true);//false if using rotated model input


			std::sprintf(f_out, "%s/%s_%s_view0%d%d_depth_back.png", tar, cls, obj, j / 10, j % 10);
			render.render_depth(src, f_out, -1, true);


		}
		fin.close();
	}
}

void render_real(int argc, char**argv) {

	float resolution = 256;
	float in_x_rot, in_y_rot, our_x_rot, our_y_rot, dist, tmp;
	char shapenet_data[1000], shapehd_data[1000], f2b_data[1000], sun_data[1000], src[1000], tar[1000];
	char f_out[1000], f_env[1000], f_in[1000];
	char f_in_1[256], f_in_2[256], f_out_1[256], f_out_2[256];
	char cls[256], obj[256];
	char data_3dr2n2[256], metadata[256], data_3dr2n2_high[256];

	int num_cls = 6;
	string cls_set[1] = { 
		//"02691156",
		//"02828884",
		//"02933112", 
		//"02958343"
		//"03001627",
		//"03211117", 
		"03636649"
		//,
		//"03691459",
		//"04090263",
		//"04256520", 
		//"04379243",
		//"04401088",
		//"04530566"
	};
	//traverse model in genre-shapehd dataset and find original model in shapenet
	for (int c = 0; c < 1; c++)
	{
		sprintf(cls, "%s", cls_set[c].c_str());

		std::sprintf(data_3dr2n2, "D:/Data/shapenetrendering_compressed/ShapeNetRendering/%s", cls);
		std::sprintf(data_3dr2n2_high, "D:/Data/ShapeNetRendering_high/%s", cls);
		////std::sprintf(data)
		std::sprintf(shapenet_data, "D:/Data/ShapeNetCore.v1/%s", cls);
		std::sprintf(f2b_data, "D:/Data/Front2Back/%s_256", cls);

		//std::sprintf(data_3dr2n2, "%s/%s", argv[1], cls);
		//std::sprintf(shapenet_data, "%s/%s", argv[2], cls);
		//std::sprintf(f2b_data, "%s/%s_256", argv[3], cls);

		//std::sprintf(sun_data, "D:/Data/SUN_bg_Shapenet/chair"); 
		if (!std::experimental::filesystem::exists(f2b_data)) {
			mkdir(f2b_data);
		}
		
		std::vector<std::string> items, bg_items;
		items = get_directories(data_3dr2n2);
		//bg_items = get_all_files(sun_data);

		ofstream fout;
		ifstream fin;
		Render render(resolution, true, false, 1.4);
		for (int i = 0; i < items.size(); i++)
		{
			std::cout << "Current model is " << items[i].c_str() << std::endl;
			sprintf(obj, "%s", items[i].c_str());
			std::sprintf(src, "%s/%s/model.obj", shapenet_data, obj);
			std::sprintf(tar, "%s/%s", f2b_data, obj);
			std::sprintf(metadata, "%s/%s/rendering/rendering_metadata.txt", data_3dr2n2, obj);
			if (!std::experimental::filesystem::exists(metadata)) {
				continue;
			}
			mkdir(tar);
			fin.open(metadata);
			for (int j = 0; j < 24; j++)
			{
				// totally render rgb+silhou+depth+depthback+normal+normalback+symnormal+symdepth = 8 images

				std::sprintf(f_out, "%s/%s_%s_view0%d%d_depth_sym.png", tar, cls, obj, j / 10, j % 10);
				//if (std::experimental::filesystem::exists(f_out))
				//{
				//	continue;
				//}

				//target 
				//std::sprintf(f_in, "%s/%s/rendering/%02d.png", data_3dr2n2, items[i].c_str(), j);
				std::sprintf(f_in, "%s/%s/%d.png", data_3dr2n2_high, items[i].c_str(), j);
				std::sprintf(f_out, "%s/%s_%s_view0%d%d_rgb.png", tar, cls, obj, j / 10, j % 10);
				std::experimental::filesystem::copy(f_in, f_out, std::experimental::filesystem::copy_options::skip_existing);

				//if (std::experimental::filesystem::exists(f_out))
				//{
				//	continue;
				//}

				//std::cout << "Still no png : " << f_out << std::endl;

				//render.view_generation(-90, 360, (int)(2 * ((j + i) % 2 - 0.5f)));
				//render.translation_generation();
				fin >> in_y_rot;
				fin >> in_x_rot;
				fin >> tmp;
				fin >> dist;
				fin >> tmp;

				//std::cout << "Read in y angle " << in_y_rot << " and x angle " << in_x_rot << " and dist " << dist << std::endl;
				//render.set_camera(0, 0, dist * 1.75);
				render.compute_angle_from_param(in_y_rot, in_x_rot, our_y_rot, our_x_rot);
				render.set_view(our_x_rot, -our_y_rot);
				//render.set_translation(0., 0., dist*1.75);
				render.input_model(src, 0.8 / dist);

				// 2.5D
				std::sprintf(f_out, "%s/%s_%s_view0%d%d_normal.png", tar, cls, obj, j / 10, j % 10);
				render.render_normal(src, f_out, 1, true);//false if using rotated model input

				std::sprintf(f_out, "%s/%s_%s_view0%d%d_normal_back.png", tar, cls, obj, j / 10, j % 10);
				render.render_normal(src, f_out, -1, true);//false if using rotated model input


				std::sprintf(f_out, "%s/%s_%s_view0%d%d_silhouette.png", tar, cls, obj, j / 10, j % 10);
				render.render_silhouette(src, f_out, 1, true);

				std::sprintf(f_out, "%s/%s_%s_view0%d%d_depth.png", tar, cls, obj, j / 10, j % 10);
				render.render_depth(src, f_out, 1, true);

				std::sprintf(f_out, "%s/%s_%s_view0%d%d_depth_back.png", tar, cls, obj, j / 10, j % 10);
				render.render_depth(src, f_out, -1, true);
				// 2D
				//std::sprintf(f_out, "%s/%s_%s_view0%d%d_rgb.xml", tar, cls, obj, j / 10, j % 10);
				//fout.open(f_out);

				//// select a bg
				//float bg_index = float(rand()) / float(RAND_MAX);
				//bg_index *= bg_items.size();
				//sprintf(f_env, "%s/%s", sun_data, bg_items[(int)bg_index].c_str());
				//write_rgb_xml(fout, render.rand_angle_x, render.rand_angle_y, render.rand_tran_x, render.rand_tran_y, src, f_env);

				//fout.close();

				// sym
				std::sprintf(f_out_1, "%s/%s_%s_view0%d%d_depth_sym.png", tar, cls, obj, j / 10, j % 10);
				std::sprintf(f_out_2, "%s/%s_%s_view0%d%d_normal_sym.png", tar, cls, obj, j / 10, j % 10);
				std::sprintf(f_in_1, "%s/%s_%s_view0%d%d_depth.png", tar, cls, obj, j / 10, j % 10);
				std::sprintf(f_in_2, "%s/%s_%s_view0%d%d_normal.png", tar, cls, obj, j / 10, j % 10);
				//std::cout << f_in_1 << std::endl;
				render.render_sym(f_out_1, f_out_2, f_in_1, f_in_2);

			}
			fin.close();
		}
	}
}

void render_real_test() {

	float resolution = 256;

	char shapenet_data[1000], shapehd_data[1000], f2b_data[1000], sun_data[1000], src[1000], tar[1000], f_out[1000], f_env[1000];
	char cls[256];
	char f_in_1[256], f_in_2[256], f_out_1[256], f_out_2[256];

	//traverse model in genre-shapehd dataset and find original model in shapenet
	sprintf(cls, "03001627");
	std::sprintf(shapehd_data, "D:/Project/GenRe-ShapeHD/downloads/data/shapenet/%s", cls);
	std::sprintf(shapenet_data, "D:/Data/ShapeNetCore.v2/ShapeNetCore.v2/%s", cls);
	std::sprintf(f2b_data, "D:/Data/Front2Back/%s_test_2.5D", cls);
	std::sprintf(sun_data, "D:/Data/SUN_bg_Shapenet/chair");

	Render render(resolution, true, true);

	std::vector<std::string> items, bg_items;
	items = get_directories(shapehd_data);
	bg_items = get_all_files(sun_data);

	ofstream fout;

	for (int i = 0; i < 200; i++)
	{
		std::sprintf(src, "%s/%s/models/model_normalized.obj", shapenet_data, items[i].c_str());
		std::sprintf(tar, "%s", f2b_data);
		//mkdir(tar);
		
		//test
		std::cout << src << std::endl;
		continue;
		
		for (int j = 0; j < 1; j++)
		{

			render.set_view(30, 150);
			render.input_model(src, 1.0);

			// 3D
			std::sprintf(f_out, "%s/%d_gt.obj", tar,  i);
			render.rotate_model(src, f_out);

			// 2.5D
			std::sprintf(f_out, "%s/%d_normal.png", tar, i);
			render.render_normal(src, f_out, 1, true);//false if using rotated model input

			std::sprintf(f_out, "%s/%d_normal_back.png", tar, i);
			render.render_normal(src, f_out, -1, true);

			std::sprintf(f_out, "%s/%d_silhouette.png", tar, i);
			render.render_silhouette(src, f_out, 1, true);

			std::sprintf(f_out, "%s/%d_mask.png", tar, i);
			render.render_silhouette(src, f_out, 1, true);

			std::sprintf(f_out, "%s/%d_depth.png", tar, i);
			render.render_depth(src, f_out, 1, true);

			std::sprintf(f_out, "%s/%d_depth_back.png", tar, i);
			render.render_depth(src, f_out, -1, true);

			//sym
			std::sprintf(f_out_1, "%s/%d_sym_depth.png", f2b_data, i);
			std::sprintf(f_out_2, "%s/%d_sym_normal.png", f2b_data, i);
			std::sprintf(f_in_1, "%s/%d_depth.png", f2b_data, i);
			std::sprintf(f_in_2, "%s/%d_normal.png", f2b_data, i);
			render.render_sym(f_out_1, f_out_2, f_in_1, f_in_2);

			// 2D
			std::sprintf(f_out, "%s/%d_rgb.xml", tar, i);
			fout.open(f_out);

			// select a bg
			float bg_index = float(rand()) / float(RAND_MAX);
			bg_index *= bg_items.size();
			sprintf(f_env, "%s/%s", sun_data, bg_items[(int)bg_index].c_str());
			write_rgb_xml_no_bg(fout, render.rand_angle_x, render.rand_angle_y, render.rand_tran_x, render.rand_tran_y, src, f_env, -1);

			fout.close();

			// 3D
			std::sprintf(src, "%s/%s/%s_%s_voxel_normalized_128.mat", shapehd_data, items[i].c_str(), cls, items[i].c_str());
			std::sprintf(tar, "%s/%d_voxel_normalized_128.mat", tar, i);

			std::experimental::filesystem::copy_file(src, tar, std::experimental::filesystem::copy_options::skip_existing);
			
			
		}

	}

}

void kdtree_test() {

	std::vector<Eigen::Vector3d> point_set, nearest_points;
	point_set.push_back(Eigen::Vector3d(0, 0, 0));
	point_set.push_back(Eigen::Vector3d(0, 0, 1));
	point_set.push_back(Eigen::Vector3d(0, 0, -1));
	point_set.push_back(Eigen::Vector3d(0, 1, 0));
	point_set.push_back(Eigen::Vector3d(0, -1, 0));
	point_set.push_back(Eigen::Vector3d(1, 0, 0));
	point_set.push_back(Eigen::Vector3d(-1, 0, 0));

	typedef KDTreeVectorOfVectorsAdaptor<std::vector<Eigen::Vector3d>, double> kd_tree_t;

	kd_tree_t mat_index(3 /*dim*/, point_set, 10 /* max leaf */);
	mat_index.index->buildIndex();
	std::vector<std::pair<size_t, double> >   ret_matches;
	nanoflann::SearchParams params;

	//nanoflann::KNNResultSet<double> resultSet(num_results);
	double query_pt[3] = { 0.5, 0.5, 0.5 };
	
	int nMatches = mat_index.index->radiusSearch(&query_pt[0], 0.75, ret_matches, params);

	std::cout << "radiusSearch(): radius=" << 1.0 << " -> " << nMatches << " matches\n";

	for (size_t i = 0; i < nMatches; i++)
		std::cout << "idx[" << i << "]=" << ret_matches[i].first << " dist[" << i << "]=" << ret_matches[i].second << endl;
	std::cout << "\n";

	query_pt[0] = -0.5;

	nMatches = mat_index.index->radiusSearch(&query_pt[0], 0.9, ret_matches, params);

	std::cout << "radiusSearch(): radius=" << 1.0 << " -> " << nMatches << " matches\n";

	for (size_t i = 0; i < nMatches; i++)
		std::cout << "idx[" << i << "]=" << ret_matches[i].first << " dist[" << i << "]=" << ret_matches[i].second << endl;
	std::cout << "\n";
}

void create_gt() {

	char src[256], src_param[256], item[256], shapenet_data[256], cls[256], model[256], out[256];
	float x_in, y_in, dist_in, our_y_rot, our_x_rot;
	std::string line;
	std::vector<string> split;

	sprintf(cls, "03001627");
	sprintf(shapenet_data, "D:/Data/ShapeNetCore.v1/%s", cls);
	sprintf(src, "D:\\Data\\ShapeNetRendering_test_res\\03001627");
	sprintf(src_param, "%s/processed/param.txt", src);
	
	ifstream fin;
	fin.open(src_param);
	
	//read name txt		
	Render render(137, true, false, 1.4);
	for (int i = 0; i < 1355; i++)
	{
		std::getline(fin, line);
		SplitString(line, split, " ");
		
		sprintf(model, "%s/%s/model.obj", shapenet_data, split[0].c_str());
		y_in = atof(split[1].c_str());
		x_in = atof(split[2].c_str());
		dist_in = atof(split[3].c_str());
		render.compute_angle_from_param(y_in, x_in, our_y_rot, our_x_rot);
		render.set_view(our_x_rot, -our_y_rot);
		render.input_model(model, 0.8 / dist_in);
		sprintf(out, "%s/gt_norot/%d.obj", src, i + 1);
		render.rotate_model(model, out);
	}

}

void create_gt_batch() {

	char src[256], src_param[256], item[256], shapenet_data[256], cls[256], model[256], out[256];
	char ref_dir[256], dst_dir[256], param_dir[256];
	float x_in, y_in, dist_in, our_y_rot, our_x_rot;
	std::string line;
	std::vector<string> split;

	sprintf(ref_dir, "D:\\Data\\ShapeNet_test\\gt");
	sprintf(dst_dir, "D:/Data/ShapeNet_test/pix_gt");
	sprintf(param_dir, "D:/Data/shapenetrendering_compressed/ShapeNetRendering");

	string cls_set[13] = {
		"02691156",
		"02828884",
		"02933112", 
		"02958343",
		"03001627",
		"03211117", 
		"03636649",
		"03691459",
		"04090263",
		"04256520", 
		"04379243",
		"04401088",
		"04530566"
	};
	
	Render render(137, true, false, 1.4);
	ifstream fin;
	std::vector<std::string> item_list;
	std::vector<std::string> item_full;

	for (int c = 7; c < 13; c++)
	{
		sprintf(cls, "%s", cls_set[c].c_str());

		sprintf(src, "%s\\%s", ref_dir, cls);
		item_list = get_all_files(src);
		std::cout << src << std::endl;;
		for (int i = 0; i < item_list.size(); i++)
		{
			SplitString(item_list[i], item_full, ".");
			sprintf(src_param, "%s/%s/%s/rendering/rendering_metadata.txt", param_dir, cls, item_full[0].c_str());

			//read name txt		
			fin.open(src_param);
			std::getline(fin, line);
			//std::cout << src_param << std::endl;
			SplitString(line, split, " ");
			y_in = atof(split[0].c_str());
			x_in = atof(split[1].c_str());
			dist_in = atof(split[3].c_str());
			render.compute_angle_from_param(y_in, x_in, our_y_rot, our_x_rot);
			render.set_view(our_x_rot, -our_y_rot);
			render.set_translation(0., 0., -dist_in);
			sprintf(model, "%s/%s/%s.obj", ref_dir, cls, item_full[0].c_str());
			render.input_model(model, 0.57);
			sprintf(out, "%s/%s/%s.obj", dst_dir, cls, item_full[0].c_str());
			render.rotate_model(model, out);

			fin.close();
		}
	}
}

void reflect_mesh() {
	
	
	string cls_set[2] = {
		"02691156",
		//"02828884",
		//"02933112",
		//"02958343",
		"03001627"
		//"03211117",
		//"03636649",
		//"03691459",
		//"04090263",
		//"04256520",
		//"04379243",
		//"04401088",
		//"04530566"
	};

	char src_data_dir[256], src_param_dir[256], cls_name[256], src_param[256], model[256], out[256];
	sprintf(cls_name, "%s", cls_set[1].c_str());
	sprintf(src_param_dir, "D:/Data/ShapeNet_test/param/%s", cls_name);
	sprintf(src_data_dir, "../res/%s_137_sym", cls_name);

	Render render(137, true, true);

	std::vector<std::string> item_list;
	std::vector<std::string> item_full;
	item_list = get_all_files(src_param_dir);
	ifstream fin;
	std::string line;
	std::vector<string> split;
	float y_in, x_in, our_y_rot, our_x_rot;
	for (int i = 0; i < item_list.size(); i++)
	{
		SplitString(item_list[i], item_full, ".");
		sprintf(src_param, "%s/%s.txt", src_param_dir, item_full[0].c_str());
		std::cout << item_full[0] << std::endl;
		//read name txt		
		fin.open(src_param);
		std::getline(fin, line);
		//std::cout << src_param << std::endl;
		SplitString(line, split, " ");
		y_in = atof(split[0].c_str());
		x_in = atof(split[1].c_str());
		render.compute_angle_from_param(y_in, x_in, our_y_rot, our_x_rot);
		render.set_view(our_x_rot, -our_y_rot);
		//render.set_translation(0., 0., -dist_in);
		sprintf(model, "%s/%s_sym.obj", src_data_dir, item_full[0].c_str());
		if (!std::experimental::filesystem::exists(model))
		{
			continue;
		}
		render.input_model(model, 1.0);
		sprintf(out, "%s/%s_sym_sym.obj", src_data_dir, item_full[0].c_str());
		render.reflect_model(out);

		sprintf(model, "%s/%s_nosym.obj", src_data_dir, item_full[0].c_str());
		render.input_model(model, 1.0);
		sprintf(out, "%s/%s_nosym_sym.obj", src_data_dir, item_full[0].c_str());
		render.reflect_model(out);

		fin.close();
	}
}

int main(int argc, char *argv[])
{
	//render_real_test();
	//render_real(argc, argv);
	//render_test();
	//render_front2back();
	//render_compare();
	//render_shapehd();
	//render_image();
	//render_perspective();

	//eval_reconstruct();

	//reconstruct();
	reconstruct_batch();
	//reconstruct_gt();
	//register_shapehd();
	//evaluate();
	
	//evaluate_test();
	
	//visualize();
	
	//sym_detection_test(argc, argv);
	
	//sym_detection_single_class(argc, argv);
	
	//compute_visible_percentage();


	return 0;
}
