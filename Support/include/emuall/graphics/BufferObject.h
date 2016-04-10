#ifndef BUFFEROBJECT_H_
#define BUFFEROBJECT_H_

#include <emuall/common.h>

class DLLEXPORT BufferObject
{
public:
	enum Type {
		PixelPack,
		PixelUnpack,
		Uniform,
		Array,
		Texture,
		DrawIndirect,
		DispatchIndirect,
		AtomicCounter,
		ElementArray,
		ShaderStorage,
		TransfromFeedback
	};
	enum Usage {
		StreamDraw,
		StreamRead,
		StreamCopy,
		DynamicDraw,
		DynamicRead,
		DynamicCopy,
		StaticDraw,
		StaticRead,
		StaticCopy,
	};

	BufferObject(Type type);
	BufferObject(BufferObject &other);
	virtual ~BufferObject();

	BufferObject &operator=(BufferObject &other);

	void BufferData(Usage usage, int size, const void *data);

	void Begin();
	void End();

private:
	unsigned int _type;
	unsigned int _bo;
};

#endif
