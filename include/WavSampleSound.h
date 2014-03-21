#pragma once

#include "SampleSound.h"

/**
 * @class	WavSampleSound
 *
 * @brief	WAV format implementation of the SampleSound abstract class.
 *
 * @author	Phil
 * @date	6/7/2013
 */

class WavSampleSound : public SampleSound
{
public:
	WavSampleSound(void);
	~WavSampleSound(void);

	HRESULT initPCM( IXAudio2* pXaudio2, LPCWSTR szFilename, UINT loopCount );
	
	HRESULT start();
	void stop();
	HRESULT run();
	void destroy();

protected:

	DWORD cbWaveSize;
	CWaveFile wav;
	BYTE* pbWaveData;

	HRESULT FindMediaFileCch( WCHAR* strDestPath, int cchDest, LPCWSTR strFilename );
};

/**
// End of WavSampleSound.h
 */

