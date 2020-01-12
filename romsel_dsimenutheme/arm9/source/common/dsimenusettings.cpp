
#include "dsimenusettings.h"
#include "bootstrappaths.h"
#include "systemdetails.h"
#include "common/inifile.h"
#include "flashcard.h"
#include <string.h>

extern const char *settingsinipath;

DSiMenuPlusPlusSettings::DSiMenuPlusPlusSettings()
{
	romfolder[0] = "sd:/";
	romfolder[1] = "fat:/";

	pagenum[0] = 0;
	pagenum[1] = 0;

	cursorPosition[0] = 0;
	cursorPosition[1] = 0;

	startMenu_cursorPosition = 0;
	consoleModel = 0;

	gotosettings = false;

	guiLanguage = ELangDefault;
	colorMode = 0;
	blfLevel = 0;
	sdRemoveDetect = true;
	useGbarunner = false;
	gbar2DldiAccess = false;
	showMainMenu = true;
	theme = 0;
	subtheme = 0;
	dsiMusic = 1;

	showNds = true;
	showRvid = true;
	showNes = true;
	showGb = true;
	showSmsGg = true;
	showMd = true;
	showSnes = true;
	showDirectories = true;
	showHidden = false;
	showBoxArt = true;
	cacheBoxArt = true;
	animateDsiIcons = true;
	sysRegion = -1;
	launcherApp = -1;
	secondaryAccess = false;
	previousUsedDevice = false;
	secondaryDevice = false;
	fcSaveOnSd = false;
	updateRecentlyPlayedList = true;
	sortMethod = 0;

	flashcard = EDSTTClone;

	slot1LaunchMethod = EDirect;

	useBootstrap = isDSiMode();
	bootstrapFile = EReleaseBootstrap;

	bstrap_language = ELangDefault;
	boostCpu = false;
	boostVram = false;
	bstrap_dsiMode = EDSMode;
	forceSleepPatch = false;
	slot1SCFGUnlock = false;
	dsiWareBooter = false;

	show12hrClock = true;

	snesEmulator = true;

	ak_viewMode = EViewInternal;
	ak_scrollSpeed = EScrollFast;
	ak_theme = "zelda";
	ak_zoomIcons = true;

	launchType = ENoLaunch;
	homebrewBootstrap = EReleaseBootstrap;

	r4_theme = "unused";
	dsi_theme = "dark";
	_3ds_theme = "light";
	
	soundfreq = EFreq32KHz;
	dsiSplash = isDSiMode();
	hsMsg = false;
	showlogo = true;
	autorun = false;

	wideScreen = false;
}

void DSiMenuPlusPlusSettings::loadSettings()
{
	printf("ms().loadSettings()\n");
	CIniFile settingsini(settingsinipath);

	// UI settings.
	romfolder[0] = settingsini.GetString("SRLOADER", "ROM_FOLDER", romfolder[0]);
	romfolder[1] = settingsini.GetString("SRLOADER", "SECONDARY_ROM_FOLDER", romfolder[1]);

	pagenum[0] = settingsini.GetInt("SRLOADER", "PAGE_NUMBER", pagenum[0]);
	pagenum[1] = settingsini.GetInt("SRLOADER", "SECONDARY_PAGE_NUMBER", pagenum[1]);

	cursorPosition[0] = settingsini.GetInt("SRLOADER", "CURSOR_POSITION", cursorPosition[0]);
	cursorPosition[1] = settingsini.GetInt("SRLOADER", "SECONDARY_CURSOR_POSITION", cursorPosition[1]);

	startMenu_cursorPosition = settingsini.GetInt("SRLOADER", "STARTMENU_CURSOR_POSITION", startMenu_cursorPosition);
	consoleModel = settingsini.GetInt("SRLOADER", "CONSOLE_MODEL", consoleModel);

	showNds = settingsini.GetInt("SRLOADER", "SHOW_NDS", showNds);
	showRvid = settingsini.GetInt("SRLOADER", "SHOW_RVID", showRvid);
	showNes = settingsini.GetInt("SRLOADER", "SHOW_NES", showNes);
	showGb = settingsini.GetInt("SRLOADER", "SHOW_GB", showGb);
	showSmsGg = settingsini.GetInt("SRLOADER", "SHOW_SMSGG", showSmsGg);
	showMd = settingsini.GetInt("SRLOADER", "SHOW_MDGEN", showMd);
	showSnes = settingsini.GetInt("SRLOADER", "SHOW_SNES", showSnes);

	updateRecentlyPlayedList = settingsini.GetInt("SRLOADER", "UPDATE_RECENTLY_PLAYED_LIST", updateRecentlyPlayedList);
	sortMethod = settingsini.GetInt("SRLOADER", "SORT_METHOD", sortMethod);

	// Customizable UI settings.
	colorMode = settingsini.GetInt("SRLOADER", "COLOR_MODE", colorMode);
	blfLevel = settingsini.GetInt("SRLOADER", "BLUE_LIGHT_FILTER_LEVEL", blfLevel);
	guiLanguage = settingsini.GetInt("SRLOADER", "LANGUAGE", guiLanguage);
	sdRemoveDetect = settingsini.GetInt("SRLOADER", "SD_REMOVE_DETECT", sdRemoveDetect);
	useGbarunner = settingsini.GetInt("SRLOADER", "USE_GBARUNNER2", useGbarunner);
	if (!sys().isRegularDS()) {
		useGbarunner = true;
	}
	gbar2DldiAccess = settingsini.GetInt("SRLOADER", "GBAR2_DLDI_ACCESS", gbar2DldiAccess);

	soundfreq = settingsini.GetInt("SRLOADER", "SOUND_FREQ", soundfreq);

	secondaryAccess = settingsini.GetInt("SRLOADER", "SECONDARY_ACCESS", secondaryAccess);
	previousUsedDevice = settingsini.GetInt("SRLOADER", "PREVIOUS_USED_DEVICE", previousUsedDevice);
	romPath = settingsini.GetString("SRLOADER", "ROM_PATH", romPath);

	// secondaryDevice = settingsini.GetInt("SRLOADER", "SECONDARY_DEVICE", secondaryDevice);
	// flashcard = settingsini.GetInt("SRLOADER", "FLASHCARD", flashcard);

	secondaryDevice = bothSDandFlashcard() ? settingsini.GetInt("SRLOADER", "SECONDARY_DEVICE", secondaryDevice) : flashcardFound();
	fcSaveOnSd = settingsini.GetInt("SRLOADER", "FC_SAVE_ON_SD", fcSaveOnSd);
	
	showMainMenu = settingsini.GetInt("SRLOADER", "SHOW_MAIN_MENU", showMainMenu);
	theme = settingsini.GetInt("SRLOADER", "THEME", theme);
	subtheme = settingsini.GetInt("SRLOADER", "SUB_THEME", subtheme);
	dsiMusic = settingsini.GetInt("SRLOADER", "DSI_MUSIC", dsiMusic);
	showDirectories = settingsini.GetInt("SRLOADER", "SHOW_DIRECTORIES", showDirectories);
	showHidden = settingsini.GetInt("SRLOADER", "SHOW_HIDDEN", showHidden);
	showBoxArt = settingsini.GetInt("SRLOADER", "SHOW_BOX_ART", showBoxArt);
	cacheBoxArt = settingsini.GetInt("SRLOADER", "CACHE_BOX_ART", cacheBoxArt);
	animateDsiIcons = settingsini.GetInt("SRLOADER", "ANIMATE_DSI_ICONS", animateDsiIcons);
	if (consoleModel < 2) {
		sysRegion = settingsini.GetInt("SRLOADER", "SYS_REGION", sysRegion);
		launcherApp = settingsini.GetInt("SRLOADER", "LAUNCHER_APP", launcherApp);
	}

	slot1LaunchMethod = settingsini.GetInt("SRLOADER", "SLOT1_LAUNCHMETHOD", slot1LaunchMethod);
	bootstrapFile = settingsini.GetInt("SRLOADER", "BOOTSTRAP_FILE", bootstrapFile);
	useBootstrap = settingsini.GetInt("SRLOADER", "USE_BOOTSTRAP", useBootstrap);

	// Default nds-bootstrap settings
	bstrap_language = settingsini.GetInt("NDS-BOOTSTRAP", "LANGUAGE", bstrap_language);
	boostCpu = settingsini.GetInt("NDS-BOOTSTRAP", "BOOST_CPU", boostCpu);
	boostVram = settingsini.GetInt("NDS-BOOTSTRAP", "BOOST_VRAM", boostVram);
	bstrap_dsiMode = settingsini.GetInt("NDS-BOOTSTRAP", "DSI_MODE", bstrap_dsiMode);
	forceSleepPatch = settingsini.GetInt("NDS-BOOTSTRAP", "FORCE_SLEEP_PATCH", forceSleepPatch);
	dsiWareBooter = settingsini.GetInt("SRLOADER", "DSIWARE_BOOTER", dsiWareBooter);

	dsiWareSrlPath = settingsini.GetString("SRLOADER", "DSIWARE_SRL", dsiWareSrlPath);
	dsiWarePubPath = settingsini.GetString("SRLOADER", "DSIWARE_PUB", dsiWarePubPath);
	dsiWarePrvPath = settingsini.GetString("SRLOADER", "DSIWARE_PRV", dsiWarePrvPath);
	launchType = settingsini.GetInt("SRLOADER", "LAUNCH_TYPE", launchType);
	homebrewArg = settingsini.GetString("SRLOADER", "HOMEBREW_ARG", homebrewArg);
	homebrewBootstrap = settingsini.GetInt("SRLOADER", "HOMEBREW_BOOTSTRAP", homebrewBootstrap);

	show12hrClock = settingsini.GetInt("SRLOADER", "SHOW_12H_CLOCK", show12hrClock);

	dsi_theme = settingsini.GetString("SRLOADER", "DSI_THEME", dsi_theme);
	_3ds_theme = settingsini.GetString("SRLOADER", "3DS_THEME", _3ds_theme);

	snesEmulator = settingsini.GetInt("SRLOADER", "SNES_EMULATOR", snesEmulator);
	
	wideScreen = settingsini.GetInt("SRLOADER", "WIDESCREEN", wideScreen);
}

void DSiMenuPlusPlusSettings::saveSettings()
{
	CIniFile settingsini(settingsinipath);

	settingsini.SetString("SRLOADER", "ROM_FOLDER", romfolder[0]);
	settingsini.SetString("SRLOADER", "SECONDARY_ROM_FOLDER", romfolder[1]);
	
	settingsini.SetInt("SRLOADER", "PAGE_NUMBER", pagenum[0]);
	settingsini.SetInt("SRLOADER", "SECONDARY_PAGE_NUMBER", pagenum[1]);

	settingsini.SetInt("SRLOADER", "CURSOR_POSITION", cursorPosition[0]);
	settingsini.SetInt("SRLOADER", "SECONDARY_CURSOR_POSITION", cursorPosition[1]);

	settingsini.SetInt("SRLOADER", "SECONDARY_ACCESS", secondaryAccess);

	if (bothSDandFlashcard())
		settingsini.SetInt("SRLOADER", "SECONDARY_DEVICE", secondaryDevice);

	if (!gotosettings) {
		settingsini.SetInt("SRLOADER", "PREVIOUS_USED_DEVICE", previousUsedDevice);
		settingsini.SetString("SRLOADER", "ROM_PATH", romPath);
		settingsini.SetString("SRLOADER", "DSIWARE_SRL", dsiWareSrlPath);
		settingsini.SetString("SRLOADER", "DSIWARE_PUB", dsiWarePubPath);
		settingsini.SetString("SRLOADER", "DSIWARE_PRV", dsiWarePrvPath);
		settingsini.SetInt("SRLOADER", "LAUNCH_TYPE", launchType);
		settingsini.SetString("SRLOADER", "HOMEBREW_ARG", homebrewArg);
		settingsini.SetInt("SRLOADER", "HOMEBREW_BOOTSTRAP", homebrewBootstrap);
	}

	settingsini.SetInt("SRLOADER", "SORT_METHOD", sortMethod);

	settingsini.SaveIniFileModified();
}

DSiMenuPlusPlusSettings::TLanguage DSiMenuPlusPlusSettings::getGuiLanguage()
{
	return (TLanguage)(guiLanguage == ELangDefault ? PersonalData->language : guiLanguage);
}
