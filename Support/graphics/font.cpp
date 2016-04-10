
#include <locale>
#include <codecvt>

#include <emuall/graphics/font.h>
#include <emuall/graphics/graphicsException.h>
#include <GL/glew.h>



Font::Font(const std::string &font, int idx, int size) :
	_face(nullptr), _size(size)
{
	FT_Library library = FontLibrary::GetSingleton()._library;
	FT_Error error = FT_New_Face(library, font.c_str(), 0, &_face);
	if (error != FT_Err_Ok) {
		throw FontException(error, "Could not open Font face");
	}
	SetCharSize(size);
}

Font::Font(const void *font, size_t fontSize, int idx, int size) :
	_face(nullptr), _size(size)
{
	FT_Library library = FontLibrary::GetSingleton()._library;
	FT_Error error = FT_New_Memory_Face(library, (const FT_Byte *)font, (FT_Long)fontSize, 0, &_face);
	if (error != FT_Err_Ok) {
		throw FontException(error, "Could not open Font face");
	}
	SetCharSize(size);
}

Font::Font(const Font &other) :
	_face(other._face), _size(other._size)
{
	FT_Error error = FT_Reference_Face(_face);
	if (error != FT_Err_Ok) {
		throw FontException(error, "Could not copy Font face");
	}
}

Font::~Font()
{
	if (_face != nullptr) {
		FT_Done_Face(_face);
		_face = nullptr;
	}
}

void Font::DrawText(GuiRenderer &renderer, float x, float y, const std::string &str, 
	unsigned int color /*= 0xFF000000*/, float scale /*= 1.0f*/, int width /*= -1*/, 
	Alignment hAlign /*= Left*/, int height /*= -1*/, Alignment vAlign /*= Top*/)
{
	std::wstring_convert<std::codecvt_utf8<unsigned int>, unsigned int> conv;
	auto str32 = conv.from_bytes(str);
	std::vector<unsigned int> u32Chars;
	for (auto &i : str32) {
		u32Chars.push_back(i);
	}
	DrawText(renderer, x, y, u32Chars, color, scale, width, hAlign, height, vAlign);
}

void Font::DrawText(GuiRenderer &renderer, float x, float y, 
	const std::vector<unsigned int> &str, unsigned int color /*= 0xFF000000*/, 
	float scale /*= 1.0f*/, int width /*= -1*/, Alignment hAlign /*= Left*/, 
	int height /*= -1*/, Alignment vAlign /*= Top*/)
{
	// Get the glyph indices
	std::vector<unsigned int> indices = GetGlyphIndices(str);
	// Create layout
	int ascender = _face->size->metrics.ascender >> 6;
	int lineHeight = _face->size->metrics.height >> 6;
	GlyphLayout layout(lineHeight, ascender);

	// Add all the glyphs
	for (unsigned int i = 0; i < indices.size(); i++) {
		unsigned int index = indices[i];
		unsigned int c = str[i];
		if (c == '\n') {
			layout.AddNewLine();
			continue;
		}
		FT_Error error = FT_Load_Glyph(_face, index, FT_LOAD_DEFAULT);
		if (error != FT_Err_Ok) {
			throw FontException(error, "Could not load glyph");
		}
		//int width = _face->glyph->metrics.width >> 6;
		//int height = _face->glyph->metrics.height >> 6;
		int bearingX = _face->glyph->metrics.horiBearingX >> 6;
		int bearingY = _face->glyph->metrics.horiBearingY >> 6;
		int advance = _face->glyph->metrics.horiAdvance >> 6;

		layout.AddGlyph(index, IsWrapChar(c), advance, 0, bearingX, bearingY);
	}
	layout.DoLayout(true, width, hAlign, height, vAlign);
	DrawText(renderer, x, y, layout, color, scale);
}

void Font::DrawText(GuiRenderer &renderer, float x, float y, 
	const GlyphLayout &layout, unsigned int color /*= 0xFF000000*/, 
	float scale /*= 1.0f*/)
{
	std::string id = GetFontID();
	FontCache &cache = FontCache::GetSingleton();
	ShaderProgram &program = renderer.program;
	program.Begin();
	program.SetUniform("glyphColor", (color & 0xFF) / 255.0f,
		((color >> 8) & 0xFF) / 255.0f, ((color >> 16) & 0xFF) / 255.0f, ((color >> 24) & 0xFF) / 255.0f);
	renderer.vao.Begin();
	for (int i = 0; i < layout.GetCount(); i++) {
		unsigned int glyphIndex = layout.GetGlyphIndex(i);
		if (layout.IsNewLine(i)) {
			continue;
		}
		Texture *texture = nullptr;
		if (cache.DoesTextureExist(id, glyphIndex)) {
			texture = &cache.GetTexture(id, glyphIndex);
		}
		else {
			FT_Error error = FT_Load_Glyph(_face, glyphIndex, FT_LOAD_DEFAULT);
			if (error != FT_Err_Ok) {
				throw FontException(error, "Could not load glyph");
			}
			error = FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_NORMAL);
			if (error != FT_Err_Ok) {
				throw FontException(error, "Could not render glyph");
			}

			texture = &cache.CreateTexture(id, glyphIndex, _face->glyph->bitmap.width, _face->glyph->bitmap.rows);
			texture->UpdateData(0, 0, _face->glyph->bitmap.width, _face->glyph->bitmap.rows,
				(const char *)_face->glyph->bitmap.buffer, Texture::Red, _face->glyph->bitmap.pitch);
			texture->Begin();
			texture->SetFilter(Texture::Linear, Texture::Linear);
		}
		program.SetUniform("textureSampler", 0, *texture);
		program.SetUniform("position", (float)layout.GetPosX(i) * scale + x,
			(float)layout.GetPosY(i) * scale + y);
		program.SetUniform("glyphSize", cache.GetGlyphWidth(id, glyphIndex), cache.GetGlyphHeight(id, glyphIndex));
		program.SetUniform("scale", scale, scale);
		renderer.vao.BindBuffer(1, cache.GetUVBuffer(id, glyphIndex), 2, VertexArrayObject::Float);

		GL_CHECKED(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	}
	renderer.vao.End();
	program.End();
}

bool Font::SupportsUnicode() const
{
	for (int i = 0; i < _face->num_charmaps; i++) {
		if (_face->charmaps[i]->encoding == FT_ENCODING_UNICODE)
			return true;
	}
	return false;
}

std::vector<unsigned int> Font::GetGlyphIndices(const std::vector<unsigned int> &str) const
{
	std::vector<unsigned int> result;
	if (!SupportsUnicode()) {
		throw FontException("The font doesn't support unicode");
	}
	FT_Error error = FT_Select_Charmap(_face, FT_ENCODING_UNICODE);
	if (error != FT_Err_Ok) {
		throw FontException(error, "Could not set Font charmap");
	}
	
	for (auto &c : str) {
		unsigned int index = FT_Get_Char_Index(_face, c);
		result.push_back(index);
	}

	return result;

}

std::string Font::GetFontID() const
{
	std::string result(_face->family_name);
	result.append(":");
	result.append(_face->style_name);
	result.append(":");
	result.append(std::to_string(_size));
	return result;
}

bool Font::IsWrapChar(unsigned int c)
{
	return c == ' ' || c == '\n' || c == '\t';
}

Font &Font::operator=(const Font &other)
{
	_face = other._face;
	_size = other._size;
	FT_Error error = FT_Reference_Face(_face);
	if (error != FT_Err_Ok) {
		throw FontException(error, "Could not copy Font face");
	}
	return *this;
}

void Font::SetCharSize(int size)
{
	// TODO: Screen DPI
	FT_Error error = FT_Set_Char_Size(_face, 0, size << 6, 75, 75);
	if (error != FT_Err_Ok) {
		throw FontException(error, "Could not set font char size");
	}
}
