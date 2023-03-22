#pragma once
#include "Camera.h"

// An orthographic camera
// note that the ortho camera uses pixel values for sizes n that
class OrthoCamera :
    public Camera
{
public:
    // Anything before near plane is cut off
    float nearPlane;
    // anything after far plane is cut off
    float farPlane;
    // width of viewport in pixels
    float width;
    // height of viewport in pixels
    float height;
    // create a new orthographic camera using near/far plane and width and height of viewport
    OrthoCamera(float width, float height, float nearPlane = -1.0f, float farPlane = 100.0f);

    // Get the view matrix of ortho camera with transformations applied
    glm::mat4 GetViewMatrix();
    // Get an orthographic projection matrix
    glm::mat4 GetProjectionMatrix();

    // Update the projection of camera whenever width and height of viewport changes
    void UpdateProjection(float width, float height);
};

