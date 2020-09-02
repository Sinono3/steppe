// later move all methods here

#include "game.hpp"
#include "components.hpp"
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

namespace cmp 
{
	Transform::Transform() 
	{
		position = glm::vec3(0.0, 0.0, 0.0);
		rotation = glm::quat(0.0, 0.0, 0.0, 1.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
	}

	glm::mat4 Transform::toModelMatrix() const
	{
		glm::mat4 m = glm::mat4(1.0);

		m = m * glm::toMat4(rotation);
		m = glm::translate(m, position);
		m = glm::scale(m, scale);

		return m;
	}
	glm::mat4 Transform::toViewMatrix() const
	{
		glm::mat4 m = glm::mat4(1.0);

		m = m * glm::toMat4(glm::inverse(rotation));
		m = glm::translate(m, -position);

		return m;
	}

	glm::mat4 Camera::getProjectionMatrix() const
	{
		return glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.04f, 10.0f);
	}

	/* TODO: ORTHO VERSION
	 *
	 * glm::mat4 Camera::getProjectionMatrix() const
	{
		return glm::ortho(-2.0, 2.0, -2.0, 2.0);
	}*/
	FlyMovement::FlyMovement()
	{
		this->speed = 5.0f;
		this->mouseSensitivity = 0.5f;
	}
	FlyMovement::FlyMovement(float speed, float mouseSensitivity)
	{
		this->speed = speed;
		this->mouseSensitivity = mouseSensitivity;
	}
}
namespace systems 
{
	void flyMovement(
			flecs::iter& it, 
			flecs::column<cmp::Transform> transforms, 
			flecs::column<cmp::FlyMovement> movements)
	{
		auto ctx = static_cast<GameContext*>(it.world().get_context());
		auto window = ctx->window;

		for (int i : it)
		{
			cmp::Transform& transform = transforms[i];
			cmp::FlyMovement& movement = movements[i];

			glm::vec3 vel(0.0);

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				vel.z -= 1;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				vel.z += 1;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				vel.x -= 1;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				vel.x += 1;

			vel *= movement.speed * it.delta_time();
			vel = glm::rotate(transform.rotation, vel);

			double x = 0.0, y = 0.0;
			glfwGetCursorPos(window, &x, &y);
			
			movement.yaw +=   (float)(x - movement.mouseX) * movement.mouseSensitivity * it.delta_time();
			movement.pitch += (float)(y - movement.mouseY) * movement.mouseSensitivity * it.delta_time();
			movement.mouseX = x;
			movement.mouseY = y;

			printf("x: %d; y: %d\n", (int)movement.pitch, (int)movement.yaw);

			transform.rotation = 
				glm::angleAxis(-movement.yaw, glm::vec3(0.0, 1.0, 0.0)) *
				glm::angleAxis(-movement.pitch, glm::vec3(1.0, 0.0, 0.0));

			transform.position += vel;
		}
	}
}
