#pragma once
#include "Component.h"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "OrthoCamera.h"

class SpriteRenderer :
    public Component
{
public:
    // Setup a new sprite renderer using given shader program, texture and colour of sprite
    // NOTE: If shader program is set to nullptr it will use a default shader
    SpriteRenderer(Texture2D* texture, glm::vec3 color = glm::vec3(1.0f) , ShaderProgram* program = nullptr);
    ~SpriteRenderer();

    // colour of the sprite
    glm::vec3 color;

    // draw a sprite using reference to scene camera and parent entity's transform
    void Draw(std::shared_ptr<OrthoCamera> camera);

private:
    // shader program that the sprite renderer uses
    ShaderProgram* shaderProgram;
    // texture that the sprite renderer uses
    Texture2D* texture;
    // default vertex sahader
    const char* defaultVertPath = "VertexShaders/SpriteDefault.vert";
    // default frag sahader
    const char* defaultFragPath = "FragmentShaders/SpriteDefault.frag";
    // vretex array object ID for the sprite's rect
    unsigned int rectVAO;
    unsigned int rectVBO;
    unsigned int rectEBO;
    // Initializes and configures the rects's buffer and vertex attributes
    void InitRenderData();

};

