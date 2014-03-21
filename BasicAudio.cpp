#include "StdAfx.h"
#include "BasicAudio.h"
#include "WavSampleSound.h"

using namespace std;

/**
 * @fn	BasicAudio::BasicAudio(void)
 *
 * @brief	Default constructor. Makes sure that the initialized flag is set to false
 *
 * @author	Phil
 * @date	6/7/2013
 */
BasicAudio::BasicAudio(void)
{
	initialized = false;
}

/**
 * @fn	BasicAudio::~BasicAudio(void)
 *
 * @brief	Destructor. If the class is initialized, it will destroy the assets.
 *
 * @author	Phil
 * @date	6/7/2013
 */
BasicAudio::~BasicAudio(void)
{
	if(initialized)
		destroy();
}

/**
 * @fn	void BasicAudio::initListener(X3DAUDIO_LISTENER *listener)
 *
 * @brief	Initialises the listener, giving it a position and velocity of (0, 0, 0),
 * 			looking down the Z axis, with the +X axis as 'up". A full description of the 
 * 			X3DAUDIO_LISTENER structure is at:
 * 			http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.x3daudio.x3daudio_listener(v=vs.85).aspx
 *
 * @author	Phil
 * @date	6/7/2013
 *
 * @param [in,out]	listener	If non-null, the listener.
 */
void BasicAudio::initListener(X3DAUDIO_LISTENER *listener){
	listener->Position = D3DXVECTOR3( 0, 0, 0 );
	listener->OrientFront = D3DXVECTOR3( 0, 0, 1 );
	listener->OrientTop = D3DXVECTOR3( 0, 1, 0 );
	listener->Velocity = D3DXVECTOR3( 0, 0, 0 );
	listener->pCone = NULL;
}

/**
 * @fn	void BasicAudio::initDspSettings(X3DAUDIO_DSP_SETTINGS *ds, XAUDIO2_DEVICE_DETAILS *dd)
 *
 * @brief	Initialises the audio Digital Signal Processor (DSP). As a piece of trivia, it's this change that pretty much wrecked all 
 * 			the other 3rd party audio APIs such as OpenAL.
 *
 * @author	Phil
 * @date	6/7/2013
 *
 * @param [in,out]	*ds pointer to the X3DAUDIO_DSP_SETTINGS structure that will be filled out by this class. A full description of the
 * 					data structure is here: 
 * 					http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.x3daudio.x3daudio_dsp_settings(v=vs.85).aspx
 * @param [in,out]	*dd	pointer to the XAUDIO2_DEVICE_DETAILS Returned by IXAudio2::GetDeviceDetails. The structure is as follows (from XAudio2.h):
 * 					typedef struct XAUDIO2_DEVICE_DETAILS
 *					{
 *						WCHAR DeviceID[256];                // String identifier for the audio device.
 *						WCHAR DisplayName[256];             // Friendly name suitable for display to a human.
 *						XAUDIO2_DEVICE_ROLE Role;           // Roles that the device should be used for.
 *						WAVEFORMATEXTENSIBLE OutputFormat;  // The device's native PCM audio output format.
 *					} XAUDIO2_DEVICE_DETAILS;
 */
void BasicAudio::initDspSettings(X3DAUDIO_DSP_SETTINGS *ds, XAUDIO2_DEVICE_DETAILS *dd){
	FLOAT32 * matrix = new FLOAT32[dd->OutputFormat.Format.nChannels];
	ds->SrcChannelCount = 1; // [in] number of source channels, must equal number of channels in respective emitter
	ds->DstChannelCount = dd->OutputFormat.Format.nChannels; // [in] number of destination channels, must equal number of channels of the final mix
	ds->pMatrixCoefficients = matrix; // matrix coefficient table, receives an array representing the volume level used to send from source channel S to destination channel D, stored as pMatrixCoefficients[SrcChannelCount * D + S], must have at least SrcChannelCount*DstChannelCount elements

	ds->pDelayTimes = NULL;         // [inout] delay time array, receives delays for each destination channel in milliseconds, must have at least DstChannelCount elements (stereo final mix only)

	ds->LPFDirectCoefficient = 0; // [out] LPF direct-path coefficient
	ds->LPFReverbCoefficient = 0; // [out] LPF reverb-path coefficient
	ds->ReverbLevel = 0; // [out] reverb send level
	ds->DopplerFactor = 0; // [out] doppler shift factor, scales resampler ratio for doppler shift effect, where the effective frequency = DopplerFactor * original frequency
	ds->EmitterToListenerAngle = 0; // [out] emitter-to-listener interior angle, expressed in radians with respect to the emitter's front orientation

	ds->EmitterToListenerDistance = 0; // [out] distance in user-defined world units from the emitter base to listener position, always calculated
	ds->EmitterVelocityComponent = 0; // [out] component of emitter velocity vector projected onto emitter->listener vector in user-defined world units/second, calculated only for doppler
	ds->ListenerVelocityComponent = 0; // [out] component of listener velocity vector projected onto emitter->listener vector in user-defined world units/second, calculated only for doppler
}

/**
 * @fn	void BasicAudio::init()
 *
 * @brief	Initialises the audio engine
 *
 * @author	Phil
 * @date	6/7/2013
 */
void BasicAudio::init()
{
	//
	// Initialize XAudio2
	//
	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	pXAudio2 = NULL;

	flags = 0;

#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif

	if( FAILED( hr = XAudio2Create( &pXAudio2, flags ) ) )
	{
		wprintf( L"Failed to init XAudio2 engine: %#X\n", hr );
		CoUninitialize();
		return;
	}

	//
	// Create a mastering voice
	//
	pMasteringVoice = NULL;

	if( FAILED( hr = pXAudio2->CreateMasteringVoice( &pMasteringVoice ) ) )
	{
		wprintf( L"Failed creating mastering voice: %#X\n", hr );
		SAFE_RELEASE( pXAudio2 );
		CoUninitialize();
		return;
	}

	pXAudio2->GetDeviceDetails(0, &deviceDetails);
	DWORD channelMask = deviceDetails.OutputFormat.dwChannelMask;
	
	X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, x3dAudioHandle);

	initDspSettings(&dspSettings, &deviceDetails);
	initListener(&listener);

	initialized = true;
}

/**
 * @fn	void BasicAudio::printMatrixCoefficients()
 *
 * @brief	Print (Audio channel) matrix coefficients.
 *
 * @author	Phil
 * @date	6/7/2013
 */
void BasicAudio::printMatrixCoefficients(){
	char* labels[] = {"Left", "Right", "Center", "Subwoofer", "Left Back", "Right Back", "Left Side", "Right Side"};
	for(int i = 0; i < deviceDetails.OutputFormat.Format.nChannels; ++i){
		printf(" %s: %.3f\n", labels[i], dspSettings.pMatrixCoefficients[i] );
	}
}

/**
 * @fn	void BasicAudio::setSingleMatrixVal(FLOAT32 * mat, int size, int index, FLOAT32 val)
 *
 * @brief	clears the matrix to a given value and sets a unique value at a specified index
 *
 * @author	Phil
 * @date	6/7/2013
 *
 * @param [in,out]	pointer to the matrix.
 * @param	size	   	In-dimensional size of the matrix.
 * @param	index	   	Zero-based index of the unique value we want to write.
 * @param	val		   	The unique value.
 * @param	clearVal    The value the rest of the matrix is set to.
 */
void BasicAudio::setSingleMatrixVal(FLOAT32 * mat, int size, int index, FLOAT32 val, FLOAT32 clearVal){
	for(int i = 0; i < size; ++i){
		mat[i] = clearVal;
	}
	mat[index] = val;
}

void BasicAudio::updateSingleMatrixVal(FLOAT32 * mat, int size, int index, FLOAT32 val){

	mat[index] = val;
}

/**
 * @fn	void BasicAudio::play3DVoice(X3DAUDIO_EMITTER *emitter, IXAudio2SourceVoice* voice)
 *
 * @brief	Sets the output matrix to reflect the emitter's 3D position
 *
 * @author	Phil
 * @date	6/7/2013
 *
 * @param [in,out]	X3DAUDIO_EMITTER emitter pointer. For more information, see:
  * 				http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.x3daudio.x3daudio_emitter(v=vs.85).aspx
 * @param [in,out]	IXAudio2SourceVoice voice pointer. For more information, see:
 * 					http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.ixaudio2sourcevoice.ixaudio2sourcevoice(v=vs.85).aspx
 * 					
 */
void BasicAudio::play3DVoice(X3DAUDIO_EMITTER *emitter, IXAudio2SourceVoice* voice){
	fwprintf(stderr, L"emitter pos = (%.2f, %.2f, %.2f)\n", emitter->Position.x, emitter->Position.y, emitter->Position.z);
	X3DAudioCalculate(x3dAudioHandle, &listener, emitter,
		X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB,
		&dspSettings );
				
	if (voice){
		// Apply X3DAudio generated DSP settings to XAudio2
		voice->SetFrequencyRatio( dspSettings.DopplerFactor );
		voice->SetOutputMatrix( getMasterVoice(), 1, deviceDetails.OutputFormat.Format.nChannels, dspSettings.pMatrixCoefficients );
	}
}

/**
 * @fn	void BasicAudio::playOnChannelVoice(IXAudio2SourceVoice* voice, int channel)
 *
 * @brief	Play a voice exclusively on a single channel
 *
 * @author	Phil
 * @date	6/7/2013
 *
 * @param [in,out]	pointer to the IXAudio2SourceVoice voice. For more info, see:
 * 					http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.ixaudio2sourcevoice.ixaudio2sourcevoice(v=vs.85).aspx
 * @param	channel	an integer between zero and the number of channels supported by the audio device.
 */

void BasicAudio::playOnChannelVoice(IXAudio2SourceVoice* voice, int channel){

	if (voice){
		channel = abs(channel)%deviceDetails.OutputFormat.Format.nChannels; // make sure that we can't go outside the range of channels.
		setSingleMatrixVal(dspSettings.pMatrixCoefficients, deviceDetails.OutputFormat.Format.nChannels, channel, 1.0, 0.0);
		voice->SetOutputMatrix( getMasterVoice(), 1, deviceDetails.OutputFormat.Format.nChannels, dspSettings.pMatrixCoefficients );
	}
}

void BasicAudio::playOnChannelVoice(IXAudio2SourceVoice* voice, int channel, float volume){

	if (voice){
		channel = abs(channel)%deviceDetails.OutputFormat.Format.nChannels; // make sure that we can't go outside the range of channels.
		setSingleMatrixVal(dspSettings.pMatrixCoefficients, deviceDetails.OutputFormat.Format.nChannels, channel, volume, 0.0);
		voice->SetOutputMatrix( getMasterVoice(), 1, deviceDetails.OutputFormat.Format.nChannels, dspSettings.pMatrixCoefficients );
	}
}

void BasicAudio::addToChannelVoice(IXAudio2SourceVoice* voice, int channel, float volume){

	if (voice){
		channel = abs(channel)%deviceDetails.OutputFormat.Format.nChannels; // make sure that we can't go outside the range of channels.
		updateSingleMatrixVal(dspSettings.pMatrixCoefficients, deviceDetails.OutputFormat.Format.nChannels, channel, volume);
		voice->SetOutputMatrix( getMasterVoice(), 1, deviceDetails.OutputFormat.Format.nChannels, dspSettings.pMatrixCoefficients );
	}
}

void BasicAudio::clearChannelVoice(IXAudio2SourceVoice* voice, int channel){

	if (voice){
		channel = abs(channel)%deviceDetails.OutputFormat.Format.nChannels; // make sure that we can't go outside the range of channels.
		setSingleMatrixVal(dspSettings.pMatrixCoefficients, deviceDetails.OutputFormat.Format.nChannels, channel, 0.0, 0.0);
		voice->SetOutputMatrix( getMasterVoice(), 1, deviceDetails.OutputFormat.Format.nChannels, dspSettings.pMatrixCoefficients );
	}
}

/**
 * @fn	SampleSound* BasicAudio::createSound(LPCWSTR soundName, LPCWSTR strFilename,
 * 		UINT loopCount)
 *
 * @brief	Creates a sound from a WAV file. The sound may have zero or more loops (0 = one play thorough - no loops) up to 
 * 			XAUDIO2_LOOP_INFINITE (XAudio2.h). The sound is associated in an unordered map with a name.
 *
 * @author	Phil
 * @date	6/7/2013
 *
 * @param	soundName  	Name of the sound.
 * @param	strFilename	Filename for the sound
 * @param	loopCount  	Number of loops.
 *
 * @return	pointer to the new sound
 */
SampleSound* BasicAudio::createSound(LPCWSTR soundName, LPCWSTR strFilename, UINT loopCount){
	WavSampleSound *newSound = new WavSampleSound();
	
	newSound->initPCM(pXAudio2, strFilename, loopCount );
	newSound->setName(soundName);
	soundMap[newSound->getName()] = newSound;

	return newSound;
}

/**
 * @fn	void BasicAudio::run()
 *
 * @brief	Calls run() on all the sound objects.
 *
 * @author	Phil
 * @date	6/7/2013
 */
void BasicAudio::run(){
	SampleSound *ss;
	SOUND_MAP::iterator it = soundMap.begin();
	while(it != soundMap.end()){
		ss = it->second;
		ss->run();
		it++;
	}
}

/**
 * @fn	void BasicAudio::destroy()
 *
 * @brief	Destroys this object and all the sounds that belong to it.
 *
 * @author	Phil
 * @date	6/7/2013
 */
void BasicAudio::destroy(){
	// All XAudio2 interfaces are released when the engine is destroyed, but being tidy

	SampleSound *ss;
	SOUND_MAP::iterator it = soundMap.begin();
	while(it != soundMap.end()){
		wprintf(L"Destroying %s\n", it->first);
		ss = it->second;
		ss->destroy();
		it++;
	}
	pMasteringVoice->DestroyVoice();

	SAFE_RELEASE( pXAudio2 );
	CoUninitialize();
	initialized = false;
}