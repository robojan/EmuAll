#version 330 

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
	ivec2 coordBL = ivec2(coord);
	ivec2 coordBR = clamp(coordBL + ivec2(1, 0), ivec2(0, 0), size);
	ivec2 coordTL = clamp(coordBL + ivec2(0, 1), ivec2(0, 0), size);
	ivec2 coordTR = clamp(coordBL + ivec2(1, 1), ivec2(0, 0), size);
	// Get the colors
	vec4 col11 = texelFetch(textureSampler, coordBL, 0);
	vec4 col21 = texelFetch(textureSampler, coordBR, 0);
	vec4 col12 = texelFetch(textureSampler, coordTL, 0);
	vec4 col22 = texelFetch(textureSampler, coordTR, 0);
	// Get factors
	vec2 factors = fract(coord);
	// Mix the colors together
	vec4 colXTop = mix(col12, col22, factors.x);
	vec4 colXBot = mix(col11, col21, factors.x);
	vec4 col = mix(colXBot, colXTop, factors.y);
	return col;
}