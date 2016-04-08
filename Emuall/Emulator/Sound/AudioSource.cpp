
#include <AL/al.h>

#include "AudioBuffer.h"
#include "AudioSource.h"
#include "AudioException.h"

AudioSource::AudioSource() : _source(AL_INVALID)
{
	alGenSources(1, &_source);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not create openAL source");
	}
}

AudioSource::AudioSource(AudioSource &other) :
	_source(other._source), _queuedBuffers(other._queuedBuffers)
{
	other._source = AL_INVALID;
	other._queuedBuffers.clear();
}

AudioSource::~AudioSource()
{
	if (alIsSource(_source)) {
		alDeleteSources(1, &_source);
		_source = AL_INVALID;
	}
}

void AudioSource::AttachBuffer(AudioBuffer &buffer)
{
	alSourcei(_source, AL_BUFFER, buffer._buffer);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not attach openAL buffer to source");
	}
}

void AudioSource::SetPosition(float x, float y, float z)
{
	alSource3f(_source, AL_POSITION, x, y, z);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source position");
	}
}

void AudioSource::SetVelocity(float x, float y, float z)
{
	alSource3f(_source, AL_VELOCITY, x, y, z);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source velocity");
	}
}

void AudioSource::SetRelative(bool relative)
{
	alSourcei(_source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source relative");
	}
}

void AudioSource::SetLooping(bool looping)
{
	alSourcei(_source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source looping");
	}
}

void AudioSource::SetMinGain(float gain)
{
	alSourcef(_source, AL_MIN_GAIN, gain);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source minimal gain");
	}
}

void AudioSource::SetMaxGain(float gain)
{
	alSourcef(_source, AL_MAX_GAIN, gain);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source maximal gain");
	}
}

void AudioSource::SetReferenceDistance(float distance)
{
	alSourcef(_source, AL_REFERENCE_DISTANCE, distance);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source reference distance");
	}
}

void AudioSource::SetRolloffFactor(float factor)
{
	alSourcef(_source, AL_ROLLOFF_FACTOR, factor);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source rolloff factor");
	}
}

void AudioSource::SetMaxDistance(float distance)
{
	alSourcef(_source, AL_MAX_DISTANCE, distance);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source max distance");
	}
}

void AudioSource::SetPitch(float pitch)
{
	alSourcef(_source, AL_PITCH, pitch);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source pitch");
	}
}

void AudioSource::SetDirection(float x, float y, float z, float innerAngle, float outerAngle, float outerGain)
{
	alSource3f(_source, AL_DIRECTION, x, y, z);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source direction");
	}
	alSourcef(_source, AL_CONE_INNER_ANGLE, innerAngle);
	error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source cone inner angle");
	}
	alSourcef(_source, AL_CONE_OUTER_ANGLE, outerAngle);
	error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source cone outer angle");
	}
	alSourcef(_source, AL_CONE_OUTER_GAIN, outerAngle);
	error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source cone outer gain");
	}

}

void AudioSource::SetOffsetSeconds(float offset)
{
	alSourcef(_source, AL_SEC_OFFSET, offset);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source offset");
	}
}

void AudioSource::SetOffsetSamples(int offset)
{
	alSourcei(_source, AL_SAMPLE_OFFSET, offset);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source offset");
	}
}

void AudioSource::SetOffsetBytes(int offset)
{
	alSourcei(_source, AL_BYTE_OFFSET, offset);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set source offset");
	}
}

int AudioSource::GetNumBuffersProcessed()
{
	int num;
	alGetSourcei(_source, AL_BUFFERS_PROCESSED, &num);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not get source processed buffers");
	}
	return num;
}

int AudioSource::GetNumBuffersQueued()
{
	int num;
	alGetSourcei(_source, AL_BUFFERS_QUEUED, &num);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not get source queued buffers");
	}
	return num;
}

void AudioSource::Play()
{
	alSourcePlay(_source);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not start playing the source");
	}
}

void AudioSource::Pause()
{
	alSourcePause(_source);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not pause the source");
	}
}

void AudioSource::Stop()
{
	alSourceStop(_source);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not stop playing the source");
	}
}

void AudioSource::Rewind()
{
	alSourceRewind(_source);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not rewind the source");
	}
}

bool AudioSource::IsPlaying()
{
	int state;
	alGetSourcei(_source, AL_SOURCE_STATE, &state);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not get source state");
	}
	return state == AL_PLAYING;
}

bool AudioSource::IsPaused()
{
	int state;
	alGetSourcei(_source, AL_SOURCE_STATE, &state);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not get source state");
	}
	return state == AL_PAUSED;
}

bool AudioSource::IsStopped()
{
	int state;
	alGetSourcei(_source, AL_SOURCE_STATE, &state);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not get source state");
	}
	return state == AL_STOPPED;
}

bool AudioSource::IsInitial()
{
	int state;
	alGetSourcei(_source, AL_SOURCE_STATE, &state);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not get source state");
	}
	return state == AL_INITIAL;
}

void AudioSource::QueueBuffers(int count, AudioBuffer *buffers)
{
	std::vector<unsigned int> bufferIDs(count);
	for (int i = 0; i < count; i++) {
		bufferIDs[i] = buffers[i]._buffer;
		_queuedBuffers[bufferIDs[i]] = &buffers[i];
	}
	alSourceQueueBuffers(_source, count, bufferIDs.data());
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not queue the buffers");
	}
}

void AudioSource::QueueBuffers(std::vector<AudioBuffer> &buffers)
{
	std::vector<unsigned int> bufferIDs(buffers.size());
	for (unsigned i = 0; i < buffers.size(); i++) {
		bufferIDs[i] = buffers[i]._buffer;
		_queuedBuffers[bufferIDs[i]] = &buffers[i];
	}
	alSourceQueueBuffers(_source, (int)bufferIDs.size(), bufferIDs.data());
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not queue the buffers");
	}
}

void AudioSource::UnqueueBuffers(int count, AudioBuffer *buffers)
{
	std::vector<unsigned int> bufferIDs(count);
	for (int i = 0; i < count; i++) {
		bufferIDs[i] = buffers[i]._buffer;
		_queuedBuffers[bufferIDs[i]] = &buffers[i];
	}
	alSourceUnqueueBuffers(_source, count, bufferIDs.data());
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not unqueue the buffers");
	}
}

AudioBuffer *AudioSource::UnqueueBuffer()
{
	unsigned int buffer;
	alSourceUnqueueBuffers(_source, 1, &buffer);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not unqueue the buffers");
	}
	return _queuedBuffers.at(buffer);
}

AudioSource & AudioSource::operator=(AudioSource &other)
{
	if (alIsSource(_source)) {
		alDeleteBuffers(1, &_source);
		_source = AL_INVALID;
	}

	_source = other._source;
	other._source = 0;
	_queuedBuffers = other._queuedBuffers;
	other._queuedBuffers.clear();
	return *this;
}

