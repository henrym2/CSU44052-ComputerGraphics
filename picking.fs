#version 330

uniform int code;

void main()
{
	gl_FragColor = vec4(code/255.0);
	//outputF = vec4(1,0,0,1);
} 
