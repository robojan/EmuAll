#ifndef _AUDIO_H
#define _AUDIO_H

#include <stddef.h>
#include <list>
#include <string>

class Audio
{
public: 
	// Constructor and destructor
	Audio();
	~Audio();

	// Initialization functions
	int Init(const std::string &device = "");
	void Close();

	static std::list<std::string> GetDevices();

private:
};

#endif