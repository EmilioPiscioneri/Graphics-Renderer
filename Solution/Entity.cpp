#include "Entity.h"

Entity::Entity(Transform transform)
{
    this->transform = transform;
}

void Entity::AddComponent(ComponentType type, std::shared_ptr<Component> component)
{
    // set the component's parent entity
    component->parentEntity = this;

    // if component doesnt exist
    if (!ComponentExists(type))
        // insert the new component
        _components.insert(std::pair<ComponentType, std::shared_ptr<Component>>(type, component));
    else
        // can't specify what type of component enum tried to add because that requires a switch case and I should just get it right first time yknow
        std::cout << "ERROR: Tried to add component to entity " << name << " which already exists" << std::endl;
}



void Entity::RemoveComponent(ComponentType type)
{
    // get the component
    std::shared_ptr<Component> compToRemove = GetBaseComponent(type);
    // if the component exists
    if (compToRemove != nullptr) {
        // TODO: check if a removed component is destroyed from memory (they are stored as a base class shared pointer so idk) 

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
