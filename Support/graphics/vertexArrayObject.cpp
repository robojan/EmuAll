
#include <GL/glew.h>

#include <emuall/graphics/vertexArrayObject.h>
#include <emuall/graphics/graphicsException.h>

static GLenum GetDataType(VertexArrayObject::DataType type) {
	switch (type) {
	case VertexArrayObject::HalfFloat:
		return GL_HALF_FLOAT;
	case VertexArrayObject::Float:
		return GL_FLOAT;
	case VertexArrayObject::Double:
		return GL_DOUBLE;
	case VertexArrayObject::Fixed:
		return GL_FIXED;
	case VertexArrayObject::Byte:
		return GL_BYTE;
	case VertexArrayObject::UByte:
		return GL_UNSIGNED_BYTE;
	case VertexArrayObject::Short:
		return GL_SHORT;
	case VertexArrayObject::UShort:
		return GL_UNSIGNED_SHORT;
	case VertexArrayObject::Int:
		return GL_INT;
	case VertexArrayObject::UInt:
		return GL_UNSIGNED_INT;
	default:
		throw GraphicsException(GL_INVALID_ENUM, "Invalid VertexArrayObject Type");
	}
}

VertexArrayObject::VertexArrayObject() : 
	_vao(0)
{
	GL_CHECKED(glGenVertexArrays(1, &_vao));
}

VertexArrayObject::~VertexArrayObject()
{
	if (glIsVertexArray(_vao)) {
		glDeleteVertexArrays(1, &_vao);
	}
}

void VertexArrayObject::Begin()
{
	GL_CHECKED(glBindVertexArray(_vao));
}

void VertexArrayObject::End()
{
	GL_CHECKED(glBindVertexArray(0));
}

void VertexArrayObject::BindBuffer(int location, BufferObject &buffer, int size, DataType type, bool normalized, int stride, int offset)
{
	GL_CHECKED(glEnableVertexAttribArray(location));
	buffer.Begin();
	GL_CHECKED(glVertexAttribPointer(location, size, GetDataType(type),
		normalized ? GL_TRUE : GL_FALSE, stride, (GLvoid *)offset));
	buffer.End();
}

void VertexArrayObject::DisableBuffer(int location)
{
	GL_CHECKED(glDisableVertexAttribArray(location));
}
