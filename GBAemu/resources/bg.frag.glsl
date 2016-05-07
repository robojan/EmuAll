#version 330 core

// Input
in vec2 uv;

// Output
out vec4 fragColor;

// Uniforms
uniform sampler2D paletteData;
uniform usamplerBuffer vramData;
uniform uint DISPCNT;
uniform uint BGCNT;
uniform uint BGHOFS;
uniform uint BGVOFS;
uniform uint BGX;
uniform uint BGY;
uniform int backgroundId;
uniform vec2 windowSize;
uniform bool grid;

// constants
const float lineWidth = 1.0;
const vec3 lineColor = vec3(0.9, 0.9, 0.9);
const vec3 disabledColor = vec3(1.0, 0.0, 0.0);
const vec3 unusedColor = vec3(0.6, 0.6, 0.6);
const vec3 windowColor = vec3(1.0, 0.0, 0.0);
const ivec2 bgSizeTable[8] = ivec2[](
	ivec2(256, 256), ivec2(512, 256), ivec2(256, 512), ivec2(512, 512), // text mode
	ivec2(128, 128), ivec2(256, 256), ivec2(512, 512), ivec2(1024, 1024)); // rotation/scaling mode
vec2 pixelSize = vec2(1.0, 1.0) / windowSize;

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

float isGrid(vec2 pixelPosf, vec2 screenDim, vec2 pixelSize) {
	vec2 distance2D = min(pixelPosf - vec2(0.0, 0.0), vec2(1.0, 1.0) - pixelPosf) / screenDim;
	return (distance2D.x <= lineWidth * pixelSize.x || distance2D.y <= lineWidth * pixelSize.y) && grid ? 1.0 : 0.0;
}

float GBAtoF(uint x) {
	if ((x & (1u << 27u)) != 0u) {
		x &= 0x7FFFFFFu;
		return float(x) / -256.0;
	}
	else {
		x &= 0x7FFFFFFu;
		return float(x) / 256.0;
	}
}

bool isInArea(vec2 TL, vec2 BR, vec2 p) {
	return TL.x <= p.x && p.x <= BR.x &&
		TL.y <= p.y && p.y <= BR.y;
}

vec3 drawMode0() {
	return unusedColor;
}

vec3 drawMode1() {
	return unusedColor;
}

vec3 drawMode2() {
	if (backgroundId < 2) return unusedColor;
	int tileBaseAddress = int(((BGCNT >> 2u) & 0x3u) * 0x4000u);
	bool mosaic = (BGCNT & (1u << 6u)) != 0u;
	int mapBaseAddress = int(((BGCNT >> 8u) & 0x1Fu) * 0x800u);
	bool overflowWraparound = (BGCNT & (1u << 13u)) != 0u;

	ivec2 screenSize = bgSizeTable[((BGCNT >> 14u) & 0x3u) + 4u];
	ivec2 screenDim = screenSize / 8;
	vec2 tileUVSize = vec2(1.0, 1.0) / vec2(screenDim);
	vec2 pos = uv * vec2(screenDim);
	ivec2 tilePos = ivec2(pos);
	vec2 pixelPosf = fract(pos);
	int tilePosIdx = tilePos.y * screenDim.x + tilePos.x;

	int mapAddress = mapBaseAddress + tilePosIdx;
	uint tileId = texelFetch(vramData, mapAddress).r;
	tileBaseAddress += int(tileId) * 64;
	ivec2 pixelPos = ivec2(pixelPosf * 8.0);
	int tileAddress = tileBaseAddress + pixelPos.y * 8 + pixelPos.x;
	uint paletteId = texelFetch(vramData, tileAddress).r;
	vec3 color = getPaletteColor(paletteId);

	// Draw framing rectangle
	float refX = GBAtoF(BGX);
	float refY = GBAtoF(BGY);
	vec2 phyScreenSize = vec2(240.0, 160.0);
	vec2 phyPos = pos * 8.0;
	vec2 windowCornerTL = mod(vec2(refX, refY), vec2(screenSize));
	vec2 windowCornerTR = mod(windowCornerTL + vec2(phyScreenSize.x, 0.0), vec2(screenSize));
	vec2 windowCornerBL = mod(windowCornerTL + vec2(0.0, phyScreenSize.y), vec2(screenSize));
	vec2 windowCornerBR = mod(windowCornerTL + phyScreenSize, vec2(screenSize));
	if (windowCornerTR.x < windowCornerTL.x) {
		if (isInArea(windowCornerTL - vec2(0.0, lineWidth / 2.0), vec2(float(screenSize.x), windowCornerTL.y + lineWidth / 2.0), phyPos) ||
			isInArea(windowCornerBL - vec2(0.0, lineWidth / 2.0), vec2(float(screenSize.x), windowCornerBL.y + lineWidth / 2.0), phyPos) ||
			isInArea(vec2(0.0, windowCornerTL.y - lineWidth / 2.0), windowCornerTL + vec2(0.0, lineWidth / 2.0), phyPos) ||
			isInArea(vec2(0.0, windowCornerBL.y - lineWidth / 2.0), windowCornerBL + vec2(0.0, lineWidth / 2.0), phyPos)) {
			return windowColor;
		}
	}
	else {
		if (isInArea(windowCornerTL - vec2(0.0, lineWidth / 2.0), windowCornerTR + vec2(0.0, lineWidth / 2.0), phyPos) ||
			isInArea(windowCornerBL - vec2(0.0, lineWidth / 2.0), windowCornerBR + vec2(0.0, lineWidth / 2.0), phyPos)) {
			return windowColor;
		}
	}
	if (windowCornerBR.y < windowCornerTL.y) {
		if (isInArea(windowCornerBL - vec2(lineWidth / 2.0, 0.0), vec2(windowCornerBL.x + lineWidth / 2.0, float(screenSize.y)), phyPos) ||
			isInArea(windowCornerBR - vec2(lineWidth / 2.0, 0.0), vec2(windowCornerBR.x + lineWidth / 2.0, float(screenSize.y)), phyPos) ||
			isInArea(vec2(windowCornerTL.x - lineWidth / 2.0, 0.0), windowCornerTL + vec2(lineWidth / 2.0, 0.0), phyPos) ||
			isInArea(vec2(windowCornerTR.x - lineWidth / 2.0, 0.0), windowCornerTR + vec2(lineWidth / 2.0, 0.0), phyPos)) {
			return windowColor;
		}
	}
	else {
		if (isInArea(windowCornerTL - vec2(lineWidth/2.0, 0.0), windowCornerBL + vec2(lineWidth / 2.0, 0.0), phyPos) ||
			isInArea(windowCornerTR - vec2(lineWidth / 2.0, 0.0), windowCornerBR + vec2(lineWidth / 2.0, 0.0), phyPos)) {
			return windowColor;
		}
	}

	return mix(color, lineColor, isGrid(pixelPosf, screenDim, pixelSize));
}

vec3 drawMode3() {
	return unusedColor;
}

vec3 drawMode4() {
	return unusedColor;
}

vec3 drawMode5() {
	return unusedColor;
}


void main() {
	fragColor.a = 1.0;
	bool enabled = (DISPCNT & (1u << uint(backgroundId + 8))) != 0u;
	
	int bgMode = int(DISPCNT & 0x7u);
	switch (bgMode) {
	case 0: // Text mode
		fragColor.rgb = drawMode0();
		break;
	case 1: // Mixed mode
		fragColor.rgb = drawMode1();
		break;
	case 2: // rot/scrolling mode
		fragColor.rgb = drawMode2();
		break;
	case 3: // still Image mode
		fragColor.rgb = drawMode3();
		break;
	case 4: // frambuffer mode
		fragColor.rgb = drawMode4();
		break;
	case 5: // frambuffer mode
		fragColor.rgb = drawMode5();
		break;
	default: 
		fragColor.rgb = unusedColor;
	}	
	
	// Draw disabled Line
	if (!enabled) {

		if (DistToLine(vec2(0.0, 0.0), vec2(1.0, 1.0), uv) < lineWidth * pixelSize.x) {
			fragColor.rgb = disabledColor;
		}
	}
}
