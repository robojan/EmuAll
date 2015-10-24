#include "audio.h"

AudioManager::AudioManager()
{
	// Init state
	alDistanceModel(AL_NONE);
	alDopplerFactor(0.0);

	// Init listener
	alListener3f(AL_POSITION,0,0,0);
	alListener3f(AL_VELOCITY,0,0,0);
	ALfloat orientation[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
	alListenerfv(AL_ORIENTATION, orientation);

}

AudioManager::~AudioManager()
{

}

void AudioManager::Tick()
{
	// Update the buffers

}