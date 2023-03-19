#pragma once
#include "Component.h"
#include "ShaderProgram.h"
#include "OrthoCamera.h"

class RectangleRenderer :
    public Component
{
public:
    // Setup a new rectangle renderer using given shader program and colour of rect
    // NOTE: If shader program is set to nullptr it will use a default shader
    RectangleRenderer(glm::vec3 color = glm::vec3(1.0f), ShaderProgram* program = nullptr);
    ~RectangleRenderer();

    // colour of the rectangle
    glm::vec3 color;

    // draw a rectangle using reference to scene camera and parent entity's transform
    void Draw(std::shared_ptr<OrthoCamera> camera);

private:
    // shader program that the renderer uses
    ShaderProgram* shaderProgram;
    // default vertex sahader
    const char* defaultVertPath = "VertexShaders/RectangleDefault.vert";
    // default frag sahader
    const char* defaultFragPath = "FragmentShaders/RectangleDefault.frag";
    // vretex array object ID for the sprite's rect
    unsigned int rectVAO;
    unsigned int rectVBO;
    unsigned int rectEBO;
    // Initializes and configures the rects's buffer and vertex attributes
    void InitRenderData();
};

