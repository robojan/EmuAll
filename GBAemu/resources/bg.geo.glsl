#line 2 2

// I/O primitives
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// Inputs
in ivec2 gBGSize[];
in ivec2 gTilePos[];
in int gId[];

// Outputs
flat out int fId;
flat out ivec2  fTilePos;
out vec2 fPixelPos;
flat out int fTileAddress;
flat out int fLargePalette;
flat out int fPaletteId;
flat out int fMode;
flat out int fTemp;

uniform int lineNr;
uniform int background;

uint SetupOutputs(ivec2 tilePos) {
	int mode = GetMode(lineNr);
	if (mode == 1) {
		// Change mixed mode to correct mode for the corresponding background
		mode = background == 2 ? 2 : 0;
	}
	switch (mode) {
	default:
	case 0: { // Text mode
		int mapBaseAddr = GetBackgroundMapBaseAddress(background, lineNr);
		int tileBaseAddr = GetBackgroundTileBaseAddress(background, lineNr);
		int mapOffset = GetBackgroundMapAddressOffsetTextMode(tilePos, background, lineNr);
		int mapAddr = mapBaseAddr + mapOffset;
		fLargePalette = IsBackgroundLargePalette(background, lineNr) ? 1 : 0;

		uint data = texelFetch(vramData, mapAddr).r;
		data |= texelFetch(vramData, mapAddr + 1).r << 8;
		int tileId = int(data & 0x3FFu);
		fPaletteId = int((data >> 12u) & 0xFu);

		if (fLargePalette != 0) {
			fTileAddress = tileBaseAddr + tileId * 64;
		}
		else {
			fTileAddress = tileBaseAddr + tileId * 32;
		}
		fMode = 0;
		return data;
	}
	case 2: { // rot scale mode
		int mapBaseAddr = GetBackgroundMapBaseAddress(background, lineNr);
		int tileBaseAddr = GetBackgroundTileBaseAddress(background, lineNr);
		int mapOffset = GetBackgroundMapAddressOffsetRSMode(tilePos, background, lineNr);
		fLargePalette = 1;
		int mapAddr = mapBaseAddr + mapOffset;
		
		uint data = texelFetch(vramData, mapAddr).r;
		int tileId = int(data);
		fTileAddress = tileBaseAddr + tileId * 64;
		fMode = 2;
		fTemp = mapAddr % (96 * 1024);
		return 0u;
	}
	case 3: { // bitmap mode color
		fTileAddress = GetVRAMaddress(0, lineNr);
		fMode = 3;
		break;
	}
	case 4: { // bitmap mode palette
		fTileAddress = GetVRAMaddress(IsFrameSelect(lineNr) ? 0xA000 : 0x0000, lineNr);
		fMode = 4;
		break;
	}
	case 5: { // bitmap mode color swap
		fTileAddress = GetVRAMaddress(IsFrameSelect(lineNr) ? 0xA000 : 0x0000, lineNr);
		fMode = 5;
		break;
	}
	}
}

void main() {
	ivec2 tileSize = gBGSize[0] / 8;
	if (gId[0] < tileSize.x * tileSize.y) {
		vec2 rectSize = 1.0 / vec2(tileSize) ;

		uint data = SetupOutputs(gTilePos[0]);
		bool hFlip = (data & (1u << 10u)) != 0u;
		bool vFlip = (data & (1u << 11u)) != 0u;
		
		fId = gId[0];
		fTilePos = gTilePos[0];
		fPixelPos = vec2(hFlip ? 8.0 : 0.0, vFlip ? 8.0 : 0.0);
		gl_Position = vec4(gl_in[0].gl_Position.x - rectSize.x, gl_in[0].gl_Position.y + rectSize.y, gl_in[0].gl_Position.z, 1.0);
		EmitVertex();

		fPixelPos = vec2(hFlip ? 0.0 : 8.0, vFlip ? 8.0 : 0.0);
		gl_Position = vec4(gl_in[0].gl_Position.x + rectSize.x, gl_in[0].gl_Position.y + rectSize.y, gl_in[0].gl_Position.z, 1.0);
		EmitVertex();

		fPixelPos = vec2(hFlip ? 8.0 : 0.0, vFlip ? 0.0 : 8.0);
		gl_Position = vec4(gl_in[0].gl_Position.x - rectSize.x, gl_in[0].gl_Position.y - rectSize.y, gl_in[0].gl_Position.z, 1.0);
		EmitVertex();

		fPixelPos = vec2(hFlip ? 0.0 : 8.0, vFlip ? 0.0 : 8.0);
		gl_Position = vec4(gl_in[0].gl_Position.x + rectSize.x, gl_in[0].gl_Position.y - rectSize.y, gl_in[0].gl_Position.z, 1.0);
		EmitVertex();

		// Output primitive
		EndPrimitive();
	}
}
