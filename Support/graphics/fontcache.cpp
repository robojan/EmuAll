
#include <emuall/graphics/font.h>
#include <emuall/graphics/graphicsException.h>
#include <emuall/math/math.h>

FontCache FontCache::_singleton;

int FontCache::GetGlyphWidth(const std::string &fontID, unsigned int glyphIndex)
{
	auto fontMap = _cache->find(fontID);
	if (fontMap == _cache->cend())
		throw FontException("Glyph not in cache");
	auto data = fontMap->second.find(glyphIndex);
	if (data == fontMap->second.cend())
		throw FontException("Glyph not in cache");
	return data->second.width;
}

int FontCache::GetGlyphHeight(const std::string &fontID, unsigned int glyphIndex)
{
	auto fontMap = _cache->find(fontID);
	if (fontMap == _cache->cend())
		throw FontException("Glyph not in cache");
	auto data = fontMap->second.find(glyphIndex);
	if (data == fontMap->second.cend())
		throw FontException("Glyph not in cache");
	return data->second.height;
}

FontCache & FontCache::GetSingleton()
{
	return _singleton;
}

void FontCache::ClearCache()
{
	_cache->clear();
}

bool FontCache::DoesTextureExist(const std::string &fontID, unsigned int glyphIndex) const
{
	auto fontMap = _cache->find(fontID);
	if (fontMap == _cache->cend())
		return false;
	auto texture = fontMap->second.find(glyphIndex);
	return texture != fontMap->second.cend();
}

Texture & FontCache::GetTexture(const std::string &fontID, unsigned int glyphIndex) const
{
	auto fontMap = _cache->find(fontID);
	if (fontMap == _cache->cend())
		throw FontException("Glyph not in cache");
	auto data = fontMap->second.find(glyphIndex);
	if (data == fontMap->second.cend())
		throw FontException("Glyph not in cache");
	return data->second.texture;
}

Texture & FontCache::CreateTexture(const std::string &fontID, unsigned glyphIndex, int width, int height)
{
	(*_cache)[fontID][glyphIndex] = FontCache::GlyphData(width, height);
	return (*_cache)[fontID][glyphIndex].texture;
}

BufferObject & FontCache::GetUVBuffer(const std::string &fontID, unsigned int glyphIndex) const
{
	auto fontMap = _cache->find(fontID);
	if (fontMap == _cache->cend())
		throw FontException("Glyph not in cache");
	auto data = fontMap->second.find(glyphIndex);
	if (data == fontMap->second.cend())
		throw FontException("Glyph not in cache");
	return data->second.uv;
}

FontCache::FontCache() :
	_cache(new std::map<std::string, std::map<unsigned int, GlyphData>>)
{

}

FontCache::~FontCache()
{
	if (_cache != nullptr) {
		delete _cache;
	}
}

FontCache::GlyphData::GlyphData(int width, int height) :
	texture(nextPowerOf2(width), nextPowerOf2(height), nullptr, Texture::Red),
	uv(BufferObject::Array), width(width), height(height)
{
	float textWidth = (float)nextPowerOf2(width);
	float textHeight = (float)nextPowerOf2(height);
	float uvData[] = {
		0, 0,
		width / textWidth, 0,
		0, height / textHeight,
		width / textWidth, height / textHeight,
	};
	if (textWidth == 0) {
		uvData[2] = 1.0f;
		uvData[6] = 1.0f;
	}
	if (textHeight == 0) {
		uvData[1] = 1.0f;
		uvData[3] = 1.0f;
	}
	uv.BufferData(BufferObject::StaticDraw, sizeof(uvData), uvData);
}

FontCache::GlyphData::GlyphData() :
	texture(), uv(BufferObject::Array)
{

}
