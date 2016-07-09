#line 2 3

// Input
in vec2 fPos;

// Output
out vec4 fragColor;

// Uniforms
uniform int lineNr;

// constants
const float lineWidth = 1.0;
const vec3 lineColor = vec3(0.9, 0.9, 0.9);
const vec3 disabledColor = vec3(1.0, 0.0, 0.0);

float DistToLine(vec2 pt1, vec2 pt2, vec2 p) {
	vec2 lineDir = pt2 - pt1;
	vec2 perpDir = vec2(lineDir.y, -lineDir.x);
	vec2 dirToPt1 = pt1 - p;
	return abs(dot(normalize(perpDir), dirToPt1));
}

void main() {
	vec2 oamDim = vec2(16.0, 8.0);
	vec2 oamUVSize = vec2(1.0, 1.0) / oamDim;
	vec2 pixelSize = vec2(1.0, 1.0) / screenSize;
	vec2 pos = fPos * oamDim;
	ivec2 objPos = ivec2(pos);
	vec2 pixelPosf = fract(pos);

	int objIdx = objPos.y * 16 + objPos.x;

	int objectAddress = GetObjectAttributeAddress(lineNr, objIdx);
	uint attribute0 = texelFetch(oamData, objectAddress + 0).r;
	uint attribute1 = texelFetch(oamData, objectAddress + 1).r;
	uint attribute2 = texelFetch(oamData, objectAddress + 2).r;

	int tileId = GetObjectTileId(attribute2);
	int paletteId = GetObjectPalette(attribute2);
	bool rotScale = IsObjectRotScaleMode(attribute0);
	bool enabled = IsObjectEnabled(attribute0);
	ivec2 objSize = GetObjectSize(attribute0, attribute1);

	bool horFlip = IsObjectHFlip(attribute1);
	bool verFlip = IsObjectVFlip(attribute1);
	bool largePalette = IsObjectLargePalette(attribute0);
	bool oneDimCharMapping = (dataRegisters[lineNr].dispcnt & 0x40) != 0;

	ivec2 tileDim = objSize / 8;
	ivec2 tilePos = ivec2(pixelPosf * vec2(tileDim));
	vec2 innerPosF = fract(pixelPosf * vec2(tileDim));
	ivec2 innerPos = ivec2(innerPosF * 8.0);
	
	if (largePalette) {
		tileId &= ~1;
		int tileOffset = 2 * tilePos.x;
		if (oneDimCharMapping) {
			tileOffset += tilePos.y * tileDim.x;
		}
		else {
			tileOffset += tilePos.y * 32;
		}
		int pixelId = innerPos.y * 8 + innerPos.x;
		int address = GetObjectTileAddress(lineNr, tileId + tileOffset) + pixelId;
		uint data = texelFetch(vramData, address).r;
		fragColor.rgb = getPaletteColor(data + 256u, lineNr);
	}
	else {
		if (oneDimCharMapping) {
			tileId += tilePos.y * tileDim.x + tilePos.x;
		}
		else {
			tileId += tilePos.y * 32 + tilePos.x;
		}
		int pixelId = innerPos.y * 8 + innerPos.x;
		int address = GetObjectTileAddress(lineNr, tileId) + pixelId / 2;
		uint data = texelFetch(vramData, address).r;
		if ((pixelId & 1) != 0) data >>= 4u;
		data &= 0xFu;
		fragColor.rgb = getPaletteColor(uint(paletteId * 16) + data + 256u, lineNr);
	}
	
	// Draw disabled Line
	if (!enabled) {
		vec2 TLCorner = vec2(objPos) * oamUVSize;
		vec2 BRCorner = TLCorner + oamUVSize;
		if (DistToLine(TLCorner, BRCorner, fPos) < lineWidth * pixelSize.x) {
			fragColor.rgb = disabledColor;
		}
	}

	vec2 distance2D = min(pixelPosf, vec2(1.0, 1.0) - pixelPosf) / oamDim;
	if (any(lessThan(distance2D, lineWidth * pixelSize))) {
		fragColor.rgb = lineColor;
	}

	fragColor.a = 1.0;
} 
