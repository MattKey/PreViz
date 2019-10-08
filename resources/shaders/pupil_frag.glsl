#version 330 core 
in vec3 fragNor;
in vec3 fragPos;
out vec4 color;

vec3 lightPos = vec3(-100,-100,-100);

void main()
{
	color = vec4(0,0,0, 1);
}
