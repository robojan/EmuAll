#include "memDbg.h"

#include <string>
#include <cassert>
#include "Options.h"

Options::Options()
{
	videoOptions.keepAspect = DEFAULT_KEEPASPECT;
	audioOptions.sampleRate = DEFAULT_SAMPLERATE;
	audioOptions.bufferSize = DEFAULT_BUFFERSIZE;
}

Options::~Options()
{

}

void Options::LoadOptions()
{

}

void Options::SaveOptions()
{

}

void Options::AddKeyBinding(std::string name, const EmulatorInput_t &keyInfo)
{
	int key = keyInfo.defaultKey;
	mKeyBindings[name][key] = keyInfo;
}

void Options::RebindKey(std::string name, int emuKey, int inputKey)
{
	assert(mKeyBindings.find(name) != mKeyBindings.end());
	std::map<int, EmulatorInput_t>::iterator it;
	EmulatorInput_t reboundEmuInput;
	int oldKey = 0;
	// Search for the keybinding
	for (it = mKeyBindings[name].begin(); it != mKeyBindings[name].end(); ++it)
	{
		if (it->second.key == emuKey)
		{
			reboundEmuInput = it->second;
			oldKey = it->first;
			// Remove binding from the list
			mKeyBindings[name].erase(it);
			break;
		}
	}

	if ((it = mKeyBindings[name].find(inputKey)) != mKeyBindings[name].end())
	{
		// Key already exists in rebindings
		EmulatorInput_t overWriteInput = it->second;
		it->second = reboundEmuInput;
		mKeyBindings[name][oldKey] = overWriteInput;
	}
	else {
		mKeyBindings[name][inputKey] = reboundEmuInput;
	}
}

int Options::GetKeyBinding(std::string name, int key, int defaultKey)
{
	if (mKeyBindings.find(name) == mKeyBindings.end() || mKeyBindings[name].find(key) == mKeyBindings[name].end())
	{
		return defaultKey;
	}
	return mKeyBindings[name][key].key;
}

int Options::GetInputKeyBinding(std::string name, int key, int defaultKey)
{
	if (mKeyBindings.find(name) == mKeyBindings.end())
		return defaultKey;

	std::map<int, EmulatorInput_t>::iterator it;
	// Search for the keybinding
	for (it = mKeyBindings[name].begin(); it != mKeyBindings[name].end(); ++it)
	{
		if (it->second.key == key)
		{
			return it->first;
		}
	}
	return defaultKey;
}


const EmulatorInput_t *Options::GetKeyBindingInfo(std::string name, int key)
{
	if (mKeyBindings.find(name) == mKeyBindings.end() || mKeyBindings[name].find(key) == mKeyBindings[name].end())
	{
		return NULL;
	}
	return &mKeyBindings[name][key];
}