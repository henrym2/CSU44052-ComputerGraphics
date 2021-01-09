// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#include <winioctl.h>

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include "glm.hpp"


// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "maths_funcs.h"
#include "obj_parser.h"
#include "camera.h"
#include "Spider.h"
#include "Mesh.h"
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Model.h"
#include "Skybox.h"
#include "boids.h"

/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define MESH_NAME "spiderBits/body.obj"
/*----------------------------------------------------------------------------
----------------------------------------------------------------------------*/

#pragma region SimpleTypes

#pragma endregion SimpleTypes

unsigned int loadTexture(const char* path);

int startX, startY, tracking = 0;
int NUM_LEGS = 8;
int NUM_LEG_JOINTS = 8;
int NUM_FEET = 8;
const int NUM_SPIDER = 10;
using namespace std;

float ambient = 0.5f;
float diffuse = 0.4f;
float spec = 1.0f;

unsigned int plane_vp_vbo = 0;
unsigned int plane_vn_vbo = 0;
unsigned int plane_vt_vbo = 0;
ModelData plane_mesh;

Camera camera(vec3(0.0f, 0.0f, 3.0f));

unsigned int mesh_vao = 0;
int width = 1080;
int height = 720;

float lastX = width / 2.0f;
float lastY = height / 2.0f;

float deltaTime = 0.0f;

bool firstMouse = true;
bool reset = false;

GLuint loc1, loc2, loc3;

Spider horde[NUM_SPIDER];

Spider spider = Spider(vec3(0.0f,0.0f, -25.0f));
Spider spider2 = Spider(vec3(-7.0f, 0.0f, -25.0f));

Shader baseShader;
Shader lightingShader;
Shader skyboxShader;
Shader pickingShader;

Model bat;

Model lightCubes[3];

Model grass[6];
vector<vec3> grassPlaces{
	vec3(0.0f, 0.9f, 0.0f),
	vec3(5.0f, 0.9f, -2.0f),
	vec3(11.0f, 0.9f, 3.0f),
	vec3(3.0f, 0.9f, -9.0f),
	vec3(12.0f, 0.9f, -15.0f),
	vec3(-15.0f, 0.9f, 4.0f),
};

Skybox skybox;

Boids bats;

vec3 lightPositions[] = {
	vec3(0.0f, 8.0f, -15.0f),
	vec3(15.0f, 6.0f, 10.0f),
	vec3(-10.0f, 3.0f, -5.0f)
};

unsigned int floorTexture;
unsigned int spiderTexture;
unsigned int grassTexture;
// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
void generateObjectBufferMesh() {
	/*----------------------------------------------------------------------------
	LOAD MESH HERE AND COPY INTO BUFFERS
	----------------------------------------------------------------------------*/
	Mesh planeLoader = Mesh();

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.
	std::vector<Mesh> tmp = Model::loadScene("./bat.obj");
	std::vector<Mesh> tmp2 = Model::loadScene("./cube.obj");

	plane_mesh = planeLoader.load_mesh("plane/plane.obj");

	plane_vp_vbo = 0;
	glGenBuffers(1, &plane_vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, plane_mesh.mPointCount * sizeof(vec3), &plane_mesh.mVertices[0], GL_STATIC_DRAW);

	plane_vn_vbo = 0;
	glGenBuffers(1, &plane_vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, plane_mesh.mPointCount * sizeof(vec3), &plane_mesh.mNormals[0], GL_STATIC_DRAW);

	plane_vt_vbo = 0;
	glGenBuffers(1, &plane_vt_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vt_vbo);
	glBufferData(GL_ARRAY_BUFFER, plane_mesh.mPointCount * sizeof(vec2), &plane_mesh.mTextureCoords[0], GL_STATIC_DRAW);

	

	for (int i = 0; i < NUM_SPIDER; i++) {
		horde[i].genObject(baseShader);
	}


	bats = Boids(tmp, 10, vec3(10.0f,30.0f,20.0f), vec3(-20.0f,-20.0f,1.0f));
	
	bats.generateObjectBufferMesh(baseShader);

	for (int i = 0; i < 3; i++) {
		lightCubes[i] = Model(tmp2);
		lightCubes[i].generateObjectBufferMesh(lightingShader);
	}

	for (int i = 0; i < 6; i++) {
		grass[i] = Model(tmp2);
		grass[i].generateObjectBufferMesh(baseShader);
	}
	
	skybox.GenObjectBuffer(skyboxShader);
	
}
#pragma endregion VBO_FUNCTIONS


float interpolate(float scale, float lower, float upper) {
	float scale_n = 0;
	if (scale > upper - 1.0f) {
		scale_n = 10.0f;
	}
	else if (scale < lower - 1.0f) {
		scale_n += 0.05f;
	}
	return scale_n;
}

void display() {

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	baseShader.use();
	
	baseShader.setVec3("viewPos", camera.Position);

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(baseShader.ID, "model");
	int view_mat_location = glGetUniformLocation(baseShader.ID, "view");
	int proj_mat_location = glGetUniformLocation(baseShader.ID, "proj");

	loc1 = glGetAttribLocation(baseShader.ID, "vertex_position");
	loc2 = glGetAttribLocation(baseShader.ID, "vertex_normal");
	loc3 = glGetAttribLocation(baseShader.ID, "vertex_texture");

	mat4 view = camera.GetViewMatrix();
	mat4 persp_proj = perspective(camera.Zoom, (float)width / (float)height, 0.1f, 100.0f);
	mat4 model = identity_mat4();
	view = translate(view, vec3(0.0f, -10.0f, -60.0f));

	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);	
	
	baseShader.setVec3("dirLight.direction", vec3(-0.2f, -1.0f, -0.3f));
	baseShader.setVec3("dirLight.ambient", vec3(0.005f, 0.005f, 0.005f));
	baseShader.setVec3("dirLight.diffuse", vec3(0.05f, 0.05f, 0.05f));
	baseShader.setVec3("dirLight.specular", vec3(0.05f, 0.05f, 0.05f));


	baseShader.setVec3("pointLights[0].position", lightPositions[0]);
	baseShader.setVec3("pointLights[0].ambient", vec3(ambient, ambient, ambient));
	baseShader.setVec3("pointLights[0].diffuse", vec3(diffuse, diffuse, diffuse));
	baseShader.setVec3("pointLights[0].specular", vec3(spec, spec, spec));
	baseShader.setFloat("pointLights[0].constant", 1.0f);
	baseShader.setFloat("pointLights[0].linear", 0.07);
	baseShader.setFloat("pointLights[0].quadratic", 0.012);


	baseShader.setVec3("pointLights[1].position", lightPositions[1]);
	baseShader.setVec3("pointLights[1].ambient", vec3(ambient, ambient, ambient));
	baseShader.setVec3("pointLights[1].diffuse", vec3(diffuse, diffuse, diffuse));
	baseShader.setVec3("pointLights[1].specular", vec3(spec, spec, spec));
	baseShader.setFloat("pointLights[1].constant", 1.0f);
	baseShader.setFloat("pointLights[1].linear", 0.014);
	baseShader.setFloat("pointLights[1].quadratic", 0.002);



	baseShader.setVec3("pointLights[2].position", lightPositions[2]);
	baseShader.setVec3("pointLights[2].ambient", vec3(ambient, ambient, ambient));
	baseShader.setVec3("pointLights[2].diffuse", vec3(diffuse, diffuse, diffuse));
	baseShader.setVec3("pointLights[2].specular", vec3(spec, spec, spec));
	baseShader.setFloat("pointLights[2].constant", 1.0f);
	baseShader.setFloat("pointLights[2].linear", 0.014);
	baseShader.setFloat("pointLights[2].quadratic", 0.0002);
	//Spotlight
	/*
	baseShader.setVec3("spotLight.position", camera.Position);
	baseShader.setVec3("spotLight.direction", camera.Front);
	baseShader.setVec3("spotLight.ambient", vec3(0.0f, 0.0f, 0.0f));
	baseShader.setVec3("spotLight.diffuse", vec3(1.0f, 1.0f, 1.0f));
	baseShader.setVec3("spotLight.specular", vec3(1.0f, 1.0f, 1.0f));
	baseShader.setFloat("spotLight.constant", 1.0f);
	baseShader.setFloat("spotLight.linear", 0.09);
	baseShader.setFloat("spotLight.quadratic", 0.032);
	baseShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	baseShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	*/
	/*baseShader.setVec3("light.position", vec3(0.0f, 6.0f, -15.0f));
	baseShader.setVec3("light.specular", vec3(1.0f, 1.0f, 1.0f));
	baseShader.setVec3("light.ambient", vec3(1.0f, 1.0f, 1.0f));
	baseShader.setVec3("light.diffuse", vec3(0.2f, 0.2f, 1.0f));
	*/
	baseShader.setVec3("material.ambient", vec3(1.0f, 1.0f, 1.0f));
	//baseShader.setVec3("material.diffuse", vec3(1.0f, 1.0f, 1.0f));
	baseShader.setVec3("material.specular", vec3(0.5f, 0.5f, 0.5f)); // specular lighting doesn't have full effect on this object's material
	baseShader.setVec3("material.specular", vec3(0.5f, 0.5f, 0.5f)); // specular lighting doesn't have full effect on this object's material
	baseShader.setFloat("material.shininess", 32.0f);

	
	mat4 Plane = identity_mat4();
	Plane = translate(Plane, vec3(5.0f, 0.0f, 0.0f));
	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vn_vbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc3);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vt_vbo);
	glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// update uniforms & draw
	

	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, Plane.m);

	glActiveTexture(GL_TEXTURE1);
	baseShader.setInt("material.diffuse", 1);

	glBindTexture(GL_TEXTURE_2D, floorTexture);

	glDrawArrays(GL_TRIANGLES, 0, plane_mesh.mPointCount);

	glActiveTexture(GL_TEXTURE2);
	baseShader.setInt("material.diffuse", 2);
	glBindTexture(GL_TEXTURE_2D, spiderTexture);
	
	for (int i = 0; i < NUM_SPIDER; i++) {
		horde[i].draw(baseShader, camera);
	}
	
	// Apply the root matrix to the child matrix
	
	mat4 batStuff = identity_mat4();
	glActiveTexture(GL_TEXTURE0);

	baseShader.setInt("material.diffuse", 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	batStuff = translate(batStuff, vec3(0.0f, 1.0f, 0.0f));
	//bat.draw(batStuff, matrix_location, floorTexture, baseShader);

	bats.drawAll(baseShader);

	glActiveTexture(GL_TEXTURE3);
	baseShader.setInt("material.diffuse", 3);
	glBindTexture(GL_TEXTURE_2D, grassTexture);
	for (int i = 0; i < 6; i++) {
		mat4 grassPos = identity_mat4();
		grassPos = rotate_x_deg(grassPos, 90.0f);
		grassPos = rotate_y_deg(grassPos, 90.0f);
		grassPos = translate(grassPos, grassPlaces[i]);
		grass[i].draw(grassPos, matrix_location, 3, baseShader);
	}

	lightingShader.use();
	view_mat_location = glGetUniformLocation(lightingShader.ID, "view");
	proj_mat_location = glGetUniformLocation(lightingShader.ID, "proj");
	matrix_location = glGetUniformLocation(lightingShader.ID, "model");


	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	for (int i = 0; i < 3; i++) {
		mat4 lightPos = identity_mat4();
		lightPos = scale(lightPos, vec3(0.4, 0.4, 0.4));
		lightPos = translate(lightPos, lightPositions[i]);
		lightCubes[i].draw(lightPos, matrix_location, 0, lightingShader);
	}


	mat4 test = identity_mat4();
	glDepthFunc(GL_LEQUAL);
	skyboxShader.use();
	matrix_location = glGetUniformLocation(skyboxShader.ID, "model");
	skyboxShader.setMat4("view", test);
	skyboxShader.setMat4("proj", persp_proj);
	skybox.draw(skyboxShader, matrix_location);
	glDepthFunc(GL_LESS);
	glutSwapBuffers();
}

void renderSelection() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 view = camera.GetViewMatrix();
	mat4 persp_proj = perspective(camera.Zoom, (float)width / (float)height, 0.1f, 100.0f);
	mat4 model = identity_mat4();
	view = translate(view, vec3(0.0f, -10.0f, -60.0f));

	pickingShader.use();

	int matrix_location = glGetUniformLocation(pickingShader.ID, "model");
	int view_mat_location = glGetUniformLocation(pickingShader.ID, "view");
	int proj_mat_location = glGetUniformLocation(pickingShader.ID, "proj");
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);

	for (int i = 0; i < NUM_SPIDER; i++) {
		horde[i].draw(pickingShader, camera);
		pickingShader.setInt("code", i+1);
	}

	//glutSwapBuffers();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void processSelection(int xx, int yy) {
	unsigned char res[4];
	GLint viewport[4];

	renderSelection();

	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(xx, viewport[3] - yy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);

	if (res[0] < NUM_SPIDER && res[0] > 0) {
		horde[res[0]].clicked();
	}
	
}


void updateScene() {

	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;
	
	for (int i = 0; i < NUM_SPIDER; i++) {
		horde[i].update(delta);
	}
	bats.updateAll();
	glutPostRedisplay();
}

void init()
{
	skybox = Skybox();
	// Set up the shaders
	//GLuint shaderProgramID = CompileShaders();
	baseShader = Shader("simpleVertexShader.txt", "simpleFragmentShader.txt");
	//baseShader.setVec3("lightPos", vec3(0.0f, 10.0f, -15.0f));
	//baseShader.setVec3("position_eye", camera.Position);
	lightingShader = Shader("lightingVertexShader.vs", "LightingFragmentShader.vs");
	skyboxShader = Shader("skyboxVet.vs", "skyboxFrag.fs");
	skyboxShader.use();
	skyboxShader.setInt("skybox", 1);

	pickingShader = Shader("picking.vs", "picking.fs");

	baseShader.use();

	floorTexture = loadTexture("./stoneFloor.jpg");
	spiderTexture = loadTexture("./spider_texture.jpg");
	grassTexture = loadTexture("./grass.png");

	// load mesh into a vertex buffer array
	glutWarpPointer(width / 2, height / 2);
	int pos = 38;
	for (int i = 0; i < NUM_SPIDER; i++) {
		
		horde[i] = Spider(vec3(pos, 0.0f, -25.0f));
		pos -= 7.0f;
	}

	generateObjectBufferMesh();
}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {
	if (key == 'w') {
		camera.ProcessKeyboard(FORWARD, 0.1);
	}
	if (key == 'a') {
		camera.ProcessKeyboard(LEFT, 0.1);
	}
	if (key == 'd') {
		camera.ProcessKeyboard(RIGHT, 0.1);
	}
	if (key == 's') {
		camera.ProcessKeyboard(BACKWARD, 0.1);
	}
	if (key == 'r') {
		reset = true;
		ambient = 0.5f;
		diffuse = 0.4f;
		spec = 1.0f;
	}
//Ambient
	if (key == 't') {
		ambient = 0.5f;
		diffuse = 0.0f;
		spec = 0.0f;
	}
//Diffuse
	if (key == 'y') {
		ambient = 0.0f;
		diffuse = 0.4f;
		spec = 0.0f;
	}
//Spec
	if (key == 'u') {
		spec = 1.0f;
		ambient = 0.0f;
		diffuse = 0.0f;
	}

	for (int i = 0; i < NUM_SPIDER; i++) {
		horde[i].keyPress(key, x, y);
	}
}

void mouseCallback(int x, int y) {
	/**if (firstMouse) {
		lastX = x;
		lastY = y;
		firstMouse = false;
	}*/

	float xoffset = x - lastX;
	float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top

	lastX = x;
	lastY = y;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		startX = x;
		startY = y;
		processSelection(x, y);
	}
/*
	if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (x - startX);
			beta += (y - startY);
			if (beta > 85.0f)
				beta = 85.0f;
			else if (beta < -85.0f)
				beta = -85.0f;
		}
	}
*/
	if (button == 4) {
		camera.ProcessMouseScroll(1.0f);
	}
	if (button == 3) {
		camera.ProcessMouseScroll(-1.0f);
	}
}


unsigned int loadTexture(char const * path) {
	unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

int main(int argc, char** argv) {
	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Hello Triangle");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(mouseCallback);
	glutMouseFunc(mouse);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}
