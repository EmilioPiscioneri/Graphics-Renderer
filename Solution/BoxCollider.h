#pragma once
#include <iostream>
#include <array>
#include <glm/ext/vector_float2.hpp>
#include "OrthoCamera.h"
#include "Collider.h"

// A box collider, must be attached to a rigid body to do anything
class BoxCollider :
    public Collider
{
public:
    // create a box collider with optional position and scalar size
    BoxCollider(glm::vec2 position = glm::vec2(0.0f), glm::vec2 size = glm::vec2(1.0f));

    // Check if there is a collision between the current collider and another input collider
    bool CheckCollision(std::shared_ptr<Collider> otherCollider, std::shared_ptr<OrthoCamera> camera);

    // returns an array of 4 bounding vertices of the box collider. Need to pass in a scene camera for calculations
    std::array<glm::vec2, 4> GetBoundingVertices(std::shared_ptr<OrthoCamera> camera);

    // get the locations of the left,right, bottom and top of box collider on each respective axis
    std::array<float, 4> GetBoundingSides(std::shared_ptr<OrthoCamera> camera);

};

