#pragma once

// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <fstream>
#include <iostream>
#include <sstream>


// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "obj_parser.h"
#include "camera.h"
#include "Mesh.h"
#include "maths_funcs.h"
#include "obj_parser.h"
#include "Shader.h"
#pragma region SimpleTypes



class Spider {
private:
	GLfloat rotate_y = 0.0f;
	GLfloat sway_y = 0.0f;
	GLfloat mid_y = 0.0f;
	GLfloat forward_x = 0.0f;
	GLfloat bob = 0.0f;
	int NUM_LEGS = 8;
	int NUM_LEG_JOINTS = 8;
	int NUM_FEET = 8;

	bool reset = false;
	vec3 initialLocation;

	ModelData* mesh_data = new ModelData[NUM_LEGS + NUM_LEG_JOINTS + NUM_FEET + 1];
	unsigned int* vp_vbo = new unsigned int[NUM_LEGS + NUM_LEG_JOINTS + NUM_FEET + 1];
	unsigned int* vn_vbo = new unsigned int[NUM_LEGS + NUM_LEG_JOINTS + NUM_FEET + 1];
	unsigned int* vt_vbo = new unsigned int[NUM_LEGS + NUM_LEG_JOINTS + NUM_FEET + 1];
	vec3* trans = new vec3[NUM_LEGS + NUM_LEG_JOINTS + NUM_FEET + 1];
public:
	Spider();
	Spider(vec3 startLoc);
	void genObject();
	void draw(Shader shaderProgram, Camera camera);
	void update(float delta);
	void keyPress(unsigned char key, int x, int y);
};