#include "RectangleRenderer.h"
#include "Entity.h"
#include "ResourceManager.h"
// glad is included already thru other include
#include <glfw3.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp> // matrix transformation math

RectangleRenderer::RectangleRenderer(glm::vec3 color, ShaderProgram* program)
{
	// if the program wasn't specified 
	if (program == nullptr)
		// use default path, add time to end to avoid lots of recursion when there are many default programs
		this->shaderProgram = ResourceManager::LoadShaderProgram("defaultRectProgram" + std::to_string(glfwGetTime()), defaultVertPath, defaultFragPath);
	else // else use given one
		this->shaderProgram = program;

	// set type of component
	this->type = Entity::RectangleRenderer;
	// set colour
	this->color = color;

	// initialise rect
	InitRenderData();
}

RectangleRenderer::~RectangleRenderer()
{
	// de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &rectVAO);
	glDeleteBuffers(1, &rectVBO);
	glDeleteBuffers(1, &rectEBO);
}

float RectangleRenderer::GetAlpha()
{
	return _alpha;
}

void RectangleRenderer::SetAlpha(float newAlpha)
{
	// if the new alpha is more than or equal to 1 (idk why it would be)
	if (newAlpha >= 1.0f)
	{
		// cap it to 1 
		_alpha = 1.0f;
		// but also set it so transparency is off
		this->hasTransprency = false;
		// if the current renderer has a parent entity set it to no transparency
		if (_parentEntity != nullptr)
			_parentEntity->SetHasTransparency(false);
	}
	else 
	{
		// do the same but for transparency turned on
		_alpha = newAlpha;
		this->hasTransprency = true;
		// if the current renderer has a parent entity set it to has transparency
		if (_parentEntity != nullptr)
			_parentEntity->SetHasTransparency(true);
	}
}

void RectangleRenderer::Draw(std::shared_ptr<OrthoCamera> camera)
{
	if (_parentEntity == nullptr)
		throw std::exception("Tried to draw a sprite which doesn't have a parent entity");

	shaderProgram->Use();

	// get the transform of this renderer's parent
	Transform spriteTransform = _parentEntity->transform;

	glm::mat4 view = camera->GetViewMatrix();
	shaderProgram->SetMatrix4("view", view);
	

	glm::mat4 projection = camera->GetProjectionMatrix();
	shaderProgram->SetMatrix4("projection", projection);

	// sey sprite transform
	shaderProgram->SetMatrix4("modelTransform", spriteTransform.ToMatrix(camera));
	// set color of rect with alpha channel included
	shaderProgram->SetVector4f("rectColor", glm::vec4(color, _alpha));


	// draw the rect
	glBindVertexArray(this->rectVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void RectangleRenderer::InitRenderData()
{
	// normalised vertics from -1 to 1 on x and y axis. These start as 1s but the size transform changes them
	float vertices[] = {
		// positions        
		1.0f,   1.0f, 0.0f, // top-right
		1.0f,  -1.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f  // top left
	};
	// define what order of vertices to draw rectangle
	unsigned int indices[] = {  // note this is 0 based index
		0, 1, 2,   // first triangle
		2, 3, 0    // second triangle
	};

	// note that the VBO, VAO and EBO are actually just IDs to their values which are handled by opengl

	// vertex buffer object, stores vertices
	glGenBuffers(1, &rectVBO); // generate one buffer 

	// vertex array object, holds all configurations for a VBO/EBO
	glGenVertexArrays(1, &rectVAO); // generate 1 vertex arrya object

	// Element buffer object, stores index to how vertices should be drawn cutting down amount needed
	glGenBuffers(1, &rectEBO); // generate 1 elment buffer object

	glBindVertexArray(rectVAO); // bind the vertex array object 

	glBindBuffer(GL_ARRAY_BUFFER, rectVBO); // bind the generated buffer to array buffer target
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // place vertex data into buffer memory

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectEBO);// bind generated element buffer object
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);// bind indicies to element buffer

	// set vertex attribute position pointer at location 0, with 3 values, of type float, don't normalise data, stride is 3 values, offser of 0 bytes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// enable the created attribute which is at location 0
	glEnableVertexAttribArray(0);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
