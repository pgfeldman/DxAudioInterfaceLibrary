#pragma once

#include "SampleSound.h"
#include <d3dx9.h>
#include <unordered_map>

using namespace std;

/**
 * @typedef	unordered_map <LPCWSTR, SampleSound*> SOUND_MAP
 *
 * @brief	Defines an alias representing the sound map. Based on the MSVC2010 projects contained with the DirectX distribution
 * 			XAudio2BasicSound - Voice definition etc
 * 			XAudio2Sound3D - 3D emitters
 */
typedef unordered_map <LPCWSTR, SampleSound*> SOUND_MAP;

/**
 * @class	BasicAudio
 *
 * @brief	Class to handle the creation and use of the XAudio2 sound engine. Usage is roughly as follows:
 * 			
 * 			BasicAudio *ba = new BasicAudio(); // create the instance
 *			ba->init(); // initialize
 *			ba->createSound(L"music", L"Wavs\\MusicMono.wav", 0); // create a sound from a file. In this case a WAV. There can be many of these.
 *			ba->getSoundByName(L"music")->start(); // get the instance to the sound and start(), stop(), run() etc.
 *			loop{
 *				// change some audio condition
 *				ba->playOnChannelVoice(voice, channelIndex); // play the voice on a specified channel or
 *				ba->play3DVoice(continuousSound);
 *				ba->run() // optional
 *			}
 *			ba->destroy();
 *
 * @author	Phil
 * @date	6/7/2013
 */
class BasicAudio
{
public:
	BasicAudio(void);
	~BasicAudio(void);

	void init();
	void run();
	SampleSound* createSound(LPCWSTR soundName, LPCWSTR strFilename, UINT loopCount);
	SampleSound* getSoundByName(LPCWSTR soundName){
		SOUND_MAP::const_iterator got = soundMap.find(soundName);
		if(got == soundMap.end()){
			return NULL;
		}
		return soundMap[soundName];
	};
	void destroy();

	IXAudio2 *getXaudioPtr(){return pXAudio2;}
	IXAudio2MasteringVoice* getMasterVoice(){return pMasteringVoice;}
	void printMatrixCoefficients();
	const FLOAT32* getMatrixCoefficients(){return dspSettings.pMatrixCoefficients;};
	const int getNumChannels(){return deviceDetails.OutputFormat.Format.nChannels;};
	

	void play3DVoice(X3DAUDIO_EMITTER *emitter, IXAudio2SourceVoice* voice);
	void play3DVoice(SampleSound* sound){play3DVoice(sound->getEmitter(), sound->getSourceVoice());};
	void play3DVoice(LPCWSTR soundName){
		SampleSound* ss = getSoundByName(soundName);
		play3DVoice(ss);
	};

	void clearChannelVoice(IXAudio2SourceVoice* voice, int channel);
	void playOnChannelVoice(IXAudio2SourceVoice* voice, int channel);
	void playOnChannelVoice(IXAudio2SourceVoice* voice, int channel, float volume);
	void addToChannelVoice(IXAudio2SourceVoice* voice, int channel, float volume);

protected:
	HRESULT hr;
	IXAudio2* pXAudio2;
	UINT32 flags;
	IXAudio2MasteringVoice* pMasteringVoice;
	bool initialized;
	SOUND_MAP soundMap;

	// 3D
	X3DAUDIO_LISTENER listener;
	XAUDIO2_DEVICE_DETAILS deviceDetails;
	X3DAUDIO_DSP_SETTINGS dspSettings;
	X3DAUDIO_HANDLE x3dAudioHandle;

	void initListener(X3DAUDIO_LISTENER *listener);
	void initDspSettings(X3DAUDIO_DSP_SETTINGS *ds, XAUDIO2_DEVICE_DETAILS *dd);
	void setSingleMatrixVal(FLOAT32 * mat, int size, int index, FLOAT32 val, FLOAT32 clearVal);
	void updateSingleMatrixVal(FLOAT32 * mat, int size, int index, FLOAT32 val);
};

/**
// End of BasicAudio.h
 */

