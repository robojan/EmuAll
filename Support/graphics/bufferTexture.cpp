#include <emuall/graphics/bufferTexture.h>
#include <emuall/graphics/graphicsexception.h>
#include <GL/glew.h>
#include <cassert>

static GLenum GetGLFormat(BufferTexture::Format format) {
	switch (format) {
	case BufferTexture::Format::R8:	return GL_R8;
	case BufferTexture::Format::R16: return GL_R16;
	case BufferTexture::Format::R16F: return GL_R16F;
	case BufferTexture::Format::R32F: return GL_R32F;
	case BufferTexture::Format::R8I: return GL_R8I;
	case BufferTexture::Format::R16I: return GL_R16I;
	case BufferTexture::Format::R32I: return GL_R32I;
	case BufferTexture::Format::R8UI: return GL_R8UI;
	case BufferTexture::Format::R16UI: return GL_R16UI;
	case BufferTexture::Format::R32UI: return GL_R32UI;
	case BufferTexture::Format::RG8: return GL_RG8;
	case BufferTexture::Format::RG16: return GL_RG16;
	case BufferTexture::Format::RG16F: return GL_RG16F;
	case BufferTexture::Format::RG32F: return GL_RG32F;
	case BufferTexture::Format::RG8I: return GL_RG8I;
	case BufferTexture::Format::RG16I: return GL_RG16I;
	case BufferTexture::Format::RG32I: return GL_RG32I;
	case BufferTexture::Format::RG8UI: return GL_RG8UI;
	case BufferTexture::Format::RG16UI: return GL_RG16UI;
	case BufferTexture::Format::RG32UI: return GL_RG32UI;
	case BufferTexture::Format::RGB32F: return GL_RGB32F;
	case BufferTexture::Format::RGB32I: return GL_RGB32I;
	case BufferTexture::Format::RGB32UI: return GL_RGB32UI;
	case BufferTexture::Format::RGBA8: return GL_RGBA8;
	case BufferTexture::Format::RGBA16: return GL_RGBA16;
	case BufferTexture::Format::RGBA16F: return GL_RGBA16F;
	case BufferTexture::Format::RGBA32F: return GL_RGBA32F;
	case BufferTexture::Format::RGBA8I: return GL_RGBA8I;
	case BufferTexture::Format::RGBA16I: return GL_RGBA16I;
	case BufferTexture::Format::RGBA32I: return GL_RGBA32I;
	case BufferTexture::Format::RGBA8UI: return GL_RGBA8UI;
	case BufferTexture::Format::RGBA16UI: return GL_RGBA16UI;
	case BufferTexture::Format::RGBA32UI: return GL_RGBA32UI;
	default:
		throw GraphicsException(GL_INVALID_ENUM, "Buffer texture format not recognized");
		break;
	}
}

BufferTexture::BufferTexture(BufferObject &bo, Format format) :
	_id(0), _bo(std::move(bo)), _format(format)
{
	GL_CHECKED(glGenTextures(1, &_id));
	Bind();
	GL_CHECKED(glTexBuffer(GL_TEXTURE_BUFFER, GetGLFormat(format), _bo._bo));
	UnBind();
}

BufferTexture::BufferTexture(int size, const char *data, Format format, BufferObject::Usage usage) :
	_id(0), _bo(BufferObject::Type::Texture), _format(format)
{
	// Create the buffer object
	_bo.BufferData(usage, size, data);

	GL_CHECKED(glGenTextures(1, &_id));
	Bind();
	GL_CHECKED(glTexBuffer(GL_TEXTURE_BUFFER, GetGLFormat(format), _bo._bo));
	UnBind();
}

BufferTexture::BufferTexture(BufferTexture &other) :
	_id(0), _bo(other._bo), _format(other._format)
{
	GL_CHECKED(glGenTextures(1, &_id));
	Bind();
	GL_CHECKED(glTexBuffer(GL_TEXTURE_BUFFER, GetGLFormat(_format), _bo._bo));
	UnBind();

}

BufferTexture::BufferTexture(BufferTexture &&other) :
	_id(other._id), _bo(std::move(other._bo)), _format(other._format)
{
	other = BufferTexture(0, nullptr, Format::R8, BufferObject::Usage::StaticDraw);
}

BufferTexture::~BufferTexture()
{
	if (glIsTexture(_id)) {
		glDeleteTextures(1, &_id);
		_id = 0;
	}
}

BufferTexture & BufferTexture::operator=(BufferTexture &other)
{
	if (_id == 0) {
		GL_CHECKED(glGenTextures(1, &_id));
	}
	_bo = other._bo;
	_format = other._format;
	Bind();
	GL_CHECKED(glTexBuffer(GL_TEXTURE_BUFFER, GetGLFormat(_format), _bo._bo));
	UnBind();
	return *this;
}

BufferTexture & BufferTexture::operator=(BufferTexture &&other)
{
	_id = other._id;
	_bo = std::move(other._bo);
	_format = other._format;
	other = BufferTexture(0, nullptr, Format::R8, BufferObject::Usage::StaticDraw);
	return *this;
}

BufferObject & BufferTexture::GetBufferObject()
{
	return _bo;
}

void BufferTexture::Bind()
{
	GL_CHECKED(glBindTexture(GL_TEXTURE_BUFFER, _id));
}

void BufferTexture::UnBind()
{
	GL_CHECKED(glBindTexture(GL_TEXTURE_BUFFER, 0));
}