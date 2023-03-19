#pragma once
#include "Component.h"
#include "ShaderProgram.h"
#include "OrthoCamera.h"

// An ellipse renderer is used to render, well ellipses. It is essentially the same as a rectangle renderer, the only difference being it has a circle shader
// I stick to using a transform and not radiusX/radiusY because it fits well with the other components
class EllipseRenderer :
    public Component
{
public:
    // Setup a new ellipse renderer using given shader program and colour 
    // NOTE: If shader program is set to nullptr it will use a default shader
    EllipseRenderer(glm::vec3 color = glm::vec3(1.0f), ShaderProgram* program = nullptr);
    ~EllipseRenderer();

    // colour of the ellipse
    glm::vec3 color;

    // draw an ellipse using reference to scene camera and parent entity's transform
    void Draw(std::shared_ptr<OrthoCamera> camera);

private:
    // shader program that the renderer uses
    ShaderProgram* shaderProgram;
    // default vertex sahader
    const char* defaultVertPath = "VertexShaders/EllipseDefault.vert";
    // default frag sahader
    const char* defaultFragPath = "FragmentShaders/EllipseDefault.frag";
    // vretex array object ID for the sprite's rect
    unsigned int rectVAO;
    unsigned int rectVBO;
    unsigned int rectEBO;
    // Initializes and configures the rects's buffer and vertex attributes
    void InitRenderData();
};

