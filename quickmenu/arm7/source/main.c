/*---------------------------------------------------------------------------------

	default ARM7 core

		Copyright (C) 2005 - 2010
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <maxmod7.h>

unsigned int * SCFG_EXT=(unsigned int*)0x4004008;

static u8 backlightLevel = 0;
static bool isDSLite = false;

//---------------------------------------------------------------------------------
void ReturntoDSiMenu() {
//---------------------------------------------------------------------------------
	if (isDSiMode()) {
		i2cWriteRegister(0x4A, 0x70, 0x01);		// Bootflag = Warmboot/SkipHealthSafety
		i2cWriteRegister(0x4A, 0x11, 0x01);		// Reset to DSi Menu
	} else {
		u8 readCommand = readPowerManagement(0x10);
		readCommand |= BIT(0);
		writePowerManagement(0x10, readCommand);
	}
}

//---------------------------------------------------------------------------------
void changeBacklightLevel() {
//---------------------------------------------------------------------------------
	backlightLevel = i2cReadRegister(0x4A, 0x41);
	backlightLevel++;
	if (backlightLevel > 0x04) {
		backlightLevel = 0;
	}
	i2cWriteRegister(0x4A, 0x41, backlightLevel);
}

//---------------------------------------------------------------------------------
void VblankHandler(void) {
//---------------------------------------------------------------------------------
}

//---------------------------------------------------------------------------------
void VcountHandler() {
//---------------------------------------------------------------------------------
	inputGetAndSend();
}

volatile bool exitflag = false;

//---------------------------------------------------------------------------------
void powerButtonCB() {
//---------------------------------------------------------------------------------
	exitflag = true;
}

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
    nocashMessage("ARM7 main.c main");
	
	// clear sound registers
	dmaFillWords(0, (void*)0x04000400, 0x100);

	REG_SOUNDCNT |= SOUND_ENABLE;
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	powerOn(POWER_SOUND);

	readUserSettings();
	ledBlink(0);

	irqInit();
	// Start the RTC tracking IRQ
	initClockIRQ();

	touchInit();

	fifoInit();
	
	mmInstall(FIFO_MAXMOD);
	
	SetYtrigger(80);
	
	installSoundFIFO();
	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT );

	setPowerButtonCB(powerButtonCB);

	u8 readCommand = readPowerManagement(4);
	isDSLite = (readCommand & BIT(4) || readCommand & BIT(5) || readCommand & BIT(6) || readCommand & BIT(7));

	for (int i = 0; i < 8; i++) {
		*(u8*)(0x2FFFD00+i) = *(u8*)(0x4004D07-i);	// Get ConsoleID
	}

	fifoSendValue32(FIFO_USER_03, *SCFG_EXT);
	fifoSendValue32(FIFO_USER_04, isDSLite);
	fifoSendValue32(FIFO_USER_07, *(u16*)(0x4004700));
	fifoSendValue32(FIFO_USER_06, 1);

	// Keep the ARM7 mostly idle
	while (!exitflag) {
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R))) {
			exitflag = true;
		}
		resyncClock();
		if (isDSiMode() && *(vu32*)(0x400481C) & BIT(4)) {
			*(u8*)(0x023FF002) = 2;
		} else if (isDSiMode() && *(vu32*)(0x400481C) & BIT(3)) {
			*(u8*)(0x023FF002) = 1;
		} else {
			*(u8*)(0x023FF002) = 0;
		}
		if(fifoCheckValue32(FIFO_USER_02)) {
			ReturntoDSiMenu();
		}
		if(fifoGetValue32(FIFO_USER_04) == 1) {
			changeBacklightLevel();
			fifoSendValue32(FIFO_USER_04, 0);
		}
		if (*(u32*)(0x2FFFD0C) == 0x454D4D43) {
			sdmmc_nand_cid((u32*)0x2FFD7BC);	// Get eMMC CID
			*(u32*)(0x2FFFD0C) = 0;
		}
		swiWaitForVBlank();
	}
	return 0;
}

