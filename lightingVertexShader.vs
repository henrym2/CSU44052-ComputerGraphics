#version 330
in vec3 vertex_position;
in vec3 vertex_normal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(vertex_position, 1.0));
    Normal = mat3(transpose(inverse(model))) * vertex_normal;  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}