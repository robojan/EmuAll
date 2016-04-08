#ifndef AUDIOCONTEXT_H_
#define AUDIOCONTEXT_H_

#include <vector>

#include <AL/alc.h>

#include "AudioDevice.h"

class AudioContext {
public:
	class Attributes {
	public:
		Attributes();
		~Attributes();

		void Reset();

		Attributes &Frequency(int frequency);
		Attributes &Refresh(int frequency);
		Attributes &Sync(bool sync);
		Attributes &MonoSources(int numMonoSources);
		Attributes &StereoSources(int numStereoSources);
		void FinishAttributes();

		const int *GetAttrList() const;
	private:
		std::vector<int> _attributes;
	};

	AudioContext();
	AudioContext(AudioDevice &device, Attributes &attributes);
	AudioContext(AudioContext &other);
	~AudioContext();

	AudioContext &operator=(AudioContext &other);

	void MakeCurrent();
	static void ReleaseContext();
	void Process();
	void Suspend();

	// Listener
	void SetListenerGain(float gain);
	float GetListenerGain();
	void SetListenerPosition(float x, float y, float z);
	void GetListenerPosition(float &x, float &y, float &z);
	void SetListenerVelocity(float x, float y, float z);
	void GetListenerVelocity(float &x, float &y, float &z);
	void SetListenerOrientation(float x_at, float y_at, float z_at, float x_up, float y_up, float z_up);
	void GetListenerOrientation(float &x_at, float &y_at, float &z_at, float &x_up, float &y_up, float &z_up);


private:
	ALCcontext *_context;
	AudioDevice _device;
};

#endif