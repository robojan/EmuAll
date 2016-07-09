#line 2 3

// Input 
in vec2 fPos;

// Output
out vec4 fragColor;

// Uniform data
uniform bool objectPalette;
uniform int lineNr;

// constants
const float lineWidth = 1.0;
const vec3 lineColor = vec3(0.9, 0.9, 0.9);

void main() {
	// 16 x 16 
	fragColor = vec4(lineColor, 1.0);
	vec2 lineSize = (lineWidth / screenSize);

	vec2 globalPos = mod(fPos * 16.0, 1.0);
	vec2 distEdge = min(globalPos, vec2(1.0, 1.0) - globalPos);
	if (all(greaterThan(distEdge, lineSize * 8.0))) {
		ivec2 palettePos = ivec2(fPos * 16.0);
		int paletteId = palettePos.x + palettePos.y * 16;
		if (objectPalette) paletteId += 256;

		vec3 color = getPaletteColor(uint(paletteId), lineNr);
		fragColor = vec4(color, 1.0);
	}
}