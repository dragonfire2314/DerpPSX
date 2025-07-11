#version 330 core

// out vec3 color;
out vec3 color;
in vec3 fragmentColor;

//VRAM texture
uniform sampler2D VRAM;

void main() 
{
	color = fragmentColor;
}