#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "components.hpp"
#include "game.hpp"
#include "shader.hpp"
#include "render.hpp"

#include "flecs.h"

class Window 
{
	private:
		GLFWwindow* glfwPointer = nullptr;
		bool initialized = false;

	public:
		Window() { } ~Window() {
			if (initialized) 
			{
				glfwDestroyWindow(glfwPointer);
				glfwTerminate();
			}
		}
		void init()
		{
			if (!glfwInit())
				exit(EXIT_FAILURE);

			//glfwSetErrorCallback(error_callback); TODO

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_RESIZABLE, 0);
			GLFWwindow* window = glfwCreateWindow(1024, 768, "Steppe", NULL, NULL);

			if (!window)
				exit(EXIT_FAILURE);

			//glfwSetKeyCallback(*window, key_callback); TODO
			glfwMakeContextCurrent(window);
			//glfwSwapInterval(1);

			glewExperimental = 1;

			GLenum err = glewInit();
			if (err != GLEW_OK)
			{
				fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
				glfwDestroyWindow(window);
				glfwTerminate();
				exit(EXIT_FAILURE);
			}

			glEnable(GL_DEBUG_OUTPUT);
			//glDebugMessageCallback(MessageCallback, 0); TODO
			glClearColor(0.05, 0.05, 0.08, 1.0);
			//adjustViewport();

			
			glfwPointer = window;
			initialized = true;
		}
		bool shouldClose()
		{
			return glfwWindowShouldClose(glfwPointer);
		}
		/*void adjustViewport() 
		{
			int width, height;
			glfwGetWindowSize(glfwPointer, &width, &height);
			glViewport(0, 0, width, height);
		}*/
		void refresh()
		{
			glfwSwapBuffers(glfwPointer);
			glfwWaitEventsTimeout(0.007f);
		}
		void setCursor(int cursor)
		{
			glfwSetInputMode(glfwPointer, GLFW_CURSOR, cursor);
		}
		GLFWwindow* getPointer()
		{
			return glfwPointer;
		}
};
  
#include <bits/stdc++.h>
int main()
{
	Window window;
	window.init();

	printf("\nSteppe Engine\n\n");

	std::vector<GLfloat> positions
	{
	   -1.0f, 1.0f, 0.0f,   // top left
	   -1.0f,-1.0f, 0.0f,   // bottom left
		1.0f,-1.0f, 0.0f,   // bottom right
		1.0f, 1.0f, 0.0f    // top right
	};
	std::vector<GLuint> indices
	{
		0, 1, 2, 
		0, 2, 3,
	};
	std::vector<Mesh> meshes;
	meshes.emplace_back();
	meshes[0].loadData(positions, indices);

	std::vector<Shader> shaders;
	shaders.emplace_back();

	try 
	{
		shaders[0].loadFromFiles("assets/shaders/static_v.glsl", "assets/shaders/static_f.glsl");
	} 
	catch(char* ex) 
	{
		printf("%s", ex);
		return -1;
	}

	std::vector<Material> materials;
	materials.emplace_back(); // TODO: write a cleaner way to do this
	materials[0].assignShader(0, shaders);
	materials[0].setUniformValue<glm::vec3>(0, glm::vec3(1.0, 0.0, 0.0));

	/*materials.emplace_back();
	materials[1].assignShader(0, shaders);
	materials[1].setUniformValue<glm::vec3>(0, glm::vec3(0.0, 0.0, 1.0));*/

	flecs::world world;

	flecs::component<cmp::Transform>(world, "Transform");
	flecs::component<cmp::RenderMesh>(world, "RenderMesh");
	flecs::component<cmp::Camera>(world, "Camera");
	flecs::component<cmp::FlyMovement>(world, "FlyMovement");

	flecs::system<cmp::Transform, cmp::RenderMesh>(world, "System_Render")
		.action(systems::render);

	flecs::system<cmp::Transform, cmp::FlyMovement>(world, "System_FlyMovement")
		.action(systems::flyMovement);
 
	world.set_target_fps(60);

	{
		auto t = cmp::Transform();
		t.scale = glm::vec3(0.5f, 0.5f, 0.5f);
		{
			auto e = flecs::entity(world, "TestRender")
				.add<cmp::Transform>()
				.add<cmp::RenderMesh>();

			e.set<cmp::Transform>(t);
			e.set<cmp::RenderMesh>({0, 0});
		}
		{
			/*auto e = flecs::entity(world, "TestRender2")
				.add<cmp::Transform>()
				.add<cmp::RenderMesh>();

			t.position.y += 1.0f;

			e.set<cmp::Transform>(t);
			e.set<cmp::RenderMesh>({0, 1});*/
		}
	}
	{
		auto e = flecs::entity(world, "Camera")
			.add<cmp::Transform>()
			.add<cmp::Camera>()
			.add<cmp::FlyMovement>();

		auto t = cmp::Transform();
		t.position = glm::vec3(0.0, 0.0, 2.0);

		e.set<cmp::Transform>(t);
		e.set<cmp::Camera>({1.5f, 4.0f / 3.0f, 0.01f, 1000.0f});
		e.set<cmp::FlyMovement>(cmp::FlyMovement(1.0, 0.02));
	}

	GameContext ctx = 
	{
		window.getPointer(),
		std::move(meshes),
		std::move(shaders),
		std::move(materials),
		world.lookup("Camera")
	};
	world.set_context(&ctx);

	window.setCursor(GLFW_CURSOR_DISABLED);
	while (!window.shouldClose())
	{
		// not adjusting viewport anymore
		// because window is no longer
		// resizable

		world.progress();

		window.refresh();
	}
}
