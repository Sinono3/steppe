#pragma once
#include <flecs.h>
#include <GL/glew.h>
#include <vector>
#include "game.hpp"
#include "components.hpp"

namespace systems
{
	void render(
			flecs::iter& it, 
			flecs::column<cmp::Transform> transforms, 
			flecs::column<cmp::RenderMesh> renders);
}

enum 
{
	STP_MESHFLAG_POS    = 1 << 0,
	STP_MESHFLAG_NORMAL = 1 << 1,
	STP_MESHFLAG_UV     = 1 << 2,
};
class Mesh 
{
	GLuint vao;
	GLuint vbos[4]; // positions, indexes, normals and UVs in that order
	int vertexCount;
	int flags;

	public:
		~Mesh();
		void loadData(std::vector<GLfloat> positions, std::vector<GLuint> indices);

		void render();
};
