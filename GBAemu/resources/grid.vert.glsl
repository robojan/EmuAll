#version 330 core

// Inputs

// Outputs
out int gId;
out int gStartLine;

// Uniforms
uniform ivec2 gridSize;
uniform float gridDepth;
uniform float boxDepth;

void main() {
	gl_Position.xyw = vec3(0.0, 0.0, 1.0);
	if (gl_VertexID == 0) {
		gId = 0;
		gStartLine = 0;
		gl_Position.z = boxDepth;
	}
	else {
		ivec2 size = (gridSize + ivec2(63,63))/ 64;
		gl_Position.z = gridDepth;
		int id = gl_VertexID - 1;
		if (id < size.x) {
			gStartLine = id * 64;
			gId = 2;
		}
		else {
			gStartLine = (id - size.x) * 64;
			gId = 1;
		}
	}
}