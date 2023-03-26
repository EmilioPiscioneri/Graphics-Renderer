#include "EllipseRenderer.h"
#include "Entity.h"
#include "ResourceManager.h"
// glad is included already thru other include
#include <glfw3.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp> // matrix transformation math

EllipseRenderer::EllipseRenderer(glm::vec3 color, ShaderProgram* program)
{
	// if the program wasn't specified 
	if (program == nullptr)
		// use default path, add time to end to avoid lots of recursion when there are many default programs
		this->shaderProgram = ResourceManager::LoadShaderProgram("defaultEllipseProgram" + std::to_string(glfwGetTime()), defaultVertPath, defaultFragPath);
	else // else use given one
		this->shaderProgram = program;

	// the smoothing shader makes use of transparency and blending to look nice
	this->hasTransprency = true;
	// set type of component
	this->type = Entity::EllipseRenderer;
	// set colour
	this->color = color;

	// initialise sprite rect
	InitRenderData();
}

EllipseRenderer::~EllipseRenderer()
{
	// de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &rectVAO);
	glDeleteBuffers(1, &rectVBO);
	glDeleteBuffers(1, &rectEBO);
}

float EllipseRenderer::GetAlpha()
{
	return _alpha;
}

void EllipseRenderer::SetAlpha(float newAlpha)
{
	// just set the alpha, it doesn't actually matter as the smoothing effect must have transparency turned on no matter what
	_alpha = newAlpha;
	
}

void EllipseRenderer::Draw(std::shared_ptr<OrthoCamera> camera)
{
	
	if (parentEntity == nullptr)
		throw std::exception("Tried to draw a sprite which doesn't have a parent entity");

	shaderProgram->Use();

	// get the transform of this renderer's parent
	Transform ellipseTransform = parentEntity->transform;

	glm::mat4 view = camera->GetViewMatrix();
	shaderProgram->SetMatrix4("view", view);


	glm::mat4 projection = camera->GetProjectionMatrix();
	shaderProgram->SetMatrix4("projection", projection);

	// sey sprite transform
	shaderProgram->SetMatrix4("modelTransform", ellipseTransform.ToMatrix(camera));
	// set color of ellipse with alpha channel included
	shaderProgram->SetVector4f("ellipseColor", glm::vec4(color, _alpha));
	// --- Calculate different values that the fragment shader uses to calculate whether a pixel of the rect is in ellipse bounds ---

	// get the global pos of the current ellipse as it will be its actual position in global coords. We want this value relative to camera so do - camera position
	glm::vec2 ellipseGlobalPosition = ellipseTransform.GetGlobalPosition(camera) - camera->position;

	// get the centre (h,k) of the current ellipse in pixel/global coords. The position of an ellipse is at the bottom-left so add half width and height to get actual centre
	glm::vec2 ellipseCentre = ellipseGlobalPosition + glm::vec2 (ellipseTransform.GetGlobalSize(camera) / 2.0f);

	// get the x radius by doing the centreX - bottomLeftPositionX
	// E.g.  if centreX is 5 and position is 2 then the difference between 2 and 5 is 3 or 5 - 3 or centreX - positionX
	float radiusX = ellipseCentre.x - ellipseGlobalPosition.x;

	// same logic then applies to y axis
	float radiusY = ellipseCentre.y - ellipseGlobalPosition.y;

	// convert to radians because trig functinos don't take degrees
	float zRotationInRadians = glm::radians(ellipseTransform.rotation.z);
	// cache what the sine and cosine of the rotation in radians is
	float sinZRotation = sin(zRotationInRadians);
	float cosZRotation = cos(zRotationInRadians);

	// -- Send the calculated values --

	shaderProgram->SetVector2f("ellipseCentre", ellipseCentre);
	shaderProgram->SetFloat("radiusX", radiusX);
	shaderProgram->SetFloat("radiusY", radiusY);
	shaderProgram->SetFloat("sinModelZRotation", sinZRotation);
	shaderProgram->SetFloat("cosModelZRotation", cosZRotation);


	//shaderProgram->SetVector2f("objectCentre", glm::vec2(ellipseTransform.position.x + ellipseTransform.size.x/2.0f, ellipseTransform.position.y + ellipseTransform.size.y/2.0f ));


	// draw the rect
	glBindVertexArray(this->rectVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	glBindVertexArray(0);
}

void EllipseRenderer::InitRenderData()
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

	// set vertex attribute position pointer at location 0, with 3 values, of type float, don't normalise data, stride is 5 values, offser of 0 bytes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// enable the created attribute which is at location 0
	glEnableVertexAttribArray(0);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
