#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include <string>
#include "glm.hpp"
#include "maths_funcs.h"
#include "stb_image.h"
#include <iostream>
#include "Shader.h"

class Skybox {
public:
	Skybox();
	unsigned int loadCubemap(std::vector<std::string> faces);
	void draw(Shader shader, GLuint matrix_location);
	void GenObjectBuffer(Shader shader);
private:
	unsigned int vao, vbo = 0;
	unsigned int texture;
};