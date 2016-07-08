#line 2 3
//#version 330 core

// Inputs
flat in int fLineNr;
flat in int fBGMode; // 0 text mode, 1 Rot scale mode, 2 Bitmap mode color, 3 bitmap mode palette, 4 bitmap mode color swap
flat in int fMapBaseAddr;
flat in int fTileBaseAddr;
flat in vec2 fRefPoint;
//flat in mat2 fAffineMatrix;
flat in int fLargePalette;
flat in ivec2 fBGSize;
flat in int fWrapAround;
in float fPosX;

// Outputs
out vec4 fragColor;

// Uniforms
uniform int background;

ivec2 GetPixelPos() {
	ivec2 screenPos = ivec2(int(fPosX), fLineNr);
	return screenPos;
}

ivec2 WrapAround(ivec2 pos, ivec2 size) {
	while (pos.x < 0) pos.x += size.x;
	while (pos.y < 0) pos.y += size.y;
	while (pos.x >= size.x) pos.x -= size.x;
	while (pos.y >= size.y) pos.y -= size.y;
	return pos;
}

void main() {
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	ivec2 pixelPos = GetPixelPos();
	switch (fBGMode) {
	default:
	case 0: { // Text mode
		// Transform position
		pixelPos = pixelPos + ivec2(fRefPoint);
		pixelPos = WrapAround(pixelPos, fBGSize);

		ivec2 tilePos = pixelPos / 8;
		ivec2 innerPos = pixelPos % 8;
		int mapOffset = GetBackgroundMapAddressOffsetTextMode(tilePos, background, fLineNr);
		
		// Get tile data
		int mapAddress = fMapBaseAddr + mapOffset;
		uint data = texelFetch(vramData, mapAddress).r;
		data |= texelFetch(vramData, mapAddress + 1).r << 8;
		int tileId = int(data & 0x3FFu);
		bool hFlip = (data & (1u << 10u)) != 0u;
		bool vFlip = (data & (1u << 11u)) != 0u;
		int paletteId = int((data << 12u) & 0xFu);
		int innerId = innerPos.x + innerPos.y * 8;

		if (fLargePalette != 0) {
			int tileAddress = fTileBaseAddr + tileId * 64;
			int pixelAddress = tileAddress + innerId;
			uint paletteIdx = texelFetch(vramData, pixelAddress).r;
			if (paletteIdx == 0u) discard;
			color = vec4(getPaletteColor(paletteIdx, fLineNr), 1.0);
		}
		else {
			int tileAddress = fTileBaseAddr + tileId * 32;
			int pixelAddress = tileAddress + innerId / 2;
			uint paletteIdx = texelFetch(vramData, pixelAddress).r;
			if ((pixelAddress & 0x1) != 0) {
				paletteIdx >>= 4u;
			}
			paletteIdx &= 0xFu;
			if (paletteIdx == 0u) discard;
			color = vec4(getPaletteColor(paletteIdx + uint(paletteId * 16), fLineNr), 1.0);
		}
		break;
	}
	case 1: { // Rot scale mode
		pixelPos = ivec2(vec2(pixelPos) * GetBackgroundAffineMatrix(background, fLineNr) + fRefPoint);
		if (fWrapAround != 0) {
			pixelPos = WrapAround(pixelPos, fBGSize);
		}
		else {
			if (pixelPos.x < 0 || pixelPos.y < 0 ||
				pixelPos.x >= fBGSize.x || pixelPos.y >= fBGSize.y) {
				discard;
			}
		}
		ivec2 tilePos = pixelPos / 8;
		ivec2 innerPos = pixelPos % 8;
		int mapOffset = GetBackgroundMapAddressOffsetRSMode(tilePos, background, fLineNr);

		// Get tile data
		int mapAddress = fMapBaseAddr + mapOffset;
		uint data = texelFetch(vramData, mapAddress).r;
		int tileId = int(data);
		int innerId = innerPos.x + innerPos.y * 8;

		int tileAddress = fTileBaseAddr + tileId * 64;
		int pixelAddress = tileAddress + innerId;
		uint paletteIdx = texelFetch(vramData, pixelAddress).r;
		if (paletteIdx == 0u) discard;
		color = vec4(getPaletteColor(paletteIdx, fLineNr), 1.0);
		break;
	}
	case 2: { // bitmap mode color
		break;
	}
	case 3: { // bitmap mode palette
		break;
	}
	case 4: { // bitmap mode color swap
		break;
	}
	}
	fragColor = color;
}