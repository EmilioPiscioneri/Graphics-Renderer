#pragma once
#include <glm/glm.hpp> // OpenGL maths: Include all GLM core / GLSL features

// forward declare entity
class Entity;

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

	// The parent entity of this transform, may be null
	Entity* parentEntity = nullptr;

	// Get the transform's zIndex which determines whether it will appear in front of other entities. Higher values mean it will appear in front.
	unsigned int GetZIndex();

	// set the transform's zIndex (updates highest zIndex if transform is attached to an entity that is attached to a scene)
	void SetZIndex(unsigned int newIndex);

	// Converts the current transform into a 4x4 local transformation matrix
	glm::mat4 ToMatrix();
private:

	// Specifies the order in stacking layer that the entity will be on. Smaller values mean the entity will be pushed back in the scene and higher means it will overlap other entities
	unsigned int _zIndex = 0;
};

