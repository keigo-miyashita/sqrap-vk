#pragma once

#include "pch.hpp"

#include "Alias.hpp"

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

	class Mesh
	{
	protected:
		const Device* pDevice_ = nullptr;

		std::string name_ = "Mesh";

		std::vector<Vertex> vertices_;
		BufferHandle vertexBuffer_ = nullptr;
		std::vector<uint32_t> indices_;
		BufferHandle indexBuffer_ = nullptr;

		static bool LoadModel(std::string modelPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
		virtual bool CreateVertexBuffer();
		bool CreateIndexBuffer();
		

	public:
		Mesh(const Device& device, std::string modelPath);
		Mesh(const Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh() = default;

		std::string GetName() const;
		BufferHandle GetVertexBuffer() const;
		BufferHandle GetIndexBuffer() const;
		int GetNumIndices() const;
	};
}