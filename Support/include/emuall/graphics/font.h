#ifndef FONT_H_
#define FONT_H_

#include <emuall/common.h>
#include <emuall/exception.h>
#include <emuall/graphics/shaderprogram.h>
#include <emuall/graphics/texture.h>
#include <emuall/graphics/bufferObject.h>
#include <emuall/graphics/VertexArrayObject.h>
#include <emuall/graphics/guiRenderer.h>
#include <string>
#include <vector>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

enum class Alignment {
	Left,
	Top = Left,
	Center,
	Centre = Center,
	Right,
	Bottom = Right,
};


class DLLEXPORT FontException : public BaseException {
public:
	FontException(int errorCode, const std::string &msg);
	explicit FontException(int errorCode);
	FontException(const std::string &msg);
	virtual ~FontException();

	static const char *GetErrorMessage(int errorCode);
};

class DLLEXPORT FontLibrary {
	friend class Font;
public:
	static FontLibrary &GetSingleton();
	
private:
	FontLibrary();
	~FontLibrary();

	FT_Library _library;

	static FontLibrary _singleton;
};

class DLLEXPORT FontCache {
public:
	static FontCache &GetSingleton();

	void ClearCache();

	bool DoesTextureExist(const std::string &fontID, unsigned int glyphIndex) const;
	Texture &GetTexture(const std::string &fontID, unsigned int glyphIndex) const;
	Texture &CreateTexture(const std::string &fontID, unsigned glyphIndex, int width, int height);
	BufferObject &GetUVBuffer(const std::string &fontID, unsigned int glyphIndex) const;
	int GetGlyphWidth(const std::string &fontID, unsigned int glyphIndex);
	int GetGlyphHeight(const std::string &fontID, unsigned int glyphIndex);

private:
	class GlyphData {
	public:
		GlyphData();
		GlyphData(int width, int height);
		~GlyphData() = default;
		Texture texture;
		BufferObject uv;
		int width;
		int height;
	};
	FontCache();
	~FontCache();
	static FontCache              _singleton;

	std::map<std::string, std::map<unsigned int, GlyphData>> *_cache;
};

class DLLEXPORT GlyphLayout {
public:
	GlyphLayout(int lineHeight, int startY);
	GlyphLayout(const GlyphLayout &other);
	~GlyphLayout();

	GlyphLayout &operator=(const GlyphLayout &other);

	void AddGlyph(unsigned int indice, bool wrapPoint, int advanceX, int advanceY, 
		int bitmapX, int bitmapY);
	void AddNewLine();

	void DoLayout(bool wraping, int width = -1, Alignment hAlign = Alignment::Left,
		int height = -1, Alignment vAlign = Alignment::Top);

	int GetCount() const;
	int GetPosX(int index) const;
	int GetPosY(int index) const;
	unsigned int GetGlyphIndex(int index) const;
	bool IsNewLine(int i) const;
private:
	struct Element {
		unsigned int index;
		bool forceNewLine;
		bool wrapPoint;
		int advanceX;
		int advanceY;
		int bitmapX;
		int bitmapY;
	};
	struct Layout {
		int x;
		int y;
		unsigned int index;
		bool lineStart;
	};

	std::vector<Element> *_elements;
	std::vector<Layout> *_layout;
	int _lineHeight;
	int _startY;
};

class DLLEXPORT Font {
public:

	Font(const std::string &font, int idx, int size);
	Font(const void *font, size_t fontSize, int idx, int size);
	Font(const Font &other);
	~Font();

	Font &operator=(const Font &other);

	void DrawText(GuiRenderer &renderer, float x, float y, const GlyphLayout &layout, 
		unsigned int color = 0xFF000000, float scale = 1.0f);
	void DrawText(GuiRenderer &renderer, float x, float y, const std::vector<unsigned int> &str,
		unsigned int color = 0xFF000000, float scale = 1.0f, int width = -1, 
	Alignment hAlign = Alignment::Left, int height = -1, Alignment vAlign = Alignment::Top);
	void DrawText(GuiRenderer &renderer, float x, float y, const std::string &str,
		unsigned int color = 0xFF000000, float scale = 1.0f, int width = -1, 
		Alignment hAlign = Alignment::Left, int height = -1, Alignment vAlign = Alignment::Top);

	bool SupportsUnicode() const;
	std::vector<unsigned int> GetGlyphIndices(const std::vector<unsigned int> &str) const;

	std::string GetFontID() const;

private:
	void SetCharSize(int size);
	static bool IsWrapChar(unsigned int c);

	FT_Face _face;
	int _size;
};

#endif
