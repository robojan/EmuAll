#line 2 3

// I/O primitives
layout(points) in;
layout(line_strip, max_vertices = 60) out;

// Inputs
in int gLineNr[];

// Outputs
flat out int fLineNr;
flat out int fRSMode;
flat out uvec3 fAttr;
out vec2 fPixelPos;

// uniforms
uniform bool maskDrawing;

int GetObjectY(int lineNr, int posY, int sizeY) {
	if (sizeY >= 128 && posY > 128) {
		posY -= 256;
	}
	if (posY + sizeY > 256) {
		return lineNr + 256 - posY;
	}
	else {
		return lineNr - posY;;
	}
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

		bool RSMode = IsObjectRotScaleMode(attr0);
		bool doubleSize = IsObjectDoubleSize(attr0);
		int objSizeModifier = doubleSize && RSMode ? 2 : 1;
		ivec2 pos = GetObjectPos(attr0, attr1);
		ivec2 objectSize = GetObjectSize(attr0, attr1);

		int objY = GetObjectY(lineNr, pos.y, objectSize.y * objSizeModifier);
		int objMode = GetObjectMode(attr0);

		if (!IsObjectEnabled(attr0) || (objY < 0 || objY >= objectSize.y * objSizeModifier) ||
			(objMode == 2 && !maskDrawing) || (objMode != 2 && maskDrawing)) {
			continue;
		}
		

		int priority = int((attr2 >> 10u) & 0x3u);		

		float depth = float(3 - priority) + 0.75 + 128.0 / (4.0 * 129.0);
		depth = (depth + 1.0) / 5.0;

		bool hFlip, vFlip;
		int startX = pos.x;
		int endX;
		if (RSMode) {
			hFlip = false;
			vFlip = false;
			endX = startX + objectSize.x * objSizeModifier;
		}
		else {
			hFlip = IsObjectHFlip(attr1);
			vFlip = IsObjectVFlip(attr1);
			endX = startX + objectSize.x;
		}

		vec4 vertexPos = vec4(startX * 2.0 / screenSize.x - 1.0,
			gl_in[0].gl_Position.y, depth, 1.0);

		// Emit line from start of object to the end of the object
		fLineNr = lineNr;
		fRSMode = RSMode ? 1 : 0;
		fAttr = uvec3(attr0, attr1, attr2);

		fPixelPos.y = float(vFlip ? objectSize.y - objY : objY);
		fPixelPos.x = float(hFlip ? objectSize.x * objSizeModifier : 0);
		gl_Position = vertexPos;
		EmitVertex();

		fPixelPos.y = float(vFlip ? objectSize.y - objY : objY);
		fPixelPos.x = float(hFlip ? 0 : objectSize.x * objSizeModifier);
		vertexPos.x = endX * 2.0 / screenSize.x - 1.0;
		gl_Position = vertexPos;
		EmitVertex();

		EndPrimitive();
	}
}

void main() {
	int lineNr = gLineNr[0];
	if (IsObjectLayerEnabled(lineNr)) {
		GenerateObjects(lineNr);
	}
}