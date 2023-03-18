#include "Texture2D.h"

Texture2D::Texture2D(std::string name)
{
	this->name = name;
	// generate the id of the texture
	glGenTextures(1, &this->ID);
}


Texture2D::~Texture2D()
{
	std::cout << "Texture 2D with name " << name << " deleted" << std::endl;
}

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
	this->width = width;
	this->height = height;
	glGenTextures(1, &ID); // generate a texture
	glBindTexture(GL_TEXTURE_2D, ID); // bind the generated texture

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS); // x axis wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT); // y axis wrapping
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin); // when texture is minified
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax); // when texture is magnified

	// Param 1: texture target,2: mipmap level,3: format, 4&5:width & height, 6: leave 0 legacy stuff, 7&8: format and type of data the source image is stored as
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
	
	glGenerateMipmap(GL_TEXTURE_2D); // generate mipmaps to ensure better quality for small textures

	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Texture2D::Bind()
{
	// Bind the current texture so you can set it as a texture unit in a fragment shader 
	glBindTexture(GL_TEXTURE_2D, ID);
}