#version 330 core

// Input 
in vec2 uv;

// Uniform data
uniform sampler2D paletteData;
uniform vec2 windowSize;
uniform bool objPalette;

// Output
out vec4 resultColor;

// constants
const float lineWidth = 0.5;
const vec3 lineColor = vec3(0.9, 0.9, 0.9);

void main() {
	// 16*16 pixel display
	vec2 pos = uv / vec2(1.0, 2.0);
	vec2 offset = vec2(0.0, 0.0);
	if (objPalette) offset.y = 0.5;
	vec3 color = texture(paletteData, pos + offset).rgb;

	vec2 pixelSize = vec2(1.0, 1.0) / windowSize;
	vec2 paletteSize = textureSize(paletteData, 0);
	vec2 invPaletteSize = vec2(1.0, 1.0) / paletteSize;
	vec2 pixelPos = mod(pos, invPaletteSize);
	vec2 distance2D = min(pixelPos - vec2(0.0, 0.0), invPaletteSize - pixelPos);

	resultColor.rgb = (distance2D.x <= lineWidth * pixelSize.x || distance2D.y <= lineWidth * pixelSize.y) ? lineColor : color;
	resultColor.a = 1.0;
}