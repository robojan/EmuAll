
#include <GL/glew.h>

#include <emuall/graphics/graphicsException.h>
#include <emuall/graphics/bufferObject.h>

static GLenum GetBufferType(BufferObject::Type type) {
	switch (type) {
	case BufferObject::Type::PixelPack: return GL_PIXEL_PACK_BUFFER;
	case BufferObject::Type::PixelUnpack: return GL_PIXEL_UNPACK_BUFFER;
	case BufferObject::Type::Uniform: return GL_UNIFORM_BUFFER;
	case BufferObject::Type::Array: return GL_ARRAY_BUFFER;
	case BufferObject::Type::Texture: return GL_TEXTURE_BUFFER;
	case BufferObject::Type::DrawIndirect: return GL_DRAW_INDIRECT_BUFFER;
	case BufferObject::Type::DispatchIndirect: return GL_DISPATCH_INDIRECT_BUFFER;
	case BufferObject::Type::AtomicCounter: return GL_ATOMIC_COUNTER_BUFFER;
	case BufferObject::Type::ElementArray: return GL_ELEMENT_ARRAY_BUFFER;
	case BufferObject::Type::ShaderStorage: return GL_SHADER_STORAGE_BUFFER;
	case BufferObject::Type::TransfromFeedback: return GL_TRANSFORM_FEEDBACK_BUFFER;
	default:
		throw GraphicsException(GL_INVALID_ENUM, "Invalid BufferObject Type");
	}
}

static GLenum GetUsage(BufferObject::Usage usage) {
	switch (usage) {
	case BufferObject::Usage::StreamDraw: return GL_STREAM_DRAW;
	case BufferObject::Usage::StreamRead: return GL_STREAM_READ;
	case BufferObject::Usage::StreamCopy: return GL_STREAM_COPY;
	case BufferObject::Usage::DynamicDraw: return GL_DYNAMIC_DRAW;
	case BufferObject::Usage::DynamicRead: return GL_DYNAMIC_READ;
	case BufferObject::Usage::DynamicCopy: return GL_DYNAMIC_COPY;
	case BufferObject::Usage::StaticDraw: return GL_STATIC_DRAW;
	case BufferObject::Usage::StaticRead: return GL_STATIC_READ;
	case BufferObject::Usage::StaticCopy: return GL_STATIC_COPY;
	default:
		throw GraphicsException(GL_INVALID_ENUM, "Invalid BufferObject usage");
	}
}

BufferObject::BufferObject(Type type) : _type(GetBufferType(type)), _bo(0)
{
	GL_CHECKED(glGenBuffers(1, &_bo));
}

BufferObject::BufferObject(BufferObject &other) :
	_type(other._type), _bo(other._bo)
{
	other._bo = 0;
}

BufferObject::~BufferObject()
{
	if (glIsBuffer(_bo)) {
		glDeleteBuffers(1, &_bo);
	}
}

BufferObject & BufferObject::operator=(BufferObject &other)
{
	_type = other._type;
	_bo = other._bo;
	other._bo = 0;
	return *this;
}

void BufferObject::BufferData(Usage usage, int size, const void *data)
{
	Begin();
	GL_CHECKED(glBufferData(_type, size, data, GetUsage(usage)));
	End();
}

void BufferObject::BufferSubData(int offset, int size, const void *data)
{
	Begin();
	GL_CHECKED(glBufferSubData(_type, offset, size, data));
	End();
}

void BufferObject::Begin()
{
	GL_CHECKED(glBindBuffer(_type, _bo));
}

void BufferObject::End()
{
	GL_CHECKED(glBindBuffer(_type, 0));
}