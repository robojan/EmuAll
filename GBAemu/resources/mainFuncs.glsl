#line 2 1

int GetMode(int lineNr) {
	return dataRegisters[lineNr].dispcnt & 0x7;
}

vec3 getPaletteColor(uint idx, int lineNr) {
	ivec3 size = textureSize(paletteData, 0).xyz;
	ivec2 colorPos = ivec2(int(idx) % size.x, int(idx) / size.x);
	vec2 paletteUV = (vec2(colorPos) + vec2(0.5, 0.5)) / vec2(size.xy);
	return texture(paletteData, vec3(paletteUV, float(lineNr))).rgb;
}

int GetVRAMaddress(int address, int lineNr) {
	return lineNr * 96 * 1024 + address;
}

bool IsFrameSelect(int lineNr) {
	return (dataRegisters[lineNr].dispcnt & (1 << 4)) != 0;
}

bool IsBackgroundLargePalette(int background, int lineNr) {
	int bgcnt = dataRegisters[lineNr].bgCnt[background];
	return (bgcnt & (1 << 7)) != 0;
}

bool IsBackgroundWraparound(int background, int lineNr) {
	int bgcnt = dataRegisters[lineNr].bgCnt[background];
	return (bgcnt & (1 << 13)) != 0;
}

bool IsBackgroundEnabled(int background, int lineNr)
{
	int dispcnt = dataRegisters[lineNr].dispcnt;
	bool bgEnabled = (dispcnt & (1 << (8 + background))) != 0;
	switch (dispcnt & 7) {
	default:
	case 0:
		return bgEnabled;
	case 1:
		return bgEnabled && background < 3;
	case 2:
		return bgEnabled && background >= 2;
	case 3:
	case 4:
	case 5:
		return bgEnabled && background == 2;
	}
}

bool IsBackgroundRotScaleMode(int background, int lineNr) {
	int mode = GetMode(lineNr);
	if (mode >= 2) return true;
	if (mode == 0) return false;
	if (background == 2) return true;
	return false;
}

ivec2 GetBackgroundSize(int background, int lineNr) {
	int bgCnt = dataRegisters[lineNr].bgCnt[background];
	int screenSizeId = (bgCnt >> 14) & 3;
	if (IsBackgroundRotScaleMode(background, lineNr)) {
		screenSizeId |= 4;
	}
	switch (screenSizeId) {
	case 0:
	default:
		return ivec2(256, 256);
	case 1:
		return ivec2(512, 256);
	case 2:
		return ivec2(256, 512);
	case 3:
		return ivec2(512, 512);
	case 4:
		return ivec2(128, 128);
	case 5:
		return ivec2(256, 256);
	case 6:
		return ivec2(512, 512);
	case 7:
		return ivec2(1024, 1024);
	}
}

ivec2 GetBackgroundOffset(int background, int lineNr) {
	return dataRegisters[lineNr].bgOfs[background];
}

vec2 GetBackgroundRefPoint(int background, int lineNr) {
	if (background < 2) {
		return vec2(0.0, 0.0);
	}
	return dataRegisters[lineNr].bgRefPoint[background - 2];
}

mat2 GetBackgroundAffineMatrix(int background, int lineNr) {
	if (background < 2) {
		return mat2(1.0);
	}
	//return dataRegisters[lineNr].bgAffineMatrix[background - 2];
	//return mat2(1.0);
	vec4 mat = dataRegisters[lineNr].bgAffineMatrix[background - 2];
	return mat2(mat.xy, mat.zw);
}

float GetBackgroundDepth(int background, int lineNr) {
	int bgCnt = dataRegisters[lineNr].bgCnt[background];
	int priority = (bgCnt & 0x3);

	float depth = float(3 - priority) + float(3 - background) / 4.0;
	depth = (depth + 1.0) / 5.0;
	return depth;
}

int GetBackgroundMapBaseAddress(int background, int lineNr) {
	int bgCnt = dataRegisters[lineNr].bgCnt[background];
	return ((bgCnt >> 8) & 31) * 0x800;
}

int GetBackgroundMapAddressOffsetTextMode(ivec2 tilePos, int background, int lineNr) {
	int bgCnt = dataRegisters[lineNr].bgCnt[background];
	int screenSizeId = (bgCnt >> 14) & 3;
	ivec2 mapIdx = tilePos / 32;
	tilePos = tilePos % 32;
	int mapId = (tilePos.x + tilePos.y * 32);
	switch (screenSizeId) {
	case 0:
	default:
		return mapId * 2;
	case 1:
		return mapIdx.x * 0x800 + mapId * 2;
	case 2: 
		return mapIdx.y * 0x800 + mapId * 2;
	case 3:
		return (mapIdx.x + mapIdx.y * 2) * 0x800 + mapId * 2;
	}
}

int GetBackgroundMapAddressOffsetRSMode(ivec2 tilePos, int background, int lineNr) {
	ivec2 bgSize = GetBackgroundSize(background, lineNr) / 8;
	int mapId = (tilePos.x + tilePos.y * bgSize.x);
	return mapId;
}

int GetBackgroundTileBaseAddress(int background, int lineNr) {
	int bgCnt = dataRegisters[lineNr].bgCnt[background];
	return ((bgCnt >> 2) & 3) * 0x4000;
}

bool IsObjectLayerEnabled(int lineNr) {
	return (dataRegisters[lineNr].dispcnt & (1 << 12)) != 0;
}

bool IsObjectRotScaleMode(uint attr0) {
	return ((attr0 & (1u << 8u)) != 0u);
}

bool IsObjectEnabled(uint attr0) {
	return ((attr0 & (1u << 9u)) == 0u) || IsObjectRotScaleMode(attr0);
}

ivec2 GetObjectSize(uint attr0, uint attr1) {
	uint id = ((attr0 >> 14u) & 3u) | (((attr1 >> 14u) & 3u) << 2u);
	switch (id) {
	default:
	case 0x0u: return ivec2(8, 8);
	case 0x1u: return ivec2(16, 8);
	case 0x2u: return ivec2(8, 16);
	case 0x4u: return ivec2(16, 16);
	case 0x5u: return ivec2(32, 8);
	case 0x6u: return ivec2(8, 32);
	case 0x8u: return ivec2(32, 32);
	case 0x9u: return ivec2(32, 16);
	case 0xau: return ivec2(16, 32);
	case 0xcu: return ivec2(64, 64);
	case 0xdu: return ivec2(64, 32);
	case 0xeu: return ivec2(32, 64);
	}
}

int GetObjectTileAddress(int lineNr, int tileId) {
	return lineNr * 96 * 1024 + 0x10000 + tileId * 32;
}

int GetObjectAttributeAddress(int lineNr, int object) {
	return lineNr * 1024/2 + object * 4;
}

int GetObjectTileId(uint attr2) {
	return int(attr2 & 0x3FFu);
}

int GetObjectPalette(uint attr2) {
	return int(attr2 >> 12) & 0xF;
}

ivec2 GetObjectPos(uint attr0, uint attr1) {
	return ivec2(int(attr1) & 0x1FF, int(attr0) & 0xFF);
}

bool IsObjectHFlip(uint attr1) {
	return (attr1 & 0x1000u) != 0u;
}

bool IsObjectVFlip(uint attr1) {
	return (attr1 & 0x2000u) != 0u;
}

bool IsObjectLargePalette(uint attr0) {
	return (attr0 & 0x2000u) != 0u;
}

bool IsObjectDoubleSize(uint attr0) {
	return (attr0 & (1u << 9u)) != 0u;
}

mat2 GetObjectAffineMatrix(int lineNr, uint attr1) {
	int idx = int(attr1 >> 9) & 0x1F;
	int baseAddr = lineNr * 1024 / 2 + 3;
	uvec4 matvecu = uvec4(texelFetch(oamData, baseAddr + (idx * 4 + 0) * 4).r,
		texelFetch(oamData, baseAddr + (idx * 4 + 1) * 4).r,
		texelFetch(oamData, baseAddr + (idx * 4 + 2) * 4).r,
		texelFetch(oamData, baseAddr + (idx * 4 + 3) * 4).r);
	bvec4 signExtend = notEqual((matvecu & uvec4(1u << 15u)), uvec4(0));
	ivec4 matvec = ivec4((uvec4(signExtend) * (0xFFFF0000u)) | matvecu);
	return mat2(vec2(matvec.xy)/256.0, vec2(matvec.zw) / 256.0);
}

int GetObjectMode(uint attr0) {
	return int((attr0 >> 10u) & 0x3u);
}

bool AreWindowsEnabled(int lineNr) {
	return (dataRegisters[lineNr].dispcnt & (0xE000)) != 0;
}

bool IsWindowEnabled(int lineNr, int window) {
	return (dataRegisters[lineNr].dispcnt & (1 << (13 + window))) != 0;
}

bool IsInsideWindow(int lineNr, ivec2 pos, int window) {
	ivec4 winDim = dataRegisters[lineNr].windim[window];
	ivec2 TL = winDim.xy;
	ivec2 BR = winDim.zw;
	if (BR.x > 240 || TL.x > BR.x) BR.x = 240;
	if (BR.y > 160 || TL.y > BR.y) BR.y = 160;
	return all(greaterThan(pos, TL)) && all(lessThan(pos, BR));
}

int GetEffectMode(int lineNr) {
	return ((dataRegisters[lineNr].effects.x >> 6) & 3);
}
