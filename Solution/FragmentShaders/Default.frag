#version 330 core
out vec4 FragColor;
// sin(time since start of app) normalised to value between 0.0 - 1.0
//uniform float sinTime;
uniform sampler2D texture1;

in vec2 texCoord;

void main()
{
//    FragColor = vec4(0.0f, 0.0f, sinTime, 1.0f);
	FragColor = texture(texture1, texCoord); // set to texture
} 