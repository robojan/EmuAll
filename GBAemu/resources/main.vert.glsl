#version 330 core

// Input
layout(location = 0) in int lineNr;
layout(location = 1) in int type;

// Output
out int vLineNr;
out int vType;

// uniforms
uniform vec2 screenSize;

void main() {
	switch (int(type)) {
	default:
	case 0: // background
		gl_Position.xzw = vec3(-1.0, 0.0, 1.0);
		gl_Position.y = 1.0 - ((float(lineNr) + 0.5) / screenSize.y) * 2.0;
		break;
	case 1: // objects
		gl_Position.xzw = vec3(0.0, 0.0, 1.0);
		gl_Position.y = 1.0 - ((float(lineNr) + 0.5) / screenSize.y) * 2.0;
		break;
	case 2: // Backdrop
		gl_Position.xzw = vec3(-1.0, 0.0, 1.0);
		gl_Position.y = 1.0 - ((float(lineNr) + 0.5) / screenSize.y) * 2.0;
		break;
	}

	vLineNr = lineNr;
	vType = type;
}
