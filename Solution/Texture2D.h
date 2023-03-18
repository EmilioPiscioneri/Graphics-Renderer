#pragma once
#include <glad/glad.h>
#include <iostream>

class Texture2D
{
public:
    // constructor (sets default texture modes)
    Texture2D(std::string name);
    //Texture2D();
    ~Texture2D();
        
    // name of the texture as referebced by the resource manager
    std::string name;
    // holds the ID of the texture object, used for all texture operations to reference to this particular texture
    unsigned int ID = 0;
    // width of loaded image in pixels
    unsigned int width = 0; 
    // height of loaded image in pixels
    unsigned int height = 0; 
    // format of texture object as gl enum
    unsigned int internalFormat = GL_RGB; 
    // format of loaded image as gl enum
    unsigned int imageFormat = GL_RGB; 
    unsigned int wrapS = GL_REPEAT; // wrapping mode on S axis
    unsigned int wrapT = GL_REPEAT; // wrapping mode on T axis
    unsigned int filterMin = GL_NEAREST; // filtering mode if texture pixels < screen pixels
    unsigned int filterMax = GL_LINEAR; // filtering mode if texture pixels > screen pixels
    
    
    // generates texture from image data
    void Generate(unsigned int width, unsigned int height, unsigned char* data);
    // binds the texture as the current active GL_TEXTURE_2D texture object
    void Bind();


};

