#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 fragNor;
out vec3 fragPos;

void main()
{
	gl_Position = P * V * M * vertPos;
	fragNor = (M * vec4(vertNor, 1)).xyz;
	fragPos = (M * vec4(vertPos.xyz, 1)).xyz;
}
