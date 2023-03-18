#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp> // matrix math

Transform::Transform(glm::vec3 position, glm::vec3 size, glm::vec3 rotation)
{
    // set to corresponding parameter
    this->position = position;
    this->size = size;
    this->rotation = rotation;
}

glm::mat4 Transform::ToMatrix()
{
    glm::mat4 transMatrix(1.0f); // start transformation matrix as an identity matrix
    // order is scale -> rotate -> translate but you do them in reverse

    // Local values (for vertices) are normalised from -1 to 1 in x and y axis. This is a total distance of 2. This means 2 local units is worth
    // 1 global unit. The width, height and position transform are all in global units so they are a ratio of 1:2 
    // In order to make them 1:1 you do global unit * 2 -> (1*2):2 -> 2:2 = 1:1
    // Then you add size.x and y to the position to make the origin of position be bottom-left corner
    // Normally you would add size.x/2 and size.y/2 but because the size is in global units you times that value by 2
    // so you end up getting size.x/2 * 2 and size.y/2 * 2 where the twos just cancel each other out 
   transMatrix = glm::translate(transMatrix, glm::vec3((position.x*2.0f) + size.x, (position.y*2.0f) + size.y, position.z)); 
    transMatrix = glm::rotate(transMatrix,glm:: radians(-(rotation.x)), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate about x axis
    transMatrix = glm::rotate(transMatrix, glm::radians(-(rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate about y axis
    transMatrix = glm::rotate(transMatrix, glm::radians(-(rotation.z)), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate about z axis
    transMatrix = glm::scale(transMatrix, size);
    return transMatrix; // return transformed matrix
}
