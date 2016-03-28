#ifndef TEXTURE_H_

#include <emuall/common.h>

class EMUEXPORT Texture {
public:
	enum Format {
		RGB,
		RGBA
	};

	Texture();
	Texture(const char *filename);
	Texture(int width, int height, const char *data, Format format, int stride = -1);
	~Texture();

	void LoadTexture(const char *fileName);
	void LoadTexture(int width, int height, const char *data, Format format, int stride = -1);
	void Clean();

	bool IsValid() const;

	void Begin();
	void End();

	void UpdateData(int x, int y, int width, int height, const char *data, Format format, int stride = -1);

private:

	unsigned int _id;
};

#endif
