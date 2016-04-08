#ifndef AUDIO_MANAGER_H_
#define AUDIO_MANAGER_H_

#include "AudioContext.h"
#include "AudioStream.h"

#include <emuall/util/wavaudiofile.h>

#include <list>

class AudioManager
{
public: 
	// Constructor and destructor
	AudioManager();
	~AudioManager();

	void Tick();
	void Play();
	void Pause();

	void AddAudioStream(AudioStream &stream);
	void ClearAudioStreams();

private:
	AudioContext _context;
	std::list<AudioStream> _streams;	
};


#endif