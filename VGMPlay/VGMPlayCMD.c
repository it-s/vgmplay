#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "stdbool.h"

#include "chips/mamedef.h"
#include "Stream.h"
#include "VGMPlay.h"
#include "VGMPlay_Intf.h"

void WaveOutLinuxCallBack(void);
extern VGM_HEADER VGMHead;
extern UINT16 AUDIOBUFFERU;
extern UINT8 PlayingMode;
extern bool EndPlay;

UINT8 CmdList[0x100];
bool ErrorHappened;	// used by VGMPlay.c and VGMPlay_AddFmts.c
int QuitPlay;

void trap(int signal){ QuitPlay = 1; }

int main(int argc, char* argv[])
{
  signal(SIGINT, &trap);
  if (argc != 2) {
    printf("Expected one argumant, but got %d\n", argc);
    exit(0);
  }
  
  char *filename = argv[1];
  printf("Playing file: %s\n", filename);
  
	VGMPlay_Init();
	VGMPlay_Init2();
  if(OpenVGMFile(filename)) {
    PlayVGM();
    switch(PlayingMode)
    {
    case 0x00:
      AUDIOBUFFERU = 10;
      break;
    case 0x01:
      AUDIOBUFFERU = 0;	// no AudioBuffers needed
      break;
    case 0x02:
      AUDIOBUFFERU = 5;	// try to sync Hardware/Software Emulator as well as possible
      break;
    }
    if (StartStream(0))
    {
      printf("Error openning Sound Device!\n");
      exit (1);
    }
    while(! QuitPlay)
    {
			WaveOutLinuxCallBack();
      if (EndPlay)
				QuitPlay = true;
    }
    StopStream();
    CloseVGMFile();
    printf("Playback ended\n");
  } else {
    printf("Not a VGM file\n");
  }
  VGMPlay_Deinit();

  return 0;
}