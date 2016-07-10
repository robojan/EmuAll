#line 2 3

// Inputs
in vec2 fUV;

// Outputs
out vec4 fragColor;

// Uniforms
uniform sampler2D background[4];
uniform sampler2D backgroundDepth[4];
uniform sampler2D objectLayer;
uniform sampler2D objectLayerDepth;
uniform sampler2D objectLayerMask;

// defines
#define BG0 0
#define BG1 1
#define BG2 2
#define BG3 3
#define OBJ 4
#define BACKDROP 5

struct LayerData {
	vec4 color;
	float depth;
	int type;
};

bool IsInsideObjectWindow() {
	vec4 t = texture(objectLayerMask, fUV);
	return t.r == 1.0;
}

bool IsObjectSemiTrans() {
	vec4 t = texture(objectLayerMask, fUV);
	return t.g == 1.0;
}

void SortLayers(inout LayerData data[5]) {
	bool finished = false;
	while (finished == false) {
		finished = true;
		for (int i = 0; i < 4; i++) {
			if (data[i].depth > data[i + 1].depth) {
				finished = false;
				LayerData temp = data[i];
				data[i] = data[i + 1];
				data[i + 1] = temp;
			}
		}
	}
}

bool IsEffectActive(int lineNr, int lastType, int newType) {
	int bldcnt = dataRegisters[lineNr].effects.x;
	int effect = GetEffectMode(lineNr);
	if (effect == 0) return false;
	if (effect == 1) {
		return (bldcnt & (1 << newType)) != 0 && (bldcnt & (1 << (lastType + 8))) != 0;
	}
	else {
		return (bldcnt & (1 << newType)) != 0;
	}
}

void main() {
	vec4 color;
	float depth;
	int type;
	LayerData layers[5];
	// Current postion
	int lineNr = int((1.0 - fUV.y) * screenSize.y);
	ivec2 pos = ivec2(int(fUV.x * screenSize.x), lineNr);

	// Forced blanking
	if ((dataRegisters[lineNr].dispcnt & 0x80) != 0) {
		gl_FragDepth = 0.0;
		fragColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
	}

	// Determine which window we are in 
	bool windowsEnabled = AreWindowsEnabled(lineNr);
	bvec4 bgEnabledMask = bvec4(true);
	bool objEnabledMask = true;
	bool specialEffectMask = true;
	if (windowsEnabled) {
		int winin = dataRegisters[lineNr].wincnt.x;
		int winout = dataRegisters[lineNr].wincnt.y;
		if (IsInsideWindow(lineNr, pos, 0) && IsWindowEnabled(lineNr, 0)) {
			// Window 0
			bgEnabledMask = bvec4(
				(winin & 0x01) != 0,
				(winin & 0x02) != 0,
				(winin & 0x04) != 0,
				(winin & 0x08) != 0);
			objEnabledMask = (winin & 0x10) != 0;
			specialEffectMask = (winin & 0x20) != 0;
		}
		else if(IsInsideWindow(lineNr, pos, 1) && IsWindowEnabled(lineNr, 1)) {
			// Window 1
			bgEnabledMask = bvec4(
				(winin & 0x0100) != 0,
				(winin & 0x0200) != 0,
				(winin & 0x0400) != 0,
				(winin & 0x0800) != 0);
			objEnabledMask = (winin & 0x1000) != 0;
			specialEffectMask = (winin & 0x2000) != 0;
		} 
		else if (IsInsideObjectWindow() && IsWindowEnabled(lineNr, 2)) {
			// Object window
			bgEnabledMask = bvec4(
				(winout & 0x0100) != 0,
				(winout & 0x0200) != 0,
				(winout & 0x0400) != 0,
				(winout & 0x0800) != 0);
			objEnabledMask = (winout & 0x1000) != 0;
			specialEffectMask = (winout & 0x2000) != 0;
		}
		else {
			// Outside window
			bgEnabledMask = bvec4(
				(winout & 0x01) != 0,
				(winout & 0x02) != 0,
				(winout & 0x04) != 0,
				(winout & 0x08) != 0);
			objEnabledMask = (winout & 0x10) != 0;
			specialEffectMask = (winout & 0x20) != 0;
		}
	}

	for (int i = 0; i < 5; i++) {
		layers[i].color = vec4(0.0);
		layers[i].depth = 0.0;
		layers[i].type = BACKDROP;
	}


	// Draw background
	for (int i = 0; i < 4; i++) {
		if (IsBackgroundEnabled(i, lineNr) && bgEnabledMask[i]) {
			float bgDepth = texture(backgroundDepth[i], fUV).r;
			vec4 bgColor = texture(background[i], fUV);
			layers[i].color = bgColor;
			layers[i].depth = bgDepth;
			layers[i].type = BG0 + i;
		}
	}

	// Draw objects
	if (IsObjectLayerEnabled(lineNr) && objEnabledMask) {
		float objDepth = texture(objectLayerDepth, fUV).r;
		vec4 objColor = texture(objectLayer, fUV);
		layers[4].color = objColor;
		layers[4].depth = objDepth;
		layers[4].type = OBJ;
	}

	SortLayers(layers);

	// Draw backdrop
	depth = 0.0;
	color = vec4(getPaletteColor(0u, lineNr), 1.0);
	type = BACKDROP;

	for (int i = 0; i < 5; i++) {
		if (layers[i].type != BACKDROP && layers[i].color.a > 0.0) {
			int bldcnt = dataRegisters[lineNr].effects.x;
			if (layers[i].type == OBJ && IsObjectSemiTrans() && ((bldcnt & (1 << (type + 8))) != 0))
			{
				int bldalpha = dataRegisters[lineNr].effects.y;
				float eva = min(1.0, float(bldalpha & 0x1f) / 16.0);
				float evb = min(1.0, float((bldalpha >> 8) & 0x1f) / 16.0);
				color = min(vec4(1.0, 1.0, 1.0, 1.0), layers[i].color * eva + color * evb);
			}
			else {
				if (specialEffectMask && IsEffectActive(lineNr, type, layers[i].type)) {
					switch (GetEffectMode(lineNr)) {
					case 1: {
						int bldalpha = dataRegisters[lineNr].effects.y;
						float eva = min(1.0, float(bldalpha & 0x1f) / 16.0);
						float evb = min(1.0, float((bldalpha >> 8) & 0x1f) / 16.0);
						color = min(vec4(1.0, 1.0, 1.0, 1.0), layers[i].color * eva + color * evb);
						break;
					}
					case 2: {
						int bldy = dataRegisters[lineNr].effects.z;
						float evy = min(1.0, float(bldy & 0x1f) / 16.0);
						vec3 effColor = layers[i].color.rgb + (vec3(1.0) - layers[i].color.rgb) * evy;
						color = vec4(min(vec3(1.0), effColor), layers[i].color.a);
						break;
					}
					case 3: {
						int bldy = dataRegisters[lineNr].effects.z;
						float evy = min(1.0, float(bldy & 0x1f) / 16.0);
						vec3 effColor = layers[i].color.rgb - layers[i].color.rgb * evy;
						color = vec4(max(vec3(0.0), effColor), layers[i].color.a);
						break;
					}
					}
				}
				else {
					color = layers[i].color;
				}
			}
			depth = layers[i].depth;
			type = layers[i].type;
		}
	}

	// Apply color
	fragColor = color;
	gl_FragDepth = depth;

}