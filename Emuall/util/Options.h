#ifndef _OPTIONS_H
#define _OPTIONS_H

#define DEFAULT_SAMPLERATE 44100
#define DEFAULT_BUFFERSIZE 1024
#define DEFAULT_NUMAUDIOBUFFERS 2
#define DEFAULT_KEEPASPECT true
#define DEFAULT_VIDEOFILTER 0


#include <map>
#include <vector>
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
	void LoadKeyBindings(const std::string &name, const std::vector<EmulatorInput_t> &bindings);

	void SaveRecentFile(std::string file);

	std::string GetConfigFileName() const;

	void RebindKey(std::string name, int id, int keycode, int idx);

	std::string recentFiles[5];
	OptionsVid_t videoOptions;
	OptionsAud_t audioOptions;
	std::map<std::string, int> dinputMap;
	std::map<std::string, std::vector<EmulatorInput_t>> _keyBindings;
private:
	Options();

	static Options _instance;
};

#endif