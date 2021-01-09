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

class Heightmap
{
public:
	struct HillAlgorithmParameters
	{
		HillAlgorithmParameters(int rows, int columns, int numHills, int hillRadiusMin, int hillRadiusMax, float hillMinHeight, float hillMaxHeight)
		{
			this->rows = rows;
			this->columns = columns;
			this->numHills = numHills;
			this->hillRadiusMin = hillRadiusMin;
			this->hillRadiusMax = hillRadiusMax;
			this->hillMinHeight = hillMinHeight;
			this->hillMaxHeight = hillMaxHeight;
		}

		int rows;
		int columns;
		int numHills;
		int hillRadiusMin;
		int hillRadiusMax;
		float hillMinHeight;
		float hillMaxHeight;
	};

	Heightmap(const HillAlgorithmParameters& params, bool withPositions = true, bool withTextureCoordinates = true, bool withNormals = true);

	void createFromHeightData(const std::vector<std::vector<float>> &heightData);

	void render();
	void renderPoints();

	int getRows() const;
	int getColumns() const;
	float getHeight(const int row, const int column) const;

	static std::vector<std::vector<float>> generateRandomHeightData(const HillAlgorithmParameters& params);

private:
	void setUpVertices();
	void setUpTextureCoordinates();
	void setUpNormals();
	void setUpIndexBuffer();
	int _columns = 0;
	int _rows = 0;
	unsigned int _vao, _vbo;
	int _numVertices;
	bool _isInitialized;

	std::vector<std::vector<float>> _heightData;
	std::vector<std::vector<glm::vec3>> _vertices;
	std::vector<std::vector<float>> _textureCoordinates;
	std::vector<std::vector<float>> _normals;
};