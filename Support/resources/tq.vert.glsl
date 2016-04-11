#version 330 core

// Input vertex data
layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexUV;

// Output data for the fragment shader
out vec2 UV;

// Position in screen coordinates
uniform vec2 position;
// size of the screen
uniform ivec2 screenSize;
uniform vec2 size;
uniform vec2 scale;

void main() {
	// Output the vertex position
	// position in screenCoordinates
	vec2 screenPos = position - vec2(0.0, size.y) + vertexPosition * size * scale;

	gl_Position.xy = (screenPos / vec2(screenSize)) * 2.0 - vec2(1.0, 1.0);
	gl_Position.w = 1.0;
	gl_Position.z = 0.0;

	// Set the output uv 
	UV = vertexUV;
}