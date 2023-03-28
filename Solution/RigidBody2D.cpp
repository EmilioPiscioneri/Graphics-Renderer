#include "RigidBody2D.h"
#include "Collider.h"

RigidBody2D::~RigidBody2D()
{
	// if attached, set to null
	if(_attachedCollider != nullptr)
		_attachedCollider->attachedRigidBody = nullptr;
}

void RigidBody2D::Update()
{
	// do nothing if not simulated or shouldn't move
	if (!isSimulated || isStatic)
		return;

	// check if an entity is attached to current component
	if (_parentEntity == nullptr)
		throw std::exception("Tried to update a Rigid body which isn't attached to an entity");

	// check if the entity is attached to a scene
	if (_parentEntity->parentScene == nullptr)
		throw std::exception("Tried to update a Rigid body which has an entity that isn't attached to a scene");

	// scene to base calculations off
	Scene* scene = _parentEntity->parentScene;

	// -- Calculate updated velocity of rigid body using drag and gravity acceleration --

	// See https://www.youtube.com/watch?v=OBq07mCMXlc&t=404s&ab_channel=jng

	/*	I'm only able to use what I know from applied maths which is a pre-cursor to specialist maths in Australia.
	* Anyway the double derivative of an equation can be used to find the acceleration of an equation.
	* In this case the x-axis for each function is time. Remember that acceleration is the rate of change of velocity over time.
	* Btw the video uses dot notation for derivatives but I'm using apostraphies because I can't do dot notation in visual studio comments
	* 
	* So if you look at the video you get
	* x-acceleration: f''(t) = (-k*f'(t)) * t
	* y-acceleration: f''(t) = (- g - k*f'(t)) * t
	* 
	* Where t is time, the k value is linear drag and f'(t) or the first derivate is velocity and g is gravitational acceleration.
	* In our case we have all of these values. Note that I also apply a scalar value to gravity so that's just a matter of g * scalar.
	* 
	* x-acceleration: f''(t) = (-(this->linearDrag) * velocity.x) * scene->deltaTime
	* y-acceleration: f''(t) = (- (scene->gravity * gravityScale) - (this->linearDrag) * velocity.y) * scene->deltaTime
	* 
	* And that's that. Also if you're wondering why gravity is only applied to the y-axis like I was, it's because gravity only affects the y-axis
	* of a theoretical 2D world. Like I'm sure there's a very small x-axis amount that is applied in real life but it's too minute to matter.
	* 
	* Anyway now that we have the acceleration/rate of change in of velocity over time. We can just add the calculated acceleration values to our velocity values.
	* However, those acceleration values are rate of change over time and we haven't account for time. So just multiply the acceleration values by deltaTime
	* and that to out velocity and then have the proper velocity values.
	* 
	* Also now that we have new velocity values we can add that to position. The same logic applies where we multiply the velocity by time passed (deltaTime)
	* and then we have a new postiion for the entity.
	* 
	* Note: the acceleration vector is opposing the current velocity vector because otherwise the velocity would remain constant and position will continue 
	* moving indefinetly
	*/

	// acceleration vector
	glm::vec2 acceleration = glm::vec2(
		// x
		(-(this->linearDrag)) * velocity.x,
		// y
		(-(scene->gravity * gravityScale)) - (this->linearDrag) * velocity.y) 
		// scale its effect by the amount of time that has passed
		* (float)scene->deltaTime;

	// Add acceleration (rate of change in velocity over time) to velocity
	velocity += acceleration;

	// then add the velocity times by how much time has passed scaled by how many pixels a metre represents
	_parentEntity->transform.offsetPosition += velocity * (float)scene->deltaTime * scene->pixelsPerMetre;
}

std::shared_ptr <Collider> RigidBody2D::GetAttachedCollider()
{
	return _attachedCollider;
}

void RigidBody2D::SetAttachedCollider(std::shared_ptr<Collider> colliderToAttach)
{
	_attachedCollider = colliderToAttach;
	// if not detatching collider
	if(colliderToAttach != nullptr)
		colliderToAttach->attachedRigidBody = this;

	if (_parentEntity != nullptr && _parentEntity->parentScene != nullptr)
		// update entity's collider. The function will deal with nullptr
		_parentEntity->parentScene->UpdateEntityCollider(_parentEntity->GetName(), colliderToAttach);
}

void RigidBody2D::SetParentEntity(Entity* parent)
{
	// if the component is being removed from entity
	if (parent == nullptr)
		// detatch collider, doesn't matter if it's already attatched
		SetAttachedCollider(nullptr);

	// set the parent entity to desired parent
	_parentEntity = parent;
}
