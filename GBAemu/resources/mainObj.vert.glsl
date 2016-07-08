//#version 330 core 
// MainInterface.glsl needs to be included before this.

// Input
layout(location = 0) in int vLineNr;

// Output
out int gLineNr;

void main() {
	gl_Position = vec4(-1.0, 1.0 - 2.0 * (float(vLineNr) + 0.5) / screenSize.y, 0.0, 1.0);
	gLineNr = vLineNr;
}