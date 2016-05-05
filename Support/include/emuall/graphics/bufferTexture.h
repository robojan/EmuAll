#pragma once

#include <emuall/common.h>
#include <emuall/graphics/BufferObject.h>

class DLLEXPORT BufferTexture {
public:
	enum class Format {
		R8,	R16, R16F, R32F, R8I, R16I, R32I, R8UI, R16UI, R32UI, RG8,
		RG16, RG16F, RG32F, RG8I, RG16I, RG32I, RG8UI, RG16UI, RG32UI,
		RGB32F, RGB32I,	RGB32UI,
		RGBA8, RGBA16, RGBA16F, RGBA32F, RGBA8I, RGBA16I, RGBA32I, RGBA8UI, RGBA16UI, RGBA32UI
	};

	BufferTexture(int size, const char *data, Format format, BufferObject::Usage usage);
	BufferTexture(BufferObject &bo, Format format);
	BufferTexture(BufferTexture &other);  // Copy
	BufferTexture(BufferTexture &&other); // Move operator
	virtual ~BufferTexture();

	BufferTexture &operator=(BufferTexture &other);// Copy
	BufferTexture &operator=(BufferTexture &&other); // Move
	
	BufferObject &GetBufferObject();

	void Bind();
	static void UnBind();
	
private:
	unsigned int _id;
	BufferObject _bo;
	Format _format;
};

