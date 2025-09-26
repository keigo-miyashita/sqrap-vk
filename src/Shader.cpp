#include "Shader.hpp"

#include "Device.hpp"

namespace sqrp
{
	Shader::Shader(const Device& device, const Compiler& compiler, const std::string& fileName, ShaderType shaderType)
		: pDevice_(&device), pCompiler_(&compiler)
	{
        switch (shaderType) {
        case ShaderType::Vertex:
            shaderStage_ = vk::ShaderStageFlagBits::eVertex; break;
        case ShaderType::Pixel:
            shaderStage_ = vk::ShaderStageFlagBits::eFragment; break;
        case ShaderType::Geometry:
            shaderStage_ = vk::ShaderStageFlagBits::eGeometry; break;
        case ShaderType::Domain:
            shaderStage_ = vk::ShaderStageFlagBits::eTessellationEvaluation; break;
        case ShaderType::Hull:
            shaderStage_ = vk::ShaderStageFlagBits::eTessellationControl; break;
        case ShaderType::Amplification:
            shaderStage_ = vk::ShaderStageFlagBits::eTaskEXT; break;
        case ShaderType::Mesh:
            shaderStage_ = vk::ShaderStageFlagBits::eMeshEXT; break;
        case ShaderType::Compute:
            shaderStage_ = vk::ShaderStageFlagBits::eCompute; break;
        case ShaderType::RayGen:
            shaderStage_ = vk::ShaderStageFlagBits::eRaygenKHR; break;
        case ShaderType::Miss:
            shaderStage_ = vk::ShaderStageFlagBits::eMissKHR; break;
        case ShaderType::ClosestHit:
            shaderStage_ = vk::ShaderStageFlagBits::eClosestHitKHR; break;
        case ShaderType::AnyHit:
            shaderStage_ = vk::ShaderStageFlagBits::eAnyHitKHR; break;
        case ShaderType::Intersection:
            shaderStage_ = vk::ShaderStageFlagBits::eIntersectionKHR; break;
        case ShaderType::Callable:
            shaderStage_ = vk::ShaderStageFlagBits::eCallableKHR; break;
        default:
            throw std::runtime_error("Unsupported shader type");
        }

        std::ifstream file(fileName, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open SPIR-V file!");
        }

		std::vector<uint32_t> buffer = pCompiler_->CompileGLSLToSPIRV(fileName, shaderType);

        /*size_t fileSize = (size_t)file.tellg();
        std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();*/

        shaderModule_ = pDevice_->GetDevice().createShaderModuleUnique(
            vk::ShaderModuleCreateInfo()
            .setCodeSize(buffer.size() * sizeof(uint32_t))
            .setPCode(buffer.data())
		);
	}

    vk::ShaderModule Shader::GetShaderModule() const
    { 
        return shaderModule_.get(); 
    }

    vk::ShaderStageFlagBits Shader::GetShaderStage() const
    {
        return shaderStage_; 
    }
}