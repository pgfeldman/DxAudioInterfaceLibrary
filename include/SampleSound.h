/**
 * @file	SampleSound.h
 *
 * @brief	Mostly abstract class that wraps most ofthe functionality of a source voice. Based on the MSVC2010 projects contained with the DirectX distribution
 * 			XAudio2BasicSound - Voice definition etc
 * 			XAudio2Sound3D - 3D emitters
 */

#pragma once

#include <windows.h>
#include <XAudio2.h>
#include <X3DAudio.h>
#include <d3dx9.h>
#include <string>
#include "SDKwavefile.h"

#ifndef S_FAILED
#define S_FAILED ((HRESULT)(-1L))
#endif

#ifndef INPUTCHANNELS
#define INPUTCHANNELS 1  // number of source channels
#endif

using namespace std;

class SampleSound
{
public:

	/**
	 * @fn	SampleSound::SampleSound()
	 *
	 * @brief	Default constructor. Initializes Emitter_Reverb_CurvePoints array and the pointer to these points
	 * 				
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 */
	SampleSound(){

		/**
		 * @summary	Specify reverb send level distance curve such that reverb send increases
		 *			slightly with distance before rolling off to silence.
		 *			With the direct channels being increasingly attenuated with distance,
		 *			this has the effect of increasing the reverb-to-direct sound ratio,
		 *			reinforcing the perception of distance..
		 */

		Emitter_Reverb_CurvePoints[0].Distance = 0.0f;
		Emitter_Reverb_CurvePoints[0].DSPSetting = 0.5f;
		Emitter_Reverb_CurvePoints[1].Distance = 0.75f;
		Emitter_Reverb_CurvePoints[1].DSPSetting = 1.0f;
		Emitter_Reverb_CurvePoints[2].Distance = 1.0f;
		Emitter_Reverb_CurvePoints[2].DSPSetting = 0.0f;

		Emitter_Reverb_Curve.pPoints = (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_Reverb_CurvePoints[0];
		Emitter_Reverb_Curve.PointCount = 3; 

		initEmitter();

		filename.clear();
	};

	~SampleSound(){};

	/**
	 * @fn	virtual HRESULT SampleSound::initPCM( IXAudio2* pXaudio2, LPCWSTR szFilename,
	 * 		UINT loopCount ) = 0;
	 *
	 * @brief	Implement code that will read in a sound file and attach that sound as a source voice to the IXAudio2 pointer
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
	 * @return	.
	 */
	virtual HRESULT initPCM( IXAudio2* pXaudio2, LPCWSTR szFilename, UINT loopCount ) = 0;

	/**
	 * @fn	virtual HRESULT SampleSound::start() = 0;
	 *
	 * @brief	Start playing the sound
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	.
	 */
	virtual HRESULT start() = 0;

	/**
	 * @fn	virtual void SampleSound::stop() = 0;
	 *
	 * @brief	Stops playing the sound
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 */
	virtual void stop() = 0;

	/**
	 * @fn	virtual HRESULT SampleSound::run() = 0;
	 *
	 * @brief	Perform periodic checks on the sound
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	.
	 */
	virtual HRESULT run() = 0;

	/**
	 * @fn	virtual void SampleSound::destroy() = 0;
	 *
	 * @brief	Destroys the sound data
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 */
	virtual void destroy() = 0;

	/**
	 * @fn	void SampleSound::setFileName(LPCWSTR wstr)
	 *
	 * @brief	Sets the file name.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	void
	 */
	void setFileName(LPCWSTR wstr) {
		filename.assign(wstr);
		cFilename.assign(filename.begin(), filename.end());
	}

	void setName(LPCWSTR wstr) {
		name.assign(wstr);
		cName.assign(name.begin(), name.end());
	}

	/**
	 * @fn	LPCWSTR SampleSound::getFileName()
	 *
	 * @brief	Gets the file name.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	The file name.
	 */
	LPCWSTR getFileName() {
		if(filename.length() > 0){
			return filename.c_str();
		}
		return NULL;
	}

	string getCFileName() {
		if(cFilename.length() > 0){
			return cFilename;
		}
		return NULL;
	}

	LPCWSTR getName() {
		if(name.length() > 0){
			return name.c_str();
		}
		return NULL;
	}

	string getCName() {
		if(cName.length() > 0){
			return cName;
		}
		return NULL;
	}

	/**
	 * @fn	IXAudio2SourceVoice* SampleSound::getSourceVoice()
	 *
	 * @brief	Gets the pointer to the source voice. Discussed in detail here: 
	 * 			http://msdn.microsoft.com/en-us/library/windows/desktop/ee415825(v=vs.85).aspx
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	null if it fails, else the source voice.
	 */
	IXAudio2SourceVoice* getSourceVoice() {return pSourceVoice;}

	/**
	 * @fn	X3DAUDIO_EMITTER* SampleSound::getEmitter()
	 *
	 * @brief	Gets the 3D audio emitter. The structure of the emitter is here: 
	 * 			http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.x3daudio.x3daudio_emitter(v=vs.85).aspx
	 * 			an overview of 3D audio is here:
	 * 			http://msdn.microsoft.com/en-us/library/windows/desktop/ee415717(v=vs.85).aspx
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	null if it fails, else the emitter.
	 */
	X3DAUDIO_EMITTER* getEmitter() {return &emitter;};

	/**
	 * @fn	FLOAT32 SampleSound::getEmitterX()
	 *
	 * @brief	Gets the emitter x coordinate.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	The emitter x coordinate.
	 */
	FLOAT32 getEmitterX(){return emitter.Position.x;};

	/**
	 * @fn	FLOAT32 SampleSound::getEmitterY()
	 *
	 * @brief	Gets the emitter y coordinate.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	The emitter y coordinate.
	 */
	FLOAT32 getEmitterY(){return emitter.Position.y;};

	/**
	 * @fn	FLOAT32 SampleSound::getEmitterZ()
	 *
	 * @brief	Gets the emitter z coordinate.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	The emitter z coordinate.
	 */
	FLOAT32 getEmitterZ(){return emitter.Position.z;};

	/**
	 * @fn	void SampleSound::setEmitterX(FLOAT32 x)
	 *
	 * @brief	Sets an emitter x coordinate.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @param	x	The FLOAT32 to process.
	 */
	void setEmitterX(FLOAT32 x) {emitter.Position.x = x;};

	/**
	 * @fn	void SampleSound::setEmitterY(FLOAT32 y)
	 *
	 * @brief	Sets an emitter y coordinate.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @param	y	The FLOAT32 to process.
	 */
	void setEmitterY(FLOAT32 y) {emitter.Position.y = y;};

	/**
	 * @fn	void SampleSound::setEmitterZ(FLOAT32 z)
	 *
	 * @brief	Sets an emitter z coordinate.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @param	z	The FLOAT32 to process.
	 */
	void setEmitterZ(FLOAT32 z) {emitter.Position.z = z;};

	/**
	 * @fn	void SampleSound::setEmitterPos(FLOAT32 x, FLOAT32 y, FLOAT32 z)
	 *
	 * @brief	Sets an emitter position.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @param	x	The emitter x coordinate.
	 * @param	y	The emitter y coordinate.
	 * @param	z	The emitter z coordinate.
	 */
	void setEmitterPos(FLOAT32 x, FLOAT32 y, FLOAT32 z){
		emitter.Position.x = x; 
		emitter.Position.y = y;
		emitter.Position.z = z;
	};

	/**
	 * @fn	FLOAT32 SampleSound::getEmitterVX()
	 *
	 * @brief	Gets the emitter x velocity. Used for doppler shift effects
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	The emitter x velocity.
	 */
	FLOAT32 getEmitterVX(){return emitter.Velocity.x;};

	/**
	 * @fn	FLOAT32 SampleSound::getEmitterVY()
	 *
	 * @brief	Gets the emitter y velocity. Used for doppler shift effects
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	The emitter y velocity.
	 */
	FLOAT32 getEmitterVY(){return emitter.Velocity.y;};

	/**
	 * @fn	FLOAT32 SampleSound::getEmitterVZ()
	 *
	 * @brief	Gets the emitter z velocity. Used for doppler shift effects
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @return	The emitter z velocity.
	 */
	FLOAT32 getEmitterVZ(){return emitter.Velocity.z;};

	/**
	 * @fn	void SampleSound::setEmitterVX(FLOAT32 x)
	 *
	 * @brief	Sets the emitter x velocity. Used for doppler shift effects
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @param	x	The emitter x velocity.
	 */
	void setEmitterVX(FLOAT32 x) {emitter.Velocity.x = x;};

	/**
	 * @fn	void SampleSound::setEmitterVY(FLOAT32 y)
	 *
	 * @brief	Sets the emitter y velocity. Used for doppler shift effects
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @param	y	The emitter y velocity.
	 */
	void setEmitterVY(FLOAT32 y) {emitter.Velocity.y = y;};

	/**
	 * @fn	void SampleSound::setEmitterVZ(FLOAT32 z)
	 *
	 * @brief	Sets the emitter z velocity. Used for doppler shift effects
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @param	z	The emitter z velocity.
	 */
	void setEmitterVZ(FLOAT32 z) {emitter.Velocity.z = z;};

	/**
	 * @fn	void SampleSound::setEmitterVelocity(FLOAT32 x, FLOAT32 y, FLOAT32 z)
	 *
	 * @brief	Sets the emitter velocity.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 *
	 * @param	x	The emitter x velocity.
	 * @param	y	The emitter y velocity.
	 * @param	z	The emitter z velocity.
	 */
	void setEmitterVelocity(FLOAT32 x, FLOAT32 y, FLOAT32 z){
		emitter.Velocity.x = x; 
		emitter.Velocity.y = y;
		emitter.Velocity.z = z;
	};

	
	
protected:
	wstring filename;
	string cFilename;
	wstring name;
	string cName;
	IXAudio2* pXaudio2;
	IXAudio2SourceVoice* pSourceVoice;
	XAUDIO2_BUFFER buffer;
	bool creationComplete;
	BOOL isRunning;

	// 3D variables
	D3DXVECTOR3 g_vEmitterPos;
	FLOAT32 g_emitterAzimuths[INPUTCHANNELS];
	X3DAUDIO_EMITTER emitter;

	X3DAUDIO_DISTANCE_CURVE_POINT Emitter_Reverb_CurvePoints[3];
	X3DAUDIO_DISTANCE_CURVE       Emitter_Reverb_Curve;

	/**
	 * @fn	virtual HRESULT SampleSound::FindMediaFileCch( WCHAR* strDestPath, int cchDest,
	 * 		LPCWSTR strFilename ) = 0;
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
	 * @return	Pointer to the file or an error message
	 */
	virtual HRESULT FindMediaFileCch( WCHAR* strDestPath, int cchDest, LPCWSTR strFilename ) = 0;

	/**
	 * @fn	void SampleSound::initEmitter()
	 *
	 * @brief	Initialises the 3D emitter.
	 *
	 * @author	Phil
	 * @date	6/7/2013
	 */
	void initEmitter(){
		emitter.ChannelCount = 1;
		emitter.CurveDistanceScaler = FLT_MIN;
		emitter.pCone = NULL;

		emitter.Velocity = D3DXVECTOR3( 0, 0, 0 );
		emitter.Position = D3DXVECTOR3( 0, 0, 0 );
		emitter.OrientFront = D3DXVECTOR3( 0, 0, 1 );
		emitter.OrientTop = D3DXVECTOR3( 0, 1, 0 );
		emitter.ChannelCount = 1;
		emitter.ChannelRadius = 1.0f;
		emitter.pChannelAzimuths = g_emitterAzimuths;

		// Use of Inner radius allows for smoother transitions as
		// a sound travels directly through, above, or below the listener.
		// It also may be used to give elevation cues.
		emitter.InnerRadius = 2.0f;
		emitter.InnerRadiusAngle = X3DAUDIO_PI/4.0f;;

		emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE*)&X3DAudioDefault_LinearCurve;
		emitter.pReverbCurve    = (X3DAUDIO_DISTANCE_CURVE*)&Emitter_Reverb_Curve;
		emitter.CurveDistanceScaler = 14.0f;
		emitter.DopplerScaler = 1.0f;

		emitter.pLFECurve = NULL;
		emitter.pLPFDirectCurve = NULL;
		emitter.pLPFReverbCurve = NULL;
	};


};



