#line 2 3

// Input
flat in int fId;
flat in ivec2  fTilePos;
in vec2 fPixelPos;
flat in int fTileAddress;
flat in int fLargePalette;
flat in int fPaletteId;
flat in int fMode;
flat in int fTemp;

// Output
out vec4 fragColor;

// Background
uniform int lineNr;
uniform int background;

// constants
const float lineWidth = 1.0;
const vec3 lineColor = vec3(0.9, 0.9, 0.9);
const vec3 disabledColor = vec3(1.0, 0.0, 0.0);
const vec3 unusedColor = vec3(0.6, 0.6, 0.6);
const vec3 windowColor = vec3(1.0, 0.0, 0.0);

void main() {
	vec3 color = unusedColor;
	ivec2 pixelPos = ivec2(fPixelPos);
	switch (fMode) {
	case 0: { // text mode
		int innerId = pixelPos.x + pixelPos.y * 8;
		if (fLargePalette != 0) {
			int pixelAddress = fTileAddress + innerId;
			uint paletteIdx = texelFetch(vramData, pixelAddress).r;
			color = getPaletteColor(paletteIdx, lineNr);
		}
		else {
			int pixelAddress = fTileAddress + innerId / 2;
			uint paletteIdx = texelFetch(vramData, pixelAddress).r;
			if ((pixelAddress & 0x1) != 0) {
				paletteIdx >>= 4u;
			}
			paletteIdx &= 0xFu;
			color = getPaletteColor(paletteIdx + uint(fPaletteId * 16), lineNr);
		}
		break;
	}		
	case 2: {// Rot/scale mode
		int innerId = pixelPos.x + pixelPos.y * 8;
		if (fLargePalette != 0) {
			int pixelAddress = fTileAddress + innerId;
			uint paletteIdx = texelFetch(vramData, pixelAddress).r;
			color = getPaletteColor(paletteIdx, lineNr);
			//int address = pixelAddress % (96 * 1024);
			//int address = fTemp;
			//color = vec3(float((address >> 16) & 0xFF) / 255.0, float((address >> 8) & 0xFF) / 255.0, float((address >> 0) & 0xFF) / 255.0);
			//color = vec3(float(fTilePos.x) / 255.0, float(fTilePos.y) / 255.0, 0.0);
		}
		else {
			int pixelAddress = fTileAddress + innerId / 2;
			uint paletteIdx = texelFetch(vramData, pixelAddress).r;
			if ((pixelAddress & 0x1) != 0) {
				paletteIdx >>= 4u;
			}
			paletteIdx &= 0xFu;
			color = getPaletteColor(paletteIdx + uint(fPaletteId * 16), lineNr);
		}
		//color = vec3(1.0, 1.0, 0.0);
		break;
	}
	}
	fragColor = vec4(color, 1.0);
}