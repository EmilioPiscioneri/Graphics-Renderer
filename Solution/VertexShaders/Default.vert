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
//    gl_Position = vec4(aPos, 1.0); // position based on vertex data
//    gl_Position = modelTransform * vec4(aPos, 1.0f); // position based on vertex data with applied transformation
    // note that we read the multiplication from right to left
//    gl_Position = projection * modelTransform * vec4(aPos, 1.0);
    gl_Position = projection * view * modelTransform * vec4(aPos, 1.0);

    texCoord = aTexCoord;
}