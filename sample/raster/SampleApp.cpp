#include "SampleApp.hpp"

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

}

void SampleApp::OnUpdate()
{

}