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

Texture::~Texture()
{
	Clean();
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
