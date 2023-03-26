#include "LineRenderer.h"
#include "Entity.h"
#include "ResourceManager.h"
// glad is included already thru other include
#include <glfw3.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp> // matrix transformation math

LineRenderer::LineRenderer(glm::vec2 point1, glm::vec2 point2, float thickness, glm::vec3 color, ShaderProgram* program)
{
	// if the program wasn't specified 
	if (program == nullptr)
		// use default path, add time to end to avoid lots of recursion when there are many defaultSpritePrograms
		this->shaderProgram = ResourceManager::LoadShaderProgram("defaultLineProgram" + std::to_string(glfwGetTime()), defaultVertPath, defaultFragPath);
	else // else use given one
		this->shaderProgram = program;

	// set type of component
	this->type = Entity::LineRenderer;
	// set colour
	this->color = color;
	_point1 = point1;
	_point2 = point2;
	_thickness = thickness;

	// initialise line rect
	InitRenderData();
}

LineRenderer::~LineRenderer()
{
	// de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);
	glDeleteBuffers(1, &lineEBO);
}

void LineRenderer::SetPoint1(glm::vec2 newPoint1)
{
	// set the new point
	_point1 = newPoint1;
	// update vertices
	UpdateLineVertices();
}

void LineRenderer::SetPoint2(glm::vec2 newPoint2)
{
	// set the new point
	_point2 = newPoint2;
	// update vertices
	UpdateLineVertices();
}

float LineRenderer::GetAlpha()
{
	return _alpha;
}

void LineRenderer::SetAlpha(float newAlpha)
{
	// if the new alpha is more than or equal to 1 (idk why it would be)
	if (newAlpha >= 1.0f)
	{
		// cap it to 1 
		_alpha = 1.0f;
		// but also set it so transparency is off
		this->hasTransprency = false;
		// if the current renderer has a parent entity set it to no transparency
		if (parentEntity != nullptr)
			parentEntity->SetHasTransparency(false);
	}
	else
	{
		// do the same but for transparency turned on
		_alpha = newAlpha;
		this->hasTransprency = true;
		// if the current renderer has a parent entity set it to has transparency
		if (parentEntity != nullptr)
			parentEntity->SetHasTransparency(true);
	}
}

void LineRenderer::Draw(std::shared_ptr<OrthoCamera> camera)
{
	if (parentEntity == nullptr)
		throw std::exception("Tried to draw a sprite which doesn't have a parent entity");

	shaderProgram->Use();

	// get the transform of this renderer's parent
	Transform spriteTransform = parentEntity->transform;

	glm::mat4 view = camera->GetViewMatrix();
	shaderProgram->SetMatrix4("view", view);


	glm::mat4 projection = camera->GetProjectionMatrix();
	shaderProgram->SetMatrix4("projection", projection);

	// sey sprite transform
	shaderProgram->SetMatrix4("modelTransform", spriteTransform.ToMatrix(camera));
	// set color of rect with alpha channel included
	shaderProgram->SetVector4f("lineColor", glm::vec4(color, _alpha));


	// draw the rect
	glBindVertexArray(this->lineVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void LineRenderer::InitRenderData()
{
	// array of floats which are rect/line vertices
	std::array<float, 12> vertices = CalculateLineVertices();

	// define what order of vertices to draw line/rectangle
	unsigned int indices[] = {  // note this is 0 based index
		0, 1, 2,   // first triangle
		2, 3, 1    // second triangle
	};

	// note that the VBO, VAO and EBO are actually just IDs to their values which are handled by opengl

	// vertex buffer object, stores vertices
	glGenBuffers(1, &lineVBO); // generate one buffer 

	// vertex array object, holds all configurations for a VBO/EBO
	glGenVertexArrays(1, &lineVAO); // generate 1 vertex arrya object

	// Element buffer object, stores index to how vertices should be drawn cutting down amount needed
	glGenBuffers(1, &lineEBO); // generate 1 elment buffer object

	glBindVertexArray(lineVAO); // bind the vertex array object 

	glBindBuffer(GL_ARRAY_BUFFER, lineVBO); // bind the generated buffer to array buffer target
	// place vertex data into buffer memory. Pass pointer to first value like normal arrays
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);// bind generated element buffer object
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);// bind indicies to element buffer

	// set vertex attribute position pointer at location 0, with 3 values, of type float, don't normalise data, stride is 53 values, offser of 0 bytes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// enable the created attribute which is at location 0
	glEnableVertexAttribArray(0);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void LineRenderer::UpdateLineVertices()
{
	// array of floats which are rect/line vertices
	std::array<float, 12> vertices = CalculateLineVertices();

	glBindVertexArray(lineVAO); // bind the vertex array object 

	glBindBuffer(GL_ARRAY_BUFFER, lineVBO); // bind the buffer object to array buffer target
	// place vertex data into buffer memory. Pass pointer to first value like normal arrays
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0]), GL_STATIC_DRAW); 
	
	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

std::array<float, 12> LineRenderer::CalculateLineVertices()
{
	/* -- Calculating the left/right side of a point --.
	* To get a vector which points from P1 (point 1) to P2 (point 2) you just do P2 - P1. Now we have a vector which has origin (0,0) that is pointing towards
	* P2 with the magnitude being the distance from P1 to P2. If we normalise this value (make hypotenuse 1) we then have vector that can be multiplied by
	* any scalar value we want which retains its original direction. However this direction is currently wrong for left/right side of a point.
	* In order to calculate the left/right side of a point we can rotate that normalised vector by 90 degrees (pi/2 rads) in either direction. Lets says
	* we wanted to calculate the left side of point 1. (All these operations will apply to right side except direction is just reversed).
	* 
	* I'm repeating myself but we currently have a vector which is the direction of P1 to P2 with magnitude 1. To get the left side of this value we rotate it
	* by 90 degrees anti-clockwise. (In mathematics, rotations that are positive are anti-clockwise).
	* 
	* The vector of a rotated 2D point (rotation around z-axis) is
		(	cos(theta) * x - sin(theta) * y,  // x-value
			sin(theta) * x + cos(theta) * y	) // y-value
	*
	* Now we want to currently rotate our vector by 90 degrees anti-clockwise. This represented as a radian is pi/2.
	* In a unit circle the cos(theta) is x-value of a point around the circumference and sin(theta) is y-value of that point.
	* 
	* cos(pi/2)	= The x value of a 90 degree anti-clockwise rotation = 0.
	* sin(pi/2) = The y value of a 90 degree anti-clockwise rotation = 1.
	* 
	* Plug it into your calc or jsut visualise it in ur head. Anyway it's very simple and easy to.
	* So now lets plus those sin and cos values into that rotation vector calc
	* 
		(	cos(pi/2) * x - sin(pi/2) * y,	  // x-value
			sin(pi/2) * x + cos(pi/2) * y	) // y-value

		= (	0 * x - 1 * y,  // x-value
			1 * x + 0 * y	) // y-value

		= (	-y,  // x-value
			x	) // y-value
	* 
	* Doing the same for right rotation (-pi/2) you get
	* cos(-pi/2) = 0
	* sin(-pi/2) = -1
	* 
	* The rotation coords become
	* (	0 * x - (-1 * y),  // x-value
			(-1 * x) + 0 * y	) // y-value
		= (	y,  // x-value
			-x) // y-value

	* 
	* Lets just continue on with calculating left side of current point. So we now have a rotated vector with magnitude 1. Well the left hand side of a line
	* needs to account for the thickness of the line. In this case we can just make the magnitude thickness/2. Due to us having a vector with magnitude 1
	* it means that whenever you want a different magnitude you can just scalarly multiply the vector by a new magnitude.
	* We can then add point 1 back on because the origin is (0,0) right now but we want Point 1 to be origin.
	* 
	* 
	* So we now have normalised((-y,x)) * thickness/2 + point1 
	* 
	* Ye that's it
	* 
	* If you want an interactive explanation of this, visit my desmos thingo I made
	* https://www.desmos.com/calculator/ddppw3tmki
	* 
	* Also, I do - 1 because normally I would do * 2 - 1 as values are from -1 to 1 for glsl. However, in this case we do thickness/2 so the 
	* times by 2 and divide by 2 cancel to get just the - 1.
	* 
	* Also I write normalise not normalize cos I'm australian not american
	*/


	glm::vec2 normalisedDifferenceVector = glm::normalize(_point2 - _point1);

	// the point to the left of point 1. (90 degrees anti-clockwise)
	glm::vec2 leftPoint1 = glm::vec2(-(normalisedDifferenceVector.y), normalisedDifferenceVector.x) * _thickness + _point1 - 1.0f;
	// the point to the right of point 1. (90 degrees clockwise)
	glm::vec2 rightPoint1 = glm::vec2(normalisedDifferenceVector.y, -(normalisedDifferenceVector.x))* _thickness + _point1 - 1.0f;

	// the point to the left of point 2. (90 degrees anti-clockwise)
	glm::vec2 leftPoint2 = glm::vec2(-(normalisedDifferenceVector.y), normalisedDifferenceVector.x)* _thickness + _point2 - 1.0f;
	// the point to the right of point 2. (90 degrees clockwise)
	glm::vec2 rightPoint2 = glm::vec2(normalisedDifferenceVector.y, -(normalisedDifferenceVector.x))* _thickness + _point2 - 1.0f;

	std::array<float, 12> vertices = {
		leftPoint1.x, leftPoint1.y, 0.0f,
		rightPoint1.x, rightPoint1.y, 0.0f,
		leftPoint2.x, leftPoint2.y, 0.0f,
		rightPoint2.x, rightPoint2.y, 0.0f,

	};


	
	return vertices;
}
