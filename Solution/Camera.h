#pragma once
#include "Transform.h"

class Camera
{
public:
	// transform of the camera's view. The size component acts as a scaling of viewport (1 is normal) instead of a definitive size.
	// Camera's start with a transform of size vec3(1,1,1) unlike most other transforms
	Transform viewTransform = Transform(glm::vec3(0.0f), glm::vec3(1.0f));
	// get the view matrix for the current camera with all transforms applied
	virtual glm::mat4 GetViewMatrix() = 0;
	// get the projection matrix for the current camera
	virtual glm::mat4 GetProjectionMatrix() = 0;
	// Update the projection of camera whenever width and height of viewport changes
	virtual void UpdateProjection(float width, float height) = 0;
};

