#include <sqrap.hpp>

#include "SampleApp.hpp"

int main()
{
	SampleApp sampleApp;

	try {
		if (!sampleApp.Init()) {
			throw std::runtime_error("Failed to initialize application.");
		}
		sampleApp.Run();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}