#pragma once
#include "Mesh.h"
#include "Spider.h"
#include "Shader.h"
#include <string>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <fstream>
#include <iostream>
#include <sstream>

Mesh m;

Spider::Spider() {};

Spider::Spider(vec3 startLoc) {
	initialLocation = startLoc;
};
void Spider::genObject() {
	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.
	std::string matFile = "spiderBits/body.matrix";
	std::ifstream Matfile(matFile);
	if (Matfile.good()) {
		std::string sLine;
		std::getline(Matfile, sLine);
		std::istringstream iss(sLine);
		float x, y, z;
		iss >> x >> y >> z;
		trans[0] = vec3(-x, -z, y);
	}
	Matfile.close();

	mesh_data[0] = m.load_mesh("spiderBits/Body.obj"); //Body

	vp_vbo[0] = 0;

	glGenBuffers(1, &vp_vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, mesh_data[0].mPointCount * sizeof(vec3), &mesh_data[0].mVertices[0], GL_STATIC_DRAW);

	vn_vbo[0] = 0;
	glGenBuffers(1, &vn_vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, mesh_data[0].mPointCount * sizeof(vec3), &mesh_data[0].mNormals[0], GL_STATIC_DRAW);

	vt_vbo[0] = 0;
	//glGenBuffers(1, &vt_vbo[0]);
	//glBindBuffer(GL_ARRAY_BUFFER, vt_vbo[0]);
	//glBufferData(GL_ARRAY_BUFFER, mesh_data[0].mPointCount * sizeof(vec2), &mesh_data[0].mTextureCoords[0], GL_STATIC_DRAW);


	for (int i = 1; i < NUM_LEGS + NUM_FEET + NUM_LEG_JOINTS + 1; i++) {

		char* n = new char[23];
		std::string file = "spiderBits/" + std::to_string(i) + ".obj";
		std::string matFile = "spiderBits/" + std::to_string(i) + ".matrix";
		std::ifstream Matfile(matFile);

		if (Matfile.good()) {
			std::string sLine;
			std::getline(Matfile, sLine);
			std::istringstream iss(sLine);
			float x, y, z;
			iss >> x >> y >> z;
			trans[i] = vec3(-x, -z, y);
		}
		Matfile.close();

		mesh_data[i] = m.load_mesh(file.c_str());

		vp_vbo[i] = 0;
		glGenBuffers(1, &vp_vbo[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, mesh_data[i].mPointCount * sizeof(vec3), &mesh_data[i].mVertices[0], GL_STATIC_DRAW);
		vn_vbo[i] = 0;
		glGenBuffers(1, &vn_vbo[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, mesh_data[i].mPointCount * sizeof(vec3), &mesh_data[i].mNormals[0], GL_STATIC_DRAW);
		vt_vbo[i] = 0;
		//glGenBuffers(1, &vt_vbo[i]);
		//glBindBuffer(GL_ARRAY_BUFFER, vt_vbo[i]);
		//glBufferData(GL_ARRAY_BUFFER, mesh_data[i].mPointCount * sizeof(vec3), &mesh_data[i].mTextureCoords[0], GL_STATIC_DRAW);
	}

	unsigned int vao = 0;
	glBindVertexArray(vao);
}

void Spider::update(float delta) {
	DWORD curr_time = timeGetTime();
	rotate_y = cos(curr_time / 240.0f) * 20.0f;
	//rotate_y = fmodf(rotate_y, 360.0f);
	mid_y = sin(curr_time / 240.0f) * 10.0f;
	sway_y = sin(curr_time / 240.0f) * 20.0f;
	forward_x += delta * 4.0f;
	bob = sin(curr_time / 120.0f) * 0.05f;
}

void Spider::draw(Shader shaderProgram, Camera camera) {
	shaderProgram.use();

	shaderProgram.setVec3("objectColor", vec3(0.2, 0.8, 0.0));
	//shaderProgram.setVec3("lightColor", vec3(1.0f, 1.0f, 1.0f));

	int matrix_location = glGetUniformLocation(shaderProgram.ID, "model");

	GLuint loc1 = glGetAttribLocation(shaderProgram.ID, "vertex_position");
	GLuint loc2 = glGetAttribLocation(shaderProgram.ID, "vertex_normal");
	GLuint loc3 = glGetAttribLocation(shaderProgram.ID, "vertex_texture");

	// Root of the Hierarchy
	mat4 model = identity_mat4();

	model = translate(model, trans[0]);
	model = translate(model, initialLocation);


	model = translate(model, vec3(0.0f, 2.7f - bob, forward_x));

	if (reset) {
		model = translate(model, vec3(0.0f, 0.0f, -forward_x));
		forward_x = 0.0f;
		reset = false;
	}

	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo[0]);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo[0]);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(loc3);
	//glBindBuffer(GL_ARRAY_BUFFER, vt_vbo[0]);
	//glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// update uniforms & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data[0].mPointCount);

	for (int i = 1; i < NUM_LEGS + 1; i++) {

		int LEG_LOWER = i;
		int LEG_MID = i + NUM_LEGS;
		int LEG_UPPER = i + NUM_LEGS + NUM_LEG_JOINTS;

		//Draw Leg
		mat4 upperLegModel = identity_mat4();
		mat4 midLegModel = identity_mat4();
		mat4 lowerLegModel = identity_mat4();

		float upperX = 0;
		float midX = 0;
		float lowerX = 0;
		float initialX = 0;

		if (i % 2 == 1) {
			upperX = initialX + rotate_y;
			midX = (-upperX / 3.0f) + mid_y;
			lowerX = sway_y;
		}
		else if (i % 2 == 0) {
			upperX = initialX - rotate_y;
			midX = (-upperX / 3.0f) - mid_y;
			lowerX = -sway_y;
		}

		upperLegModel = rotate_x_deg(upperLegModel, upperX);
		midLegModel = rotate_x_deg(midLegModel, midX);
		lowerLegModel = rotate_x_deg(lowerLegModel, lowerX);

		upperLegModel = translate(upperLegModel, trans[LEG_UPPER]);
		//First child to body
		upperLegModel = model * upperLegModel;

		midLegModel = translate(midLegModel, trans[LEG_MID] - trans[LEG_UPPER]);
		//First child to the upper leg
		midLegModel = upperLegModel * midLegModel;

		lowerLegModel = translate(lowerLegModel, trans[LEG_LOWER] - trans[LEG_MID]);
		//First child to the mid leg
		lowerLegModel = midLegModel * lowerLegModel;

		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo[LEG_UPPER]);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo[LEG_UPPER]);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		//glEnableVertexAttribArray(loc3);
		//glBindBuffer(GL_ARRAY_BUFFER, vt_vbo[LEG_UPPER]);
		//glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		// Update the appropriate uniform and draw the mesh again


		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, upperLegModel.m);
		glDrawArrays(GL_TRIANGLES, 3, mesh_data[LEG_UPPER].mPointCount);


		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo[LEG_MID]);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo[LEG_MID]);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		//glEnableVertexAttribArray(loc3);
		//glBindBuffer(GL_ARRAY_BUFFER, vt_vbo[LEG_MID]);
		//glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		// Update the appropriate uniform and draw the mesh again

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, midLegModel.m);
		glDrawArrays(GL_TRIANGLES, 3, mesh_data[LEG_MID].mPointCount);



		glEnableVertexAttribArray(loc1);
		glBindBuffer(GL_ARRAY_BUFFER, vp_vbo[LEG_LOWER]);
		glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(loc2);
		glBindBuffer(GL_ARRAY_BUFFER, vn_vbo[LEG_LOWER]);
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		//glEnableVertexAttribArray(loc3);
		//glBindBuffer(GL_ARRAY_BUFFER, vt_vbo[LEG_LOWER]);
		//glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		// Update the appropriate uniform and draw the mesh again

		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, lowerLegModel.m);
		glDrawArrays(GL_TRIANGLES, 3, mesh_data[LEG_LOWER].mPointCount);
	}
}

void Spider::keyPress(unsigned char key, int x, int y) {
	if (key == 'r') {
		reset = true;
	}
}