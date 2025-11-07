#include "Shader.hpp"

#include "Device.hpp"

namespace sqrp
{
	Shader::Shader(const Device& device, const Compiler& compiler, const std::string& fileName, ShaderType shaderType)
		: pDevice_(&device), pCompiler_(&compiler)
	{
		std::filesystem::path path(fileName);
		std::string name = path.stem().string();
        switch (shaderType) {
        case ShaderType::Vertex:
            shaderStage_ = vk::ShaderStageFlagBits::eVertex; name + "_vertex"; break;
        case ShaderType::Pixel:
            shaderStage_ = vk::ShaderStageFlagBits::eFragment; name + "_fragment"; break;
        case ShaderType::Geometry:
            shaderStage_ = vk::ShaderStageFlagBits::eGeometry; name + "_geometry"; break;
        case ShaderType::Domain:
            shaderStage_ = vk::ShaderStageFlagBits::eTessellationEvaluation; name + "_tessellationEvalution"; break;
        case ShaderType::Hull:
            shaderStage_ = vk::ShaderStageFlagBits::eTessellationControl; name + "_tessellationControl"; break;
        case ShaderType::Amplification:
            shaderStage_ = vk::ShaderStageFlagBits::eTaskEXT; name + "_task"; break;
        case ShaderType::Mesh:
            shaderStage_ = vk::ShaderStageFlagBits::eMeshEXT; name + "_mesh"; break;
        case ShaderType::Compute:
            shaderStage_ = vk::ShaderStageFlagBits::eCompute; name + "_compute"; break;
        case ShaderType::RayGen:
            shaderStage_ = vk::ShaderStageFlagBits::eRaygenKHR; name + "_raygen"; break;
        case ShaderType::Miss:
            shaderStage_ = vk::ShaderStageFlagBits::eMissKHR; name + "_miss"; break;
        case ShaderType::ClosestHit:
            shaderStage_ = vk::ShaderStageFlagBits::eClosestHitKHR; name + "_closesthit"; break;
        case ShaderType::AnyHit:
            shaderStage_ = vk::ShaderStageFlagBits::eAnyHitKHR; name + "_anyhit"; break;
        case ShaderType::Intersection:
            shaderStage_ = vk::ShaderStageFlagBits::eIntersectionKHR; name + "_intersect"; break;
        case ShaderType::Callable:
            shaderStage_ = vk::ShaderStageFlagBits::eCallableKHR; name + "_callable"; break;
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