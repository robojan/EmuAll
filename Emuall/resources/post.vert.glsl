#version 330 core

// Input data
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;

// Output data; will be interpolated for each fragment
out vec2 UV;

void main() {
	// Output position of the vertex
	gl_Position.xyz = vertexPos;
	gl_Position.w = 1;

	// UV of the vertex
	UV = vertexUV;
}