#version 450

layout(location = 0) in vec4 fWorldPosition;
layout(location = 1) in vec4 fNormal;
layout(location = 2) in vec4 fTangent;
layout(location = 3) in vec2 fUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform Light /*type name*/
{
	vec4 lightPos;
	vec4 lightColor;
} light;

void main()
{
	vec3 N = normalize(fNormal.xyz);
	vec3 L = normalize(light.lightPos.xyz);

	float diff = max(dot(N, L), 0.0);

	vec3 color = light.lightColor.rgb * diff;

	outColor = vec4(color, 1.0);
}