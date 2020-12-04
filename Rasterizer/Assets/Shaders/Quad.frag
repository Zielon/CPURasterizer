#version 460 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D rendering;

void main()
{
	FragColor = texture(rendering, TexCoord);
}