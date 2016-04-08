#ifndef AUDIOSOURCE_H_
#define AUDIOSOURCE_H_

#include <vector>
#include <map>

class AudioBuffer;

class AudioSource {
public:
	AudioSource();
	AudioSource(AudioSource &other);
	~AudioSource();

	AudioSource &operator=(AudioSource &other);

	void AttachBuffer(AudioBuffer &buffer);

	void SetPosition(float x, float y, float z);
	void SetVelocity(float x, float y, float z);
	// Is the position relative to the listener
	void SetRelative(bool relative); 
	// Don't stop playing when the last buffer is reached
	void SetLooping(bool looping);
	// Set the minimal gain. range: 0.0-1.0
	void SetMinGain(float gain);
	// Set the max gain. range: 0.0-1.0
	void SetMaxGain(float gain);
	void SetReferenceDistance(float distance);
	void SetRolloffFactor(float factor);
	void SetMaxDistance(float disatance);
	void SetPitch(float pitch);
	void SetDirection(float x, float y, float z, float innerAngle, float outerAngle, float outerGain);
	void SetOffsetSeconds(float offset);
	void SetOffsetSamples(int offset);
	void SetOffsetBytes(int offset);
	int GetNumBuffersProcessed();
	int GetNumBuffersQueued();

	void Play();
	void Pause();
	void Stop();
	void Rewind();
	bool IsPlaying();
	bool IsPaused();
	bool IsStopped();
	bool IsInitial();

	void QueueBuffers(std::vector<AudioBuffer> &buffers);
	void QueueBuffers(int count, AudioBuffer *buffers);
	void UnqueueBuffers(int count, AudioBuffer *buffers);
	AudioBuffer *UnqueueBuffer();
private:
	std::map <unsigned int, AudioBuffer *> _queuedBuffers;
	unsigned int _source;
};

#endif
