#pragma once
#include "Scene.h"

// A component which simulates physics for an entity. Any transformations are applied directly to an entity's 
class RigidBody2D : 
	public Component
{
public:

	// Apply physics calculations for the current rigid body to parent entity's transform
	void Update();

	// Velocity of rigid body. Represents the rate of change in position over time 
	glm::vec2 velocity = glm::vec2(0.0f);

	// this is the linear drag coefficient of the rigid body
	float linearDrag = 0.0f;

	// A scalar value that represents how much gravity affects an entity
	float gravityScale = 1.0f;

	// whether or not the current rigid body should be simulated by physics system
	bool isSimulated = true;
};

