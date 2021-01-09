#pragma once
//#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "assimp/scene.h"

#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "maths_funcs.h"
#include "Mesh.h"

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
#include "maths_funcs.h"
#include "obj_parser.h"
#include "Shader.h"
class Model {
public:
	//static vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

	std::vector<Mesh> meshes;

	Model();

	Model(std::vector<Mesh> meshesVec);
	//Model(const char* file_name);

	void generateObjectBufferMesh(Shader shaderProgram);

	void draw(mat4 transform, GLuint matrix_location, GLuint texture, Shader shaderProgram);

	void setMeshes(std::vector<Mesh> meshes);

	static std::vector<Mesh> loadScene(const char* file_name);

private:
	static vec3 matFile(const char* file_name);
};

#endif // !MODEL_H