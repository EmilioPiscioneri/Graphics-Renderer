
#include "Collider.h"

#include "RigidBody2D.h"

Collider::~Collider()
{
	// if attached, set to null
	if (attachedRigidBody != nullptr)
		attachedRigidBody->SetAttachedCollider(nullptr);
}

void Collider::SetParentEntity(Entity* parent)
{
	// if setting parent entity to nullptr (detatching) and this collider is already attached to a parent entity which is attached to a scene
	if (parent == nullptr && _parentEntity != nullptr && _parentEntity->parentScene != nullptr)
		// remove the collider from scene's active map of colliders
		_parentEntity->parentScene->UpdateEntityCollider(_parentEntity->GetName(), nullptr);


	// actually set it
	_parentEntity = parent;
}
