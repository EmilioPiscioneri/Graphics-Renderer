#pragma once
#include <iostream>
#include <map>
#include "ShaderProgram.h"
#include "Texture2D.h"

// based off
// https://learnopengl.com/code_viewer_gh.php?code=src/7.in_practice/3.2d_game/0.full_source/resource_manager.h
// Handles resources that need to be loaded in via a file
class ResourceManager
{
public:
    // map of all shader programs in file, indexed by name
    static std::map<std::string, ShaderProgram> shaderPrograms;
    // map of all textures in file indexed by name
    static std::map<std::string, Texture2D> textures;
    // Loads (and stores in map) a shader program under specified name from vertex and fragment shader files. 
    // "1" is added to name if it already exists 
    static ShaderProgram* LoadShaderProgram(std::string name, const char* vertShaderFilePath, const char* fragShaderFilePath);
    // retrieves a stored sader as pointer. Nullptr if not found
    static ShaderProgram* GetShader(std::string name);
    // loads (and stores) a texture from file under specified name. 
    // "1" is added to name if it already exists
    static Texture2D* LoadTexture(std::string name, const char* file, bool alpha);
    // retrieves a stored texture as pointer. Nullptr if not found
    static Texture2D* GetTexture(std::string name);
    // properly de-allocates all loaded resources
    static void Clear();
private:
    // whether an item of specific name exists in a map (passed as pointer) is a template in case more maps are added
    template <typename T>
    static bool ItemExistsInMap(std::string name, std::map<std::string, T>& inputMap );
    // Checks if desired name doesn't exist in map (passed as pointer) and keeps adding " 1" to name until there is an available unused name 
    template <typename T>
    static std::string GetValidNameForMap(std::string inputName, std::map<std::string, T>& inputMap);

    // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager();
    // loads and generates a shader progran from shader files, with specified name
    static ShaderProgram loadShaderProgramFromFiles(std::string name, const char* vShaderFile, const char* fShaderFile);
    // loads a single texture from file with specified name
    static Texture2D loadTextureFromFile(std::string name, const char* filePath, bool alpha);
};
