#version 330 core

// Input data
layout(location = 0) in vec2 pos;

// Output data to fragment shader
out vec2 fPos;

void main() {
	gl_Position.xy = vec2(-1.0, 1.0) + pos * vec2(2.0, -2.0);
	gl_Position.zw = vec2(0.0, 1.0);

	fPos = pos;
}