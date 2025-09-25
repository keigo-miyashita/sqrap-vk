#include "Mesh.hpp"

#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "Device.hpp"

using namespace std;
using namespace glm;

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>

namespace sqrp
{
	bool Mesh::LoadModel(std::string modelPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		string err, warn;

		bool success = loader.LoadASCIIFromFile(&model, &err, &warn, modelPath.c_str());
		if (!success) {
			cerr << "failed to load gltf\n";
			assert(0);
			return false;
		}

		vertices.clear();
		indices.clear();

		for (auto& mesh : model.meshes) {
			for (auto& primitive : mesh.primitives) {
				int posAccessorIndex = primitive.attributes["POSITION"];
				const tinygltf::Accessor& posAccessor = model.accessors[posAccessorIndex];
				const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
				tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];
				float* positions = reinterpret_cast<float*>(&posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

				int normalAccessorIndex = primitive.attributes["NORMAL"];
				const tinygltf::Accessor& normalAccessor = model.accessors[normalAccessorIndex];
				const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
				tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
				float* normals = reinterpret_cast<float*>(&normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);

				int tangentAccessorIndex = primitive.attributes["TANGENT"];
				const tinygltf::Accessor& tangentAccessor = model.accessors[tangentAccessorIndex];
				const tinygltf::BufferView& tangentBufferView = model.bufferViews[tangentAccessor.bufferView];
				tinygltf::Buffer& tangentBuffer = model.buffers[tangentBufferView.buffer];
				float* tangents = reinterpret_cast<float*>(&tangentBuffer.data[tangentBufferView.byteOffset + tangentAccessor.byteOffset]);

				int uvAccessorIndex = primitive.attributes["TEXCOORD_0"];
				const tinygltf::Accessor& uvAccessor = model.accessors[uvAccessorIndex];
				const tinygltf::BufferView& uvBufferView = model.bufferViews[uvAccessor.bufferView];
				tinygltf::Buffer& uvBuffer = model.buffers[uvBufferView.buffer];
				float* uvs = reinterpret_cast<float*>(&uvBuffer.data[uvBufferView.byteOffset + uvAccessor.byteOffset]);

				for (int i = 0; i < posAccessor.count; i++) {
					vertices.push_back(
						{
							(vec4(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2], 1.0f)),
							(vec4(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2], 1.0f)),
							(vec4(tangents[i * 4], tangents[i * 4 + 1], tangents[i * 4 + 2], tangents[i * 4 + 3])),
							(vec2(uvs[i * 2], uvs[i * 2 + 1]))
						});
				}

				int indicesAccessorIndex = primitive.indices;
				const tinygltf::Accessor& indicesAccessor = model.accessors[indicesAccessorIndex];
				const tinygltf::BufferView& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
				tinygltf::Buffer& indicesBuffer = model.buffers[indicesBufferView.buffer];
				unsigned short* index = reinterpret_cast<unsigned short*>(&indicesBuffer.data[indicesBufferView.byteOffset + indicesAccessor.byteOffset]);
				for (int i = 0; i < indicesAccessor.count; i++) {
					indices.push_back(index[i]);
				}
			}
		}
		return true;
	}

	HRESULT Mesh::CreateVertexBuffer()
	{
		shared_ptr<Buffer> vertexStagingBuffer;
		vertexStagingBuffer = pDevice_->CreateBuffer(
			sizeof(Vertex) * vertices_.size(),
			vk::BufferUsageFlagBits::eTransferSrc,
			VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
			VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);
		void* rawPtr = vertexStagingBuffer->Map();
		if (rawPtr) {
			Vertex* pVertex = static_cast<Vertex*>(rawPtr);
			memcpy(pVertex, vertices_.data(), sizeof(Vertex) * vertices_.size());
			vertexStagingBuffer->Unmap();
		}

		vertexBuffer_ = pDevice_->CreateBuffer(
			sizeof(Vertex) * vertices_.size(),
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,
			VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);

		pDevice_->OneTimeSubmit([&](CommandBufferHandle pCommandBuffer) {
			pCommandBuffer->CopyBuffer(vertexStagingBuffer, vertexBuffer_);
		});
		/*command_->CopyBuffer(vertexUploadBuffer, vertexBuffer_);
		command_->WaitCommand();*/

		return S_OK;
	}

	HRESULT Mesh::CreateIndexBuffer()
	{
		shared_ptr<Buffer> indexStagingBuffer;
		indexStagingBuffer = pDevice_->CreateBuffer(
			sizeof(uint32_t) * indices_.size(),
			vk::BufferUsageFlagBits::eTransferSrc,
			VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_HOST,
			VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);
		void* rawPtr = indexStagingBuffer->Map();
		if (rawPtr) {
			uint32_t* pIndex = static_cast<uint32_t*>(rawPtr);
			memcpy(pIndex, indices_.data(), sizeof(uint32_t) * indices_.size());
			indexStagingBuffer->Unmap();
		}

		indexBuffer_ = pDevice_->CreateBuffer(
			sizeof(uint32_t) * indices_.size(),
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY,
			VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);
		pDevice_->OneTimeSubmit([&](CommandBufferHandle pCommandBuffer) {
			pCommandBuffer->CopyBuffer(indexStagingBuffer, indexBuffer_);
		});
		/*command_->CopyBuffer(indexUploadBuffer, indexBuffer_);
		command_->WaitCommand();*/

		return S_OK;
	}

	Mesh::Mesh(const Device& device, std::string modelPath)
		: pDevice_(&device)
	{
		LoadModel(modelPath, vertices_, indices_);
		CreateVertexBuffer();
		CreateIndexBuffer();
	}

	Mesh::Mesh(const Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		: pDevice_(&device), vertices_(vertices), indices_(indices)
	{
		CreateVertexBuffer();
		CreateIndexBuffer();
	}


	BufferHandle Mesh::GetVertexBuffer() const
	{
		return vertexBuffer_;
	}
	
	BufferHandle Mesh::GetIndexBuffer() const
	{
		return indexBuffer_;
	}

}