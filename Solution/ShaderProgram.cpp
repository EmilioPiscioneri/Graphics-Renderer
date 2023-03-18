#include "ShaderProgram.h"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp> // used to convert glm matrices to data readable for opengl

ShaderProgram::ShaderProgram(std::string name)
{
	// asign the name variable
	this->name = name;
}

void ShaderProgram::Compile(const char* vertexSource, const char* fragmentSource)
{
	// generate an id for a vertex shader
	unsigned int vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	// attach shader source code to the generated vertex shader
	// param 1: shader id, 2: how many strings passing as source code, 3: source code, 4: unknown
	glShaderSource(vertShaderID, 1, &vertexSource, NULL);
	// compile the shader
	glCompileShader(vertShaderID);
	// error check
	CheckCompileErrors(vertShaderID, ShaderType::Vertex);


	// do the same for frag shader
	unsigned int fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShaderID, 1, &fragmentSource, NULL);
	glCompileShader(fragShaderID);
	// error check
	CheckCompileErrors(fragShaderID, ShaderType::Fragment);

	// create a shader program, returns ID of it
	ID = glCreateProgram();

	// attach shaders to prorgram
	glAttachShader(ID, vertShaderID);
	glAttachShader(ID, fragShaderID);
	// link shaders
	glLinkProgram(ID);

	// error check
	CheckCompileErrors(ID, ShaderType::Program);

	// cleanup shaders don't need them anymore cos they attached
	glDeleteShader(vertShaderID);
	glDeleteShader(fragShaderID);
}



ShaderProgram::~ShaderProgram()
{
	std::cout << "Shader program with name: " << name << " deleted" << std::endl;
}


void ShaderProgram::Use()
{
	// use/activate the shader
	glUseProgram(ID);
}

void ShaderProgram::SetBool(const char* uniformName, bool value)
{
	// get location of uniform
	int uniformLocation = glGetUniformLocation(ID, uniformName);
	if (uniformLocation == -1)
	{
		std::cout << "ERROR: Tried to assign boolean to uniform " << uniformName << " which doesn't exist" << std::endl;
		return;
	}
	// booleans are set to false if value is 0 or 0.0f but is true otherwise. Therefore you can just parse the boolean to integer
	glUniform1i(uniformLocation, (int)value);
}

void ShaderProgram::SetInt(const char* uniformName, int value)
{
	// get location of uniform
	int uniformLocation = glGetUniformLocation(ID, uniformName);
	if (uniformLocation == -1)
	{
		std::cout << "ERROR: Tried to assign integer to uniform " << uniformName << " which doesn't exist" << std::endl;
		return;
	}
	glUniform1i(uniformLocation, value);
}

void ShaderProgram::SetFloat(const char* uniformName, float value)
{
	// get location of uniform
	int uniformLocation = glGetUniformLocation(ID, uniformName);
	if (uniformLocation == -1)
	{
		std::cout << "ERROR: Tried to assign float to uniform " << uniformName << " which doesn't exist" << std::endl;
		return;
	}
	glUniform1f(uniformLocation, value);
}

void ShaderProgram::SetMatrix4(const char* uniformName, glm::mat4 matrix)
{
	// get location of uniform
	int uniformLocation = glGetUniformLocation(ID, uniformName);
	if (uniformLocation == -1)
	{
		std::cout << "ERROR: Tried to assign 4x4 float matrix to uniform " << uniformName << " which doesn't exist" << std::endl;
		return;
	}
	// Params 1: name, 2: how many matrices, 3: whether to swap positions of rows and columns, 4: matrices data which is converted into format for openGL
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ShaderProgram::SetVector3f(const char* uniformName, glm::vec3 vector)
{
	// get location of uniform
	int uniformLocation = glGetUniformLocation(ID, uniformName);
	if (uniformLocation == -1)
	{
		std::cout << "ERROR: Tried to assign float vec3 to uniform " << uniformName << " which doesn't exist" << std::endl;
		return;
	}
	glUniform3f(uniformLocation, vector.x, vector.y, vector.z);
}

void ShaderProgram::CheckCompileErrors(unsigned int objectID, ShaderType type)
{
	int success;
	char infoLog[1024];
	// check for errors and output associated error message if so

	if (type != ShaderType::Program)
	{
		glGetShaderiv(objectID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(objectID, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
	else
	{
		glGetProgramiv(objectID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(objectID, 1024, NULL, infoLog);
			std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
				<< infoLog << "\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
}
