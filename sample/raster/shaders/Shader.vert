#version 450

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vNormal;
layout(location = 2) in vec4 vTangent;
layout(location = 3) in vec2 vUV;

layout(location = 0) out vec4 fWorldPosition;
layout(location = 1) out vec4 fNormal;
layout(location = 2) out vec4 fTangent;
layout(location = 3) out vec2 fUV;

layout(set = 0, binding = 0) uniform MVP /*type name*/
{
	mat4 model;
	mat4 view;
	mat4 proj;
	mat4 ITModel;
} mvp;

void main()
{
	fWorldPosition = mvp.model * vPosition;
	fNormal = normalize(mvp.ITModel * vNormal);
	fTangent = normalize(mvp.ITModel * vTangent);
	fUV = vUV;

	gl_Position = mvp.proj * mvp.view * fWorldPosition;
}