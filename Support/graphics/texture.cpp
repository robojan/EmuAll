#include <emuall/graphics/texture.h>
#include <emuall/graphics/graphicsexception.h>
#include <GL/glew.h>
#include <cassert>

Texture::Texture() :
	_id(0)
{
	SetGLType(Texture::Texture2D);
}

Texture::Texture(const char *filename, Type type) :
	_id(0)
{
	SetGLType(type);
	LoadTexture(filename);
}

Texture::Texture(int width, int height, const char *data, Format format, int stride /*= -1*/, Type type) :
	_id(0)
{
	SetGLType(type);
	LoadTexture(width, height, data, format, stride);
}

Texture::Texture(Texture &other) :
	_id(other._id), _type(other._type)
{
	other._id = 0;
}

Texture::~Texture()
{
	Clean();
}

Texture & Texture::operator=(Texture &other)
{
	Clean();
	_type = other._type;
	_id = other._id;
	other._id = 0;
	return *this;
}

void Texture::LoadTexture(const char *fileName)
{
	assert(false);
}

void Texture::LoadTexture(int width, int height, const char *data, Format format, int stride /*= -1*/)
{
	GLint glFormat;
	GLint glInternalFormat;
	int elementSize = 0;
	switch (format) {
	case RGB:
		elementSize = 3;
		glFormat = GL_RGB;
		glInternalFormat = GL_RGB;
		break;
	case RGBA:
		elementSize = 4;
		glFormat = GL_RGBA;
		glInternalFormat = GL_RGBA;
		break;
	case Red:
		elementSize = 1;
		glFormat = GL_RED;
		glInternalFormat = GL_RED;
		break;
	default:
		throw GraphicsException(GL_INVALID_ENUM, "Texture format not recognized");
		break;
	}
	if (stride == -1) {
		stride = width * elementSize;
	}

	// Generate an texture	
	GL_CHECKED(glGenTextures(1, &_id));

	Begin();
	GL_CHECKED(glPixelStorei(GL_UNPACK_ROW_LENGTH, stride));
	GL_CHECKED(glTexImage2D(_type, 0, glInternalFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data));
	GL_CHECKED(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
	End();
}

void Texture::Clean()
{
	if (glIsTexture(_id)) {
		GL_CHECKED(glDeleteTextures(1, &_id));
	}
}

static GLenum GetGLFilter(Texture::Filter filter) {
	switch (filter) {
	case Texture::Nearest:
		return GL_NEAREST;
	case Texture::Linear:
		return GL_LINEAR;
	case Texture::NearestMipmapNearest:
		return GL_NEAREST_MIPMAP_NEAREST;
	case Texture::LinearMipmapNearest:
		return GL_LINEAR_MIPMAP_NEAREST;
	case Texture::NearestMipmapLinear:
		return GL_NEAREST_MIPMAP_LINEAR;
	case Texture::LinearMipmapLinear:
		return GL_LINEAR_MIPMAP_LINEAR;
	}
	return GL_LINEAR;
}

static GLenum GetGLWrap(Texture::Wrap wrap) {
	switch (wrap) {
	case Texture::ClampToEdge:
		return GL_CLAMP_TO_EDGE;
	case Texture::MirroredRepeat:
		return GL_MIRRORED_REPEAT;
	case Texture::Repeat:
		return GL_REPEAT;
	}
	return GL_REPEAT;
}

Texture & Texture::SetMinFilter(Filter filter)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GetGLFilter(filter)));
	return *this;
}

Texture & Texture::SetMagFilter(Filter filter)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GetGLFilter(filter)));
	return *this;
}

Texture & Texture::SetFilter(Filter min, Filter mag)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GetGLFilter(min)));
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GetGLFilter(mag)));
	return *this;
}

Texture & Texture::SetWrapS(Wrap wrap)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_WRAP_S, GetGLWrap(wrap)));
	return *this;
}

Texture & Texture::SetWrapT(Wrap wrap)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_WRAP_T, GetGLWrap(wrap)));
	return *this;
}

Texture & Texture::SetWrap(Wrap wrapS, Wrap wrapT)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_WRAP_S, GetGLWrap(wrapS)));
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_WRAP_T, GetGLWrap(wrapT)));
	return *this;
}

bool Texture::IsValid() const
{
	return glIsTexture(_id) == GL_TRUE;
}

void Texture::Begin()
{
	GL_CHECKED(glBindTexture(_type, _id));
}

void Texture::End()
{
	GL_CHECKED(glBindTexture(_type, 0));
}

void Texture::UpdateData(int x, int y, int width, int height, const char *data, Format format, int stride /*= -1*/)
{
	GLint glFormat;
	switch (format) {
	case RGB:
		glFormat = GL_RGB;
		break;
	case RGBA:
		glFormat = GL_RGBA;
		break;
	case Red:
		glFormat = GL_RED;
		break;
	default:
		throw GraphicsException(GL_INVALID_ENUM, "Texture format not recognized");
		break;
	}
	if (stride == -1) {
		stride = width;
	}

	Begin();
	GL_CHECKED(glPixelStorei(GL_UNPACK_ROW_LENGTH, stride));
	GL_CHECKED(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GL_CHECKED(glTexSubImage2D(_type, 0, x, y, width, height,glFormat, GL_UNSIGNED_BYTE, data));
	GL_CHECKED(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
	End();
}

void Texture::SetGLType(Type type)
{
	switch (type) {
	default:
	case Texture2D:
		_type = GL_TEXTURE_2D;
		break;
	case TextureRectangle:
		_type = GL_TEXTURE_RECTANGLE;
		break;
	}
}
