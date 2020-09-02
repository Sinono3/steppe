#include "game.hpp"
#include "render.hpp"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtc/type_ptr.hpp>

Mesh::~Mesh() 
{
	glDeleteBuffers(4, &(vbos[0]));
	glDeleteVertexArrays(1, &(vao));
}
void Mesh::loadData(std::vector<GLfloat> positions, std::vector<GLuint> indices) 
{
	glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	// TODO: I don't know if generating unused buffers is a good idea
	glGenBuffers(4, &vbos[0]);

	// Load position vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * positions.size(), &positions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // 0 attrib will always be vertex position

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	flags = STP_MESHFLAG_POS; // add pos to flags

	// Load indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

	vertexCount = indices.size();

	printf("Loaded model with VAO %d\n", vao);
}
void Mesh::render()
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0); // assumes pos was loaded
	if (flags & STP_MESHFLAG_NORMAL)
		glEnableVertexAttribArray(1);
	if (flags & STP_MESHFLAG_UV)
		glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
	glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
	if (flags & STP_MESHFLAG_NORMAL)
		glDisableVertexAttribArray(1);
	if (flags & STP_MESHFLAG_UV)
		glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}

namespace systems
{
	void render(
			flecs::iter& it, 
			flecs::column<cmp::Transform> transforms, 
			flecs::column<cmp::RenderMesh> renders)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto ctx = static_cast<GameContext*>(it.world().get_context());

		std::vector<Mesh>& meshes = ctx->meshes;
		std::vector<Shader>& shaders = ctx->shaders;
		std::vector<Material>& materials= ctx->materials;

		flecs::entity camera = ctx->activeCamera;//it.world().lookup("Camera");
		glm::mat4 viewMatrix = camera.get<cmp::Transform>()->toViewMatrix();
		glm::mat4 projectionMatrix = camera.get<cmp::Camera>()->getProjectionMatrix();

		for (int i : it) 
		{
			// TEMPORARY: test spin
			/*if (i == 0) 
			{
				transforms[i].rotation = transforms[i].rotation * glm::angleAxis<float>(0.02, glm::vec3(0.0, 0.0, 1.0));
			}*/
			
			glm::mat4 m = transforms[i].toModelMatrix();

			Material& material = materials[renders[i].materialId];
			Shader& shader = shaders[material.getShaderId()];

			glUseProgram(shader.getProgram());

			for (int u = 0; u < shader.getUniformCount(); u++) 
			{
				auto uniform = shader.getUniform(u);

				if (strcmp(&uniform.name[0], "time") == 0)
				{
					glUniform1f(u, glfwGetTime());
					continue;
				}
				if (strcmp(&uniform.name[0], "m_Model") == 0)
				{
					glUniformMatrix4fv(u, 1, GL_FALSE, glm::value_ptr(m));
					continue;
				}
				if (strcmp(&uniform.name[0], "m_View") == 0)
				{
					glUniformMatrix4fv(u, 1, GL_FALSE, glm::value_ptr(viewMatrix));
					continue;
				}
				if (strcmp(&uniform.name[0], "m_Projection") == 0)
				{
					glUniformMatrix4fv(u, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
					continue;
				}

				switch(uniform.type) 
				{
					case GL_FLOAT:
						{
							auto f = material.getUniformValue<GLfloat>(u);
							glUniform1f(u, f);
							break;
						}
					case GL_FLOAT_VEC3:
						{
							//auto fvec3 = material.getUniformValue<glm::vec3>(u);
							glUniform3fv(u, 1, glm::value_ptr(glm::vec3(1.0, 0.0, 1.0)));
							break;
						}
					case GL_FLOAT_MAT4:
						{
							auto m4x4 = material.getUniformValue<glm::mat4>(u);
							glUniformMatrix4fv(u, 1, GL_FALSE, glm::value_ptr(m4x4));
							break;
						}
					default:
						{
							printf("warning: Uniform of type %d not registered. \n", uniform.type);
							break;
						}
				}
			}

			meshes[renders[i].meshId].render();

			glUseProgram(0);
		}
	}
}
