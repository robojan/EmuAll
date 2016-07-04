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
out float fObjPos;
out vec2 fTilePos;
flat out int fBGTileAddress;
flat out int fBGMapAddress;
flat out int fLargePalette;
flat out int fPaletteId;

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

bool IsRotScaleMode(int background, int lineNr) {
	int mode = dataRegisters[lineNr].dispcnt;
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
	float depth = -float(priority) - float(lineNr) / 4.0;

	// scale depth from -100 - 100 to 0.0 - 1.0
	depth = (depth + 100) / 200;

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
			fBGTileAddress = lineNr * 96 * 1024 + baseTileAddress + tileId * 64;
			fLargePalette = 1;
		}
		else {
			fBGTileAddress = lineNr * 96 * 1024 + baseTileAddress + tileId * 32;
			fLargePalette = 0;
		}

		vec4 vertexPos = vec4((float(x) + 0.5) * 2.0 / float(screenSize.x) - 1.0, gl_in[0].gl_Position.y, depth, 1.0);

		fLineNr = lineNr;
		fTypeId = background;

		fTilePos.y = float(vFlip ? (7 - (clippedPos.y & 7)) : (clippedPos.y & 7));
		fTilePos.x = float(hFlip ? (7 - (clippedPos.x & 7)) : (clippedPos.x & 7));
		fObjPos = 0.0;
		gl_Position = vertexPos;
		EmitVertex();

		fTilePos.y = float(vFlip ? (7 - (clippedPos.y & 7)) : (clippedPos.y & 7));
		fTilePos.x = float(hFlip ? (7 - ((clippedPos.x + 7) & 7)) : ((clippedPos.x + 7) & 7));
		fObjPos = 1.0;
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

void GenerateObjects(int lineNr) {
	for (int i = 0; i < 128; i++) {
		uint attr0 = texelFetch(oamData, lineNr * 1024 + i * 8 + 0).r;
		uint attr1 = texelFetch(oamData, lineNr * 1024 + i * 8 + 2).r;
		uint attr2 = texelFetch(oamData, lineNr * 1024 + i * 8 + 4).r;
		//uint attr3 = texelFetch(oamData, i * 8 + 6).r;

		ivec2 pos = ivec2(int(attr1 & 0x1FFu), int(attr0 & 0xFFu));
		ivec2 objectSize = GetObjectSize(attr0, attr1);
		if (pos.y + objectSize.y < lineNr || pos.y > lineNr) {
			continue;
		}

		float depth = 0.0;

		fTypeId = 4;
		fLineNr = lineNr;
		vec4 vertexPos = vec4((float(pos.x) + 0.5) * 2.0 / float(screenSize.x) - 1.0, gl_in[0].gl_Position.y, depth, 1.0);
		gl_Position = vertexPos;
		EmitVertex();

		vertexPos.x += float(objectSize.x) * 2.0 / float(screenSize.x);
		gl_Position = vertexPos;
		EmitVertex();

		EndPrimitive();
	}
}

void main() {
	int lineNr = vLineNr[0];
	switch (vType[0]) {
	case 0:
		// Generate background tiles
		for (int i = 0; i < 4; i++)
		{
			if ((dataRegisters[lineNr].dispcnt & (1 << (8 + i))) != 0) {
				GenerateBackground(i, lineNr);
			}
		}
		break;
	case 1:
		// Generate objects
		if ((dataRegisters[lineNr].dispcnt & (1 << 15)) != 0 || 1 == 1) {
			//GenerateObjects(lineNr);
			gl_Position = vec4(-0.1, gl_in[0].gl_Position.y, 0.0, 1.0);
			fTypeId = 4;
			fLineNr = lineNr;
			EmitVertex();

			gl_Position = vec4(0.1, gl_in[0].gl_Position.y, 0.0, 1.0);
			EmitVertex();
			EndPrimitive();
		}
		break;
	case 2: 
		// Generate backdrop
		gl_Position = vec4(gl_in[0].gl_Position.xy, 0.0, 1.0);
		fTypeId = 5;
		fLineNr = lineNr;
		EmitVertex();

		gl_Position = vec4(gl_in[0].gl_Position.xy + vec2(2.0,0.0), 0.0, 1.0);
		EmitVertex();
		EndPrimitive();
		break;
	}
}