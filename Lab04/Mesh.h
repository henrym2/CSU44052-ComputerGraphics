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
#include "maths_funcs.h"
#include "obj_parser.h"
#include "Shader.h"


typedef struct
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
	std::vector<unsigned int> mIndices;
} ModelData;

class Mesh {
private:
	ModelData modelData;
	GLuint loc1, loc2, loc3;
	unsigned int vp_vbo, vn_vbo, vao, ebo, vt_vbo;

public:
	const char* name;
	vec3 transformMat;
	Mesh();
	Mesh(const aiMesh* mesh, vec3 transformation, const char* name);
	ModelData load_mesh(const char* file_name);
	void draw(mat4 transform, GLuint matrix_location, GLuint texture, Shader shaderProgram);
	void generateObjectBufferMesh(Shader shaderProgram);

};