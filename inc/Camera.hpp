#pragma once

#include "pch.hpp"

namespace sqrp
{
	struct CameraMatrix
	{
		glm::mat4x4 view;
		glm::mat4x4 proj;
	};

	class Camera
	{
	private:
		glm::vec4 position_ = { 0.0f, 0.0f, 0.0f, 1.0f };
		glm::quat rotation_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		float aspectRatio_;
		float fovYAngle_ = 60.0f;
		float nearZ_ = 0.1f;
		float farZ_ = 100.0f;
		float moveScale_ = 0.0030f;
		float rotateScale_ = 0.060f;

	public:
		Camera();
		~Camera() = default;
		bool Init(
			float aspectRatio,
			glm::vec3 position,
			glm::quat rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)),
			float fovYAngle = 60.0f,
			float nearZ = 0.1f,
			float farZ = 100.0f
		);

		void Update(uint32_t width, uint32_t height);
		glm::vec4 GetPos() const;
		glm::quat GetQuatRotation() const;
		glm::vec3 GetRotation() const;
		glm::vec3 GetFront() const;
		glm::vec3 GetUp() const;
		glm::vec3 GetRight() const;
		glm::mat4x4 GetView() const;
		glm::mat4x4 GetProj() const;
		glm::mat4x4 GetInvViewProj() const;
		glm::mat4x4 GetInvView() const;
		glm::mat4x4 GetInvProj() const;
		float GetMoveScale() const;
		float GetRotateScale() const;
		float GetNearClip() const;
		float GetFarClip() const;
		float* GetMoveScalePtr();
		float* GetRotateScalePtr();

		void SetMoveScale(float scale);
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::quat rotation);
		void SetRotateScale(float scale);
	};
}