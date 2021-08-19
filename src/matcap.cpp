//#include <igl/read_triangle_mesh.h>
//#include <igl/png/readPNG.h>
//#include <igl/opengl/glfw/Viewer.h>
//#include <igl/opengl/create_shader_program.h>
//#include <igl/opengl/destroy_shader_program.h>
//#include <Eigen/Core>
//
//int main(int argc, char *argv[])
//{
//	using namespace Eigen;
//	using namespace std;
//	Eigen::MatrixXd V;
//	Eigen::MatrixXi F;
//	igl::opengl::glfw::Viewer v;
//	igl::read_triangle_mesh(argv[1], V, F);
//	Eigen::Matrix<unsigned char, Eigen::Dynamic, Eigen::Dynamic> R, G, B, A;
//	igl::png::readPNG(argv[2], R, G, B, A);
//
//	v.data().set_mesh(V, F);
//	v.data().set_texture(R, G, B, A);
//	v.data().set_face_based(false);
//	v.data().show_lines = false;
//	v.data().show_texture = true;
//	v.launch_init(true, false);
//
//	v.data().meshgl.init();
//	igl::opengl::destroy_shader_program(v.data().meshgl.shader_mesh);
//
//	{
//		std::string mesh_vertex_shader_string =
//			R"(#version 150
//uniform mat4 view;
//uniform mat4 proj;
//uniform mat4 normal_matrix;
//in vec3 position;
//in vec3 normal;
//out vec3 normal_eye;
//
//void main()
//{
//  normal_eye = normalize(vec3 (normal_matrix * vec4 (normal, 0.0)));
//  gl_Position = proj * view * vec4(position, 1.0);
//})";
//
//		std::string mesh_fragment_shader_string =
//			R"(#version 150
//in vec3 normal_eye;
//out vec4 outColor;
//uniform sampler2D tex;
//void main()
//{
//  vec2 uv = normalize(normal_eye).xy * 0.5 + 0.5;
//  outColor = texture(tex, uv);
//})";
//
//		igl::opengl::create_shader_program(
//			mesh_vertex_shader_string,
//			mesh_fragment_shader_string,
//			{},
//			v.data().meshgl.shader_mesh);
//	}
//
//	v.launch_rendering(true);
//	v.launch_shut();
//}