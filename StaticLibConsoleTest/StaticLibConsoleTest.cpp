// StaticLibConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <conio.h>
#include "WavSampleSound.h"

#include "BasicAudio.h"


int _tmain(int argc, _TCHAR* argv[])
{
	BasicAudio *ba = new BasicAudio();
	ba->init();

	fprintf(stderr, "\nReady to play mono WAV PCM file(s)...\n" );

	WavSampleSound *singleSound = (WavSampleSound *)ba->createSound(L"music", L"Wavs\\MusicMono.wav", 0);
	WavSampleSound *continuousSound = (WavSampleSound *)ba->createSound(L"heli", L"Wavs\\heli.wav", XAUDIO2_LOOP_INFINITE);

	int keyIn;
	printf("Type 'x' to quit\nC start continuous\nc stop continuous\nS start single\n");
	bool doit = true;

	int channelIndex = -1;
	while(doit){
		if(_kbhit()){
			keyIn = _getch();
			channelIndex = -1;

			printf("key = %c\n", keyIn);
			switch(keyIn){
			case 'x' : doit = false; 

				break;
			case 'C' : continuousSound->start();                  // play sound
				break;
			case 'c' : continuousSound->stop();                  // cease sound
				break;
			case 'S' : ba->getSoundByName(L"music")->start();	//singleSound->start();						// play sound
				break;
			case 'w' : 
				continuousSound->setEmitterZ(continuousSound->getEmitterZ() + (FLOAT32)0.5);
				break;
			case 's' : 
				continuousSound->setEmitterZ(continuousSound->getEmitterZ() - (FLOAT32)0.5);
				break;
			case 'a' : 
				continuousSound->setEmitterX(continuousSound->getEmitterX() - (FLOAT32)0.5);
				break;
			case 'd' : 
				continuousSound->setEmitterX(continuousSound->getEmitterX() + (FLOAT32)0.5);
				break;
			case 'p':
				ba->printMatrixCoefficients();
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
				IXAudio2SourceVoice* voice = continuousSound->getSourceVoice();
				//ba->play3DVoice(continuousSound->getEmitter(), voice);
				// ba->play3DVoice(continuousSound);
				ba->play3DVoice(L"heli");
			}else{
				printf("channel = %d\n", channelIndex);
				IXAudio2SourceVoice* voice = continuousSound->getSourceVoice();
				ba->playOnChannelVoice(voice, channelIndex);
			}
		}

		ba->run(); // perform periodic sound engine tasks
		Sleep(100);
	}

	fprintf(stderr, "\nFinished playing\n" );

	// All XAudio2 interfaces are released when the engine is destroyed, but being tidy

	ba->destroy();

	// printf("hit return to exit");
	// getchar();
	return 0;
}

