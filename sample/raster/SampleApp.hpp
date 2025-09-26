#pragma once

#include <pch.hpp>
#include <sqrap.hpp>

class SampleApp : public sqrp::Application
{
private:
	sqrp::Device device_;
	sqrp::SwapchainHandle swapchain_;
	sqrp::RenderPassHandle renderPass_;
	sqrp::FrameBufferHandle	frameBuffer_;
	sqrp::MeshHandle mesh_;

public:
	SampleApp(std::string appName = "sample-raster", unsigned int windowWidth = 1280, unsigned int windowHeight = 720);
	~SampleApp() = default;

	virtual void OnStart() override;
	virtual void OnUpdate() override;
};