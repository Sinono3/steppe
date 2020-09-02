#include "shader.hpp"

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

std::string readFile(const char* filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.reserve(in.tellg());
		in.seekg(0, std::ios::beg);
		std::copy((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>(), std::back_inserter(contents));
		in.close();
		return(contents);
	}
	throw "Unable to open file/File not found."; 
}
void Shader::loadFromText(const char* vs_text, const char* fs_text) 
{
	GLuint vs, fs;
	GLint success;

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_text, NULL);
    glCompileShader(vs);

	/*glGetShaderiv(vs, GL_COMPILE_STATUS, &success); TODO
	if (success == GL_FALSE) 
	{
		GLint length;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &length);

		char* str = malloc(length);
		glGetShaderInfoLog(vs, length, &length, str);

		printerr("Vertex shader error: ");
		printerr(str);
		free(str);

		glDeleteShader(vs);
		throw "Couldn't compile vertex shader.";
	}*/
 
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_text, NULL);
    glCompileShader(fs);
	
	/*glGetShaderiv(fs, GL_COMPILE_STATUS, &success); TODO
	if (success == GL_FALSE) 
	{
		GLint length;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &length);

		char* str = malloc(length);
		glGetShaderInfoLog(fs, length, &length, str);

		printerr("Fragment shader error: ");
		printerr(str);
		free(str);

		glDeleteShader(vs);
		glDeleteShader(fs);
		return GLENN_ERR;
	}*/
 
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

	glDetachShader(program, vs);
	glDetachShader(program, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	
	int uniform_count;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);

	std::vector<GLuint> indices;
	for (int i = 0; i < uniform_count; i++)
		indices.push_back(i);

	std::vector<GLenum> types;
	//std::vector<GLint> array_sizes;

	uniforms.resize(uniform_count);
	types.resize(uniform_count);
	//array_sizes.resize(uniform_count);

	glGetActiveUniformsiv(program, uniform_count, &indices[0], GL_UNIFORM_TYPE, (GLint*)&types[0]);
	//glGetActiveUniformsiv(program, uniform_count, &indices[0], GL_UNIFORM_SIZE, &array_sizes[0]);

	std::array<char, 16> name; // WARNING: 16 chars max

	printf("Uniforms (%d):\n", uniform_count);
	for (int i = 0; i < uniform_count; i++) 
	{
		glGetActiveUniformName(program, (GLuint)i, 16, NULL, &name[0]);

		uniforms[i].name = name;
		uniforms[i].type = types[i];

		printf("  Uniform %d (%s): type=%d\n", i, &uniforms[i].name[0], uniforms[i].type);
	}
}
void Shader::loadFromFiles(const char* vs_path, const char* fs_path)
{
	std::string vs_text, fs_text;

	try
	{
		vs_text = readFile(vs_path);
	}
	catch(char* ex) 
	{
		throw "Couldn't open vertex shader.";
	}
	try
	{
		fs_text = readFile(fs_path);
	}
	catch(char* ex) 
	{
		throw "Couldn't open fragment shader.";
	}
	loadFromText(&vs_text[0], &fs_text[0]);
}
Shader::~Shader() 
{
	glDeleteProgram(program);
}
Material::Material()
{
	shaderId = 0;
}
Material::~Material()
{
	deassignShader();
}
void Material::assignShader(int shaderId, const std::vector<Shader>& shaders) 
{
	if (this->shaderId != 0)
		deassignShader();

	this->shaderId = shaderId;
	
	const Shader& shader = shaders[shaderId];
	uniformValues.reserve(shader.getUniformCount());

	for (int i = 0; i < shader.getUniformCount(); i++) 
	{
		const Uniform& uniform = shader.getUniform(i);
		std::any value;

		switch(uniform.type) 
		{
			case GL_FLOAT:
				value = (GLfloat)0.0;
				break;
			case GL_FLOAT_VEC3:
				value = glm::vec3(0.0);
				break;
			case GL_FLOAT_MAT4:
				value = glm::mat4(0.0);
				break;
			default:
				printf("warning: Uniform %s of type %d not registered when allocating material values. \n", &uniform.name[0], uniform.type);
				value = (GLfloat)0.0;
				break;
		}
		uniformValues.push_back(value);
	}
}
void Material::deassignShader()
{
	shaderId = 0;
	uniformValues.clear();
}
int Material::getShaderId() const
{
	return shaderId;
}
