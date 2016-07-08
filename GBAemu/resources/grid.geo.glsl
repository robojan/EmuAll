#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 128) out;

// Inputs
in int gId[];
in int gStartLine[];

// Outputs
out vec4 fColor;

// Uniforms
uniform ivec2 gridSize;
uniform vec4 gridColor;
uniform bool gridEnabled;
uniform vec2 boxSize;
uniform vec2 boxPos;
uniform bool boxEnabled;
uniform bool boxWraparound;
uniform vec4 boxColor;

void GenerateLine(vec2 p1, vec2 p2, vec4 color) {
	gl_Position = vec4(p1, gl_in[0].gl_Position.z, 1.0);
	fColor = color;
	EmitVertex();

	gl_Position = vec4(p2, gl_in[0].gl_Position.z, 1.0);
	fColor = color;
	EmitVertex();

	EndPrimitive();
}

void GenerateBox(vec2 TL, vec2 BR, vec4 color) {
	gl_Position = vec4(TL, gl_in[0].gl_Position.z, 1.0);
	fColor = color;
	EmitVertex();

	gl_Position = vec4(vec2(BR.x, TL.y), gl_in[0].gl_Position.z, 1.0);
	fColor = color;
	EmitVertex();

	gl_Position = vec4(BR, gl_in[0].gl_Position.z, 1.0);
	fColor = color;
	EmitVertex();

	gl_Position = vec4(vec2(TL.x, BR.y), gl_in[0].gl_Position.z, 1.0);
	fColor = color;
	EmitVertex();

	gl_Position = vec4(TL, gl_in[0].gl_Position.z, 1.0);
	fColor = color;
	EmitVertex();

	EndPrimitive();
}

void GenerateBoxWraparound(vec2 TL, vec2 BR, vec4 color) {
	GenerateBox(TL + vec2(-2.0, 2.0), BR + vec2(-2.0, 2.0), color);
	GenerateBox(TL + vec2(0.0, 2.0), BR + vec2(0.0, 2.0), color);
	GenerateBox(TL + vec2(2.0, 2.0), BR + vec2(2.0, 2.0), color);

	GenerateBox(TL + vec2(-2.0, 0.0), BR + vec2(-2.0, 0.0), color);
	GenerateBox(TL + vec2(0.0, 0.0), BR + vec2(0.0, 0.0), color);
	GenerateBox(TL + vec2(2.0, 0.0), BR + vec2(2.0, 0.0), color);

	GenerateBox(TL + vec2(-2.0, -2.0), BR + vec2(-2.0, -2.0), color);
	GenerateBox(TL + vec2(0.0, -2.0), BR + vec2(0.0, -2.0), color);
	GenerateBox(TL + vec2(2.0, -2.0), BR + vec2(2.0, -2.0), color);
}

void GenerateHLines() {
	float hDistance = 2.0 / float(gridSize.x);
	float startX = -1.0 + hDistance * gStartLine[0];
	float endX = min(1.0, -1.0 + hDistance * (64 + gStartLine[0]));
	for (float x = startX; x <= endX; x += hDistance) {
		GenerateLine(vec2(x, 1.0), vec2(x, -1.0), gridColor);
	}
}

void GenerateVLines() {
	float vDistance = 2.0 / float(gridSize.y);
	float startY = -1.0 + vDistance * gStartLine[0];
	float endY = min(1.0, -1.0 + vDistance * (64 + gStartLine[0]));
	for (float y = startY; y <= endY; y += vDistance) {
		GenerateLine(vec2(-1.0, y), vec2(1.0, y), gridColor);
	}
}

void main() {
	switch (gId[0]) {
	case 0: // Box
		if (boxEnabled) {
			vec2 BR = boxPos + boxSize * vec2(1.0, -1.0);
			if (boxWraparound) {
				GenerateBoxWraparound(boxPos, BR, boxColor);
			}
			else {
				GenerateBox(boxPos, BR, boxColor);
			}
		}
		break;
	case 1: // V-lines
		if (gridEnabled) {
			GenerateVLines();
		}
		break;
	case 2: // H-lines
		if (gridEnabled) {
			GenerateHLines();
		}
		break;
	}
}
