//#version 330 core

// I/O primitives
layout(points) in;
layout(line_strip, max_vertices = 60) out;

// Inputs
in int gLineNr[];

// Outputs
flat out int fLineNr;
flat out int fPaletteId;
flat out int fLargePalette;
flat out int fTileAddress;
out vec2 fTilePos;

// uniforms

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
			fLargePalette = largePalette ? 1 : 0;

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

void main() {
	int lineNr = gLineNr[0];
	if (IsObjectLayerEnabled(lineNr)) {
		GenerateObjects(lineNr);
	}
}