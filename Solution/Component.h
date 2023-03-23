#pragma once

// delcare entity class exists. Automatically forward declared if u just #include "Entity.h"
class Entity;

// base class for anything attached to an entity
class Component
{
public:
	// An enum of Entity::ComponentType that specifies what type of component this is
	unsigned int type;
	// parent entity of component
	Entity* parentEntity = nullptr;

	// whether the current component has transparency. (false if opaque)
	bool hasTransprency = false;
};

