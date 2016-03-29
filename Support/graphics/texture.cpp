#include <emuall/graphics/texture.h>
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
	int elementSize = 0;
	switch (format) {
	case RGB:
		elementSize = 3;
		glFormat = GL_RGB;
		break;
	case RGBA:
		elementSize = 4;
		glFormat = GL_RGBA;
		break;
	default:
		return;
		break;
	}
	if (stride == -1) {
		stride = width * elementSize;
	}

	// Generate an texture
	if (glGetError() != GL_NO_ERROR) __debugbreak();
	glGenTextures(1, &_id);
	if (glGetError() != GL_NO_ERROR) __debugbreak();

	Begin();
	glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
	if (glGetError() != GL_NO_ERROR) __debugbreak();
	glTexImage2D(_type, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);
	if (glGetError() != GL_NO_ERROR) __debugbreak();
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	if (glGetError() != GL_NO_ERROR) __debugbreak();
	End();
}

void Texture::Clean()
{
	if (glIsTexture(_id)) {
		glDeleteTextures(1, &_id);
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
	glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GetGLFilter(filter));
	return *this;
}

Texture & Texture::SetMagFilter(Filter filter)
{
	glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GetGLFilter(filter));
	return *this;
}

Texture & Texture::SetFilter(Filter min, Filter mag)
{
	glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GetGLFilter(min));
	glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GetGLFilter(mag));
	return *this;
}

Texture & Texture::SetWrapS(Wrap wrap)
{
	glTexParameteri(_type, GL_TEXTURE_WRAP_S, GetGLWrap(wrap));
	return *this;
}

Texture & Texture::SetWrapT(Wrap wrap)
{
	glTexParameteri(_type, GL_TEXTURE_WRAP_T, GetGLWrap(wrap));
	return *this;
}

Texture & Texture::SetWrap(Wrap wrapS, Wrap wrapT)
{
	glTexParameteri(_type, GL_TEXTURE_WRAP_S, GetGLWrap(wrapS));
	glTexParameteri(_type, GL_TEXTURE_WRAP_T, GetGLWrap(wrapT));
	return *this;
}

bool Texture::IsValid() const
{
	return glIsTexture(_id) == GL_TRUE;
}

void Texture::Begin()
{
	glBindTexture(_type, _id);
	if (glGetError() != GL_NO_ERROR) __debugbreak();
}

void Texture::End()
{
	glBindTexture(_type, 0);
	if (glGetError() != GL_NO_ERROR) __debugbreak();
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
	default:
		return;
		break;
	}
	if (stride == -1) {
		stride = width;
	}

	Begin();
	glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
	glTexSubImage2D(_type, 0, x, y, width, height,glFormat, GL_UNSIGNED_BYTE, data);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
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
