#include "Shader.hpp"

#include "Device.hpp"

namespace sqrp
{
	Shader::Shader(const Device& device, const std::string& filename)
		: pDevice_(&device)
	{
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open SPIR-V file!");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();

        shaderModule_ = pDevice_->GetDevice().createShaderModuleUnique(
            vk::ShaderModuleCreateInfo()
            .setCodeSize(buffer.size() * sizeof(uint32_t))
            .setPCode(buffer.data())
		);
	}
}