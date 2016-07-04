#version 330 core

// I/O primitives
layout(points) in;
layout(line_strip, max_vertices = 60) out;

// Inputs
in int vLineNr[];
in int vType[];

// Outputs
flat out int fLineNr;
// 0 BG0, 1 BG1, 2 BG2, 3 BG3, 4 OBJ, 5 Backdrop
flat out int fTypeId;
out vec2 fTilePos;
flat out int fTileAddress;
flat out int fBGMapAddress;
flat out int fLargePalette;
flat out int fPaletteId;
flat out int fTemp;

// uniforms
uniform vec2 screenSize;
uniform usamplerBuffer vramData;
uniform usamplerBuffer oamData;

struct Registers {
	int dispcnt;
	ivec4 bgCnt;
	ivec2 bgOfs[4];
};

layout(std140) uniform DrawingRegisters {
	Registers dataRegisters[160];
};

int GetMode(int lineNr) {
	return dataRegisters[lineNr].dispcnt & 0x7;
}

bool IsRotScaleMode(int background, int lineNr) {
	int mode = GetMode(lineNr);
	if (mode >= 2) return true;
	if (mode == 0) return false;
	if (background == 2) return true;
	return false;
}

ivec2 GetScreenSize(int bgCnt, int background, int lineNr) {
	int screenSizeId = (bgCnt >> 14) & 3;
	if (IsRotScaleMode(background, lineNr)) {
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

void GenerateBackground(int background, int lineNr) {
	int bgCnt = dataRegisters[lineNr].bgCnt[background];
	ivec2 bgOfs = dataRegisters[lineNr].bgOfs[background];
	int priority = (bgCnt & 0x3);
	float depth = float(3 - priority) + float(3 - background) / 4.0;
	depth = (depth + 1.0) / 5.0;

	int bgY = lineNr - bgOfs.y;
	int baseTileAddress = ((bgCnt >> 2) & 3) * 0x4000;
	int baseMapAddress = ((bgCnt >> 8) & 31) * 0x800;
	bool largePalette = (bgCnt & (1 << 7)) != 0;
	bool rotScaleMode = IsRotScaleMode(background, lineNr);
	ivec2 bgSize = GetScreenSize(bgCnt, background, lineNr);
	ivec2 bgTileSize = bgSize / 8;
	for (int x = 0; x < screenSize.x; ) {
		int bgX = x - bgOfs.x;
		ivec2 clippedPos = ivec2(bgX, bgY);
		if ((bgCnt & (1 << 13)) != 0) {
			// wraparound background
			if (clippedPos.x < 0) clippedPos.x += bgSize.x;
			if (clippedPos.y < 0) clippedPos.y += bgSize.y;
			if (clippedPos.x >= bgSize.x) clippedPos.x -= bgSize.x;
			if (clippedPos.x >= bgSize.y) clippedPos.y -= bgSize.y;
		}
		else {
			// Transparent background
			if (clippedPos.x < 0 || clippedPos.y < 0 ||
				clippedPos.x >= bgSize.x || clippedPos.y >= bgSize.y)
				continue;
		}
		
		int mapId = clippedPos.x / 8 + (clippedPos.y / 8) * (bgSize.x / 8);


		int tileId;
		bool hFlip;
		bool vFlip;
		if (rotScaleMode) {
			fBGMapAddress = lineNr * 96 * 1024 + baseMapAddress + mapId;
			tileId = int(texelFetch(vramData, fBGMapAddress).r);
			hFlip = false;
			vFlip = false;
			fPaletteId = 0;
			largePalette = true;
		}
		else {
			fBGMapAddress = lineNr * 96 * 1024 + baseMapAddress + mapId * 2;
			uint data = texelFetch(vramData, fBGMapAddress).r;
			data |= texelFetch(vramData, fBGMapAddress + 1).r << 8;
			tileId = int(data & 0x3FFu);
			hFlip = (data & (1u << 10u)) != 0u;
			vFlip = (data & (1u << 11u)) != 0u;
			fPaletteId = int((data << 12u) & 0xFu);
		}

		if (largePalette) {
			fTileAddress = lineNr * 96 * 1024 + baseTileAddress + tileId * 64;
			fLargePalette = 1;
		}
		else {
			fTileAddress = lineNr * 96 * 1024 + baseTileAddress + tileId * 32;
			fLargePalette = 0;
		}

		vec4 vertexPos = vec4((float(x) + 0.5) * 2.0 / float(screenSize.x) - 1.0, gl_in[0].gl_Position.y, depth, 1.0);

		fLineNr = lineNr;
		fTypeId = background;

		fTilePos.y = float(vFlip ? (7 - (clippedPos.y & 7)) : (clippedPos.y & 7));
		fTilePos.x = float(hFlip ? (7 - (clippedPos.x & 7)) : (clippedPos.x & 7));
		gl_Position = vertexPos;
		EmitVertex();

		fTilePos.y = float(vFlip ? (7 - (clippedPos.y & 7)) : (clippedPos.y & 7));
		fTilePos.x = float(hFlip ? (7 - ((clippedPos.x + 7) & 7)) : ((clippedPos.x + 7) & 7));
		vertexPos.x += 8.0 * 2.0 / float(screenSize.x);
		gl_Position = vertexPos;
		EmitVertex();

		EndPrimitive();
		x += 8;
	}
	
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

bool IsObjectRotScaleMode(uint attr0) {
	return ((attr0 & (1u << 8u)) != 0u);
}

bool IsObjectEnabled(uint attr0) {
	 return ((attr0 & (1u << 9u)) == 0u) || IsObjectRotScaleMode(attr0);
}

void GenerateObjects(int lineNr) {
	bool obj1Dmapping = (dataRegisters[lineNr].dispcnt & (1 << 6)) != 0;
	int baseTileAddress = 96 * 1024 * lineNr + 0x10000;
	for (int i = 0; i < 128; i++) {
		int baseOamAddr = (lineNr * 1024 + i * 8) / 2;
		uint attr0 = texelFetch(oamData, baseOamAddr + 0).r;
		uint attr1 = texelFetch(oamData, baseOamAddr + 1).r;
		uint attr2 = texelFetch(oamData, baseOamAddr + 2).r;
		//uint attr3 = texelFetch(oamData, i * 8 + 6).r;

		ivec2 pos = ivec2(int(attr1 & 0x1FFu), int(attr0 & 0xFFu));
		ivec2 objectSize = GetObjectSize(attr0, attr1);
		if (!IsObjectEnabled(attr0) || (lineNr < pos.y || lineNr >= pos.y + objectSize.y)) {
			continue;
		}

		bool largePalette = (attr0 & (1u << 13u)) != 0u;
		bool hFlip = (attr1 & (1u << 12u)) != 0u;
		bool vFlip = (attr1 & (1u << 13u)) != 0u;
		int baseTileId = int(attr2 & 0x3FFu);
		int paletteId = int((attr2 >> 12u) & 0xFu);
		int priority = int((attr2 >> 10u) & 0x3u);

		if (largePalette || !obj1Dmapping) {
			baseTileId &= ~1;
		}

		int widthTiles = objectSize.x / 8;
		int tileY = (lineNr - pos.y) / 8;
		float depth = float(3 - priority) + 0.75 + float(127 - i) / (4.0 * 128);
		depth = (depth + 1.0) / 5.0;
		for (int tileX = 0; tileX < widthTiles; tileX++) {


			int tileOffset;
			if (obj1Dmapping) {
				tileOffset = tileY * widthTiles;
			}
			else {
				tileOffset = tileY * 32;
			}
			
			int tileId;
			if (largePalette) {
				tileId = baseTileId + 2 * tileX + tileOffset;
			}
			else {
				tileId = baseTileId + tileX + tileOffset;
			}

			if (GetMode(lineNr) >= 3 && tileId < 512) {
				continue;
			}


			fTileAddress = baseTileAddress + tileId * 32;
			
			fLineNr = lineNr;
			fTypeId = 4;
			fLargePalette = largePalette ? 1 : 0;
			fTemp = tileOffset;

			vec4 vertexPos = vec4((float(pos.x + tileX * 8) + 0.5) * 2.0 / screenSize.x - 1.0, 
				gl_in[0].gl_Position.y, depth, 1.0);
			fTilePos.y = float(vFlip ? (7 - ((lineNr - pos.y) & 7)) : ((lineNr - pos.y) & 7));
			fTilePos.x = float(hFlip ? 8 : 0);
			gl_Position = vertexPos;
			EmitVertex();

			fTilePos.y = float(vFlip ? (7 - ((lineNr - pos.y) & 7)) : ((lineNr - pos.y) & 7));
			fTilePos.x = float(hFlip ? 0 : 8);
			vertexPos.x += 8.0 * 2.0 / screenSize.x;
			gl_Position = vertexPos;
			EmitVertex();

			EndPrimitive();
		}
	}
}

void GenerateBackdrop(int lineNr) {
	gl_Position = vec4(gl_in[0].gl_Position.xy, 0.0, 1.0);
	fTypeId = 5;
	fLineNr = lineNr;
	EmitVertex();

	gl_Position = vec4(gl_in[0].gl_Position.xy + vec2(2.0, 0.0), 0.0, 1.0);
	EmitVertex();
	EndPrimitive();
}

void main() {
	int lineNr = vLineNr[0];
	int dispcnt = dataRegisters[lineNr].dispcnt;
	switch (vType[0]) {
	case 0:
		// Generate background tiles
		for (int i = 0; i < 4; i++)
		{
			if ((dispcnt & (1 << (8 + i))) != 0) {
				GenerateBackground(i, lineNr);
			}
		}
		break;
	case 1:
		// Generate objects
		if ((dispcnt & (1 << 12)) != 0) {
			GenerateObjects(lineNr);
		}
		break;
	case 2: 
		GenerateBackdrop(lineNr);
		break;
	}
}