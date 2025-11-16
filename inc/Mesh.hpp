#pragma once

#include "pch.hpp"

#include "Alias.hpp"

#include "Object.hpp"

namespace sqrp
{
	class Buffer;
	class Device;

	struct Vertex
	{
		glm::vec4 position = { 0.0f, 0.0f, 0.0f, 1.0f };
		glm::vec4 normal = { 0.0f, 0.0f, 0.0f, 1.0f };
		glm::vec4 tangent = { 0.0f, 0.0f, 0.0f, 1.0f };
		glm::vec2 uv = { 0.0f, 0.0f };
	};

	class MeshBase
	{
	protected:
		const Device* pDevice_ = nullptr;

		std::string name_ = "Mesh";

		std::vector<Vertex> vertices_;
		BufferHandle vertexBuffer_ = nullptr;
		std::vector<uint32_t> indices_;
		BufferHandle indexBuffer_ = nullptr;

		virtual bool CreateVertexBuffer();
		bool CreateIndexBuffer();


	public:
		MeshBase(const Device& device);
		~MeshBase() = default;

		std::string GetName() const;
		BufferHandle GetVertexBuffer() const;
		BufferHandle GetIndexBuffer() const;
		virtual int GetNumIndices() const;
	};

	// For simple mesh
	class Mesh : public MeshBase
	{
	protected:

		bool LoadModel(std::string modelPath);
		

	public:
		Mesh(const Device& device, std::string modelPath);
		Mesh(const Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh() = default;

	};

	// For detailed glTF mesh
	class GLTFMesh : public MeshBase
	{
		struct MeshRange
		{
			uint32_t offset = 0;
			uint32_t count = 0;
		};

		struct SubMeshInfo
		{
			TransformMatrix mat = {};
			int meshIndex = 0;
		};
	protected:
		int meshNum_ = 0;
		std::vector<int> primitiveNumPerMesh_;
		// Mesh data is saved per primitive of glTF mesh (= per material)
		// key : (meshIndex, primitiveIndex)
		std::map<std::pair<int, int>, MeshRange> vertexRanges_;
		std::map<std::pair<int, int>, MeshRange> indexRanges_;
		std::map<std::pair<int, int>, int> materialIndices_;

		// SubMeshInfo per node (except for nodes without mesh)
		std::vector<SubMeshInfo> subMeshInfos_;

		bool LoadModel(std::string modelPath);


	public:
		GLTFMesh(const Device& device, std::string modelPath);
		~GLTFMesh() = default;

		int GetMeshNum() const;
		int GetPrimitiveNumPerMesh(int meshIndex) const;
		MeshRange GetVertexRange(int meshIndex, int primitiveIndex) const;
		MeshRange GetIndexRange(int meshIndex, int primitiveIndex) const;
		int GetMaterialIndex(int meshIndex, int primitiveIndex) const;
		const std::vector<SubMeshInfo>& GetSubMeshInfos() const;
		int GetNumIndices(int meshIndex, int primitiveIndex) const;

	};
}