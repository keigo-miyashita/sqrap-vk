#include "Camera.hpp"

#include "Application.hpp"

using namespace std;
using namespace glm;

namespace sqrp
{
	Camera::Camera()
	{

	}

	bool Camera::Init(
		float aspectRatio,
		vec3 position,
		float rotateX,
		float rotateY,
		float fovYAngle,
		float nearZ,
		float farZ
	)
	{
		aspectRatio_ = aspectRatio;
		position_ = { position.x, position.y, position.z, 1.0f };
		rotation_ = vec3(rotateX, rotateY, 0.0f);
		fovYAngle_ = fovYAngle;
		nearZ_ = nearZ;
		farZ_ = farZ;

		return true;
	}

	void Camera::Update(uint32_t width, uint32_t height)
	{
		vec3 front = GetFront();
		vec3 right = GetRight();
		vec3 up = GetUp();
		if (Input::IsPushKey(GLFW_KEY_W)) {
			position_.x += front.x * moveScale_;
			position_.y += front.y * moveScale_;
			position_.z += front.z * moveScale_;
		}
		if (Input::IsPushKey(GLFW_KEY_S)) {
			position_.x -= front.x * moveScale_;
			position_.y -= front.y * moveScale_;
			position_.z -= front.z * moveScale_;
		}
		if (Input::IsPushKey(GLFW_KEY_D)) {
			position_.x += right.x * moveScale_;
			position_.y += right.y * moveScale_;
			position_.z += right.z * moveScale_;
		}
		if (Input::IsPushKey(GLFW_KEY_A)) {
			position_.x -= right.x * moveScale_;
			position_.y -= right.y * moveScale_;
			position_.z -= right.z * moveScale_;
		}
		if (Input::IsPushKey(GLFW_KEY_SPACE)) {
			position_.x += up.x * moveScale_;
			position_.y += up.y * moveScale_;
			position_.z += up.z * moveScale_;
		}
		if (Input::IsPushKey(GLFW_KEY_LEFT_CONTROL)) {
			position_.x -= up.x * moveScale_;
			position_.y -= up.y * moveScale_;
			position_.z -= up.z * moveScale_;
		}

		if (Input::IsPushedLButton()) {
			auto pushedMousePos = Input::GetPushedPos();
			auto prevMousePos = Input::GetPrevPos();
			auto currentMousePos = Input::GetPos();

			if (Input::GetDeltaPos().y != 0 && Input::GetDeltaPos().x != 0 ) {
				rotation_.x += Input::GetDeltaPos().y * rotateScale_;
				rotation_.y += Input::GetDeltaPos().x * rotateScale_;
			}
		}

		aspectRatio_ = (float)(width) / (float)(height);
	}

	vec4 Camera::GetPos()
	{
		return position_;
	}

	vec3 Camera::GetFront()
	{
		mat4x4 identity = glm::mat4(1.0f);
		// Rotate X + -> counter clockwise -> camera direction turn up
		mat4x4 rotationX = glm::rotate(identity, -rotation_.x, glm::vec3{ 1, 0, 0 });
		// Rotate Y + -> counter clockwise -> camera direction turn right
		mat4x4 rotationY = glm::rotate(identity, -rotation_.y, glm::vec3{ 0, 1, 0 });

		vec4 frontDir = vec4(0.0f, 0.0f, -1.0f, 1.0f);

		frontDir = rotationY * rotationX * frontDir;

		return normalize(glm::vec3(frontDir));
	}

	vec3 Camera::GetUp()
	{
		vec3 frontDir = GetFront();
		vec3 rightDir = GetRight();

		return normalize(glm::cross(rightDir, frontDir));
	}

	vec3 Camera::GetRight()
	{
		vec3 frontDir = GetFront();
		vec3 yPlusDir = vec3(0.0f, 1.0f, 0.0f);

		return normalize(glm::cross(frontDir, yPlusDir));
	}

	mat4x4 Camera::GetView()
	{
		return glm::lookAt(vec3(position_), vec3(position_) + GetFront(), vec3(0, 1, 0));
	}

	mat4x4 Camera::GetProj()
	{
		return glm::perspective(glm::radians(fovYAngle_), aspectRatio_, nearZ_, farZ_);
	}

	mat4x4 Camera::GetInvViewProj()
	{
		return glm::inverse(GetProj());
	}

	mat4x4 Camera::GetInvView()
	{
		return glm::inverse(GetView());
	}

	glm::mat4x4 Camera::GetInvProj()
	{
		return glm::inverse(GetProj());
	}

	float Camera::GetMoveScale()
	{
		return moveScale_;
	}

	void Camera::SetRotation(glm::vec3 rotation)
	{
		rotation_ = rotation;
	}

	float Camera::GetRotateScale()
	{
		return rotateScale_;
	}

	float* Camera::GetMoveScalePtr()
	{
		return &moveScale_;
	}

	float* Camera::GetRotateScalePtr()
	{
		return &rotateScale_;
	}

	float Camera::GetNearClip()
	{
		return nearZ_;
	}

	float Camera::GetFarClip()
	{
		return farZ_;
	}

	void Camera::SetMoveScale(float scale)
	{
		moveScale_ = scale;
	}

	void Camera::GetRotateScale(float scale)
	{
		rotateScale_ = scale;
	}
}