
#include <emuall/graphics/font.h>
#include <emuall/graphics/graphicsException.h>

FontLibrary FontLibrary::_singleton;

FontLibrary & FontLibrary::GetSingleton()
{
	return _singleton;
}

FontLibrary::FontLibrary()
{
	FT_Error error = FT_Init_FreeType(&_library);
	if (error != FT_Err_Ok) {
		_library = nullptr;
		throw FontException(error, "Could not initialize Freetype library");
	}
}

FontLibrary::~FontLibrary()
{
	if (_library != nullptr) {
		FT_Done_FreeType(_library);
	}
}