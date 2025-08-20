#pragma once

#include <pch.hpp>
#include <sqrap.hpp>

#include "SampleScene.hpp"

class SampleApp : public sqrp::Application
{
private:
	SampleScene sampleScene_;
	virtual void Render() override;

public:
	SampleApp(std::string appName = "sample-raster", unsigned int windowWidth = 1280, unsigned int windowHeight = 720);
	~SampleApp() = default;
};