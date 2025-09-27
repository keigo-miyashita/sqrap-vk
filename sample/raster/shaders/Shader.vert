#version 450

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vNormal;
layout(location = 2) in vec4 vTangent;
layout(location = 3) in vec2 vUV;

layout(location = 0) out vec4 fWorldPosition;
layout(location = 1) out vec4 fNormal;
layout(location = 2) out vec4 fTangent;
layout(location = 3) out vec2 fUV;

layout(set = 0, binding = 0) uniform Camera /*type name*/
{
	mat4 view;
	mat4 proj;
} camera;

layout(set = 0, binding = 1) uniform Object /*type name*/
{
	// add model uniforms here
	mat4 model;
	mat4 ITModel;
} object;

void main()
{
	fWorldPosition = object.model * vPosition;
	fNormal = normalize(object.ITModel * vNormal);
	fTangent = normalize(object.ITModel * vTangent);
	fUV = vUV;

	gl_Position = camera.proj * camera.view * fWorldPosition;
}