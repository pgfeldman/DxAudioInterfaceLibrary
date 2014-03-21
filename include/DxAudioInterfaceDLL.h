#include "BasicAudio.h"

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DXAUDIOINTERFACEDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DXAUDIOINTERFACEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DXAUDIOINTERFACEDLL_EXPORTS
#define DXAUDIOINTERFACEDLL_API __declspec(dllexport)
#else
#define DXAUDIOINTERFACEDLL_API __declspec(dllimport)
#endif



// This class is exported from the DxAudioInterfaceDLL.dll

/**
 * @class	CDxAudioInterfaceDLL
 *
 * @brief	Dx audio interface dll.
 *
 * @author	Phil
 * @date	6/10/2013
 */

class DXAUDIOINTERFACEDLL_API CDxAudioInterfaceDLL {
public:

	/**
	 * @fn	CDxAudioInterfaceDLL::CDxAudioInterfaceDLL(void)
	 *
	 * @brief	Default constructor.
	 *
	 * @author	Phil
	 * @date	6/10/2013
	 */

	CDxAudioInterfaceDLL(void){ 
		ba = NULL;	
	};

	/**
	 * @fn	CDxAudioInterfaceDLL::~CDxAudioInterfaceDLL(void)
	 *
	 * @brief	Destructor.
	 *
	 * @author	Phil
	 * @date	6/10/2013
	 */

	~CDxAudioInterfaceDLL(void){};

	/**
	 * @fn	void CDxAudioInterfaceDLL::init()
	 *
	 * @brief	Initialises this object.
	 *
	 * @author	Phil
	 * @date	6/10/2013
	 */

	void init(){
		//if(ba != NULL){
			ba = new BasicAudio();
			ba->init();
		//}
	};

	/**
	 * @fn	void CDxAudioInterfaceDLL::createSound(LPCWSTR soundName, LPCWSTR strFilename,
	 * 		UINT loopCount)
	 *
	 * @brief	Creates a sound.
	 *
	 * @author	Phil
	 * @date	6/10/2013
	 *
	 * @param	soundName  	Name of the sound.
	 * @param	strFilename	Filename of the file.
	 * @param	loopCount  	Number of loops.
	 */

	void createSound(LPCWSTR soundName, LPCWSTR strFilename, UINT loopCount){
		if(ba == NULL)
			return;
		ba->createSound(soundName, strFilename, loopCount);
	};

	/**
	 * @fn	void CDxAudioInterfaceDLL::startSound(LPCWSTR soundName)
	 *
	 * @brief	Starts a sound.
	 *
	 * @author	Phil
	 * @date	6/10/2013
	 *
	 * @param	soundName	Name of the sound.
	 */

	void startSound(LPCWSTR soundName) {
		if(ba == NULL)
			return;
		ba->getSoundByName(soundName)->start();
	};

	/**
	 * @fn	void CDxAudioInterfaceDLL::stopSound(LPCWSTR soundName)
	 *
	 * @brief	Stops a sound.
	 *
	 * @author	Phil
	 * @date	6/10/2013
	 *
	 * @param	soundName	Name of the sound.
	 */

	void stopSound(LPCWSTR soundName) {
		if(ba == NULL)
			return;
		ba->getSoundByName(soundName)->stop();
	};

	/**
	 * @fn	void CDxAudioInterfaceDLL::play3DVoice(LPCWSTR soundName)
	 *
	 * @brief	Play 3 d voice.
	 *
	 * @author	Phil
	 * @date	6/10/2013
	 *
	 * @param	soundName	Name of the sound.
	 */

	void play3DVoice(LPCWSTR soundName){
		if(ba == NULL)
			return;
		ba->play3DVoice(ba->getSoundByName(soundName));
	};

	/**
	 * @fn	void CDxAudioInterfaceDLL::playOnChannelVoice(LPCWSTR soundName, int channel)
	 *
	 * @brief	Play on channel voice.
	 *
	 * @author	Phil
	 * @date	6/10/2013
	 *
	 * @param	soundName	Name of the sound.
	 * @param	channel  	The channel.
	 */

	void playOnChannelVoice(LPCWSTR soundName, int channel){
		if(ba == NULL)
			return;
		SampleSound* ss = ba->getSoundByName(soundName);
		IXAudio2SourceVoice* voice = ss->getSourceVoice();
		ba->playOnChannelVoice(voice, channel);
	}

	/**
	 * @fn	void CDxAudioInterfaceDLL::run()
	 *
	 * @brief	Runs this object.
	 *
	 * @author	Phil
	 * @date	6/10/2013
	 */

	void run(){
		if(ba == NULL)
			return;
		ba->run();
	};

	FLOAT32 getEmitterX(LPCWSTR soundName){
		if(ba == NULL)
			return 0;
		SampleSound* ss = ba->getSoundByName(soundName);
		return ss->getEmitterX();
	};
	
	FLOAT32 getEmitterY(LPCWSTR soundName){
		if(ba == NULL)
			return 0;
		SampleSound* ss = ba->getSoundByName(soundName);
		return ss->getEmitterY();
	};

	FLOAT32 getEmitterZ(LPCWSTR soundName){
		if(ba == NULL)
			return 0;
		SampleSound* ss = ba->getSoundByName(soundName);
		return ss->getEmitterZ();
	};

	void setEmitterX(LPCWSTR soundName, FLOAT32 x) {
		if(ba == NULL)
			return;
		SampleSound* ss = ba->getSoundByName(soundName);
		ss->setEmitterX(x);
	};

	void setEmitterY(LPCWSTR soundName, FLOAT32 y) {
		if(ba == NULL)
			return;
		SampleSound* ss = ba->getSoundByName(soundName);
		ss->setEmitterY(y);
	};
	void setEmitterZ(LPCWSTR soundName, FLOAT32 z) {
		if(ba == NULL)
			return;
		SampleSound* ss = ba->getSoundByName(soundName);
		ss->setEmitterZ(z);
	};

	void printMatrixCoefficients(){
		ba->printMatrixCoefficients();
	};

	/**
	 * @fn	void CDxAudioInterfaceDLL::destroy()
	 *
	 * @brief	Destroys this object.
	 *
	 * @author	Phil
	 * @date	6/10/2013
	 */

	void destroy(){
		if(ba == NULL)
			return;
		ba->destroy();
	};

private:

	/**
	 * @summary	The ba.
	 */

	BasicAudio *ba;

};

/**
 * @summary	The dx audio interface dll.
 */

extern DXAUDIOINTERFACEDLL_API int nDxAudioInterfaceDLL;

/**
 * @fn	DXAUDIOINTERFACEDLL_API int fnDxAudioInterfaceDLL(void);
 *
 * @brief	Gets the dx audio interface dll.
 *
 * @author	Phil
 * @date	6/10/2013
 *
 * @return	.
 */

DXAUDIOINTERFACEDLL_API int fnDxAudioInterfaceDLL(void);

/**
 * @fn	DXAUDIOINTERFACEDLL_API BasicAudio* getBasicAudio(void);
 *
 * @brief	Gets the basic audio.
 *
 * @author	Phil
 * @date	6/10/2013
 *
 * @return	null if it fails, else the basic audio.
 */

DXAUDIOINTERFACEDLL_API BasicAudio* getBasicAudio(void);
