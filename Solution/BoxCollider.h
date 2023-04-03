#pragma once
#include <iostream>
#include <array>
#include <glm/ext/vector_float2.hpp>
#include "OrthoCamera.h"
#include "Collider.h"

// A box collider, must be attached to a rigid body to do anything
class BoxCollider :
    public Collider, public std::enable_shared_from_this<BoxCollider>
{
public:
    // create a box collider with optional position and scalar size
    BoxCollider(glm::vec2 position = glm::vec2(0.0f), glm::vec2 size = glm::vec2(1.0f));

    // Check if there is a collision between the current collider and another input collider
    // third param is a bool that is whether or not you should push out the collided object from the current one if there was a collision
    bool CheckCollision(std::shared_ptr<Collider> otherCollider, std::shared_ptr<OrthoCamera> camera, bool pushOut = true);

    // returns an array of 4 bounding vertices of the box collider. Need to pass in a scene camera for calculations
    std::array<glm::vec2, 4> GetBoundingVertices(std::shared_ptr<OrthoCamera> camera);

    // get the locations of the left,right, bottom and top of box collider on each respective axis
    std::array<float, 4> GetBoundingSides(std::shared_ptr<OrthoCamera> camera);

    // when you have another collider that has collided and clipped into the current one (they are inside each other), this pushes the second object out of 
    // the current collider. If both objects are non-static then they will be pushed out of each other by differing amounts
    // The 1st param: the pre calculated bounding sides of the current collider
    // The 2nd param: the pre calculated bounding sides of the other collider
    // The 3rd param: pointer to the other collider (assumed that it has an attached rigid body)
    void PushOutCollider(std::array<float, 4> boundingSides, std::array<float, 4> otherBoundingSides, std::shared_ptr<Collider> otherCollider);
};

