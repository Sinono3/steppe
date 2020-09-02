#pragma once
#include <flecs.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>

namespace cmp 
{
	struct Transform
	{
		glm::vec3 position;
		glm::quat rotation;   // quaternion
		glm::vec3 scale;

		glm::mat4 toModelMatrix() const;
		glm::mat4 toViewMatrix() const;

		Transform();
	};
	struct RenderMesh 
	{
		int meshId;
		int materialId;
	};
	struct Camera
	{
		float fov;
		float aspect;
		float near;
		float far;

		glm::mat4 getProjectionMatrix() const;
	};
	struct FlyMovement
	{
		float speed;
		float mouseSensitivity;

		double mouseX;
		double mouseY;

		//glm::vec3 velocity;
		float pitch;
		float yaw;

		FlyMovement();
		FlyMovement(float speed, float mouseSensitivity);
	};
}
namespace systems 
{
	void flyMovement(
			flecs::iter& it, 
			flecs::column<cmp::Transform> transforms, 
			flecs::column<cmp::FlyMovement> movements);
}
