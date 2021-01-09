#include "Heightmap.h"
#include <random>

/*
std::vector<std::vector<float>> Heightmap::generateRandomHeightData(const HillAlgorithmParameters& params)
{
	std::vector<std::vector<float>> heightData(params.rows, std::vector<float>(params.columns, 0.0f));

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> hillRadiusDistribution(params.hillRadiusMin, params.hillRadiusMax);
	std::uniform_real_distribution<> hillHeightDistribution(params.hillMinHeight, params.hillMaxHeight);
	std::uniform_int_distribution<> hillCenterRowIntDistribution(0, params.rows - 1);
	std::uniform_int_distribution<> hillCenterColIntDistribution(0, params.columns - 1);

	for (int i = 0; i < params.numHills; i++)
	{
		const auto hillCenterRow = hillCenterRowIntDistribution(generator);
		const auto hillCenterCol = hillCenterColIntDistribution(generator);
		const auto hillRadius = hillRadiusDistribution(generator);
		const auto hillHeight = hillHeightDistribution(generator);

		for (auto r = hillCenterRow - hillRadius; r < hillCenterRow + hillRadius; r++)
		{
			for (auto c = hillCenterCol - hillRadius; c < hillCenterCol + hillRadius; c++)
			{
				if (r < 0 || r >= params.rows || c < 0 || c >= params.columns) {
					continue;
				}
				const auto r2 = hillRadius * hillRadius; // r*r term
				const auto x2x1 = hillCenterCol - c; // (x2-x1) term
				const auto y2y1 = hillCenterRow - r; // (y2-y1) term
				const auto height = float(r2 - x2x1 * x2x1 - y2y1 * y2y1);
				if (height < 0.0f) {
					continue;
				}
				const auto factor = height / r2;
				heightData[r][c] += hillHeight * factor;
				if (heightData[r][c] > 1.0f) {
					heightData[r][c] = 1.0f;
				}
			}
		}
	}
	return heightData;
}

void Heightmap::createFromHeightData(const std::vector<std::vector<float>> &heightData)
{
	_heightData = heightData;
	_rows = _heightData.size();
	_columns = _heightData[0].size();
	_numVertices = _rows * _columns;

	// First, prepare VAO and VBO for vertex data
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _numVertices * sizeof(vec3), &_heightData, GL_STATIC_DRAW);

	setUpVertices();
	
	setUpTextureCoordinates();

	setUpNormals();

	setUpIndexBuffer();

	// Clear the data, we won't need it anymore
	_vertices.clear();
	_textureCoordinates.clear();
	_normals.clear();

	// If get here, we have succeeded with generating heightmap
	_isInitialized = true;
}

void Heightmap::setUpVertices()
{
	_vertices = std::vector<std::vector<glm::vec3>> (_rows, std::vector<glm::vec3>(_columns));

	for (auto i = 0; i < _rows; i++)
	{
		for (auto j = 0; j < _columns; j++)
		{
			const auto factorRow = float(i) / float(_rows - 1);
			const auto factorColumn = float(j) / float(_columns - 1);
			const auto& vertexHeight = _heightData[i][j];
			_vertices[i][j] = glm::vec3(-0.5f + factorColumn, vertexHeight, -0.5f + factorRow);
		}
		//_vbo.addData(_vertices[i].data(), _columns * sizeof(glm::vec3));
	}
}

*/