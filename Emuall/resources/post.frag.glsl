#version 330 core

// Interpolated values from the vertex shader
in vec2 UV;

// Output data
out vec3 color;

// values that stay constant for the whole mesh
uniform sampler2D textureSampler;

void main() {
	color = texture(textureSampler, UV).rgb;
}