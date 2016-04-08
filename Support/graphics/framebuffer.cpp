#include <emuall/graphics/framebuffer.h>
#include <emuall/graphics/graphicsException.h>

#include <GL/glew.h>
#include <cassert>

FrameBuffer::FrameBuffer(int width, int height) :
	_fbo(0), _depthBuffer(0), _width(width), _height(height)
{
	_textures = new std::map<int, Texture>;
	_drawBuffers = new std::vector<unsigned int>;

	GL_CHECKED(glGenFramebuffers(1, &_fbo));
}

FrameBuffer::FrameBuffer(FrameBuffer &buffer)
{
	*this = buffer;
}

FrameBuffer::~FrameBuffer()
{
	Clean();
	delete _textures;
	delete _drawBuffers;
}

FrameBuffer & FrameBuffer::operator=(FrameBuffer &other)
{
	Clean();
	_fbo = other._fbo;
	_depthBuffer = other._depthBuffer;
	_width = other._width;
	_height = other._height;
	delete _drawBuffers;
	delete _textures;
	_textures = other._textures;
	_drawBuffers = other._drawBuffers;
	other._textures = new std::map<int, Texture>;
	other._drawBuffers = new std::vector<unsigned int>;
	return *this;
}

bool FrameBuffer::IsValid() const
{
	return glIsFramebuffer(_fbo) == GL_TRUE;
}

void FrameBuffer::Begin() const
{
	GL_CHECKED(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));
}

void FrameBuffer::End() const
{
	GL_CHECKED(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::AttachColorBuffer(int level, bool rectangle /*= false*/)
{
	GLint maxColorAttachments;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	assert(level >= 0 && level < maxColorAttachments);

	Texture texture(_width, _height, NULL, Texture::RGBA, -1, 
		rectangle ? Texture::TextureRectangle : Texture::Texture2D);
	assert(texture.IsValid());
	texture.Begin();
	texture.SetFilter(Texture::Nearest, Texture::Nearest);

	Begin();

	GL_CHECKED(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + level, texture._id, 0));

	if (_textures->find(level) == _textures->end()) {
		_drawBuffers->push_back(GL_COLOR_ATTACHMENT0 + level);

		GL_CHECKED(glDrawBuffers((GLsizei)_drawBuffers->size(), _drawBuffers->data()));
	}
	(*_textures)[level] = texture;

	End();
	texture.End();
}

void FrameBuffer::AttachDepthBuffer()
{
	if (_depthBuffer != 0) {
		return;
	}
	Begin();
	GL_CHECKED(glGenRenderbuffers(1, &_depthBuffer));
	GL_CHECKED(glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer));
	GL_CHECKED(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height));
	GL_CHECKED(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer));
	End();
}

int FrameBuffer::GetWidth() const
{
	return _width;
}

int FrameBuffer::GetHeight() const
{
	return _height;
}

void FrameBuffer::Clean()
{
	if (_depthBuffer) {
		GL_CHECKED(glDeleteRenderbuffers(1, &_depthBuffer));
		_depthBuffer = 0;
	}
	if (_fbo) {
		GL_CHECKED(glDeleteFramebuffers(1, &_fbo));
		_fbo = 0;
	}
	_textures->clear();
	_drawBuffers->clear();
}

Texture & FrameBuffer::GetColorBuffer(int level)
{
	auto &it = _textures->find(level);
	if (it == _textures->end()) {
		throw BaseException("Color buffer not found in frameBuffer object");
	}
	return it->second;
}
