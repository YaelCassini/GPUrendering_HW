#version 330 core

out vec4 FragColor;

in vec4 fcolor;
in vec3 normal;
in vec3 lightVec;
in vec3 viewVec;

uniform int renderindex;

void main()
{
	vec3 norm = normalize(normal);
	vec3 L = normalize(lightVec);
	vec3 V = normalize(viewVec);
	vec3 halfAngle = normalize(L + V);
	float NdotL = dot(L, norm);
	float NdotH = clamp(dot(halfAngle, norm), 0.0, 1.0);

	// 半兰伯特模型计算漫反射
	float diffuse = 0.5 * NdotL + 0.5;
	// Blinn-Phong模型计算高光
	float specular = pow(NdotH, 64.0) * 0.3f;
	float result = diffuse + specular;

	if(renderindex == 0)FragColor = vec4(result);
	else if(renderindex == 1)  FragColor = fcolor;
	else if(renderindex == 2)  FragColor = vec4(norm, 1.0f);
}