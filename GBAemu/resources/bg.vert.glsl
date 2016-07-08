#line 2 1

// output
out ivec2 gBGSize;
out ivec2 gTilePos;
out int gId;

// Uniforms
uniform int lineNr;
uniform int background;

void main() {
	gBGSize = GetBackgroundSize(background, lineNr);
	ivec2 tileSize = gBGSize / 8;
	gId = gl_VertexID;
	gTilePos = ivec2(gId % tileSize.x, gId / tileSize.x);
	gl_Position = vec4(-1.0 + 1.0 / (tileSize.x) + float(gTilePos.x) * (2.0 / (tileSize.x)),
		1.0 - 1.0 / (tileSize.y) - float(gTilePos.y) * (2.0 / (tileSize.y)), 0.97, 1.0);
}
