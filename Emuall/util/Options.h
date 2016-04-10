#ifndef _OPTIONS_H
#define _OPTIONS_H

#define DEFAULT_SAMPLERATE 44100
#define DEFAULT_BUFFERSIZE 1024
#define DEFAULT_NUMAUDIOBUFFERS 2
#define DEFAULT_KEEPASPECT true
#define DEFAULT_VIDEOFILTER 0


#include <map>
#include "../Emulator/Emulator.h"

typedef struct
{
	bool keepAspect;
	int filter;
	std::string messageFont;
	int messageFontIdx;
	int messageFontSize;
} OptionsVid_t;

typedef struct
{
	int sampleRate;
	int bufferSize;
	int numBuffers;
} OptionsAud_t;

class Options
{
	friend class InputOptionsFrame;
public:
	~Options();

	static Options &GetSingleton();

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
	Options();
	
	static Options _instance;
	std::map<std::string, std::map<int, EmulatorInput_t>> _keyBindings;
};

#endif