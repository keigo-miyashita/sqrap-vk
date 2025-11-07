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
		glm::quat rotation,
		float fovYAngle,
		float nearZ,
		float farZ
	)
	{
		aspectRatio_ = aspectRatio;
		position_ = { position.x, position.y, position.z, 1.0f };
		rotation_ = rotation;
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

			if (!(Input::GetDeltaPos().y == 0 && Input::GetDeltaPos().x == 0 )) {
				float yaw = glm::radians((float)Input::GetDeltaPos().x) * rotateScale_ * -1; // rotation around Y
				float pitch = glm::radians((float)Input::GetDeltaPos().y) * rotateScale_ * -1; // rotation around X

				glm::quat qYaw = glm::angleAxis(yaw, GetUp());
				glm::quat qPitch = glm::angleAxis(pitch, GetRight());
				rotation_ = qYaw * rotation_;
				glm::vec3 pitchRotation = glm::degrees(glm::eulerAngles(qPitch));
				rotation_ = qPitch * rotation_;

				//rotation_ = qYaw * qPitch * rotation_;
				//rotation_ = glm::normalize(rotation_);
			}
		}

		aspectRatio_ = (float)(width) / (float)(height);
	}

	vec4 Camera::GetPos() const
	{
		return position_;
	}

	glm::vec3 Camera::GetRotation() const
	{
		return glm::degrees(glm::eulerAngles(rotation_));
	}

	glm::quat Camera::GetQuatRotation() const
	{
		return rotation_;
	}

	vec3 Camera::GetFront() const
	{
		/*vec4 frontDir = vec4(0.0f, 0.0f, -1.0f, 1.0f);
		frontDir = rotation_ * frontDir;*/

		return normalize(rotation_ * vec3(0.0f, 0.0f, -1.0f));
	}

	vec3 Camera::GetUp() const
	{
		vec3 frontDir = GetFront();
		vec3 rightDir = GetRight();

		return normalize(glm::cross(rightDir, frontDir));
		//return normalize(rotation_ * vec3(0.0f, 1.0f, 0.0f));
	}

	vec3 Camera::GetRight() const
	{
		vec3 frontDir = GetFront();
		vec3 yPlusDir = vec3(0.0f, 1.0f, 0.0f);

		return normalize(glm::cross(frontDir, yPlusDir));
		//return normalize(rotation_ * vec3(1.0f, 0.0f, 0.0f));
	}

	mat4x4 Camera::GetView() const
	{
		return glm::lookAt(vec3(position_), vec3(position_) + GetFront(), vec3(0, 1, 0));
	}

	mat4x4 Camera::GetProj() const
	{
		return glm::perspective(glm::radians(fovYAngle_), aspectRatio_, nearZ_, farZ_);
	}

	mat4x4 Camera::GetInvViewProj() const
	{
		return glm::inverse(GetProj());
	}

	mat4x4 Camera::GetInvView() const
	{
		return glm::inverse(GetView());
	}

	glm::mat4x4 Camera::GetInvProj() const
	{
		return glm::inverse(GetProj());
	}

	float Camera::GetMoveScale() const
	{
		return moveScale_;
	}

	float Camera::GetRotateScale() const
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

	float Camera::GetNearClip() const
	{
		return nearZ_;
	}

	float Camera::GetFarClip() const
	{
		return farZ_;
	}

	void Camera::SetMoveScale(float scale)
	{
		moveScale_ = scale;
	}

	void Camera::SetPosition(glm::vec3 position)
	{
		position_ = glm::vec4(position, 1.0f);
	}

	void Camera::SetRotation(glm::quat rotation)
	{
		rotation_ = rotation;
	}

	void Camera::SetRotateScale(float scale)
	{
		rotateScale_ = scale;
	}
}