//#version 330 core

// Inputs
in vec2 fTilePos;
flat in int fLineNr;
flat in int fTileAddress;
flat in int fLargePalette;
flat in int fPaletteId;

// Outputs
out vec4 fragColor;

// Uniforms

void main() {
	int pixelId = int(fTilePos.x + fTilePos.y * 8);
	if (fLargePalette != 0) {
		uint paletteIdx = texelFetch(vramData, fTileAddress + pixelId).r;
		if (paletteIdx == 0u) discard;
		fragColor = vec4(getPaletteColor(paletteIdx + 256u, fLineNr), 1.0);
	}
	else {
		uint paletteIdx = texelFetch(vramData, fTileAddress + pixelId / 2).r;
		if ((pixelId & 0x1) != 0) {
			paletteIdx >>= 4u;
		}
		paletteIdx &= 0xFu;
		if (paletteIdx == 0u) discard;
		fragColor = vec4(getPaletteColor(paletteIdx + uint(fPaletteId * 16) + 256u, fLineNr), 1.0);
	}
}