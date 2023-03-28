#pragma once

// delcare entity class exists. Automatically forward declared if u just #include "Entity.h"
class Entity;

// base class for anything attached to an entity
class Component
{
public:
	// An enum of Entity::ComponentType that specifies what type of component this is
	unsigned int type;
	
	// returns pointer to parent entity of component
	Entity* GetParentEntity();

	// sets the parent entity of the component
	virtual void SetParentEntity(Entity* parent);

	// whether the current component has transparency. (false if opaque or doesn't apply)
	bool hasTransprency = false;
	
protected:
	// parent entity of component
	Entity* _parentEntity = nullptr;
};

