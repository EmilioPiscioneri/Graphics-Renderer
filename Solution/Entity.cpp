#include "Entity.h"
#include "Scene.h"
#include "RigidBody2D.h"

Entity::Entity(Transform transform)
{
    // set the transform's parent entity
    transform.parentEntity = this;
    this->transform = transform;
}

Entity::~Entity()
{
    std::cout << "Entity with name " << _name << " was destroyed" << std::endl;
}

bool Entity::GetHasTransparency()
{
    // just return private var
    return _hasTransparency;
}

void Entity::SetHasTransparency(bool newTransparency)
{

    // if entity is attached to a scene and there has been a change in the transparency of entity
    if (parentScene != nullptr && newTransparency != _hasTransparency)
    {
        // set it to new transparency
        _hasTransparency = newTransparency;
        // update the entity in the scene so it can be sorted properly
        parentScene->UpdateEntityTransparency(shared_from_this());
    }
    else if (parentScene == nullptr)
        // else isn't attached to scene, doesn't matter if there has been a change just set it to new value. It gets handled whenever entity is added to a scene
        _hasTransparency = newTransparency;
}

std::string Entity::GetName()
{
    // return the entity's name
    return _name;
}

void Entity::SetName(std::string newName, bool updateInScene)
{
    // if entity is attached to a scene and there has been an actual change in the name of the entity
    if (updateInScene && parentScene != nullptr && newName != _name)
    {
        // update the entity's name in scene
        parentScene->UpdateEntityName(shared_from_this(), newName);
        // Don't need to set the new name of entity, it is set throught the UpdateEntityName function
        
    }
    else if (!updateInScene || parentScene == nullptr)
        // else isn't attached to a scene or specifically told not to update
        // then, doesn't matter if there has been a change just set it to new value. It gets handled whenever entity is added to a scene if it hasn't been added
        _name = newName;

}

void Entity::AddComponent(ComponentType type, std::shared_ptr<Component> component)
{
    // error check
    if (ComponentExists(type)){
        // can't specify what type of component enum tried to add because that requires a switch case and I should just get it right first time yknow
        std::cout << "ERROR: Tried to add component to entity " << _name << " which already exists" << std::endl;
        return;
    }

    // insert the new component to list of added components
    _components.insert(std::pair<ComponentType, std::shared_ptr<Component>>(type, component));
    
    // set the component's parent entity
    component->SetParentEntity(this);

    // If the added component has transparency
    if (component->hasTransprency)
        // set the current entity's transparency
        SetHasTransparency(true);

    
}



void Entity::RemoveComponent(ComponentType type)
{
    // get the component
    std::shared_ptr<Component> compToRemove = GetBaseComponent(type);
    // if the component exists
    if (compToRemove != nullptr) {
        // TODO: check if a removed component is destroyed from memory (they are stored as a base class shared pointer so idk) 
  
        // update its parent entity to null
        compToRemove->SetParentEntity(nullptr);

        // remove component from map
        _components.erase(type);
    }

}

std::map<Entity::ComponentType, std::shared_ptr<Component>>& Entity::GetComponents()
{
    return _components;
}

bool Entity::ComponentExists(ComponentType type)
{
    // if find function returns .end() then it doesn't exist
    return (_components.find(type) != _components.end());
}

std::shared_ptr<Component> Entity::GetBaseComponent(ComponentType type)
{
    if (ComponentExists(type))
        return _components[type];
    else
        return nullptr;
}
