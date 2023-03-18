#version 330 core
// vertex position
layout (location = 0) in vec3 aPos;
// texture coordinate
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 modelTransform; // transformation to apply to each vertex
uniform mat4 view; 
uniform mat4 projection; 


void main()
{
    // note that you read the multiplication from right to left
    gl_Position = projection * view * modelTransform * vec4(aPos, 1.0);

    texCoord = aTexCoord;
}