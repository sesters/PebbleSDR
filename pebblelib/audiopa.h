#pragma once
//GPL license and attributions are in gpl.h and terms are included in this file by reference
#include "gpl.h"
#include "audio.h"
#include "../portaudio/include/portaudio.h"

using namespace std;

class Receiver; //Forward declaration since soundcard and receiver are dependent on each other

class AudioPA:public Audio
{
public:
	AudioPA(CB_AudioProducer cb, int fpb);
	~AudioPA(void);
	//Virtual functions
	//We may get input from some other source, if so inputSampleRate = 0
    int StartInput(QString inputDeviceName, int inputSampleRate);
    int StartOutput(QString outputDeviceName, int outputSampleRate);
	int Stop();
	int Flush();
	int Pause();
	int Restart();
    void SendToOutput(CPX *, int outSamples, float gain = 1.0, bool mute = false);
	void ClearCounts();

    //Return device index for matching device
    int FindDeviceByName(QString name, bool inputDevice);

	//Returns a list of input devices for settings to chose from
    static QStringList InputDeviceList();
    static QStringList OutputDeviceList();
    static int DefaultOutputDevice();
	static int DefaultInputDevice();


private:

    static QStringList DeviceList(bool input);

	//Input may come from another source, so we manage these streams separately
	PaStream *inStream;
	PaStream *outStream;
	float *outStreamBuffer; 
	PaSampleFormat sampleFormat;
	PaError error;
	static int streamCallback(const void *input, void *output,
		unsigned long numSamples,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData );
	static QMutex mutex;
};
