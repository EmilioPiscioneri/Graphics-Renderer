#pragma once
#include <glm/glm.hpp> // OpenGL maths: Include all GLM core / GLSL features

// represents transformation applied to an entity
class Transform
{
public:
	// position of the transform in local coordinates. Starts from bottom-left of object
	glm::vec2 position;
	// scale of transform as local vector 3 (typically don't need z axis value)
	glm::vec3 size;
	// rotation of transform about each axis in degrees E.g. (0,45,90) rotates 45 degrees around the y axis and then 90 degrees around the z axis
	glm::vec3 rotation;
	// Create a new transform with optional position, scale and rotation values
	Transform(glm::vec2 position = glm::vec2(0.0f), glm::vec3 size = glm::vec3(100.0f,100.0f, 0.0f), glm::vec3 rotation = glm::vec3(0.0f));

	// Specifies the order in stacking layer that the entity will be on. Smaller values mean the entity will be pushed back in the scene and higher means it will overlap other entities
	unsigned int zIndex = 0;

	// Converts the current transform into a 4x4 local transformation matrix
	glm::mat4 ToMatrix();

};

