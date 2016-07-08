//#version 330 core

// Inputs
in vec2 fUV;

// Outputs
out vec4 fragColor;

// Uniforms
uniform sampler2D background[4];
uniform sampler2D backgroundDepth[4];
uniform sampler2D objectLayer;
uniform sampler2D objectLayerDepth;

void main() {
	int lineNr = int((1.0 - fUV.y) * screenSize.y);
	vec4 color;
	float depth;

	// Draw backdrop
	depth = 0.0;
	color = vec4(getPaletteColor(0u, lineNr), 1.0);

	// Draw background
	for (int i = 0; i < 4; i++) {
		if (IsBackgroundEnabled(i, lineNr)) {
			float bgDepth = texture(backgroundDepth[i], fUV).r;
			vec4 bgColor = texture(background[i], fUV);
			if (bgDepth >= depth && bgColor.a > 0.0) {
				color = bgColor;
				depth = bgDepth;
			}
		}
	}

	// Draw objects
	if (IsObjectLayerEnabled(lineNr)) {
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