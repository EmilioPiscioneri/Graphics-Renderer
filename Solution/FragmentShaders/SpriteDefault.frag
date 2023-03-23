#version 330 core
out vec4 FragColor;

uniform vec4 spriteColor;
uniform sampler2D texture1;

in vec2 texCoord;

void main()
{
	FragColor = spriteColor * texture(texture1, texCoord) ; // set to texture mixed with spriteColor
} 