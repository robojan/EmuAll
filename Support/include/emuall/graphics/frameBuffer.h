#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <emuall/common.h>
#include <emuall/graphics/texture.h>
#include <vector>
#include <map>

class DLLEXPORT FrameBuffer {
public:
	FrameBuffer(int width, int height);
	FrameBuffer(FrameBuffer &buffer);
	~FrameBuffer();

	FrameBuffer &operator= (FrameBuffer &other);

	bool IsValid() const;

	void Begin() const;
	void End() const;

	void AttachColorBuffer(int level, bool rectangle = false);
	void AttachDepthBuffer();
	void AttachDepthTexture();
	int GetWidth() const;
	int GetHeight() const;

	void Clean();

	Texture &GetColorBuffer(int level);
	Texture &GetDepthTexture();

private:
	std::vector<unsigned int> *_drawBuffers;
	std::map<int, Texture> *_textures;
	Texture _depthTexture;
	unsigned int _fbo;
	unsigned int _depthBuffer;
	int _width;
	int _height;
};

#endif