#include "StdAfx.h"
#include "WavSampleSound.h"
#include <stdio.h>

/**
 * @fn	WavSampleSound::WavSampleSound(void)
 *
 * @brief	Default constructor. Initializes the playback buffers and sets states to 
 * 			ensure correct loading and playback
 *
 * @author	Phil
 * @date	6/7/2013
 */

WavSampleSound::WavSampleSound(void)
{
	creationComplete = false;
	isRunning = false;

	buffer.Flags = 0;                       // Either 0 or XAUDIO2_END_OF_STREAM.
	buffer.AudioBytes = 0;                  // Size of the audio data buffer in bytes.
	buffer.pAudioData = NULL;               // Pointer to the audio data buffer.
	buffer.PlayBegin = 0;                   // First sample in this buffer to be played.
	buffer.PlayLength = 0;                  // Length of the region to be played in samples,
											//  or 0 to play the whole buffer.
	buffer.LoopBegin = 0;                   // First sample of the region to be looped.
	buffer.LoopLength = 0;                  // Length of the desired loop region in samples,
											//  or 0 to loop the entire buffer.
	buffer.LoopCount = 0;                   // Number of times to repeat the loop region,
											//  or XAUDIO2_LOOP_INFINITE to loop forever.
	buffer.pContext = NULL;                 // Context value to be passed back in callbacks.
}

/**
 * @fn	WavSampleSound::~WavSampleSound(void)
 *
 * @brief	Destructor. Destroys the buffers as needed.
 *
 * @author	Phil
 * @date	6/7/2013
 */

WavSampleSound::~WavSampleSound(void)
{
	destroy();

}


/**
 * @fn	HRESULT WavSampleSound::initPCM( IXAudio2* pXaudio2, LPCWSTR szFilename, UINT loopCount )
 *
 * @brief	Read in a sound file and attach that sound as a source voice to the IXAudio2 pointer
 *
 * @author	Phil
 * @date	6/7/2013
 *
 * @param [in,out]	pXaudio2	The pointer to the IXAudio2 described in detail here: 
 * 					http://msdn.microsoft.com/en-us/library/windows/desktop/ee415813(v=vs.85).aspx
 * @param	szFilename			Name of the file.
 * @param	loopCount			Number of times to loop through the loop region. This value can be between 0 and 
 * 								XAUDIO2_MAX_LOOP_COUNT. To loop forever, set LoopCount to XAUDIO2_LOOP_INFINITE.
 *
 * @return	pointer to the resulting voice or an error.
 */
HRESULT WavSampleSound::initPCM( IXAudio2* pXaudio2, LPCWSTR szFilename, UINT loopCount )
{
	HRESULT hr = S_OK;
	setFileName(szFilename);
	//
	// Locate the wave file
	//
	WCHAR strFilePath[MAX_PATH];
	if( FAILED( hr = FindMediaFileCch( strFilePath, MAX_PATH, szFilename ) ) )
	{
		fwprintf(stderr, L"Failed to find media file: %s\n", szFilename );
		return hr;
	}

	//
	// Read in the wave file
	//
	if( FAILED( hr = wav.Open( strFilePath, NULL, WAVEFILE_READ ) ) )
	{
		fwprintf(stderr, L"Failed reading WAV file: %#X (%s)\n", hr, strFilePath );
		return hr;
	}

	// Get format of wave file
	WAVEFORMATEX* pwfx = wav.GetFormat();

	// Calculate how many bytes and samples are in the wave
	cbWaveSize = wav.GetSize();

	// Read the sample data into memory
	pbWaveData = new BYTE[ cbWaveSize ];

	if( FAILED( hr = wav.Read( pbWaveData, cbWaveSize, &cbWaveSize ) ) )
	{
		fwprintf(stderr, L"Failed to read WAV data: %#X\n", hr );
		SAFE_DELETE_ARRAY( pbWaveData );
		return hr;
	}

	//
	// Play the wave using a XAudio2SourceVoice
	//

	// Create the source voice
	if( FAILED( hr = pXaudio2->CreateSourceVoice( &pSourceVoice, pwfx ) ) )
	{
		fwprintf(stderr, L"Error %#X creating source voice\n", hr );
		SAFE_DELETE_ARRAY( pbWaveData );
		return hr;
	}

	// Submit the wave sample data using an XAUDIO2_BUFFER structure
	buffer.pAudioData = pbWaveData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	buffer.AudioBytes = cbWaveSize;
	buffer.LoopCount = loopCount; 

	creationComplete = true;
	isRunning = false;
	return hr;
}

/**
 * @fn	HRESULT WavSampleSound::start()
 *
 * @brief	Start playing the sound if it's not already playing
 *
 * @author	Phil
 * @date	6/7/2013
 *
 * @return	S_OK (0) or S_FAILED (-1), depending on success.
 */

HRESULT WavSampleSound::start(){
	if(!creationComplete){
		return S_FAILED;
	}
	HRESULT hr = S_OK;

	// Let the sound play
	if(!isRunning){
		if( FAILED( hr = pSourceVoice->SubmitSourceBuffer( &buffer ) ) )
		{
			fwprintf(stderr, L"Error %#X submitting source buffer\n", hr );
			pSourceVoice->DestroyVoice();
			SAFE_DELETE_ARRAY( pbWaveData );
			creationComplete = false;
			return hr;
		}
		hr = pSourceVoice->Start( 0 );
		isRunning = true;
		fwprintf(stderr, L"WavSampleSound::stop(): starting playing %s\n", getFileName());
	}

	return hr;
}

/**
 * @fn	void WavSampleSound::stop()
 *
 * @brief	Stops the sound playing
 *
 * @author	Phil
 * @date	6/7/2013
 */

void WavSampleSound::stop(){
	if(creationComplete && isRunning){
		pSourceVoice->Stop( 0 );
		isRunning = false;
		fwprintf(stderr, L"WavSampleSound::stop(): stopped playing %s\n", getFileName());
	}
}

/**
 * @fn	HRESULT WavSampleSound::run()
 *
 * @brief	Do periodic checks on the playing sound. Currently all this does is to check if the sound has stopped playing.
 * 			This means that if you want to not sample the state at all, simply call run once before you want to play the 
 * 			sound again.
 *
 * @author	Phil
 * @date	6/7/2013
 *
 * @return	.
 */
HRESULT WavSampleSound::run(){
	if(isRunning && creationComplete)
	{
		XAUDIO2_VOICE_STATE state;
		pSourceVoice->GetState( &state );
		isRunning = ( state.BuffersQueued > 0 ) != 0;
		if(!isRunning){
			fwprintf(stderr, L"finished playing %s\n", getName());
			 //pSourceVoice->SubmitSourceBuffer( &buffer );
		}
	}

	return S_OK;
}

/**
 * @fn	void WavSampleSound::destroy()
 *
 * @brief	Clears out the wav data and sets the flags so that we know this 
 * 			sound can't be played.
 *
 * @author	Phil
 * @date	6/7/2013
 */

void WavSampleSound::destroy(){
	if(creationComplete){
		pSourceVoice->DestroyVoice();
		SAFE_DELETE_ARRAY( pbWaveData );
	}
	creationComplete = false;
}


//--------------------------------------------------------------------------------------
// Helper function to try to find the location of a media file
//--------------------------------------------------------------------------------------

/**
 * @fn	HRESULT WavSampleSound::FindMediaFileCch( WCHAR* strDestPath, int cchDest,
 * 		LPCWSTR strFilename )
 *
 * @brief	Search for the first media file that matches the parameters
 *
 * @author	Phil
 * @date	6/7/2013
 *
 * @param [in,out]	strDestPath	If non-null, full pathname of the destination file.
 * @param	cchDest			   	length of characters that can be in the path
 * @param	strFilename		   	Name of the file.
 *
 * @return	Pointer to the file or an error message.
 */

HRESULT WavSampleSound::FindMediaFileCch( WCHAR* strDestPath, int cchDest, LPCWSTR strFilename )
{
	bool bFound = false;

	if( NULL == strFilename || strFilename[0] == 0 || NULL == strDestPath || cchDest < 10 )
		return E_INVALIDARG;

	// Get the exe name, and exe path
	WCHAR strExePath[MAX_PATH] = {0};
	WCHAR strExeName[MAX_PATH] = {0};
	WCHAR* strLastSlash = NULL;
	GetModuleFileName( NULL, strExePath, MAX_PATH );
	strExePath[MAX_PATH - 1] = 0;
	strLastSlash = wcsrchr( strExePath, TEXT( '\\' ) );
	if( strLastSlash )
	{
		wcscpy_s( strExeName, MAX_PATH, &strLastSlash[1] );

		// Chop the exe name from the exe path
		*strLastSlash = 0;

		// Chop the .exe from the exe name
		strLastSlash = wcsrchr( strExeName, TEXT( '.' ) );
		if( strLastSlash )
			*strLastSlash = 0;
	}

	wcscpy_s( strDestPath, cchDest, strFilename );
	if( GetFileAttributes( strDestPath ) != 0xFFFFFFFF )
		return S_OK;

	// Search all parent directories starting at .\ and using strFilename as the leaf name
	WCHAR strLeafName[MAX_PATH] = {0};
	wcscpy_s( strLeafName, MAX_PATH, strFilename );

	WCHAR strFullPath[MAX_PATH] = {0};
	WCHAR strFullFileName[MAX_PATH] = {0};
	WCHAR strSearch[MAX_PATH] = {0};
	WCHAR* strFilePart = NULL;

	GetFullPathName( L".", MAX_PATH, strFullPath, &strFilePart );
	if( strFilePart == NULL )
		return E_FAIL;

	while( strFilePart != NULL && *strFilePart != '\0' )
	{
		swprintf_s( strFullFileName, MAX_PATH, L"%s\\%s", strFullPath, strLeafName );
		if( GetFileAttributes( strFullFileName ) != 0xFFFFFFFF )
		{
			wcscpy_s( strDestPath, cchDest, strFullFileName );
			bFound = true;
			break;
		}

		swprintf_s( strFullFileName, MAX_PATH, L"%s\\%s\\%s", strFullPath, strExeName, strLeafName );
		if( GetFileAttributes( strFullFileName ) != 0xFFFFFFFF )
		{
			wcscpy_s( strDestPath, cchDest, strFullFileName );
			bFound = true;
			break;
		}

		swprintf_s( strSearch, MAX_PATH, L"%s\\..", strFullPath );
		GetFullPathName( strSearch, MAX_PATH, strFullPath, &strFilePart );
	}
	if( bFound )
		return S_OK;

	// On failure, return the file as the path but also return an error code
	wcscpy_s( strDestPath, cchDest, strFilename );

	return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );
}
