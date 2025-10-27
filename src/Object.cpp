#include "Object.hpp"

#include "Mesh.hpp"

using namespace std;

namespace sqrp
{
	Object::Object(MeshHandle mesh,
		glm::vec4 position,
		glm::quat quatRotation,
		float scale)
		: mesh_(mesh), position_(position), quatRotation_(quatRotation), scale_(glm::vec3(scale))
	{

	}

	void Object::UpdateTransform(glm::mat4 model)
	{
		glm::vec3 translation, scale, skew;
		glm::vec4 perspective;
		glm::quat quat;
		glm::decompose(model, scale, quat, translation, skew, perspective);

		position_ = glm::vec4(translation, 1.0f);
		quatRotation_ = quat;
		scale_ = scale;
	}

	glm::mat4x4 Object::GetModel()
	{
		return glm::translate(glm::vec3(position_)) * glm::toMat4(quatRotation_) * glm::scale(scale_);
	}

	glm::mat4x4 Object::GetInvTransModel()
	{
		return glm::inverse(GetModel());
	}

	TransformMatrix Object::GetTransform()
	{
		TransformMatrix transformMatrix;
		transformMatrix.model = GetModel();
		transformMatrix.invTransModel = GetInvTransModel();
		return transformMatrix;
	}

	glm::vec3 Object::GetPosition()
	{
		return glm::vec3(position_);
	}

	glm::vec3 Object::GetRotation()
	{
		return glm::eulerAngles(quatRotation_);
	}

	glm::vec3 Object::GetScale()
	{
		return scale_;
	}

	void Object::SetPosition(glm::vec3 position)
	{
		position_ = glm::vec4(position, 1.0f);
	}

	void Object::SetRotation(glm::quat quatRotation)
	{
		quatRotation_ = quatRotation;
	}

	void Object::SetScale(glm::vec3 scale)
	{
		scale_ = scale;
	}
}