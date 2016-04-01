#version 330

// Constants
const float PI = 3.14159265359;
const float variance = 0.5;

float gaussian(float x) {
	float a = 1 / (variance * sqrt(2.0 * PI));
	return a * exp(-x * x / (2.0 * variance * variance)); 
}

// Bi-linear texture filtering
vec4 GetTextureColor(sampler2D textureSampler, vec2 textureCoord) {
	// Get the texturesize
	ivec2 size = textureSize(textureSampler, 0);
	// Normalize the coordinate
	textureCoord.x = textureCoord.x < 0.0 ? 1.0 - mod(-textureCoord.x, 1.0) : mod(textureCoord.x, 1.0);
	textureCoord.y = textureCoord.y < 0.0 ? 1.0 - mod(-textureCoord.y, 1.0) : mod(textureCoord.y, 1.0);
	
	// calculate the coordinates
	vec2 coord = textureCoord * vec2(size);
	size = size - ivec2(1, 1);
	ivec2 startCoord = ivec2(coord) + ivec2(0, 1);
	vec4 sum = vec4(0.0);
	float denom = 0.0;

	// Get factors
	vec2 factors = fract(coord);
	factors.y = 1.0 - factors.y;
	for (int m = -1; m <= 2; m++) {
		for (int n = -1; n <= 2; n++) {
			ivec2 texelCoord = clamp(startCoord + ivec2(m, -n), ivec2(0, 0), size);
			vec4 texelColor = texelFetch(textureSampler, texelCoord, 0);
			float f1 = gaussian(float(m) - factors.x);
			float f2 = gaussian(factors.y - float(n));
			sum = sum + texelColor * f1 * f2;
			denom = denom + f1 * f2;
		}
	}
	return sum / denom;
	
}