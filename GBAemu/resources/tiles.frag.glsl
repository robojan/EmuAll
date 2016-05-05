#version 330 core

// Input
in vec2 uv;

// Output
out vec4 fragColor;

// Uniforms
uniform sampler2D paletteData;
uniform usamplerBuffer vramData;
uniform int baseAddress;
uniform bool depth8bit;
// bit 0-3, palette number in 4 bit depth mode
// bit 4, obj palette
uniform int paletteId; 
uniform vec2 windowSize;
uniform bool grid;

// constants
const float lineWidth = 0.25;
const vec3 lineColor = vec3(0.9, 0.9, 0.9);


vec3 getPaletteColor(uint idx) {
	ivec2 size = textureSize(paletteData, 0);
	ivec2 colorPos = ivec2(int(idx) % size.x, int(idx) / size.x);
	vec2 paletteUV = (vec2(colorPos) + vec2(0.5, 0.5)) / vec2(size);
	return texture(paletteData, paletteUV).rgb;
}

vec3 hsvToRGB(vec3 c) {
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	vec2 pixelPosf;
	if (depth8bit) {
		// 64 bytes per tile
		// each pixel 1 byte
		// 16 tiles per line
		// 16 lines
		vec2 pos = uv * vec2(16.0,32.0);
		ivec2 tilePos = ivec2(pos);
		pixelPosf = fract(pos);
		ivec2 pixelPos = ivec2(pixelPosf * 8.01);

		int tileId = tilePos.y * 16 + tilePos.x;
		int pixelId = pixelPos.y * 8 + pixelPos.x;
		int address = baseAddress + tileId * 64 + pixelId;
		uint data = texelFetch(vramData, address).r;
		if ((paletteId & 0x10) != 0) {
			// OBJ palette
			fragColor = vec4(getPaletteColor(data + 256u), 1.0);
		}
		else {
			// BG palette
			fragColor = vec4(getPaletteColor(data), 1.0);
		}
	}
	else {
		// 32 bytes per tile
		// 2 pixels per byte
		// 32 tiles per line
		// 32 lines
		vec2 pos = uv * 32.0;
		ivec2 tilePos = ivec2(pos);
		pixelPosf = fract(pos);
		ivec2 pixelPos = ivec2(pixelPosf * 8);

		int tileId = tilePos.y * 32 + tilePos.x;
		int pixelId = pixelPos.y * 8 + pixelPos.x;
		int address = baseAddress + tileId * 32 + pixelId / 2;
		uint data = texelFetch(vramData, address).r;
		if ((address & 1) != 0) data >>= 4;
		data &= 0xFu;
		if ((paletteId & 0x10) != 0) {
			// OBJ palette
			fragColor = vec4(getPaletteColor(data + (uint(paletteId) & 0xFu) * 16u + 256u), 1.0);
		}
		else {
			// BG palette
			fragColor = vec4(getPaletteColor(data + (uint(paletteId) & 0xFu) * 16u), 1.0);
		}
	}
	if (grid) {
		vec2 distance2D = min(pixelPosf - vec2(0.0, 0.0), vec2(1.0, 1.0) - pixelPosf);
		float pixelSize = (1.0 / 8.0);
		if (distance2D.x <= lineWidth * pixelSize || distance2D.y <= lineWidth * pixelSize) {
			fragColor.rgb = lineColor;
		}
	}
}