#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	struct TransformMatrix
	{
		glm::mat4x4 model = glm::mat4(1.0f);
		glm::mat4x4 invTransModel = glm::mat4(1.0f);
	};

	class Object
	{
	private:
		MeshHandle mesh_;
		glm::vec4 position_;
		glm::quat quatRotation_;
		glm::vec3 scale_;

	public:
		Object(
			MeshHandle mesh,
			glm::vec4 position = { 0.0f, 0.0f, 0.0f, 1.0f },
			glm::quat quatRotation = { 0.0f, 0.0f, 0.0f, 0.0f },
			float scale = 1.0f
		);
		~Object() = default;

		void UpdateTransform(glm::mat4 model);
		glm::mat4x4 GetModel();
		glm::mat4x4 GetInvTransModel();
		TransformMatrix GetTransform();
		glm::vec3 GetPosition();
		glm::vec3 GetRotation();
		glm::vec3 GetScale();

		void SetPosition(glm::vec3 position);
		void SetRotation(glm::quat quatRotation);
		void SetScale(glm::vec3 scale);
	};
}