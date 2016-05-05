#version 330 core

// Input
in vec2 uv;

// Output
out vec4 fragColor;

// Uniforms
uniform sampler2D paletteData;
uniform usamplerBuffer vramData;
uniform usamplerBuffer oamData;
uniform uint DISPCNT;
uniform vec2 windowSize;

// constants
const float lineWidth = 1.0;
const vec3 lineColor = vec3(0.9, 0.9, 0.9);
const vec3 disabledColor = vec3(1.0, 0.0, 0.0);
const ivec2 objSizeTable[16] = ivec2[]( 
	ivec2(8,8), ivec2(16, 16), ivec2(32, 32), ivec2(64, 64), // square
	ivec2(16,8), ivec2(32,8), ivec2(32,16), ivec2(64,32), // horizontal
	ivec2(8,16), ivec2(8,32), ivec2(16,32), ivec2(32,64), // vertical
	ivec2(8,8), ivec2(8,8), ivec2(8,8), ivec2(8,8)); // invalid

vec3 getPaletteColor(uint idx) {
	ivec2 size = textureSize(paletteData, 0);
	ivec2 colorPos = ivec2(int(idx) % size.x, int(idx) / size.x);
	vec2 paletteUV = (vec2(colorPos) + vec2(0.5, 0.5)) / vec2(size);
	return texture(paletteData, paletteUV).rgb;
}

vec3 hsvToRGB(vec3 c) {
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float DistToLine(vec2 pt1, vec2 pt2, vec2 p) {
	vec2 lineDir = pt2 - pt1;
	vec2 perpDir = vec2(lineDir.y, -lineDir.x);
	vec2 dirToPt1 = pt1 - p;
	return abs(dot(normalize(perpDir), dirToPt1));
}

vec3 DrawObj(uint tileId, uint paletteId, ivec2 size, bool hFlip, bool vFlip, bool largePalette, bool oneDimCharMapping, vec2 pos) {
	ivec2 tileDim = ivec2(size / 8.0);
	ivec2 tilePos = ivec2(pos * vec2(tileDim));
	vec2 pixelPosf = fract(pos * vec2(tileDim));
	ivec2 pixelPos = ivec2(pixelPosf * 8.0);
	if (largePalette) {
		tileId &= ~1u;
		if (oneDimCharMapping) {
			tileId += 2u * uint(tilePos.y * tileDim.x + tilePos.x);
		}
		else {
			tileId += 2u * uint(tilePos.y * 16 + tilePos.x);
		}
		int pixelId = pixelPos.y * 8 + pixelPos.x;
		int address = 0x10000 + int(tileId) * 32 + pixelId;
		uint data = texelFetch(vramData, address).r;
		return getPaletteColor(data + 256u);
		//return vec3(0.0, float((tileId >> 8u) & 0xFFu) / 255.0, float(tileId & 0xFFu) / 255.0);
	}
	else {
		if (oneDimCharMapping) {
			tileId += uint(tilePos.y * tileDim.x + tilePos.x);
		}
		else {
			tileId += uint(tilePos.y * 32 + tilePos.x);
		}
		int pixelId = pixelPos.y * 8 + pixelPos.x;
		int address = 0x10000 + int(tileId) * 32 + pixelId / 2;
		uint data = texelFetch(vramData, address).r;
		if ((pixelId & 1) != 0) {
			return getPaletteColor(paletteId * 16u + ((data >> 4u) & 0xfu) + 256u);
		}
		else {
			return getPaletteColor(paletteId * 16u + ((data >> 0u) & 0xfu) + 256u);
		}
	}
}

void main() {
	vec2 oamDim = vec2(16.0, 8.0);
	vec2 oamUVSize = vec2(1.0, 1.0) / oamDim;
	vec2 pixelSize = vec2(1.0, 1.0) / windowSize;
	vec2 pos = uv * oamDim;
	ivec2 objPos = ivec2(pos);
	vec2 pixelPosf = fract(pos);
	int objIdx = objPos.y * 16 + objPos.x;
	fragColor = vec4(pixelPosf, 0.0, 1.0);
	uint attribute0 = texelFetch(oamData, objIdx * 4 + 0).r;
	uint attribute1 = texelFetch(oamData, objIdx * 4 + 1).r;
	uint attribute2 = texelFetch(oamData, objIdx * 4 + 2).r;

	uint charName = attribute2 & 0x3FFu;
	uint paletteNumber = (attribute2 >> 12u) & 0xFu;
	bool rotScale = (attribute0 & 0x100u) != 0u;
	bool objDisable = (attribute0 & 0x200u) != 0u;
	bool enabled = rotScale || !objDisable;
	ivec2 objCoord = ivec2(int(attribute1) & 0x1F, int(attribute0) & 0xF);
	uint objShape = (attribute0 >> 14u) & 0x3u;
	uint objSizeIdx = (objShape << 2u) | ((attribute1 >> 14u) & 0x3u);
	ivec2 objSize = objSizeTable[objSizeIdx];
	bool horFlip = (attribute1 & 0x1000u) != 0u;
	bool verFlip = (attribute1 & 0x2000u) != 0u;
	bool largePalette = (attribute0 & 0x2000u) != 0u;
	bool oneDimCharMapping = (DISPCNT & 0x40u) != 0u;

	fragColor.rgb = DrawObj(charName, 
		paletteNumber, 
		objSize, 
		horFlip, 
		verFlip, 
		largePalette, 
		oneDimCharMapping, 
		pixelPosf);

	// Draw disabled Line
	if (!enabled) {
		vec2 TLCorner = vec2(objPos) * oamUVSize;
		vec2 BRCorner = TLCorner + oamUVSize;
		if (DistToLine(TLCorner, BRCorner, uv) < lineWidth * pixelSize.x) {
			fragColor.rgb = disabledColor;
		}
	}

	vec2 distance2D = min(pixelPosf - vec2(0.0, 0.0), vec2(1.0, 1.0) - pixelPosf) / oamDim;
	if (distance2D.x <= lineWidth * pixelSize.x  || distance2D.y <= lineWidth * pixelSize.y) {
		fragColor.rgb = lineColor;
	}
}
