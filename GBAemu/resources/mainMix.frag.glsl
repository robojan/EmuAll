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
#define BACKDROP 0
#define BG0 1
#define BG1 2
#define BG2 3
#define BG3 4
#define OBJ 5

bool IsInsideObjectWindow() {
	vec4 t = texture(objectLayerMask, fUV);
	return t.x == 1.0;
}

void main() {
	vec4 color;
	float depth;
	int type;
	// Current postion
	int lineNr = int((1.0 - fUV.y) * screenSize.y);
	ivec2 pos = ivec2(int(fUV.x * screenSize.x), lineNr);

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

	// Draw backdrop
	depth = 0.0;
	color = vec4(getPaletteColor(0u, lineNr), 1.0);
	type = BACKDROP;

	// Draw background
	for (int i = 0; i < 4; i++) {
		if (IsBackgroundEnabled(i, lineNr) && bgEnabledMask[i]) {
			float bgDepth = texture(backgroundDepth[i], fUV).r;
			vec4 bgColor = texture(background[i], fUV);
			if (bgDepth >= depth && bgColor.a > 0.0) {
				color = bgColor;
				depth = bgDepth;
			}
		}
	}

	// Draw objects
	if (IsObjectLayerEnabled(lineNr) && objEnabledMask) {
		float objDepth = texture(objectLayerDepth, fUV).r;
		vec4 objColor = texture(objectLayer, fUV);
		if (objDepth >= depth && objColor.a > 0.0) {
			color = objColor;
			depth = objDepth;
		}
	}

	// Apply color
	fragColor = color;
	gl_FragDepth = depth;
}