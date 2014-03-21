// DynamicLibConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <conio.h>
#include <WavSampleSound.h>
#include <DxAudioInterfaceDLL.h>


int _tmain(int argc, _TCHAR* argv[])
{
	CDxAudioInterfaceDLL *dai = new CDxAudioInterfaceDLL();
	dai->init();
	dai->createSound(L"music", L"Wavs\\MusicMono.wav", 0);
	dai->createSound(L"heli", L"Wavs\\heli.wav", XAUDIO2_LOOP_INFINITE);
	//BasicAudio *ba = getBasicAudio();


	fprintf(stderr, "\nReady to play mono WAV PCM file(s)...\n" );

	int keyIn;
	printf("Type 'x' to quit\nC start continuous\nc stop continuous\nS start single\n");
	bool doit = true;

	FLOAT32 pos;
	int channelIndex = -1;
	while(doit){
		if(_kbhit()){
			keyIn = _getch();
			channelIndex = -1;

			printf("key = %c\n", keyIn);
			switch(keyIn){
			case 'x' : doit = false; 

				break;
			case 'C' : dai->startSound(L"heli");                  // play sound
				break;
			case 'c' : dai->stopSound(L"heli");                  // cease sound
				break;
			case 'S' : dai->startSound(L"music");	//singleSound->start();						// play sound
				break;
			case 'w' : 
				pos = dai->getEmitterZ(L"heli");
				dai->setEmitterZ(L"heli", pos + (FLOAT32)0.5);
				break;
			case 's' : 
				pos = dai->getEmitterZ(L"heli");
				dai->setEmitterZ(L"heli", pos - (FLOAT32)0.5);
				break;
			case 'a' : 
				pos = dai->getEmitterX(L"heli");
				dai->setEmitterX(L"heli", pos - (FLOAT32)0.5);
				break;
			case 'd' : 
				pos = dai->getEmitterX(L"heli");
				dai->setEmitterZ(L"heli", pos + (FLOAT32)0.5);
				break;
			case 'p':
				dai->printMatrixCoefficients();
				break;
			case '0' : channelIndex = 0; break;
			case '1' : channelIndex = 1; break;
			case '2' : channelIndex = 2; break;
			case '3' : channelIndex = 3; break;
			case '4' : channelIndex = 4; break;
			case '5' : channelIndex = 5; break;
			case '6' : channelIndex = 6; break;
			case '7' : channelIndex = 7; break;

			}
			if(channelIndex == -1){
				dai->play3DVoice(L"heli");
			}else{
				dai->playOnChannelVoice(L"heli", channelIndex);
			}
		}

		dai->run(); // perform periodic sound engine tasks
		Sleep(100);
	}

	fprintf(stderr, "\nFinished playing\n" );

	// All XAudio2 interfaces are released when the engine is destroyed, but being tidy

	dai->destroy();

	// printf("hit return to exit");
	// getchar();
	/****/
	
	return 0;
}

