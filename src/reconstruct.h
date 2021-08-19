#pragma once

#include <string>


class Reconstruct
{
public:
	Reconstruct();
	~Reconstruct();

	float cvtValue2Normal(unsigned char value);
	float cvtValue2Depth(unsigned char value);
	float cvtValue2Width(float value);
	float cvtValue2Height(float value);

	unsigned char cvtDepth2Value(float z);
	unsigned char cvtWidth2Value(float x);
	unsigned char cvtHeight2Value(float y);

	void generate_poisson_input(std::string f_opc, std::string f_normal_front, std::string f_normal_back, std::string f_depth_front, std::string f_depth_back);
	void generate_point_cloud(std::string f_opc, std::string f_normal_front, std::string f_normal_back, std::string f_depth_front, std::string f_depth_back, bool test = false, std::string f_density = "", float thre=5.);
	
	void generate_poisson_input_sym(std::string f_opc, std::string f_normal_front, std::string f_normal_back, std::string f_depth_front, std::string f_depth_back, std::string f_sym);
	void generate_point_cloud_sym(std::string f_opc, std::string f_normal_front, std::string f_normal_back, std::string f_depth_front, std::string f_depth_back, std::string f_sym);
	void generate_poisson_input_sym_double(std::string f_out, std::string f_out_sym, std::string f_normal_front, std::string f_normal_back, std::string f_depth_front, std::string f_depth_back, std::string f_sym);
	void generate_point_cloud_single(std::string f_opc, std::string f_normal_front, std::string f_depth_front);

	void smoothing(std::string f_out, std::string f_in);



private:

};

