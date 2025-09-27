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

	glm::mat4x4 Object::GetModelMat()
	{
		return glm::translate(glm::vec3(position_)) * glm::toMat4(quatRotation_) * glm::scale(scale_);
	}

	glm::mat4x4 Object::GetInvTransMat()
	{
		return glm::inverse(GetModelMat());
	}

	void Object::SetPosition(glm::vec4 position)
	{
		position_ = position;
	}

	void Object::SetRotation(glm::quat quatRotation)
	{
		quatRotation_ = quatRotation;
	}

	void Object::SetScale(float scale)
	{
		scale_ = glm::vec3(scale);
	}
}