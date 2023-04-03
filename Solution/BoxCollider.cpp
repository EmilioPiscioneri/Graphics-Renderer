#include "BoxCollider.h"
#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include "RigidBody2D.h"


BoxCollider::BoxCollider(glm::vec2 position, glm::vec2 size)
{
	this->position = position;
	this->size = size;
	colliderType = Collider::BoundingBoxCollider;
	type = Entity::BoxCollider;
}

bool BoxCollider::CheckCollision(std::shared_ptr<Collider> otherCollider, std::shared_ptr<OrthoCamera> camera, bool pushOut)
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
		{
			// if push out then push out smh
			if (pushOut)
				PushOutCollider(boundingSides, otherBoundingSides, otherCollider);
			return true; // collision occured
		}
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
		glm::vec2(entityPosition.x + this->position.x + entitySize.x * this->size.x, entityPosition.y + this->position.y + entitySize.y * this->size.y), // top-right
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



void BoxCollider::PushOutCollider(std::array<float, 4> boundingSides, std::array<float, 4> otherBoundingSides, std::shared_ptr<Collider> otherCollider)
{
	// Ok so there are static and non-static rigid bodies. Static ones (S) stay still while non-static (N) ones move around
	// there are three types of collisions we check for: S to N, N to S, N to N
	// You treat each of those differntly (S to N is basically the same except you must always push the non-static of the static)

	// There are also different types of colliders and depending on each one the push out method differentiates

	// first determine what type of colliders we are pushing out of this one

	switch (otherCollider->colliderType)
	{
	case Collider::BoundingBoxCollider: {
		//Lets deal with static to non-static
		if (attachedRigidBody->isStatic && !otherCollider->attachedRigidBody->isStatic) {
			// get reference to rigid body's velocity
			glm::vec2& velocity = otherCollider->attachedRigidBody->velocity;

			// the centre of this bounding box
			glm::vec2 thisCentre = glm::vec2(
				boundingSides[0] + ((boundingSides[1] - boundingSides[0]) / 2.0f), // x centre is left + half size (right - left = size) on x-axis
				boundingSides[2] + ((boundingSides[3] - boundingSides[2]) / 2.0f)); // y centre is bottom + half size (top - bottom = size) on y-axis

			// the centre of the non-static bounding box
			glm::vec2 otherCentre = glm::vec2(
				otherBoundingSides[0] + ((otherBoundingSides[1] - otherBoundingSides[0]) / 2.0f), // x centre is left + half size (right - left = size) on x-axis
				otherBoundingSides[2] + ((otherBoundingSides[3] - otherBoundingSides[2]) / 2.0f)); // y centre is bottom + half size (top - bottom = size) on y-axis

			// the new pushed out position of the other bounding box (bottom-left based positioning as per usual)
			glm::vec2 pushoutPosition = glm::vec2(0.0f);
			// reference to non-static collider transform
			Transform& otherColliderTransform = otherCollider->GetParentEntity()->transform;
			

			// now you deal with the x and y-axis independently.
			// You have to reverse the velocity and send it out the opposite way by accounting for the difference in that opposite direction

			// if the x velocity of the collided object is nothing 

			if (velocity == glm::vec2(0.0f))
			{
				//  -- x-axis --

				// see my desmos for distance between 2 1D points where the blue point represents the distance between the 2 along the x-axis
				// https://www.desmos.com/calculator/jg8dr1caqq

				// distance from centre of non-static collider to left of static collider
				float distToLeft = abs(boundingSides[0] - otherCentre.x);
				// distance from centre of non-static collider to right of static collider
				float distToRight = abs(boundingSides[1] - otherCentre.x);

				// If the non-static collider is closer to the left 
				if (distToLeft < distToRight)
					// set it to the left position of static collider - width of non-static collider to push out
					pushoutPosition.x = boundingSides[0] - (otherBoundingSides[1] - otherBoundingSides[0]);
				// If the non-static collider is closer to the right
				else if (distToRight < distToLeft)
					// set it to the right position of static collider 
					pushoutPosition.x = boundingSides[1];
				// ah shit, they are both equal distance to the edges, just push it out to the left side
				else
					// set it to the left position of static collider - width of non-static collider to push out
					pushoutPosition.x = boundingSides[0] - (otherBoundingSides[1] - otherBoundingSides[0]);

				// -- y-axis --

				// distance from centre of non-static collider to bottom of this collider
				float distToBottom = abs(boundingSides[2] - otherCentre.y);
				// distance from centre of non-static collider to top of this collider
				float distToTop = abs(boundingSides[3] - otherCentre.y);

				// If the non-static collider is closer to the bottom
				if (distToBottom < distToTop)
					// set it to the top position of static collider - height of non-static collider to push out
					pushoutPosition.y = boundingSides[2] - (otherBoundingSides[3] - otherBoundingSides[2]);
				// If the non-static collider is closer to the top
				else if (distToTop < distToBottom)
					// set it to the top position of static collider 
					pushoutPosition.y = boundingSides[3];
				// ah shit, they are both equal distance to the edges, just push it out to the bottom side
				else
					// set it to the top position of static collider - height of non-static collider to push out
					pushoutPosition.y = boundingSides[2] - (otherBoundingSides[3] - otherBoundingSides[3]);

			}
			else if (velocity.x == 0) // velocity has vertical (y)   velocity that is a rational number not equal to 0
			{
				// leave the x as the same, it doesn't move cos no velocity
				pushoutPosition.x = otherBoundingSides[0];
				
				// inversed velocity of non-static collider
				glm::vec2 inverseVelocity = otherCollider->attachedRigidBody->velocity * -1.0f;

				// if push downwards
				if(inverseVelocity.y < 0)
					// set it to the top position of static collider - height of other collider to push out
					pushoutPosition.y = boundingSides[2] - (otherBoundingSides[3] - otherBoundingSides[2]);
				else // push upwards
					// set it to the top position of static collider 
					pushoutPosition.y = boundingSides[3];

			}
			else if (velocity.y == 0) // velocity has horizontal (x) velocity that is a rational number not equal to 0
			{
				// leave the y as the same, it doesn't move cos no velocity
				pushoutPosition.y = otherBoundingSides[2];

				// inversed velocity of non-static collider
				glm::vec2 inverseVelocity = otherCollider->attachedRigidBody->velocity * -1.0f;

				// if push left
				if (inverseVelocity.x < 0)
					// set it to the left position of static collider - width of non-static collider to push out
					pushoutPosition.x = boundingSides[0] - (otherBoundingSides[1] - otherBoundingSides[0]);
				else // push right
					// set it to the right position of static collider 
					pushoutPosition.x = boundingSides[1];
			}
			else // velocity is neither 0 on x or y
			{
				// inversed velocity of non-static collider
				glm::vec2 inverseVelocity = otherCollider->attachedRigidBody->velocity * -1.0f;

				// First you need to establish what the two potential sides that this velocity can be pushed out to. E.g. if velocity is 1,1 then you would
				// be looking at top or right because the x and y are positive. If you had -2, 5 as a velocity you would be looking at left and top sides for
				// a potential location to be pushed out to. Once you have the side you can turn it into an equation e.g. x = 2 and x = 4 where this is two
				// vertical lines that show the left and right side



				// this is an equation x = xLine which represents a horizontal side (left/right) of a rect that can be checked for intersection
				float xLine = 0.0f;
				// this is an equation y = yLine which represents a vertical side (bottom/top) of a rect that can be checked for intersection
				float yLine = 0.0f;

				// determine what the lines are

				if (inverseVelocity.x < 0) // negative: pushed out to left
				{
					xLine = boundingSides[0]; // left of static collider
				}
				else if (inverseVelocity.x > 0) // positive: push out to right
				{
					xLine = boundingSides[1]; // right of static collider
				}

				if (inverseVelocity.y < 0) // negative: pushed out to bottom
				{
					yLine = boundingSides[2]; // bottom of static collider
				}
				else if (inverseVelocity.y > 0) // positive: push out to top
				{
					yLine = boundingSides[3]; // top of static collider
				}


				// Now that you have two sides (the side x/y) you need to turn the velocity into a straight line relative to each corner of the rect
				// and find an intersection. See my interactive desmos graph https://www.desmos.com/calculator/p2t0ryvfmi

				
				// You're basically gonna check for an intercept at each corner until you find one and then end the loop if you do

				// equations for each corner are the same except P is different where P is the position of the corner relative to non-static rect.
				// so you have y = (V.x/V.y) (x-P.x) + P.y
				// and also x = (y-p.y)/(v.x/v.y) + p.x

				// gradient of velocity equation
				float gradient = inverseVelocity.y / inverseVelocity.x;
				
				// the corners are 0 = BL, 1 = BR, 2 = TL, 3 = TR
				
				// this is the corner that the intercept from the non-static rect to the static rect was found from
				int cornerFromIntercept = 0;
				float interceptX = 0.0f; // the x value that the inversed velocity intercepts with on the static rect
				float interceptY = 0.0f; // the y value that the inversed velocity intercepts with on the static rect

				for (int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
				{
					// x and y position of the corner (no need for vec2)
					float cornerX = 0.0f;
					float cornerY = 0.0f;

					// if statement is easier to read than switch case. It is also a small if statement

					if (cornerIndex == 0) // bottom-left
					{
						cornerX = otherBoundingSides[0]; // left
						cornerY = otherBoundingSides[2]; // bottom
					}
					else if (cornerIndex == 1) // bottom-right
					{
						cornerX = otherBoundingSides[1]; // right
						cornerY = otherBoundingSides[2]; // bottom
					}
					else if (cornerIndex == 2) // top-left
					{
						cornerX = otherBoundingSides[0]; // left
						cornerY = otherBoundingSides[3]; // top
					}
					else // == 3, top-right
					{
						cornerX = otherBoundingSides[1]; // right
						cornerY = otherBoundingSides[3]; // top
					}

					// indexed bounding sides are bottom and left
					float xResult = (yLine - cornerY) / gradient + cornerX;
					float yResult = gradient * (xLine - cornerX) + cornerY;
					

					// check if the results of the x or y equation is between the x and y bounds and whether in appropriate domain based on velocity direction. Direction just
					// refers to whether it is pointing in positive or negative direction in this instance. If the results pass, there is an intercept. 

					// x bewteen: left <= xResult <= right and ( (xResult <= cornerX when velocity.x < 0) or (xResult >= cornerX when velocity.x > 0) )
					if (boundingSides[0] <= xResult && xResult <= boundingSides[1] && ( (inverseVelocity.x < 0 && xResult <= cornerX) || (inverseVelocity.x > 0 && xResult >= cornerX)))
					{
						cornerFromIntercept = cornerIndex;
						interceptX = xResult;
						interceptY = yLine;
					}
					// y bewteen: bottom <= yResult <= top and ( (yResult <= cornerY when velocity.y < 0) or (yResult >= cornerY when velocity.y > 0) )
					else if	(boundingSides[2] <= yResult && yResult <= boundingSides[3] && ((inverseVelocity.y < 0 && yResult <= cornerY) || (inverseVelocity.y > 0 && yResult >= cornerY)))
					{
						cornerFromIntercept = cornerIndex;
						interceptX = xLine;
						interceptY = yResult;
					}

				}

				// ok now we have an intercept location and what corner that intercept came from
				// now we have to move the current position which is bottom-left based to that intercept location.
				// think of it like if the intercept was found using the top-right corner. We need to the position the current rect so that the top-right
				// corner is in the position we want it to be. This can be done by just minusing width or height to work back to bottom-left

				// note that when you're pushing out something from a corner. It will still be inside it if you move it to a side that is corrseponding with corner.
				// Ok so basically if you are moving from the bottom-right side to the intercept which is on either the bottom or right. Then the rect
				// will still be inside when moved. To avoid this you will need to add or subtract the width/height of the rect to push it out properly.

				// width of non-static rect
				float width = otherBoundingSides[1] - otherBoundingSides[0];
				// height of non-static rect
				float height = otherBoundingSides[3] - otherBoundingSides[2];

				if (cornerFromIntercept == 0) // bottom-left
				{
					// just positition at intercept coords
					pushoutPosition.x = interceptX; 
					pushoutPosition.y = interceptY;

					// check if intercept is at left
					if(interceptX == boundingSides[0])
						pushoutPosition.x -= width; // pushout
					// check if intercept is at bottom
					else if (interceptY == boundingSides[2])
						pushoutPosition.y -= height; // pushout
				} 
				else if (cornerFromIntercept == 1) // bottom-right
				{
					pushoutPosition.x = interceptX - width; 
					pushoutPosition.y = interceptY;
					// check if intercept is at right
					if (interceptX == boundingSides[1])
						pushoutPosition.x += width; // pushout
					// check if intercept is at bottom
					else if (interceptY == boundingSides[2])
						pushoutPosition.y -= height; // pushout
				}
				else if (cornerFromIntercept == 2) // top-left
				{
					pushoutPosition.x = interceptX ; 
					pushoutPosition.y = interceptY - height;
					// check if intercept is at left
					if (interceptX == boundingSides[0])
						pushoutPosition.x -= width; // pushout
					// check if intercept is at top
					else if (interceptY == boundingSides[3])
						pushoutPosition.y += height; // pushout
				}
				else // (cornerFromIntercept == 3) // top-right
				{
					pushoutPosition.x = interceptX - width; 
					pushoutPosition.y = interceptY - height;
					// check if intercept is at right
					if (interceptX == boundingSides[1])
						pushoutPosition.x += width; // pushout
					// check if intercept is at top
					else if (interceptY == boundingSides[3])
						pushoutPosition.y += height; // pushout
				}
				




				/*
				// bottom of other collider is below this one ( B2 < B1 or B1 > B2
				if (otherBoundingSides[2] < boundingSides[2]) {

					// TODO: check if the x or y distance is unkown

					// if being pushed to negative (below 0). Move it to bottom of this collider
					if (otherInvVelocity.y < 0.0f)
					{
						// T2 - B1
						float yDist = otherBoundingSides[3] - boundingSides[2];
						float ratio = otherInvVelocity.y / yDist;
						float xDist = otherInvVelocity.x / ratio;



						// if being pushed to the right
						if (otherInvVelocity.x > 0.0f) {
							// check if the x or y is unknown

							// left2 - xDist (negative cos y has a negative ratio as it's going downwards)
							float newX = otherBoundingSides[0] - xDist;

							// if newX > right1 (the y is unknown)
							if (newX > boundingSides[1])
							{
								// R1 - L2
								xDist = boundingSides[1] - otherBoundingSides[0];
								ratio = otherInvVelocity.x / xDist;
								yDist = otherInvVelocity.y / ratio;

								// xDist is now positive so add it
								pushoutPosition.x = otherBoundingSides[0] + xDist;
								// yDist is now negative so add it (2 negatives make positive, avoid that)
								pushoutPosition.y = otherBoundingSides[2] + yDist;
							}
							else // the y is known (x is unkown but calculated)
							{
								pushoutPosition.x = otherBoundingSides[0] - xDist;
								pushoutPosition.y = otherBoundingSides[2] - yDist;
							}
						}
						// being pushed to the left
						else if (otherInvVelocity.x < 0.0f)
						{
							// check if the x or y is unknown

							// right - xDist
							float newX = otherBoundingSides[1] - xDist;

							// if newX < left1 (the y is unknown)
							if (newX < boundingSides[0])
							{
								// R2 - L1
								xDist = otherBoundingSides[1] - boundingSides[0];
								ratio = otherInvVelocity.x / xDist;
								yDist = otherInvVelocity.y / ratio;

								pushoutPosition.x = otherBoundingSides[0] - xDist;
								pushoutPosition.y = otherBoundingSides[2] - yDist;
							}
							else // the y is known (x is unkown but calculated)
							{
								pushoutPosition.x = otherBoundingSides[0] - xDist;
								pushoutPosition.y = otherBoundingSides[2] - yDist;
							}
						}

					}
					// else if it is being pushed to positive (greater than 0). Move it to above this collider
					else if (otherInvVelocity.y > 0.0f)
					{
						// T1 - T2
						float yDist = boundingSides[3] - otherBoundingSides[3];
						float ratio = otherInvVelocity.y / yDist;
						float xDist = otherInvVelocity.x / ratio;

						// if being pushed out to the right
						if (otherInvVelocity.x > 0.0f) {
							// check if the x or y is unknown

							// left2 + xDist (x is positive)
							float newX = otherBoundingSides[0] + xDist;

							// newX > right1 (y is unkown)
							if (newX > boundingSides[1]) {
								// R1 - L2
								xDist = boundingSides[1] - otherBoundingSides[0];
								ratio = otherInvVelocity.x / xDist;
								yDist = otherInvVelocity.y / ratio;
								// (x is positive) left2 + xDist
								pushoutPosition.x = otherBoundingSides[0] + xDist;
								// bottom + yDist (yDist is positive)
								pushoutPosition.y = otherBoundingSides[2] + yDist;
							}
							else { // y is known
								pushoutPosition.x = otherBoundingSides[0] + xDist; // x is positive
								// bottom + (size of other collider on y-axis)
								pushoutPosition.y = otherBoundingSides[2] + yDist + (otherBoundingSides[3] - otherBoundingSides[2]);
							}
						}
						// being pushed out to the left
						else if (otherInvVelocity.x < 0.0f) {
							// check if the x or y is unknown

							// BROKEN

							// left2 - xDist (x is negative)
							float newX = otherBoundingSides[0] - xDist;

							// newX < left1 (y is unkown)
							if (newX < boundingSides[0]) {
								// R2 - L1
								xDist = otherBoundingSides[1] - boundingSides[0];
								ratio = otherInvVelocity.x / xDist;
								yDist = otherInvVelocity.y / ratio;
								// (x is positive) right 2 + xDist - (size of other collider on-axis)
								pushoutPosition.x = otherBoundingSides[1] - xDist - (otherBoundingSides[1] - otherBoundingSides[0]);
								// bottom - yDist (yDist is negative)
								pushoutPosition.y = otherBoundingSides[2] - yDist;
							}
							else { // y is known
								pushoutPosition.x = otherBoundingSides[0] + xDist; // x is negative
								// bottom + (size of other collider on y-axis)
								pushoutPosition.y = otherBoundingSides[2] + yDist + (otherBoundingSides[3] - otherBoundingSides[2]);
							}



						}



					}

				}
				else if (otherBoundingSides[2] > boundingSides[2]) { // B2 > B1 or B1 < B2 or this collider has its bottom below the other collider


					// if being pushed to negative (below 0). Move it to bottom of this collider
					if (otherInvVelocity.y < 0.0f) {
						// B2 - B1
						float yDist = otherBoundingSides[2] - boundingSides[2];
						float ratio = otherInvVelocity.y / yDist;
						float xDist = otherInvVelocity.x / ratio;

						// if being pushed out to the right
						if (otherInvVelocity.x > 0.0f) {

						}
						// being pushed out to the left
						else if (otherInvVelocity.x < 0.0f) {
							// check if the x or y is unknown

							// left2 + xDist (x is positive)
							float newX = otherBoundingSides[0] + xDist;

							// newX > right1 (y is unkown)
							if (newX > boundingSides[1]) {
								// R1 - L2
								xDist = boundingSides[1] - otherBoundingSides[0];
								ratio = otherInvVelocity.x / xDist;
								yDist = otherInvVelocity.y / ratio;
								// (x is positive) left2 + xDist
								pushoutPosition.x = otherBoundingSides[0] + xDist;
								// bottom + yDist (yDist is positive)
								pushoutPosition.y = otherBoundingSides[2] + yDist;
							}
							else { // y is known
								pushoutPosition.x = otherBoundingSides[0] + xDist; // x is positive
								// bottom + (size of other collider on y-axis)
								pushoutPosition.y = otherBoundingSides[2] + yDist + (otherBoundingSides[3] - otherBoundingSides[2]);
							}
						}
						// left
						pushoutPosition.x = otherBoundingSides[0] - xDist;
						// bottom - (size of other collider)
						pushoutPosition.y = otherBoundingSides[2] - yDist - (otherBoundingSides[3] - otherBoundingSides[2]);
					}
					// else if it is being pushed to positive (greater than 0). Move it to above this collider
					else if (otherInvVelocity.y > 0.0f) {
						// T1 - B2
						float yDist = boundingSides[3] - otherBoundingSides[2];
						float ratio = otherInvVelocity.y / yDist;
						float xDist = otherInvVelocity.x / ratio;

						// being pushed out to the right
						if (otherInvVelocity.x > 0.0f)
						{
							// check if x or y is unknown

							// L2 + xDist
							float newX = otherBoundingSides[0] + xDist;

							// newX > right1 (y is unknown
							if (newX > boundingSides[1])
							{
								// R1 - L2
								xDist = boundingSides[1] - otherBoundingSides[0];
								ratio = otherInvVelocity.x / xDist;
								yDist = otherInvVelocity.y / ratio;

								pushoutPosition.x = otherBoundingSides[0] + xDist;
								pushoutPosition.y = otherBoundingSides[2] + yDist;
							}
							else { // y is known
								pushoutPosition.x = otherBoundingSides[0] + xDist;
								pushoutPosition.y = otherBoundingSides[2] + yDist;
							}


						}
						// being pushed out to the left
						else if (otherInvVelocity.x < 0.0f)
						{

							// check if x or y is unknown

							// R2 + xDist
							float newX = otherBoundingSides[1] + xDist;

							// newX < left1 (y is unknown)
							if (newX < boundingSides[0]) {
								// R2 - L1
								xDist = otherBoundingSides[1] - boundingSides[0];
								ratio = otherInvVelocity.x / xDist;
								yDist = otherInvVelocity.y / ratio;

								// x is positive so minus it to go left
								pushoutPosition.x = otherBoundingSides[0] - xDist;
								// y is negative so minus it to go up
								pushoutPosition.y = otherBoundingSides[2] - yDist;
							}
							else { // y is known
								pushoutPosition.x = otherBoundingSides[0] + xDist;
								pushoutPosition.y = otherBoundingSides[2] + yDist;
							}


						}
					}

				}
				*/
				/*
				// x-axis
				if (velocity.x < 0.0f) // if the other collider is heading to the left (-x movement)
				{
					// move it to the right of this collider (inverse it)
					pushoutPosition.x = boundingSides[1];
				}
				else if(velocity.x > 0.0f) // else, the velocity.x is positive. This means the collider is heading to the right
				{
					// now push out in inverse direction
					// set it to the left position of this collider  - width of other collider to push out
					//pushoutPosition.x = boundingSides[0] - (otherBoundingSides[1] - otherBoundingSides[0]);

					// top2 - bottom1
					float y1 = otherBoundingSides[3] - boundingSides[2];
					float yv = otherInvVelocity.y;
					float xv = otherInvVelocity.x;

					float ratio = yv / y1;
					float x1 = xv / ratio;

					pushoutPosition.x = otherBoundingSides[0] - x1;
				}
				else { // velocity.x == 0, just leave it in same position
					pushoutPosition.x = otherColliderTransform.offsetPosition.x;
				}

				// y-axis

				if (velocity.y < 0.0f) // if the other collider is heading downwards (-x movement)
				{
					// move it to the top of this collider (inverse it)
					pushoutPosition.y = boundingSides[3];
				}
				else if (velocity.y > 0.0f) // else, the velocity.y is positive. This means the collider is heading to the top
				{
					// now push out in inverse direction
					// set it to the bottom position of this collider  - top of other collider to push out
					//pushoutPosition.y = boundingSides[2] - (otherBoundingSides[3] - otherBoundingSides[2]);
					// top1 - bottom2
					float y1 = otherBoundingSides[3] - boundingSides[2];

					pushoutPosition.y = otherBoundingSides[2] - y1;
				}
				else { // velocity.y == 0, just leave it in same position
					pushoutPosition.y = otherColliderTransform.offsetPosition.y;
				}*/
			}



			// set the new position of the 
			otherColliderTransform.offsetPosition = pushoutPosition;
			otherColliderTransform.relativePosition = glm::vec2(0.0f);

		}
		break;
	}
	default:
		break;
	}
}

