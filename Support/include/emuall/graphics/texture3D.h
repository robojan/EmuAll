#ifndef TEXTURE3D_H_
#define TEXTURE3D_H_

#include <emuall/common.h>

class DLLEXPORT Texture3D {
public:
	enum Format {
		RGB,
		RGBA,
		Red,
		USHORT_5_5_5_1,
		USHORT_1_5_5_5,
	};

	enum Filter {
		// Both minimization and magnification
		Nearest,
		Linear,
		// Only minimization
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear,
	};
	enum Wrap {
		ClampToEdge,
		MirroredRepeat,
		Repeat,
	};
	enum Type {
		Texture2DArray,
	};

	Texture3D();
	Texture3D(Texture3D &other);
	Texture3D(int width, int height, int depth, const char *data, Format format, int stride = -1, Type type = Texture2DArray);
	~Texture3D();

	Texture3D &operator=(Texture3D &other);

	void LoadTexture(int width, int height, int depth, const char *data, Format format, int stride = -1);
	void Clean();

	Texture3D &SetMinFilter(Filter filter);
	Texture3D &SetMagFilter(Filter filter);
	Texture3D &SetFilter(Filter min, Filter mag);
	Texture3D &SetWrapS(Wrap wrap);
	Texture3D &SetWrapT(Wrap wrap);
	Texture3D &SetWrap(Wrap wrapS, Wrap wrapT);

	bool IsValid() const;

	void Bind();
	void UnBind();

	void UpdateData(int x, int y, int z, int width, int height, int depth, const char *data, Format format, int stride = -1);

private:
	void SetGLType(Type type);

	unsigned int _id;
	unsigned int _type;
};

#endif
