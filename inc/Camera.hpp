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
		glm::vec3 rotation_ = { 0.0f, 0.0f, 0.0f };
		float aspectRatio_;
		float fovYAngle_ = 60.0f;
		float nearZ_ = 0.1f;
		float farZ_ = 100.0f;
		float moveScale_ = 0.0005f;
		float rotateScale_ = 0.0005f;

	public:
		Camera();
		~Camera() = default;
		bool Init(
			float aspectRatio,
			glm::vec3 position,
			float rotateX = 0.0f,
			float rotateY = 0.0f,
			float fovYAngle = 60.0f,
			float nearZ = 0.1f,
			float farZ = 100.0f
		);

		void Update();
		glm::vec4 GetPos();
		glm::vec3 GetFront();
		glm::vec3 GetUp();
		glm::vec3 GetRight();
		glm::mat4x4 GetView();
		glm::mat4x4 GetProj();
		glm::mat4x4 GetInvViewProj();
		glm::mat4x4 GetInvView();
		float GetMoveScale();
		float GetRotateScale();
		float* GetMoveScalePtr();
		float* GetRotateScalePtr();

		void SetMoveScale(float scale);
		void GetRotateScale(float scale);
	};
}