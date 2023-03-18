#pragma once
#include <iostream>
#include <map>
#include "Component.h"
#include "Transform.h"

// Base object which holds functional components which perform different behaviour in a scene.
// NOTE: Entities should always be created as smart pointers so they never go out of scope and then become removed from stack
class Entity
{
public:
	// create an entity with transform
	Entity(Transform transform = Transform());


	enum ComponentType {
		SpriteRenderer
	} ;
	// whether or not the entity is active in scene. Dictates whether components are called each frame
	bool isActive = true;

	// Name of entity. Will be empty until entity is added to a scene with specified name
	std::string name = "";

	// transformation of entity
	Transform transform;

	/// <summary>
	/// Adds a component to entity. Does nothing if there is already a component with the same type. 
	/// </summary>
	/// <param name="type">The type of component to add</param>
	/// <param name="component">shared pointer to component to add</param>
	void AddComponent(ComponentType type, std::shared_ptr<Component> component);

	/// <summary>
	/// Retrieves a component (casted to a class derived from base component) from entity based on enum type and input generic type
	/// </summary>
	/// <param name="type">The type of component to retrieve from entity</param>
	/// <returns>Smart component pointer (nullptr if not found) that is casted to given generic type</returns>
	template<typename DerivedClass>
	std::shared_ptr<DerivedClass> GetComponent(ComponentType type);

	/// <summary>
	/// Removes a component from entity
	/// </summary>
	/// <param name="type">The type of component to remove</param>
	void RemoveComponent(ComponentType type);

	// returns a reference to map of all entity's components.
	// this should ONLY be used for iterating over. Use remove and add methods to modify components under entity.
	// The map stores only base component pointers to each component so you must cast to appropriate type
	std::map<ComponentType, std::shared_ptr<Component>>& GetComponents();
	
protected:
	// Returns whether or not an entity contains a component of type
	bool ComponentExists(ComponentType type);
	// function that returns base component using type
	std::shared_ptr<Component> GetBaseComponent(ComponentType type);

	// list of all components under the entity (stored as unique pointers), indexed by type
	std::map<ComponentType,  std::shared_ptr<Component>> _components;
};

// template method must be defined in header file?

template<typename DerivedClass>
std::shared_ptr<DerivedClass> Entity::GetComponent(ComponentType type)
{
	// check if component exists because [] operator will simply insert a new element and return reference if its not found
	if (ComponentExists(type))
		return std::static_pointer_cast<DerivedClass>(_components[type]);
	else
		return nullptr;
}

