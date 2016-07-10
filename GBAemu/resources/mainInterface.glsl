#version 330 core
// uniforms 
uniform vec2 screenSize;
uniform usamplerBuffer vramData;
uniform usamplerBuffer oamData;
uniform sampler2DArray paletteData;

struct Registers {
	int dispcnt;
	ivec4 bgCnt;
	ivec2 bgOfs[4];
	vec2 bgRefPoint[2];
	// bug in glsl compiler when using mat2 and nsight debugger
	vec4 bgAffineMatrix[2];
	ivec4 windim[2];
	ivec2 wincnt;
};

layout(std140) uniform DrawingRegisters{
	Registers dataRegisters[160];
};


