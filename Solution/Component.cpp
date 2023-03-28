#include "Component.h"

Entity* Component::GetParentEntity()
{
	return _parentEntity;
}

void Component::SetParentEntity(Entity* parent)
{
	// this function may be overrided
	_parentEntity = parent;
}
