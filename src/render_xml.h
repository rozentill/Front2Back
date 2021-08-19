#pragma once
#include <iostream>

#include "xml_writer.hh"

using namespace std;

void write_rgb_xml(ostream& os, int x_angle, int y_angle, float x_tran, float y_tran, char* s_model, char* s_envmap) {

	char tmp[256], tmp2[256];
	float distance;

	// set fov(25~65)
	float fov = float(rand()) / float(RAND_MAX);

	fov *= 40;
	fov += 25;

	distance = 2.2 * tan(igl::PI* 20. / 180) / (tan(igl::PI * fov / 360.));

	Writer writer(os);

	writer.openElt("scene").attr("version", "0.6.0");

	writer.openElt("shape").attr("type", "shapenet");

	writer.openElt("string").attr("name", "filename").attr("value", s_model).closeElt();
	writer.openElt("float").attr("name", "maxSmoothAngle").attr("value", "60").closeElt();

	writer.openElt("transform").attr("name", "toWorld");

	sprintf(tmp, "%d", y_angle);
	writer.openElt("rotate").attr("y", "1").attr("angle", tmp).closeElt();
	sprintf(tmp, "%d", x_angle);
	writer.openElt("rotate").attr("x", "1").attr("angle", tmp).closeElt();
	sprintf(tmp, "%.2f", x_tran);
	sprintf(tmp2, "%.2f", y_tran);
	writer.openElt("translate").attr("x", tmp).attr("y", tmp).attr("z", "0").closeElt();

	writer.closeElt();
	writer.closeElt();

	writer.openElt("emitter").attr("type", "envmap").attr("id", "env");
	writer.openElt("string").attr("name", "filename").attr("value", s_envmap).closeElt();
	writer.openElt("float").attr("name", "scale").attr("value", "1.5").closeElt();
	writer.openElt("boolean").attr("name", "cache").attr("value", "false").closeElt();
	writer.closeElt();

	writer.openElt("integrator").attr("type", "path").closeElt();

	writer.openElt("sensor").attr("type", "perspective");
	writer.openElt("transform").attr("name", "toWorld");
	sprintf(tmp, "0, 0, %.2f", distance);
	writer.openElt("lookAt").attr("target", "0, 0, 0").attr("origin", tmp).attr("up", "0, 1, 0").closeElt();
	writer.closeElt();
	sprintf(tmp, "%.2f", fov);
	writer.openElt("float").attr("name", "fov").attr("value", tmp).closeElt();
	writer.openElt("sampler").attr("type", "halton");
	writer.openElt("integer").attr("name", "sampleCount").attr("value", "64").closeElt();
	writer.closeElt();

	writer.openElt("film").attr("type", "ldrfilm");
	writer.openElt("integer").attr("name", "width").attr("value", "256").closeElt();
	writer.openElt("integer").attr("name", "height").attr("value", "256").closeElt();
	writer.openElt("boolean").attr("name", "banner").attr("value", "false").closeElt();

	writer.closeAll();
}


void write_rgb_xml_no_bg(ostream& os, int x_angle, int y_angle, float x_tran, float y_tran, char* s_model, char* s_envmap, int side=1) {

	char tmp[256], tmp2[256];
	float distance;

	// set fov(25~65)
	float fov = float(rand()) / float(RAND_MAX);

	fov *= 40;
	fov += 25;

	distance = 2.2 * tan(igl::PI* 20. / 180) / (tan(igl::PI * fov / 360.));
	distance *= side;

	Writer writer(os);

	writer.openElt("scene").attr("version", "0.6.0");

	writer.openElt("shape").attr("type", "shapenet");

	writer.openElt("string").attr("name", "filename").attr("value", s_model).closeElt();
	writer.openElt("float").attr("name", "maxSmoothAngle").attr("value", "60").closeElt();

	writer.openElt("transform").attr("name", "toWorld");

	sprintf(tmp, "%d", y_angle);
	writer.openElt("rotate").attr("y", "1").attr("angle", tmp).closeElt();
	sprintf(tmp, "%d", x_angle);
	writer.openElt("rotate").attr("x", "1").attr("angle", tmp).closeElt();
	sprintf(tmp, "%.2f", x_tran);
	sprintf(tmp2, "%.2f", y_tran);
	writer.openElt("translate").attr("x", tmp).attr("y", tmp).attr("z", "0").closeElt();

	writer.closeElt();
	writer.closeElt();

	//writer.openElt("emitter").attr("type", "envmap").attr("id", "env");
	//writer.openElt("string").attr("name", "filename").attr("value", s_envmap).closeElt();
	//writer.openElt("float").attr("name", "scale").attr("value", "1.5").closeElt();
	//writer.openElt("boolean").attr("name", "cache").attr("value", "false").closeElt();
	//writer.closeElt();

	writer.openElt("emitter").attr("type", "constant").attr("id", "env");
	writer.openElt("spectrum").attr("name", "radiance").attr("value", "1.0").closeElt();
	writer.closeElt();

	writer.openElt("integrator").attr("type", "path").closeElt();

	writer.openElt("sensor").attr("type", "perspective");
	writer.openElt("transform").attr("name", "toWorld");
	sprintf(tmp, "0, 0, %.2f", distance);
	writer.openElt("lookAt").attr("target", "0, 0, 0").attr("origin", tmp).attr("up", "0, 1, 0").closeElt();
	writer.closeElt();
	sprintf(tmp, "%.2f", fov);
	writer.openElt("float").attr("name", "fov").attr("value", tmp).closeElt();
	writer.openElt("sampler").attr("type", "halton");
	writer.openElt("integer").attr("name", "sampleCount").attr("value", "64").closeElt();
	writer.closeElt();

	writer.openElt("film").attr("type", "ldrfilm");
	writer.openElt("integer").attr("name", "width").attr("value", "256").closeElt();
	writer.openElt("integer").attr("name", "height").attr("value", "256").closeElt();
	writer.openElt("boolean").attr("name", "banner").attr("value", "false").closeElt();

	writer.closeAll();
}