#ifndef TEXTURE_H_

#include <emuall/common.h>

class EMUEXPORT Texture {
	friend class FrameBuffer;
public:
	enum Format {
		RGB,
		RGBA,
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

	Texture();
	Texture(Texture &other);
	Texture(const char *filename);
	Texture(int width, int height, const char *data, Format format, int stride = -1);
	~Texture();

	Texture &operator=(Texture &other);

	void LoadTexture(const char *fileName);
	void LoadTexture(int width, int height, const char *data, Format format, int stride = -1);
	void Clean();

	Texture &SetMinFilter(Filter filter);
	Texture &SetMagFilter(Filter filter);
	Texture &SetFilter(Filter min, Filter mag);
	Texture &SetWrapS(Wrap wrap);
	Texture &SetWrapT(Wrap wrap);
	Texture &SetWrap(Wrap wrapS, Wrap wrapT);

	bool IsValid() const;

	void Begin();
	void End();

	void UpdateData(int x, int y, int width, int height, const char *data, Format format, int stride = -1);

private:

	unsigned int _id;
};

#endif
