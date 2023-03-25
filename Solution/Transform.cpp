#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp> // matrix math
#include "Scene.h"

glm::vec2 Transform::GetGlobalPosition(std::shared_ptr<OrthoCamera> mainCamera)
{
    // starts with offset position
    glm::vec2 globalPosition = offsetPosition;
    // add the relative positon values
    globalPosition.x += mainCamera->width * relativePosition.x;
    globalPosition.y += mainCamera->height * relativePosition.y;

    // If this is a sticky transform
    if (type == Type::Sticky)
    {
        // add the camera's position to the global positon if it sticky
        globalPosition += mainCamera->position;
    }

    return globalPosition;
}

glm::vec3 Transform::GetGlobalSize(std::shared_ptr<OrthoCamera> mainCamera)
{
    // starts with offset size 
    glm::vec3 globalSize = offsetSize;
    // add the relative size values
    globalSize.x += mainCamera->width * relativeSize.x;
    globalSize.y += mainCamera->height * relativeSize.y;

    return globalSize;
}

Transform::Transform(glm::vec2 offsetPosition, glm::vec3 offsetSize, glm::vec3 rotation)
{
    // set to corresponding parameter
    this->offsetPosition = offsetPosition;
    this->offsetSize = offsetSize;
    this->rotation = rotation;
    // default
    this->relativePosition = glm::vec2(0.0f);
    this->relativeSize = glm::vec2(0.0f);

}

Transform::Transform(glm::vec2 relativePosition, glm::vec2 relativeSize, glm::vec3 rotation)
{
    // set to corresponding parameter
    this->relativePosition = relativePosition;
    this->relativeSize = relativeSize;
    this->rotation = rotation;
    // default
    this->offsetPosition = glm::vec2(0.0f);
    this->offsetSize = glm::vec3(0.0f);
}

Transform::Transform(glm::vec2 offsetPosition, glm::vec2 relativePosition, glm::vec3 offsetSize, glm::vec2 relativeSize, glm::vec3 rotation)
{
    // set to corresponding parameter
    this->offsetPosition = offsetPosition;
    this->offsetSize = offsetSize;
    this->relativePosition = relativePosition;
    this->relativeSize = relativeSize;
    this->rotation = rotation;
}

unsigned int Transform::GetZIndex()
{
    return _zIndex;
}

void Transform::SetZIndex(unsigned int newIndex)
{
    // if the current transform is attached to an entity which is attached to a scene
    if (parentEntity != nullptr && parentEntity->parentScene != nullptr) {
        Scene* scene = parentEntity->parentScene;

        // the below conditional is meant to improve the speed of the program because we only want to run the scene->UpdateHighestZIndex function when we have to

        // check if this entity had the scene's highest index but it has been changed to a lower value
        if (scene->GetHighestZIndex() == _zIndex && newIndex < scene->GetHighestZIndex())
            // if this is true that means that an entity with the highest zIndex has changed zIndex so we need to run a check for any changes
            scene->UpdateHighestZIndex();
        // else if the new index is higher than the scene's current one
        else if (newIndex > scene->GetHighestZIndex())
            // set the scene's highest to be the new one
            scene->SetHighestZIndex(newIndex);

        // set the z index to new one
        _zIndex = newIndex;
    }
    else
        // else, just set the zIndex no updates needed
        _zIndex = newIndex;
    
}

glm::mat4 Transform::ToMatrix(std::shared_ptr<OrthoCamera> mainCamera)
{
    glm::mat4 transMatrix(1.0f); // start transformation matrix as an identity matrix
    // order is scale -> rotate -> translate but you do them in reverse

    // Local values (for vertices) are normalised from -1 to 1 in x and y axis. This is a total distance of 2. This means 2 local units is worth
    // 1 global unit. The width, height and position transform are all in global units so they are a ratio of 1:2 
    // In order to make them 1:1 you do global unit * 2 -> (1*2):2 -> 2:2 = 1:1
    // Then you add size.x and y to the position to make the origin of position be bottom-left corner
    // Normally you would add size.x/2 and size.y/2 but because the size is in global units you times that value by 2
    // so you end up getting size.x/2 * 2 and size.y/2 * 2 where the twos just cancel each other out 

    // get global pos and size
    glm::vec2 globalPosition = GetGlobalPosition(mainCamera);
    glm::vec3 globalSize = GetGlobalSize(mainCamera);


    // if the current transform is attached to an entity which is attached to a scene
    if (parentEntity != nullptr && parentEntity->parentScene != nullptr) 
        // set the z axis (depth) to be highest - current index because the higher the zIndex the closer it should be to screen. The lower the z-axis the closer it is to
        // camera meaning smaller values appear in front but zIndex works opposite so you do highest-current to get the desired overlapping result.
        // also note you turn the highest-current zIndex into a negative because the camera faces the negative direction, idk why but ye this fixes it
        transMatrix = glm::translate(transMatrix, glm::vec3((globalPosition.x*2.0f) + globalSize.x, (globalPosition.y*2.0f) + globalSize.y,-((float) parentEntity->parentScene->GetHighestZIndex() - _zIndex)));
    else
        // else, the transform isn't attached to entity or that entity isn't attached to a scene. Anyway zIndex only matters if it is attached to a scene so just set to 0 (right in front of camera)
        transMatrix = glm::translate(transMatrix, glm::vec3((globalPosition.x * 2.0f) + globalSize.x, (globalPosition.y * 2.0f) + globalSize.y, 0));

    transMatrix = glm::rotate(transMatrix,glm:: radians(-(rotation.x)), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate about x axis
    transMatrix = glm::rotate(transMatrix, glm::radians(-(rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate about y axis
    transMatrix = glm::rotate(transMatrix, glm::radians(-(rotation.z)), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate about z axis
    transMatrix = glm::scale(transMatrix, globalSize);
    return transMatrix; // return transformed matrix
}

glm::mat4 Transform::ValuesToMatrix(glm::vec2 position, glm::vec3 size, glm::vec3 rotation)
{
    glm::mat4 transMatrix(1.0f); // start transformation matrix as an identity matrix

    // order is scale -> rotate -> translate but you do them in reverse

    // Local values (for vertices) are normalised from -1 to 1 in x and y axis. This is a total distance of 2. This means 2 local units is worth
    // 1 global unit. The width, height and position transform are all in global units so they are a ratio of 1:2 
    // In order to make them 1:1 you do global unit * 2 -> (1*2):2 -> 2:2 = 1:1
    // Then you add size.x and y to the position to make the origin of position be bottom-left corner
    // Normally you would add size.x/2 and size.y/2 but because the size is in global units you times that value by 2
    // so you end up getting size.x/2 * 2 and size.y/2 * 2 where the twos just cancel each other out 

    transMatrix = glm::translate(transMatrix, glm::vec3((position.x * 2.0f) + size.x, (position.y * 2.0f) + size.y, 0));
    transMatrix = glm::rotate(transMatrix, glm::radians(-(rotation.x)), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate about x axis
    transMatrix = glm::rotate(transMatrix, glm::radians(-(rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate about y axis
    transMatrix = glm::rotate(transMatrix, glm::radians(-(rotation.z)), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate about z axis
    transMatrix = glm::scale(transMatrix, size);

    return transMatrix;
}
