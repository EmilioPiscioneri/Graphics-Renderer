#include "BoxCollider.h"
#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>


BoxCollider::BoxCollider(glm::vec2 position, glm::vec2 size)
{
	this->position = position;
	this->size = size;
	colliderType = Collider::BoundingBoxCollider;
	type = Entity::BoxCollider;
}

bool BoxCollider::CheckCollision(std::shared_ptr<Collider> otherCollider, std::shared_ptr<OrthoCamera> camera)
{
	// bounding sides of of this box collider
	std::array < float, 4> boundingSides = GetBoundingSides(camera);

	switch (otherCollider->colliderType)
	{
	case Collider::BoundingBoxCollider: {
		// based on the concept explained here (timestap in url) https://www.youtube.com/watch?v=oOEnWQZIePs&t=125s&ab_channel=MacroPixel
		
		// I made a simple condition check while on ride home from chiropractor
		
		// so as he explained you can have either LLRR or LRLR. There is no other ways to have the sides be if we only focus on the x-axis
		// the same applies to vertical axis where you can only have BTBT or BBTT
		
		// so if both the x and y have a match of LLRR and BBTT then it means there is a collision.
		// If the first set of sides represent 1 and second set represent sides then we have to check 2 conditions for x and 2 for y axis
		// To check if this is the case for LLRR you check if L1 < R2 && L2 < R1
		// To check if this is the case for BBTT you check if B1 < T2 && B2 < T1
		// if both conditions are met, we have a collision
		
		// bounding sides of other collider
		std::array < float, 4> otherBoundingSides = std::static_pointer_cast<BoxCollider>(otherCollider)->GetBoundingSides(camera);

		// do the 2 conditions check
		if (boundingSides[0] < otherBoundingSides[1] && // L1 < R2
			otherBoundingSides[0] < boundingSides[1] && // L2 < R1
			boundingSides[2] < otherBoundingSides[3] && // B1 < T2
			otherBoundingSides[2] < boundingSides[3])	// B2 < T1
			return true; // collision occured
		else
			return false; // no collision
	}
	default:
		throw std::invalid_argument("You have provided a collider type which cannot be checked for collisions against bounding box collider");
		break;
	}

}

std::array<glm::vec2, 4> BoxCollider::GetBoundingVertices(std::shared_ptr<OrthoCamera> camera)
{
	if (_parentEntity == nullptr) {
		throw std::exception("Tried to get bounding box of a collider that isn't attached to an entity");
	}
	
	// get position and size of entity
	glm::vec2 entityPosition = _parentEntity->transform.GetGlobalPosition(camera);
	glm::vec2 entitySize = _parentEntity->transform.GetGlobalSize(camera);

	// for each value that is position + size. You do entity size * this->size bcause the size property of this collider is scalar so a simple multiplication
	// does the trick
	// also just add the collider position


	// position of each vertex
	std::array<glm::vec2, 4> boundingVertices = {
		entityPosition + this->position, // bottom-left
		glm::vec2(entityPosition.x + this->position.x + entitySize.x * this->size.x, entityPosition.y + this->position.y), // bottom-right
		glm::vec2(entityPosition.x + this->position.x, entityPosition.y + this->position.y + entitySize.y * this->size.y), // top-left
		glm::vec2(entityPosition.x + this->position.x + entitySize.x * this->size.x, entityPosition.y  + this->position.y + entitySize.y * this->size.y), // top-right
	};
	 
	return boundingVertices;
}

std::array<float, 4> BoxCollider::GetBoundingSides(std::shared_ptr<OrthoCamera> camera)
{
	if (_parentEntity == nullptr) {
		throw std::exception("Tried to get bounding sides of a collider that isn't attached to an entity");
	}

	// get position and size of entity
	glm::vec2 entityPosition = _parentEntity->transform.GetGlobalPosition(camera);
	glm::vec2 entitySize = _parentEntity->transform.GetGlobalSize(camera);

	// for each value that is position + size. You do entity size * this->size bcause the size property of this collider is scalar so a simple multiplication
	// does the trick
	// also just add the collider position


	// position of each vertex
	std::array<float, 4> boundingSides = {
		entityPosition.x + this->position.x, // left
		entityPosition.x + this->position.x + entitySize.x * this->size.x, // right
		entityPosition.y + this->position.y, // bottom
		entityPosition.y + this->position.y + entitySize.y * this->size.y, // top
	};
	return boundingSides;
}

