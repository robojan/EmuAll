#include <emuall/graphics/texture.h>
#include <GL/glew.h>
#include <cassert>

Texture::Texture() :
	_id(0)
{

}

Texture::Texture(const char *filename) :
	_id(0)
{
	LoadTexture(filename);
}

Texture::Texture(int width, int height, const char *data, Format format, int stride /*= -1*/) :
	_id(0)
{
	LoadTexture(width, height, data, format, stride);
}

Texture::Texture(Texture &other) :
	_id(other._id)
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
	GLenum result;
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
	glGetError();
	glGenTextures(1, &_id);
	if ((result = glGetError()) != GL_NO_ERROR) {
		assert(result == GL_NO_ERROR);
		return;
	}

	Begin();
	glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
	glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLFilter(filter));
	return *this;
}

Texture & Texture::SetMagFilter(Filter filter)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLFilter(filter));
	return *this;
}

Texture & Texture::SetFilter(Filter min, Filter mag)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetGLFilter(min));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetGLFilter(mag));
	return *this;
}

Texture & Texture::SetWrapS(Wrap wrap)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGLWrap(wrap));
	return *this;
}

Texture & Texture::SetWrapT(Wrap wrap)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGLWrap(wrap));
	return *this;
}

Texture & Texture::SetWrap(Wrap wrapS, Wrap wrapT)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetGLWrap(wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetGLWrap(wrapT));
	return *this;
}

bool Texture::IsValid() const
{
	return glIsTexture(_id) == GL_TRUE;
}

void Texture::Begin()
{
	glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::End()
{
	glBindTexture(GL_TEXTURE_2D, 0);
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
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height,glFormat, GL_UNSIGNED_BYTE, data);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	End();
}
