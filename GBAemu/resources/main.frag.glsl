#version 330 core

// Input
flat in int fLineNr;
flat in int fTypeId;
flat in int fBGTileAddress;
flat in int fBGMapAddress;
flat in int fLargePalette;
flat in int fPaletteId;

in vec2 fTilePos;

// Output
out vec4 fragColor;

// uniforms
uniform sampler2DArray paletteData;
uniform usamplerBuffer vramData;

vec3 getPaletteColor(uint idx) {
	ivec3 size = textureSize(paletteData, 0).xyz;
	ivec2 colorPos = ivec2(int(idx) % size.x, int(idx) / size.x);
	vec2 paletteUV = (vec2(colorPos) + vec2(0.5, 0.5)) / vec2(size.xy);
	return texture(paletteData, vec3(paletteUV, float(fLineNr))).rgb;
}

void main() {
	switch (fTypeId) {
	case 0:
	case 1:
	case 2:
	case 3: {
		// Background
		int pixelId = int(fTilePos.x + fTilePos.y * 8);
		if (fLargePalette != 0) {
			uint paletteIdx = texelFetch(vramData, fBGTileAddress + pixelId).r;
			fragColor = vec4(getPaletteColor(paletteIdx), 1.0);
		}
		else {
			uint paletteIdx = texelFetch(vramData, fBGTileAddress + pixelId / 2).r;
			if ((pixelId & 0x1) != 0) {
				paletteIdx >>= 4u;
			}
			paletteIdx &= 0xFu;
			fragColor = vec4(getPaletteColor(paletteIdx + uint(fPaletteId * 16)), 1.0);
		}
		break;
	}
	case 4: { // Object
		fragColor = vec4(1.0, 0.0, 1.0, 1.0);
		break;
	}
	case 5: { // Backdrop
		fragColor = vec4(getPaletteColor(0u), 1.0);
		break;
	}
	default:
		fragColor = vec4(1.0, 0.0, 0.0, 1.0);
		break;
	}
}