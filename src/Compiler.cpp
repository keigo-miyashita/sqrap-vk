#include "Compiler.hpp"

#include "Shader.hpp"

using namespace std;

namespace sqrp
{
    TBuiltInResource Compiler::DefaultTBuiltInResource() const {
        TBuiltInResource res = {};
        res.maxLights = 32;
        res.maxClipPlanes = 6;
        res.maxTextureUnits = 32;
        res.maxTextureCoords = 32;
        res.maxVertexAttribs = 64;
        res.maxVertexUniformComponents = 4096;
        res.maxVaryingFloats = 64;
        res.maxVertexTextureImageUnits = 32;
        res.maxCombinedTextureImageUnits = 80;
        res.maxTextureImageUnits = 32;
        res.maxFragmentUniformComponents = 4096;
        res.maxDrawBuffers = 32;
        res.maxVertexUniformVectors = 128;
        res.maxVaryingVectors = 8;
        res.maxFragmentUniformVectors = 16;
        res.maxVertexOutputVectors = 16;
        res.maxFragmentInputVectors = 15;
        res.minProgramTexelOffset = -8;
        res.maxProgramTexelOffset = 7;
        res.maxClipDistances = 8;
        res.maxComputeWorkGroupCountX = 65535;
        res.maxComputeWorkGroupCountY = 65535;
        res.maxComputeWorkGroupCountZ = 65535;
        res.maxComputeWorkGroupSizeX = 1024;
        res.maxComputeWorkGroupSizeY = 1024;
        res.maxComputeWorkGroupSizeZ = 64;
        res.maxComputeUniformComponents = 1024;
        res.maxComputeTextureImageUnits = 16;
        res.maxComputeImageUniforms = 8;
        res.maxComputeAtomicCounters = 8;
        res.maxComputeAtomicCounterBuffers = 1;
        res.maxVaryingComponents = 60;
        res.maxVertexOutputComponents = 64;
        res.maxGeometryInputComponents = 64;
        res.maxGeometryOutputComponents = 128;
        res.maxFragmentInputComponents = 128;
        res.maxImageUnits = 8;
        res.maxCombinedImageUnitsAndFragmentOutputs = 8;
        res.maxCombinedShaderOutputResources = 8;
        res.maxImageSamples = 0;
        res.maxVertexImageUniforms = 0;
        res.maxTessControlImageUniforms = 0;
        res.maxTessEvaluationImageUniforms = 0;
        res.maxGeometryImageUniforms = 0;
        res.maxFragmentImageUniforms = 8;
        res.maxCombinedImageUniforms = 8;
        res.maxGeometryTextureImageUnits = 16;
        res.maxGeometryOutputVertices = 256;
        res.maxGeometryTotalOutputComponents = 1024;
        res.maxGeometryUniformComponents = 1024;
        res.maxGeometryVaryingComponents = 64;
        res.maxTessControlInputComponents = 128;
        res.maxTessControlOutputComponents = 128;
        res.maxTessControlTextureImageUnits = 16;
        res.maxTessControlUniformComponents = 1024;
        res.maxTessControlTotalOutputComponents = 4096;
        res.maxTessEvaluationInputComponents = 128;
        res.maxTessEvaluationOutputComponents = 128;
        res.maxTessEvaluationTextureImageUnits = 16;
        res.maxTessEvaluationUniformComponents = 1024;
        res.maxTessPatchComponents = 120;
        res.maxPatchVertices = 32;
        res.maxTessGenLevel = 64;
        res.maxViewports = 16;
        res.maxVertexAtomicCounters = 0;
        res.maxTessControlAtomicCounters = 0;
        res.maxTessEvaluationAtomicCounters = 0;
        res.maxGeometryAtomicCounters = 0;
        res.maxFragmentAtomicCounters = 8;
        res.maxCombinedAtomicCounters = 8;
        res.maxAtomicCounterBindings = 1;
        res.maxVertexAtomicCounterBuffers = 0;
        res.maxTessControlAtomicCounterBuffers = 0;
        res.maxTessEvaluationAtomicCounterBuffers = 0;
        res.maxGeometryAtomicCounterBuffers = 0;
        res.maxFragmentAtomicCounterBuffers = 1;
        res.maxCombinedAtomicCounterBuffers = 1;
        res.maxAtomicCounterBufferSize = 16384;
        res.maxTransformFeedbackBuffers = 4;
        res.maxTransformFeedbackInterleavedComponents = 64;
        res.maxCullDistances = 8;
        res.maxCombinedClipAndCullDistances = 8;
        return res;
    }

    Compiler::Compiler() {
        glslang::InitializeProcess();
    }

    Compiler::~Compiler() {
        glslang::FinalizeProcess();
    }

    std::vector<uint32_t> Compiler::CompileGLSLToSPIRV(const std::string& fileName, ShaderType shaderType) const
    {
        std::ifstream file(fileName, std::ios::in);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open GLSL file: " + fileName);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string glslSource = buffer.str();

		EShLanguage stage;
        switch(shaderType) {
            case ShaderType::Vertex:
                stage = EShLangVertex; break;
			case ShaderType::Pixel:
				stage = EShLangFragment; break;
            case ShaderType::Geometry:
                stage = EShLangGeometry; break;
            case ShaderType::Domain:
                stage = EShLangTessEvaluation; break;
            case ShaderType::Hull:
                stage = EShLangTessControl; break;
            case ShaderType::Amplification:
                stage = EShLangTask; break;
            case ShaderType::Mesh:
                stage = EShLangMesh; break;
            case ShaderType::Compute:
                stage = EShLangCompute; break;
            case ShaderType::RayGen:
                stage = EShLangRayGen; break;
            case ShaderType::Miss:
				stage = EShLangMiss; break;
			case ShaderType::ClosestHit:
				stage = EShLangClosestHit; break;
			case ShaderType::AnyHit:
				stage = EShLangAnyHit; break;
			case ShaderType::Intersection:
				stage = EShLangIntersect; break;
            case ShaderType::Callable:
				stage = EShLangCallable; break;
            default:
				throw std::runtime_error("Unsupported shader type");
        }

        glslang::TShader shader(stage);
        const char* sourceCStr = glslSource.c_str();
        shader.setStrings(&sourceCStr, 1);
		// Disable automatic binding and location assignment
        shader.setAutoMapBindings(false);
		shader.setAutoMapLocations(false);

        TBuiltInResource resources = DefaultTBuiltInResource();  // デフォルト
        int defaultVersion = 450;

        EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

        if (!shader.parse(&resources, defaultVersion, false, messages)) {
            throw std::runtime_error(
                "GLSL compilation failed:\n" +
                std::string(shader.getInfoLog()) + "\n" +
                std::string(shader.getInfoDebugLog())
            );
        }

        glslang::TProgram program;
        program.addShader(&shader);

        if (!program.link(messages)) {
            throw std::runtime_error(
                "GLSL linking failed:\n" +
                std::string(program.getInfoLog()) + "\n" +
                std::string(program.getInfoDebugLog())
            );
        }

        std::vector<uint32_t> spirv;
        glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
        
		return spirv;
    }
}