#line 2 3

// Inputs
flat in int fLineNr;
flat in int fRSMode;
flat in uvec3 fAttr;
in vec2 fPixelPos;

// Outputs
out vec4 fragColor;

// Uniforms

void main() {
	ivec2 objSize = GetObjectSize(fAttr.x, fAttr.y);
	ivec2 pixelPos = ivec2(fPixelPos);
	if (fRSMode != 0) { // Rotate scale mode
		bool doubleSize = IsObjectDoubleSize(fAttr.x);
		int objSizeModifier = doubleSize ? 2 : 1;
		mat2 affineMatrix = GetObjectAffineMatrix(fLineNr, fAttr.y);
		vec2 q = vec2(pixelPos - (objSize * objSizeModifier) / 2);
		vec2 p = q * affineMatrix;
		pixelPos = ivec2(p) + objSize / 2;
		if (any(lessThan(pixelPos, ivec2(0))) || any(greaterThanEqual(pixelPos, objSize))) {
			discard;
		}
	}
	ivec2 tilePos = pixelPos / 8;
	ivec2 innerPos = pixelPos % 8;
	int baseTileId = GetObjectTileId(fAttr.z);
	bool largePalette = IsObjectLargePalette(fAttr.x);
	bool oneDimCharMapping = (dataRegisters[fLineNr].dispcnt & 0x40) != 0;
	ivec2 tileDim = objSize / 8;
	if (largePalette) {
		baseTileId &= ~1;
		int tileOffset = 2 * tilePos.x;
		if (oneDimCharMapping) {
			tileOffset += tilePos.y * tileDim.x * 2;
		}
		else {
			tileOffset += tilePos.y * 32;
		}
		int innerId = innerPos.y * 8 + innerPos.x;
		int address = GetObjectTileAddress(fLineNr, baseTileId + tileOffset) + innerId;
		uint data = texelFetch(vramData, address).r;
		if (data == 0u) discard;
		fragColor = vec4(getPaletteColor(data + 256u, fLineNr), 1.0);
	}
	else {
		int tileOffset;
		if (oneDimCharMapping) {
			tileOffset = tilePos.y * tileDim.x + tilePos.x;
		}
		else {
			tileOffset = tilePos.y * 32 + tilePos.x;
		}
		int innerId = innerPos.y * 8 + innerPos.x;
		int address = GetObjectTileAddress(fLineNr, baseTileId + tileOffset) + innerId / 2;
		uint data = texelFetch(vramData, address).r;
		if ((innerId & 1) != 0) data >>= 4u;
		data &= 0xFu;
		if (data == 0u) discard;
		int paletteId = GetObjectPalette(fAttr.z);
		fragColor = vec4(getPaletteColor(data + 256u + uint(paletteId * 16), fLineNr), 1.0);
	}
}