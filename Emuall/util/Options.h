#ifndef _OPTIONS_H
#define _OPTIONS_H

#define DEFAULT_SAMPLERATE 44100
#define DEFAULT_BUFFERSIZE 4096
#define DEFAULT_KEEPASPECT true


#include <map>
#include "../Emulator/Emulator.h"

typedef struct
{
	bool keepAspect;
} OptionsVid_t;

typedef struct
{
	int sampleRate;
	int bufferSize;
} OptionsAud_t;

class Options
{
	friend class InputOptionsFrame;
public:
	Options();
	~Options();
	void LoadOptions();
	void SaveOptions();
	int GetKeyBinding(std::string name, int key, int defaultKey);
	int GetInputKeyBinding(std::string name, int key, int defaultKey);
	const EmulatorInput_t *GetKeyBindingInfo(std::string name, int key);
	void AddKeyBinding(std::string name, const EmulatorInput_t &keyInfo);
	void RebindKey(std::string name, int emuKey, int inputKey);

	void SaveRecentFile(std::string file);

	std::string GetConfigFileName() const;

	std::string recentFiles[5];
	OptionsVid_t videoOptions;
	OptionsAud_t audioOptions;
private:
	std::map<std::string, std::map<int, EmulatorInput_t>> _keyBindings;
};

#endif