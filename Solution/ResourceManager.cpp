#include "ResourceManager.h"

#include <fstream>
#include <sstream>

// store current state of warnings
#pragma warning ( push )
// turn off annoying warnings
#pragma warning ( disable: 26451 )
#pragma warning ( disable: 6262 )

// define for image library
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// put warnings back on
#pragma warning ( pop )

std::map<std::string, ShaderProgram> ResourceManager::shaderPrograms;
std::map<std::string, Texture2D> ResourceManager::textures;


ShaderProgram* ResourceManager::LoadShaderProgram(std::string name, const char* vShaderFile, const char* fShaderFile)
{
	// change the name to one that is available in map. Adds "1" until there is an available name
	name = GetValidNameForMap<ShaderProgram>(name, shaderPrograms);
	// create a shader program
	ShaderProgram program = loadShaderProgramFromFiles(name, vShaderFile, fShaderFile);
	// add it to map
	shaderPrograms.insert(std::pair<std::string, ShaderProgram>(name, program) );
	// return pointer to program (at is used instead of [] because it requires a default constructor) 
	// program will go out of scope if don't return pointer which is a big no no
	return &shaderPrograms.at(name);
}

ShaderProgram* ResourceManager::GetShader(std::string name)
{
	// return pointer if found, else not because .at() will throw exception
	if (ItemExistsInMap(name, shaderPrograms))
		return &shaderPrograms.at(name); // use at instead of [] because it requires a default constructor
	else
		// not found
		return nullptr;
}

Texture2D* ResourceManager::LoadTexture(std::string name, const char* file, bool alpha)
{
	// change the name to one that is available in map. Adds "1" until there is an available name
	name = GetValidNameForMap<Texture2D>(name, textures);
	// create new texture
	Texture2D texture = loadTextureFromFile(name, file, alpha);
	// add to map
	textures.insert(std::pair<std::string, Texture2D>(name, texture));
	// return pointer to texture (at is used instead of [] because it requires a default constructor) 
	//Texture will go out of scope if don't return pointer which is a big no no
	return &textures.at(name);
}

Texture2D* ResourceManager::GetTexture(std::string name)
{
	// return pointer if found, else not because .at() will throw exception
	if (ItemExistsInMap<Texture2D>(name, textures))
		return &textures.at(name); // use at instead of [] because it requires a default constructor
	else
		// not found
		return nullptr;
}

void ResourceManager::Clear()
{
	// TODO: check if objects get destroyed without using delete
	for (std::pair<std::string, ShaderProgram> iterator : shaderPrograms)
		// delete the program 
		glDeleteProgram(iterator.second.ID);
	for (std::pair<std::string, Texture2D> iterator : textures)
		// delete the texture
		glDeleteTextures(1, &iterator.second.ID);
	// erase all map elements
	shaderPrograms.clear();
	textures.clear();


}

ShaderProgram ResourceManager::loadShaderProgramFromFiles(std::string name, const char* vertShaderFilePath, const char* fragShaderFilePath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream (read file stream) objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files for reading
		vShaderFile.open(vertShaderFilePath);
		fShaderFile.open(fragShaderFilePath);
		// string streams that the file stream writes to
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	// on error reading file
	catch (std::exception e)
	{
		std::cout << "ERROR: shader file not successfully read" << std::endl;
	}
	// convert shaders into char arrays
	const char* vertShaderCode = vertexCode.c_str();
	const char* fragShaderCode = fragmentCode.c_str();

	// create shader program 
	ShaderProgram program = ShaderProgram(name);
	// compile the source code
	program.Compile(vertShaderCode, fragShaderCode);
	// return created program
	return program;
}

Texture2D ResourceManager::loadTextureFromFile(std::string name, const char* filePath, bool alpha)
{
	// create a texture object
	Texture2D texture = Texture2D(name);
	// set appropriate format if alpha channel is active
	if (alpha)
	{
		texture.internalFormat = GL_RGBA;
		texture.imageFormat = GL_RGBA;
	}

	// width and height of image along with number of colour channels 
	int width, height, numChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis. It starts with y on the top which is not cool cos opengl is y on bottom

	// load in the image file. Idk what the 0 is for
	unsigned char* imageData = stbi_load(filePath, &width, &height, &numChannels, 0);

	if (imageData)
	{
		// generate the texture
		texture.Generate(width, height, imageData);
	}
	else
	{
		throw std::exception("Failed to load texture");
	}
	// no longer need image data
	stbi_image_free(imageData);
	// return the created texture
	return texture;
}

template<typename T>
bool ResourceManager::ItemExistsInMap(std::string name, std::map<std::string, T>& inputMap)
{
    // return whether or not the item exists in map
    return (inputMap.find(name) != inputMap.end());
}

template<typename T>
std::string ResourceManager::GetValidNameForMap(std::string inputName, std::map<std::string, T>& inputMap)
{
    // start with the input name and then keep adding "1" until you find a unique name
    std::string outputName = inputName;

    // while there is an element with the same name as desired one in map
    while (ItemExistsInMap<T>(outputName, inputMap))
    {
        outputName += "1"; // add 1 to the end of the name
    }

    return outputName;
}