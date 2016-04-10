#ifndef VERTEXARRAYOBJECT_H_
#define VERTEXARRAYOBJECT_H_

#include <emuall/common.h>
#include <emuall/graphics/bufferObject.h>

class DLLEXPORT VertexArrayObject {
public:
	enum DataType {
		HalfFloat,
		Float,
		Double,
		Fixed,
		Byte,
		UByte,
		Short,
		UShort,
		Int,
		UInt,
	};

	VertexArrayObject();
	virtual ~VertexArrayObject();

	void Begin();
	void End();
	void BindBuffer(int location, BufferObject &buffer, int size, DataType type, bool normalized = false, int stride = 0, int offset = 0);
	void DisableBuffer(int location);
private:
	unsigned int _vao;
};

#endif