#version 330 core

// uv data
in vec2 UV;

// output data
out vec4 resultColor;

// glyph texture
uniform sampler2D tex0;
uniform vec4 color;

void main() {
	float intensity = texture(tex0, UV).r;
	resultColor = color * vec4(1.0, 1.0, 1.0, intensity);
}