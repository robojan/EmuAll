#version 330 core

vec4 GetTextureColor(sampler2D textureSampler, vec2 textureCoord) {
	return texture(textureSampler, textureCoord);
}