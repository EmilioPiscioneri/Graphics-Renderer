#pragma once

#include <glm/glm.hpp> // OpenGL maths: Include all GLM core / GLSL features
#include <iostream>

// Make a a shader program easily
class ShaderProgram
{	
public: 
	// ID of shader program default is 0
	unsigned int ID = 0;
	
	// name of the shader prorgram which can be indexed through resource manager
	std::string name;
	// Reads in a vertex and fragment shader from files, then builds the shaders, optional boolean reference is whether or not everything built without errors
	ShaderProgram(std::string name);
	//ShaderProgram();
	~ShaderProgram();

	// compiles the shader from given source code
	void Compile(const char* vertexSource, const char* fragmentSource);

	// use/activate the shader
	void Use();
	// set a boolean uniform for shader program
	void SetBool(const char* uniformName, bool value);
	// set an integer uniform for shader program
	void SetInt(const char* uniformName, int value);
	// set a float uniform for shader program
	void SetFloat(const char* uniformName, float value);
	// set a 4x4 float matrix uniform 
	void SetMatrix4(const char* uniformName, glm::mat4 matrix);
	// set a float vector 3 uniform
	void SetVector3f(const char* uniformName, glm::vec3 vector);
	// set a float vector 2 uniform
	void SetVector2f(const char* uniformName, glm::vec2 vector);
private:
	// used to check for compile errors
	enum ShaderType {
		Vertex,
		Fragment,
		Geometry,
		Program
	};

	// checks for compile errors based on given shader type
	void CheckCompileErrors(unsigned int objectID, ShaderType type);
};