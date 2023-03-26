#include "OrthoCamera.h"
#include "Transform.h"
OrthoCamera::OrthoCamera(float width, float height, float nearPlane, float farPlane) {
	// just assign variables
	this->width = width;
	this->height = height;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
}

glm::mat4 OrthoCamera::GetViewMatrix()
{
	// get the view matrix using camera's properties
	glm::mat4 viewMatrix = Transform::ValuesToMatrix(position, glm::vec3(scalarSize, 1.0f), rotation); // copy current transform
	// you need to inverse the view. The view matrix moves the entire scene however if you inverse that it moves only the camera
	// E.g. if you move the scene by 10 pixel up and to the right. It looks as if the camera has moved 10 pixels down and to the left
	// So you inverse this to make to get the desired effect. This also applies to rotations and scale 
	return glm::inverse(viewMatrix);
}

glm::mat4 OrthoCamera::GetProjectionMatrix()
{
	// The projection matrix is in global units which is actually just screen pixels.
	// local values are normalised from -1 to 1 which is a total distance of 2, therefore 2 local units is worth 1 global unit. 
	// A ratio of 2:1. in order to convert from local coordinates to global you multiply viewport by 2 to make 1 global unit worth
	// 2 local pixel. 2:(1*2) = 2:2 = 1:1
	return glm::ortho(0.0f, width * 2.0f, 0.0f, height * 2.0f, nearPlane, farPlane);
}

void OrthoCamera::UpdateProjection(float width, float height)
{
	// all you need to do is update the width and height because GetProjectionMatrix is called each frame and it does the math then
	this->width = width;
	this->height = height;
}
