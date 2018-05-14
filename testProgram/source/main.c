#include <stdint.h>
#include <stdio.h>
#include <3ds.h>

#include "../../exec_3dsx/exec_3dsx.h"
#include "../../exec_3dsx/exec_cia.h"

int main(int argc, char* argv[]){
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	
	//print arguments sent to this test
	printf("Argv args:\n");
	for(uint16_t args = 0; args < argc; args++){
		printf("%s\n", argv[args]);
	}
	printf("(End of argv args)\n");
	
	printf("A = launch sdmc:/debug.3dsx\n");
	printf("B = launch sdmc:/debug.cia\n");
	
	// Main loop
	while (aptMainLoop()) {

		gspWaitForVBlank();
		hidScanInput();
		u32 kDown = hidKeysDown();
		
		if(kDown & KEY_A)
			exec_3dsx("sdmc:/debug.3dsx", "arg1 arg2 \"arg3 with spaces\"");
		
		if(kDown & KEY_B)
			exec_cia("sdmc:/debug.cia", "arg1 arg2 \"arg3 with spaces\"");
		
		if(kDown & KEY_START)
			break; // break in order to return to hbmenu

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}