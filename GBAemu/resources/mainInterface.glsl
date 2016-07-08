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
	mat2 bgAffineMatrix[2];
};

layout(std140) uniform DrawingRegisters{
	Registers dataRegisters[160];
};


