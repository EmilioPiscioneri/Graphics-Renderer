#pragma once
#include <glm/glm.hpp> // OpenGL maths: Include all GLM core / GLSL features
#include <iostream>
#include "OrthoCamera.h"
// forward declare entity
class Entity;

// represents transformation applied to an entity
class Transform
{
public:
	enum Type {
		// Transform is relative to global/viewport coordinates
		Normal,
		// Transform is stuck to camera viewport. This means if the camera in a scene moves, the entity will stay in position
		Sticky
	};
	
	// position of the transform in global (pixel) coordinates. Starts from bottom-left of object
	glm::vec2 offsetPosition;
	// position of the transform relative to viewport width and height. Starts from bottom-left of object
	// e.g. (0.5, 0.5) with viewport width 400 and height 800 will position an object at (200, 400)
	glm::vec2 relativePosition;

	// returns transform's position in global coords. Takes into account relative values and sticky type transforms
	glm::vec2 GetGlobalPosition(std::shared_ptr<OrthoCamera> mainCamera);

	// size of transform using global(pixel) values in a vector 3 (typically don't need z axis value)
	glm::vec3 offsetSize;
	// size of transform with values that are relative to viewport width and height and in a vector 2
	glm::vec2 relativeSize;

	// returns transform's size in global coords. Takes into account relative values
	glm::vec3 GetGlobalSize(std::shared_ptr<OrthoCamera> mainCamera);

	// rotation of transform about each axis in degrees E.g. (0,45,90) rotates 45 degrees around the y axis and then 90 degrees around the z axis
	glm::vec3 rotation;

	// Create a new transform with optional position, scale and rotation values that are in global(pixel) coordinates
	Transform(glm::vec2 offsetPosition = glm::vec2(0.0f), glm::vec3 offsetSize = glm::vec3(0.0f,0.0f, 0.0f), glm::vec3 rotation = glm::vec3(0.0f));

	// Create a new transform with position, scale and rotation values which haves values relative to viewport
	Transform(glm::vec2 relativePosition, glm::vec2 relativeSize, glm::vec3 rotation = glm::vec3(0.0f));

	// Create a new transform with (offset and realtive) position, scale and rotation values
	Transform(glm::vec2 offsetPosition,  glm::vec2 relativePosition, glm::vec3 offsetSize, glm::vec2 relativeSize, glm::vec3 rotation = glm::vec3(0.0f));
	
	// The parent entity of this transform, may be null
	Entity* parentEntity = nullptr;

	// Get the transform's zIndex which determines whether it will appear in front of other entities. Higher values mean it will appear in front.
	unsigned int GetZIndex();

	// set the transform's zIndex (updates highest zIndex if transform is attached to an entity that is attached to a scene)
	void SetZIndex(unsigned int newIndex);

	// Converts the current transform into a 4x4 local transformation matrix. Requires a camera to determine relative size and position values. Also if is sticky transform
	glm::mat4 ToMatrix(std::shared_ptr<OrthoCamera> mainCamera);

	// static func that converts given position, size (in global coords) and rotation into a matrix than can be passed to a vertex shader.
	static glm::mat4 ValuesToMatrix(glm::vec2 position, glm::vec3 size, glm::vec3 rotation);

	// the type of transform that this transform is
	Type type = Type::Normal; 

	
private:

	// Specifies the order in stacking layer that the entity will be on. Smaller values mean the entity will be pushed back in the scene and higher means it will overlap other entities
	unsigned int _zIndex = 0;
};

