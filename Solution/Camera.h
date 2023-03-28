#pragma once
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	// scalar size of the camera
	glm::vec2 scalarSize = glm::vec2(1.0f);
	
	// the position of the current camera
	glm::vec2 position = glm::vec2(0.0f);

	// rotation of camera about each axis in radians E.g. (0,pi/6,pi/2) rotates 30 degrees around the y axis and then 90 degrees around the z axis
	glm::vec3 rotation = glm::vec3(0.0f);

	// get the view matrix for the current camera with all transforms applied
	virtual glm::mat4 GetViewMatrix() = 0;
	// get the projection matrix for the current camera
	virtual glm::mat4 GetProjectionMatrix() = 0;
	// Update the projection of camera whenever width and height of viewport changes
	virtual void UpdateProjection(float width, float height) = 0;
};

