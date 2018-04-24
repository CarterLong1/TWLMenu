/*-----------------------------------------------------------------
 Copyright (C) 2005 - 2013
	Michael "Chishm" Chisholm
	Dave "WinterMute" Murphy
	Claudio "sverx"

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <sys/stat.h>
#include <limits.h>

#include <string.h>
#include <unistd.h>
#include <gl2d.h>

#include "graphics/graphics.h"

#include "nds_loader_arm9.h"

#include "graphics/fontHandler.h"

using namespace std;

//---------------------------------------------------------------------------------
void stop (void) {
//---------------------------------------------------------------------------------
	while (1) {
		swiWaitForVBlank();
	}
}

char filePath[PATH_MAX];

//---------------------------------------------------------------------------------
void doPause(int x, int y) {
//---------------------------------------------------------------------------------
	// iprintf("Press start...\n");
	printSmall(false, x, y, "Press start...");
	while(1) {
		scanKeys();
		if(keysDown() & KEY_START)
			break;
	}
	scanKeys();
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Turn on screen backlights if they're disabled
	powerOn(PM_BACKLIGHT_TOP);
	powerOn(PM_BACKLIGHT_BOTTOM);
	
	bool graphicsInited = false;

	// overwrite reboot stub identifier
	extern u64 *fake_heap_end;
	*fake_heap_end = 0;

	defaultExceptionHandler();

	// Read user name
	char *username = (char*)PersonalData->name;
		
	// text
	for (int i = 0; i < 10; i++) {
		if (username[i*2] == 0x00)
			username[i*2/2] = 0;
		else
			username[i*2/2] = username[i*2];
	}

	scanKeys();
	if(keysHeld() & KEY_RIGHT) {
		if(!graphicsInited) {
			graphicsInit();
			fontInit();
			graphicsInited = true;
		}
		printSmall(true, 1, 2, username);
		printSmall(false, 4, 4, "Please remove your SD Card,");
		printSmall(false, 4, 12, "and insert one containing the");
		printSmall(false, 4, 20, "NDS ROMs and SRLoader.");
		printSmall(false, 4, 36, "After inserting it, press START");
		printSmall(false, 4, 44, "to proceed.");
		while(1) {
			scanKeys();
			if(keysHeld() & KEY_START) break;
		}
	}

	if (!fatInitDefault()) {
		if(!graphicsInited) {
			graphicsInit();
			fontInit();
			graphicsInited = true;
		}
		printSmall(true, 1, 2, username);
		printSmall(false, 4, 4, "fatinitDefault failed!");
		stop();
	}

	runNdsFile ("/_nds/srloader/main.srldr", 0, NULL);

	if(!graphicsInited) {
		graphicsInit();
		fontInit();
		graphicsInited = true;
	}

	clearText();
	printSmall(true, 1, 2, username);

	printSmall(false, 4, 4, "sd:/_nds/srloader/main.srldr");
	printSmall(false, 4, 12, "not found.");
	printSmall(false, 4, 28, "Press B to return to menu.");
	
	while (1) {
		scanKeys();
		if (keysHeld() & KEY_B) fifoSendValue32(FIFO_USER_01, 1);	// Tell ARM7 to reboot into 3DS HOME Menu (power-off/sleep mode screen skipped)
	}

	return 0;
}
