#include "SampleApp.hpp"

using namespace std;
using namespace sqrp;

SampleApp::SampleApp(std::string appName, unsigned int windowWidth, unsigned int windowHeight)
	: Application(appName, windowWidth, windowHeight)
{

}

void SampleApp::OnStart()
{
	device_.Init(*this);

	swapchain_ = device_.CreateSwapchain(windowWidth_, windowHeight_);

	renderPass_ = device_.CreateRenderPass(swapchain_);
	frameBuffer_ = device_.CreateFrameBuffer(swapchain_, 1);

	mesh_ = device_.CreateMesh(string(MODEL_DIR) + "Suzanne.gltf");

	//vertShader_ = compiler_.CompileShader(string(SHADER_DIR) + "shader.vert", sqrp::ShaderType::Vertex);
	vertShader_ = device_.CreateShader(compiler_, string(SHADER_DIR) + "Shader.vert", sqrp::ShaderType::Vertex);
	pixelShader_ = device_.CreateShader(compiler_, string(SHADER_DIR) + "Shader.frag", sqrp::ShaderType::Pixel);

	//pipeline_

}

void SampleApp::OnUpdate()
{

}