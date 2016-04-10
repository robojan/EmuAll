#version 330 core

// uv data
in vec2 UV;

// output data
out vec4 color;

// glyph texture
uniform sampler2D textureSampler;
uniform vec4 glyphColor;

void main() {
	float intensity = texture(textureSampler, UV).r;
	color = glyphColor * vec4(1.0, 1.0, 1.0, intensity);
}