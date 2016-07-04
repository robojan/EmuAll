#include <emuall/graphics/texture3D.h>
#include <emuall/graphics/graphicsexception.h>
#include <GL/glew.h>
#include <cassert>

Texture3D::Texture3D() :
	_id(0)
{
	SetGLType(Texture3D::Texture2DArray);
}

Texture3D::Texture3D(int width, int height, int depth, const char *data, Format format, int stride /*= -1*/, Type type) :
	_id(0)
{
	SetGLType(type);
	LoadTexture(width, height, depth, data, format, stride);
}

Texture3D::Texture3D(Texture3D &other) :
	_id(other._id), _type(other._type)
{
	other._id = 0;
}

Texture3D::~Texture3D()
{
	Clean();
}

Texture3D & Texture3D::operator=(Texture3D &other)
{
	Clean();
	_type = other._type;
	_id = other._id;
	other._id = 0;
	return *this;
}

void Texture3D::LoadTexture(int width, int height, int depth, const char *data, Format format, int stride /*= -1*/)
{
	GLint glFormat;
	GLint glInternalFormat;
	int elementSize = 0;
	switch (format) {
	case RGB:
		elementSize = 3;
		glFormat = GL_RGB;
		glInternalFormat = GL_RGB8;
		break;
	case RGBA:
		elementSize = 4;
		glFormat = GL_RGBA;
		glInternalFormat = GL_RGBA8;
		break;
	case Red:
		elementSize = 1;
		glFormat = GL_RED;
		glInternalFormat = GL_R8;
		break;
	case USHORT_5_5_5_1:
		elementSize = 2;
		glFormat = GL_UNSIGNED_SHORT_5_5_5_1;
		glInternalFormat = GL_RGBA8;
		break;
	case USHORT_1_5_5_5:
		elementSize = 2;
		glFormat = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		glInternalFormat = GL_RGBA8;
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

	Bind();
	GL_CHECKED(glPixelStorei(GL_UNPACK_ROW_LENGTH, stride));
	//GL_CHECKED(glTexImage3D(_type, 0, glInternalFormat, width, height, depth, 0, glFormat, GL_UNSIGNED_BYTE, data));
	GL_CHECKED(glTexStorage3D(_type, 1, glInternalFormat, width, height, depth));
	if (data != nullptr) {
		GL_CHECKED(glTexSubImage3D(_type, 0, 0, 0, 0, width, height, depth, glFormat, GL_UNSIGNED_BYTE, data));
	}
	GL_CHECKED(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
	UnBind();
}

void Texture3D::Clean()
{
	if (glIsTexture(_id)) {
		GL_CHECKED(glDeleteTextures(1, &_id));
	}
}

static GLenum GetGLFilter(Texture3D::Filter filter) {
	switch (filter) {
	case Texture3D::Nearest:
		return GL_NEAREST;
	case Texture3D::Linear:
		return GL_LINEAR;
	case Texture3D::NearestMipmapNearest:
		return GL_NEAREST_MIPMAP_NEAREST;
	case Texture3D::LinearMipmapNearest:
		return GL_LINEAR_MIPMAP_NEAREST;
	case Texture3D::NearestMipmapLinear:
		return GL_NEAREST_MIPMAP_LINEAR;
	case Texture3D::LinearMipmapLinear:
		return GL_LINEAR_MIPMAP_LINEAR;
	}
	return GL_LINEAR;
}

static GLenum GetGLWrap(Texture3D::Wrap wrap) {
	switch (wrap) {
	case Texture3D::ClampToEdge:
		return GL_CLAMP_TO_EDGE;
	case Texture3D::MirroredRepeat:
		return GL_MIRRORED_REPEAT;
	case Texture3D::Repeat:
		return GL_REPEAT;
	}
	return GL_REPEAT;
}

Texture3D & Texture3D::SetMinFilter(Filter filter)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GetGLFilter(filter)));
	return *this;
}

Texture3D & Texture3D::SetMagFilter(Filter filter)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GetGLFilter(filter)));
	return *this;
}

Texture3D & Texture3D::SetFilter(Filter min, Filter mag)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_MIN_FILTER, GetGLFilter(min)));
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_MAG_FILTER, GetGLFilter(mag)));
	return *this;
}

Texture3D & Texture3D::SetWrapS(Wrap wrap)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_WRAP_S, GetGLWrap(wrap)));
	return *this;
}

Texture3D & Texture3D::SetWrapT(Wrap wrap)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_WRAP_T, GetGLWrap(wrap)));
	return *this;
}

Texture3D & Texture3D::SetWrap(Wrap wrapS, Wrap wrapT)
{
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_WRAP_S, GetGLWrap(wrapS)));
	GL_CHECKED(glTexParameteri(_type, GL_TEXTURE_WRAP_T, GetGLWrap(wrapT)));
	return *this;
}

bool Texture3D::IsValid() const
{
	return glIsTexture(_id) == GL_TRUE;
}

void Texture3D::Bind()
{
	GL_CHECKED(glBindTexture(_type, _id));
}

void Texture3D::UnBind()
{
	GL_CHECKED(glBindTexture(_type, 0));
}

void Texture3D::UpdateData(int x, int y, int z, int width, int height, int depth, const char *data, Format format, int stride /*= -1*/)
{
	GLint glFormat;
	GLint glDataFormat;
	switch (format) {
	case RGB:
		glFormat = GL_RGB;
		glDataFormat = GL_UNSIGNED_BYTE;
		break;
	case RGBA:
		glFormat = GL_RGBA;
		glDataFormat = GL_UNSIGNED_BYTE;
		break;
	case Red:
		glFormat = GL_RED;
		glDataFormat = GL_UNSIGNED_BYTE;
		break;
	case USHORT_5_5_5_1:
		glFormat = GL_RGBA;
		glDataFormat = GL_UNSIGNED_SHORT_5_5_5_1;
		break;
	case USHORT_1_5_5_5:
		glFormat = GL_RGBA;
		glDataFormat = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		break;
	default:
		throw GraphicsException(GL_INVALID_ENUM, "Texture format not recognized");
		break;
	}
	if (stride == -1) {
		stride = width;
	}

	Bind();
	GL_CHECKED(glPixelStorei(GL_UNPACK_ROW_LENGTH, stride));
	GL_CHECKED(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GL_CHECKED(glTexSubImage3D(_type, 0, x, y, z, width, height, depth, glFormat, glDataFormat, data));
	GL_CHECKED(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
	UnBind();
}

void Texture3D::SetGLType(Type type)
{
	switch (type) {
	default:
	case Texture2DArray:
		_type = GL_TEXTURE_2D_ARRAY;
		break;
	}
}
