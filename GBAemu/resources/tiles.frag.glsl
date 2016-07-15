#line 2 3 

// Input
in vec2 fPos;

// Output
out vec4 fragColor;

// Uniforms
uniform bool largePalette;
uniform int baseAddress;
uniform int lineNr;
uniform bool objectTiles;
uniform bool grid;

// Constants
const float lineWidth = 1.0;
const vec3 lineColor = vec3(0.9, 0.9, 0.9);

void main() {
	fragColor = vec4(lineColor, 1.0);
	vec2 lineSize = (lineWidth / screenSize);
	if (largePalette) {
		// 64 bytes per tile
		// each pixel 1 byte
		// 16 tiles per line
		// 16 lines
		vec2 pos = fPos * vec2(16.0,32.0);
		ivec2 tilePos = ivec2(pos);
		vec2  pixelPosf = mod(pos, 1.0);
		ivec2 pixelPos = ivec2(pixelPosf * 8.0);
		vec2 distEdge = min(pixelPosf, vec2(1.0, 1.0) - pixelPosf);

		if (all(greaterThan(distEdge, lineSize * vec2(8.0, 16.0))) || !grid) {
			int tileId = tilePos.y * 16 + tilePos.x;
			int pixelId = pixelPos.y * 8 + pixelPos.x;
			int address = baseAddress + tileId * 64 + pixelId;
			uint data = texelFetch(vramData, address).r;
			if (objectTiles) {
				// OBJ palette
				fragColor = vec4(getPaletteColor(data + 256u, lineNr), 1.0);
			}
			else {
				// BG palette
				fragColor = vec4(getPaletteColor(data, lineNr), 1.0);
			}
		}
	}
	else {
		// 32 bytes per tile
		// 2 pixels per byte
		// 32 tiles per line
		// 32 lines
		vec2 pos = fPos * 32.0;
		ivec2 tilePos = ivec2(pos);
		vec2  pixelPosf = mod(pos, 1.0);
		ivec2 pixelPos = ivec2(pixelPosf * 8.0);
		vec2 distEdge = min(pixelPosf, vec2(1.0, 1.0) - pixelPosf);

		if (all(greaterThan(distEdge, lineSize * 16.0)) || !grid) {
			int tileId = tilePos.y * 32 + tilePos.x;
			int pixelId = pixelPos.y * 8 + pixelPos.x;
			int address = baseAddress + tileId * 32 + pixelId / 2;
			uint data = texelFetch(vramData, address).r;
			if ((pixelId & 1) != 0) data >>= 4;
			data &= 0xFu;
			if (objectTiles) {
				// OBJ palette
				fragColor = vec4(getPaletteColor(data + 256u, lineNr), 1.0);
			}
			else {
				// BG palette
				fragColor = vec4(getPaletteColor(data, lineNr), 1.0);
			}
		}
	}
}