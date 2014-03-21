// DxAudioInterfaceDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DxAudioInterfaceDLL.h"



// This is an example of an exported variable
DXAUDIOINTERFACEDLL_API int nDxAudioInterfaceDLL=0;

// This is an example of an exported function.
DXAUDIOINTERFACEDLL_API int fnDxAudioInterfaceDLL(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see DxAudioInterfaceDLL.h for the class definition
/***
CDxAudioInterfaceDLL::CDxAudioInterfaceDLL()
{
	return;
}
/****/

DXAUDIOINTERFACEDLL_API BasicAudio* getBasicAudio(void){
	BasicAudio *ba = new BasicAudio();
	ba->init();
	return ba;
}