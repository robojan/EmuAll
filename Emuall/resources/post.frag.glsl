#version 330 core

// Interpolated values from the vertex shader
in vec2 UV;

// Output data
out vec4 color;

// values that stay constant for the whole mesh
uniform sampler2D textureSampler;

// Texture sampling function
vec4 GetTextureColor(sampler2D textureSampler, vec2 textureCoord);


void main() {
	color = GetTextureColor(textureSampler, UV);
}