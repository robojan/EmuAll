//#version 330 core 
// MainInterface.glsl needs to be included before this.

// Input
layout(location = 0) in vec2 vPos;

// Output
out vec2 fUV;

void main() {
	gl_Position = vec4(vPos * vec2(2.0, 2.0) + vec2(-1.0, -1.0), 0.0, 1.0);
	fUV = vPos;
}