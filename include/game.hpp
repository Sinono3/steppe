#pragma once
#include <vector>
#include "render.hpp"
#include "shader.hpp"
#include <GLFW/glfw3.h>

struct GameContext 
{
	GLFWwindow* window;
	std::vector<Mesh> meshes;
	std::vector<Shader> shaders; 
	std::vector<Material> materials; 
	flecs::entity activeCamera;
};
