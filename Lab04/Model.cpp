// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <gtc/type_ptr.hpp>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "maths_funcs.h"
#include "Mesh.h"
#include "Model.h"


Model::Model() {};

Model::Model(std::vector<Mesh> meshesVec) {
	meshes = meshesVec;
}

std::vector<Mesh> Model::loadScene(const char* file_name) {
	std::vector<Mesh> meshes;

	const aiScene* scene = aiImportFile(
		file_name,
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	);

	std::string name = std::string(file_name);
	size_t pos = 0;
	
	pos = name.find(".");
	std::string token = name.substr(0, pos);

	vec3 trans = matFile(token.c_str());
	if (!scene) {
		fprintf(stderr, "error reading mesh %s \n", file_name);
	}
	else {
		printf("  %i materials\n", scene->mNumMaterials);
		printf("  %i meshes\n", scene->mNumMeshes);
		printf("  %i node children\n", scene->mRootNode->mNumChildren);
		printf("  %i textures\n", scene->mNumTextures);
		
		for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
			const aiMesh* mesh = scene->mMeshes[m_i];
			printf(mesh->mName.C_Str());
			printf("\n");
			meshes.push_back(Mesh(mesh, trans, token.c_str()));
		}
	}

	aiReleaseImport(scene);
	
	return meshes;
}

vec3 Model::matFile(const char* file_name) {
	vec3 trans = vec3(0.0f, 0.0f, 0.0f);
	std::string matFile = "spiderBits/body.matrix";
	std::ifstream Matfile(matFile);
	if (Matfile.good()) {
		std::string sLine;
		std::getline(Matfile, sLine);
		std::istringstream iss(sLine);
		float x, y, z;
		iss >> x >> y >> z;
		trans = vec3(-x, -z, y);
	}
	Matfile.close();
	return trans;
}

void Model::generateObjectBufferMesh(Shader shaderProgram) {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].generateObjectBufferMesh(shaderProgram);
	}
}

void Model::draw(mat4 transform, GLuint matrix_location, GLuint texture, Shader shaderProgram) {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].draw(transform, matrix_location, texture, shaderProgram);
	}
}
void Model::setMeshes(std::vector<Mesh> newMeshes) {
	meshes = newMeshes;
}