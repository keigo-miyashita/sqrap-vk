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
	MeshBase::MeshBase(const Device& device)
		: pDevice_(&device)
	{

	}

	bool MeshBase::CreateVertexBuffer()
	{
		BufferHandle vertexStagingBuffer;
		vertexStagingBuffer = pDevice_->CreateBuffer(
			sizeof(Vertex) * vertices_.size(),
			vk::BufferUsageFlagBits::eTransferSrc,
			VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_HOST
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
			0,
			VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY
		);

		pDevice_->OneTimeSubmit([&](CommandBufferHandle pCommandBuffer) {
			pCommandBuffer->CopyBuffer(vertexStagingBuffer, vertexBuffer_);
		});

		return true;
	}

	bool MeshBase::CreateIndexBuffer()
	{
		BufferHandle indexStagingBuffer;
		indexStagingBuffer = pDevice_->CreateBuffer(
			sizeof(uint32_t) * indices_.size(),
			vk::BufferUsageFlagBits::eTransferSrc,
			VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
			VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO_PREFER_HOST
		);
		void* rawPtr = indexStagingBuffer->Map();
		if (rawPtr) {
			uint32_t* pIndex = static_cast<uint32_t*>(rawPtr);
			memcpy(pIndex, indices_.data(), sizeof(uint32_t) * indices_.size());
			indexStagingBuffer->Unmap();
		}

		indexBuffer_ = pDevice_->CreateBuffer(
			sizeof(uint32_t) * indices_.size(),
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			0,
			VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY
		);
		pDevice_->OneTimeSubmit([&](CommandBufferHandle pCommandBuffer) {
			pCommandBuffer->CopyBuffer(indexStagingBuffer, indexBuffer_);
		});

		return true;
	}

	std::string MeshBase::GetName() const
	{
		return name_;
	}

	BufferHandle MeshBase::GetVertexBuffer() const
	{
		return vertexBuffer_;
	}
	
	BufferHandle MeshBase::GetIndexBuffer() const
	{
		return indexBuffer_;
	}

	int MeshBase::GetNumIndices() const
	{
		return static_cast<int>(indices_.size());
	}

	bool Mesh::LoadModel(std::string modelPath)
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
					vertices_.push_back(
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
					indices_.push_back(index[i]);
				}
			}
		}
		return true;
	}

	Mesh::Mesh(const Device& device, std::string modelPath)
		: MeshBase(device)
	{
		LoadModel(modelPath);
		std::filesystem::path fullPath = modelPath;

		name_ = fullPath.stem().string();

		CreateVertexBuffer();
		CreateIndexBuffer();
	}

	Mesh::Mesh(const Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		: MeshBase(device)
	{
		// Cannot initialize base class member variables in constructor
		vertices_ = vertices;
		indices_ = indices;
		CreateVertexBuffer();
		CreateIndexBuffer();
	}

	bool GLTFMesh::LoadModel(std::string modelPath)
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

		uint32_t vertexOffset = 0;
		uint32_t indexOffset = 0;
		for (auto& mesh : model.meshes) {
			meshNum_ = model.meshes.size();
			primitiveNumPerMesh_.push_back(static_cast<int>(mesh.primitives.size()));
			for (auto& primitive : mesh.primitives) { // mesh is devided if it has multiple material
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
					vertices_.push_back(
						{
							(vec4(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2], 1.0f)),
							(vec4(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2], 1.0f)),
							(vec4(tangents[i * 4], tangents[i * 4 + 1], tangents[i * 4 + 2], tangents[i * 4 + 3])),
							(vec2(uvs[i * 2], uvs[i * 2 + 1]))
						}
					);
				}

				int indicesAccessorIndex = primitive.indices;
				const tinygltf::Accessor& indicesAccessor = model.accessors[indicesAccessorIndex];
				const tinygltf::BufferView& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
				tinygltf::Buffer& indicesBuffer = model.buffers[indicesBufferView.buffer];
				unsigned short* index = reinterpret_cast<unsigned short*>(&indicesBuffer.data[indicesBufferView.byteOffset + indicesAccessor.byteOffset]);
				for (int i = 0; i < indicesAccessor.count; i++) {
					indices_.push_back(index[i]);
				}
				vertexRanges_.push_back({ vertexOffset, static_cast<uint32_t>(vertices_.size()) });
				indexRanges_.push_back({ indexOffset, static_cast<uint32_t>(indices_.size()) });
				materialIndices_.push_back(primitive.material);
				vertexOffset = static_cast<uint32_t>(vertices_.size());
				indexOffset = static_cast<uint32_t>(indices_.size());
			}
		}

		for (const auto& node : model.nodes) {
			glm::mat4 modelMat(1.0f);

			if (node.matrix.size() == 16) {
				// When a 4x4 matrix is provided directly
				modelMat = glm::make_mat4(node.matrix.data());
			}
			else {
				glm::vec3 translation(0.0f);
				glm::quat rotation(1.0f, 0.0f, 0.0f, 0.0f);
				glm::vec3 scale(1.0f);

				if (node.translation.size() == 3) {
					translation = glm::vec3(
						static_cast<float>(node.translation[0]),
						static_cast<float>(node.translation[1]),
						static_cast<float>(node.translation[2])
					);
				}
				if (node.rotation.size() == 4) {
					rotation = glm::quat(
						static_cast<float>(node.rotation[3]), // w
						static_cast<float>(node.rotation[0]), // x
						static_cast<float>(node.rotation[1]), // y
						static_cast<float>(node.rotation[2])  // z
					);
				}
				if (node.scale.size() == 3) {
					scale = glm::vec3(
						static_cast<float>(node.scale[0]),
						static_cast<float>(node.scale[1]),
						static_cast<float>(node.scale[2])
					);
				}

				modelMat = glm::translate(glm::mat4(1.0f), translation) *
					glm::mat4_cast(rotation) *
					glm::scale(glm::mat4(1.0f), scale);
			}
			if (node.mesh != -1) {
				SubMeshInfo subMeshInfo;
				subMeshInfo.meshIndex = node.mesh; // if node has no mesh, node.mesh == -1
				subMeshInfo.mat.model = modelMat;
				glm::mat3x3 modelMat3x3 = glm::mat3x3(modelMat);
				subMeshInfo.mat.invTransModel = glm::mat4x4(glm::inverse(glm::transpose(modelMat3x3)));
				subMeshInfos_.push_back(subMeshInfo);
			}

			if (node.children.size() > 0) {
				// If node has child nodes, process them recursively
				for (const int childIndex : node.children) {
					glm::mat4 childModelMat = subMeshInfos_[childIndex].mat.model;
					subMeshInfos_[childIndex].mat.model = modelMat * childModelMat;
					glm::mat3x3 modelMat3x3 = glm::mat3x3(subMeshInfos_[childIndex].mat.model);
					subMeshInfos_[childIndex].mat.invTransModel = glm::mat4x4(glm::inverse(glm::transpose(modelMat3x3)));
				}
			}
		}
		return true;
	}

	GLTFMesh::GLTFMesh(const Device& device, std::string modelPath)
		: MeshBase(device)
	{
		LoadModel(modelPath);
		std::filesystem::path fullPath = modelPath;

		name_ = fullPath.stem().string();

		CreateVertexBuffer();
		CreateIndexBuffer();
	}

	int GLTFMesh::GetMeshNum() const
	{
		return meshNum_;
	}

	int GLTFMesh::GetPrimitiveNumPerMesh(int meshIndex) const
	{
		return primitiveNumPerMesh_[meshIndex];
	}

	GLTFMesh::MeshRange GLTFMesh::GetVertexRange(int primitiveIndex) const
	{
		return vertexRanges_[primitiveIndex];
	}

	GLTFMesh::MeshRange GLTFMesh::GetIndexRange(int primitiveIndex) const
	{
		return indexRanges_[primitiveIndex];
	}

	int GLTFMesh::GetMaterialIndex(int primitiveIndex) const
	{
		return materialIndices_[primitiveIndex];
	}

	const std::vector<GLTFMesh::SubMeshInfo>& GLTFMesh::GetSubMeshInfos() const
	{
		return subMeshInfos_;
	}

	int GLTFMesh::GetNumIndices(int primitiveIndex) const
	{
		return indexRanges_[primitiveIndex].count;
	}
}