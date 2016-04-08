#ifndef AUDIOBUFFER_H_
#define AUDIOBUFFER_H_

#include <vector>

class AudioContext;

class AudioBuffer {
	friend class AudioSource;
public:
	enum Format {
		Mono8,
		Mono16,
		Stereo8,
		Stereo16
	};

	AudioBuffer();
	AudioBuffer(AudioBuffer &other);
	~AudioBuffer();

	AudioBuffer &operator=(AudioBuffer &other);

	void BufferData(Format format, const void *data, int size, int freq);
	void AddBufferIDToList(std::vector<unsigned int> &list);

private:
	unsigned int _buffer;
	Format _format;
	int _size;
	int _freq;
};

#endif
