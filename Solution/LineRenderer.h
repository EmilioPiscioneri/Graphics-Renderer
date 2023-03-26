#pragma once
#include <array>
#include "Component.h"
#include "ShaderProgram.h"
#include "OrthoCamera.h"

// Renders a line between two points. It is actually just a rect behind the scenes. 
// Note that transform's size just acts as a scalar value for the line. This means if you want just a normal size you have to set offsetSize to (1,1,0)
class LineRenderer :
    public Component
{
public:
    // Setup a new line renderer using two points, a thickness (global coords), a given shader program and colour of line
    // NOTE: If shader program is set to nullptr it will use a default shader
    LineRenderer(glm::vec2 point1, glm::vec2 point2, float thickness = 1.0f, glm::vec3 color = glm::vec3(1.0f), ShaderProgram* program = nullptr);
    ~LineRenderer();

    // colour of the rectangle
    glm::vec3 color;

    // set the first point of the line renderer
    void SetPoint1(glm::vec2 newPoint1);
    // set the second point of the line renderer
    void SetPoint2(glm::vec2 newPoint2);


    // get the alpha (transparency) value of this rect
    float GetAlpha();

    // set the alpha (transparency) value of this rect
    void SetAlpha(float newAlpha);

    // draw a rectangle using reference to scene camera and parent entity's transform
    void Draw(std::shared_ptr<OrthoCamera> camera);

private:
    // first point of line
    glm::vec2 _point1;

    // second point of line
    glm::vec2 _point2;

    // how thick the rendered line is
    float _thickness;

    // the alpha channel (transparency) of the current rect
    float _alpha = 1.0f;
    // shader program that the renderer uses
    ShaderProgram* shaderProgram;
    // default vertex sahader
    const char* defaultVertPath = "VertexShaders/LineDefault.vert";
    // default frag sahader
    const char* defaultFragPath = "FragmentShaders/LineDefault.frag";
    // vretex array object ID for the line's rect
    unsigned int lineVAO;
    unsigned int lineVBO;
    unsigned int lineEBO;

    // Initializes and configures the line's buffer and vertex attributes
    void InitRenderData();

    // Each line is just a rect. This updates its vertices whenever there is a change in either points
    void UpdateLineVertices();

    // Calculates the vertices of the rect for a line renderer based on its two points and thickness
    std::array<float, 12> CalculateLineVertices();
};

