#version 460 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D rendering;

void main()
{
	float y = 1.f - TexCoord.y;
	FragColor = texture(rendering, vec2(TexCoord.x, y));
}