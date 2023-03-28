#pragma once
#include "Component.h"
#include <glm/vec2.hpp>

// forward declare rigid body
class RigidBody2D;

class Collider :
	public Component
{
public:

	~Collider();
	
	// What the type a collider is
	enum Type {
		// an axis-aligned bounding box collider
		BoundingBoxCollider,

	};

	

	// What type of collider the object is
	Type colliderType;

	// scalar size of the current collider relative to the size of the attached entity's size transform. It expands starting from the centre of collider
	glm::vec2 size = glm::vec2(1.0f);

	// position of collider in global (pixel) coords, relative to the parent entity's transform
	glm::vec2 position = glm::vec2(0.0f);


	// the rigid body that this collider is attached to 
	RigidBody2D* attachedRigidBody = nullptr;

	// sets the parent entity of the component
	void SetParentEntity(Entity* parent) override;
};

