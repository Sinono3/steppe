#pragma once
#include <vector>
#include <string>
#include <array>
#include <any>
#include <memory>
#include <string.h>
#include <GL/glew.h>

std::string readFile(const char* filename);

struct Uniform 
{
	std::array<char, 16> name;
	GLenum type;
};
class Shader 
{
	GLuint program;
	std::vector<Uniform> uniforms;

	public:
		Shader() {}
		~Shader();
		
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader(Shader&&) noexcept {}
		Shader& operator=(Shader&&) noexcept { return *this; }

		void loadFromText 	(const char* vs_text, const char* fs_text);
		void loadFromFiles	(const char* vs_path, const char* fs_path);

		void unload	();
		
		GLuint			getProgram() const { return program; };

		int 			getUniformCount() const { return uniforms.size(); };
		const Uniform&	getUniform(int i) const { return uniforms[i]; };
};

class Material 
{
	int shaderId;
	std::vector<std::any> uniformValues;
	
	public:
		Material();
		//Material(const Material&) = delete; // disabling copying because of the void pointers
		Material(const Material&) = delete;
		Material& operator=(const Material&) = delete;
		Material(Material&&) noexcept {}
		Material& operator=(Material&&) noexcept { return *this; }
		/*Material(Material&&);
		Material& operator=(Material&&);*/
		~Material();

		void assignShader(int shaderId, const std::vector<Shader>& shaders);
		void deassignShader();

		int getShaderId() const;
		// WARNING: both get and set are unsafe as
		// they are interacting with raw void pointers
		template<typename T>
		void setUniformValue(int i, T value)
		{
			printf("Setting uniform %d of type %s with value of type %s\n", i, uniformValues[i].type().name(), typeid(T).name());
			fflush(stdout);

			T* ptr = std::any_cast<T>(&uniformValues[i]);
			*ptr = value;
		}
		template<typename T>
		const T& getUniformValue(int i) const
		{
			return *std::any_cast<T>(&uniformValues[i]);
		}
};
