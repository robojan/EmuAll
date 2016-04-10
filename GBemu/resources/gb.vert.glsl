#version 330 core

// Input vertex data
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data; will be interpolated for each fragment
out vec2 UV;

// Values that stay constant for the whole mesh
//uniform mat4 MVP;

void main() {
	// Output position of the vertex
	//gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
	gl_Position.xyz = vertexPosition_modelspace;
	gl_Position.w = 1;

	// UV of the vertex
	UV = vertexUV;
}