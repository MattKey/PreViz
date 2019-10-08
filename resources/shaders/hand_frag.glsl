#version 330 core 
in vec3 fragNor;
in vec3 fragPos;
out vec4 color;

vec3 lightPos = vec3(-100,-100,-100);

void main()
{
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 normal = normalize(fragNor);
	float diffuse =  pow(dot(normal, lightDir), 2);
	vec3 basecolor = vec3(1,0.77,0.6);
	color = vec4(diffuse * basecolor, 1);
	//color = vec4(normal, 1);
}
