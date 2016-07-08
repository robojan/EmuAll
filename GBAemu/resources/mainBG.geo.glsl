#line 2 3

// I/O primitives
layout(points) in;
layout(line_strip, max_vertices = 2) out;

// Inputs
in int gLineNr[];

// Outputs
flat out int fLineNr;
flat out int fBGMode; // 0 text mode, 1 Rot scale mode, 2 Bitmap mode color, 3 bitmap mode palette, 4 bitmap mode color swap
flat out int fMapBaseAddr;
flat out int fTileBaseAddr;
flat out vec2 fRefPoint;
//flat out mat2 fAffineMatrix;
flat out int fLargePalette;
flat out ivec2 fBGSize;
flat out int fWrapAround;
out float fPosX;

// uniforms
uniform int background;

void SetupOutputs(int background, int lineNr) {
	fLineNr = lineNr;
	int mode = GetMode(lineNr);
	if (mode == 1) {
		// Change mixed mode to correct mode for the corresponding background
		mode = background == 2 ? 2 : 0;
	}
	switch (mode) {
	default:
	case 0: { // Text mode
		fBGMode = 0;
		fMapBaseAddr = GetBackgroundMapBaseAddress(background, lineNr);
		fTileBaseAddr = GetBackgroundTileBaseAddress(background, lineNr);
		fRefPoint = vec2(GetBackgroundOffset(background, lineNr));
		//fAffineMatrix = mat2(1.0);
		fLargePalette = IsBackgroundLargePalette(background, lineNr) ? 1 : 0;
		fBGSize = GetBackgroundSize(background, lineNr);
		fWrapAround = 1;
		break;
	}
	case 2: { // rot scale mode
		fBGMode = 1;
		fMapBaseAddr = GetBackgroundMapBaseAddress(background, lineNr);
		fTileBaseAddr = GetBackgroundTileBaseAddress(background, lineNr);
		fRefPoint = GetBackgroundRefPoint(background, lineNr);
		//fAffineMatrix = GetBackgroundAffineMatrix(background, fLineNr);
		//fAffineMatrix = mat2(1.0);
		fLargePalette = IsBackgroundLargePalette(background, lineNr) ? 1 : 0;
		fBGSize = GetBackgroundSize(background, lineNr);
		fWrapAround = IsBackgroundWraparound(background, lineNr) ? 1 : 0;
		break;
	}
	case 3: { // bitmap mode color
		fBGMode = 2;
		fMapBaseAddr = GetVRAMaddress(0, lineNr);
		fTileBaseAddr = 0;
		fRefPoint = vec2(0.0, 0.0);
		//fAffineMatrix = mat2(1.0);
		fLargePalette = 0;
		fBGSize = ivec2(240, 160);
		fWrapAround = 0;
		break;
	}
	case 4: { // bitmap mode palette
		fBGMode = 3;
		fMapBaseAddr = GetVRAMaddress(IsFrameSelect(lineNr) ? 0xA000 : 0x0000, lineNr);
		fTileBaseAddr = 0;
		fRefPoint = vec2(0.0, 0.0);
		//fAffineMatrix = mat2(1.0);
		fLargePalette = 1;
		fBGSize = ivec2(240, 160);
		fWrapAround = 0;
		break;
	}
	case 5: { // bitmap mode color swap
		fBGMode = 4;
		fMapBaseAddr = GetVRAMaddress(IsFrameSelect(lineNr) ? 0xA000 : 0x0000, lineNr);
		fTileBaseAddr = 0;
		fRefPoint = vec2(0.0, 0.0);
		//fAffineMatrix = mat2(1.0);
		fLargePalette = 0;
		fBGSize = ivec2(160, 128);
		fWrapAround = 0;
		break;
	}
	}
}

void main() {
	int lineNr = gLineNr[0];

	if (IsBackgroundEnabled(background, lineNr)) {
		float depth = GetBackgroundDepth(background, lineNr);
		
		vec4 vertexPosition = vec4(-1.0, gl_in[0].gl_Position.y, depth, 1.0);

		// Output left vertex
		SetupOutputs(background, lineNr);
		fPosX = 0.0;
		gl_Position = vertexPosition;
		EmitVertex();

		// Output right vertex
		//SetupOutputs(background, lineNr);
		fPosX = screenSize.x;
		gl_Position = vertexPosition + vec4(2.0, 0.0, 0.0, 0.0);
		EmitVertex();

		// Output primitive
		EndPrimitive();
	}
}
