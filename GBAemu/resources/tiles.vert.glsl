#version 330 core

// Input
layout(location = 0) in vec2 vertexPos;
layout(location = 1) in vec2 vertexUV;

// output
out vec2 uv;

void main() {
	gl_Position.xy = vertexPos;
	gl_Position.zw = vec2(0.0, 1.0);

	uv = vertexUV;
}