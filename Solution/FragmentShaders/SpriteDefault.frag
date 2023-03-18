#version 330 core
out vec4 FragColor;

uniform vec3 spriteColor;
uniform sampler2D texture1;

in vec2 texCoord;

void main()
{
	FragColor = vec4(spriteColor, 1.0) * texture(texture1, texCoord) ; // set to texture mixed with spriteColor
} 