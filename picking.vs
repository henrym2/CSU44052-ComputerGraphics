#version 330

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

in vec3 vertex_position;

void main()
{
	gl_Position = proj * view * model * vec4(vertex_position,1.0);
} 
