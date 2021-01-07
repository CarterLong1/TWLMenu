/*
    mainwnd.cpp
    Copyright (C) 2007 Acekard, www.acekard.com
    Copyright (C) 2007-2009 somebody
    Copyright (C) 2009 yellow wood goblin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "tool/dbgtool.h"
#include "ui/windowmanager.h"
#include "mainwnd.h"
#include "ui/msgbox.h"
#include "systemfilenames.h"

#include "time/datetime.h"
#include "time/timer.h"

#include "tool/timetool.h"
#include "tool/fifotool.h"

#include "ui/progresswnd.h"
#include "common/bootstrapconfig.h"
#include "common/loaderconfig.h"
#include "common/widescreenconfig.h"
#include "common/playedconfig.h"
#include "common/pergamesettings.h"
#include "common/cardlaunch.h"
#include "common/systemdetails.h"
#include "common/dsargv.h"
#include "common/flashcard.h"
#include "common/flashcardlaunch.h"
#include "common/gbaswitch.h"
#include "common/unlaunchboot.h"
#include "common/files.h"
#include "common/filecopy.h"
#include "common/nds_loader_arm9.h"
#include "incompatibleGameMap.h"

#include "common/inifile.h"
#include "language.h"
#include "common/dsimenusettings.h"
#include "windows/rominfownd.h"
#include "windows/cheatwnd.h"

#include <nds/arm9/dldi.h>
#include <sys/iosupport.h>

using namespace akui;

MainWnd::MainWnd(s32 x, s32 y, u32 w, u32 h, Window *parent, const std::string &text)
    : Form(x, y, w, h, parent, text), _mainList(NULL), _startMenu(NULL), _startButton(NULL),
      _brightnessButton(NULL), _batteryIcon(NULL), _folderUpButton(NULL),  _folderText(NULL), _processL(false)
{
}

MainWnd::~MainWnd()
{
    delete _folderText;
    delete _folderUpButton;
    delete _brightnessButton;
    delete _batteryIcon;
    delete _startButton;
    delete _startMenu;
    delete _mainList;
    windowManager().removeWindow(this);
}

void MainWnd::init()
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    bool showBatt = 0;
    COLOR color = 0;
    std::string file("");
    std::string text("");
    CIniFile ini(SFN_UI_SETTINGS);

    // self init
    dbg_printf("mainwnd init() %08x\n", this);
    loadAppearance(SFN_LOWER_SCREEN_BG);
    windowManager().addWindow(this);

    // init game file list
    _mainList = new MainList(4, 20, 248, 152, this, "main list");
    _mainList->setRelativePosition(Point(4, 20));
    _mainList->init();
    _mainList->selectChanged.connect(this, &MainWnd::listSelChange);
    _mainList->selectedRowClicked.connect(this, &MainWnd::onMainListSelItemClicked);
    _mainList->directoryChanged.connect(this, &MainWnd::onFolderChanged);
    _mainList->animateIcons.connect(this, &MainWnd::onAnimation);

    addChildWindow(_mainList);
    dbg_printf("mainlist %08x\n", _mainList);

    //waitMs( 1000 );

    // init start button
    x = ini.GetInt("start button", "x", 0);
    y = ini.GetInt("start button", "y", 172);
    w = ini.GetInt("start button", "w", 48);
    h = ini.GetInt("start button", "h", 10);
    color = ini.GetInt("start button", "textColor", 0x7fff);
    file = ini.GetString("start button", "file", "none");
    text = ini.GetString("start button", "text", "START");
    if (file != "none")
    {
        file = SFN_UI_CURRENT_DIRECTORY + file;
    }
    if (text == "ini")
    {
        text = LANG("start menu", "START");
    }
    _startButton = new Button(x, y, w, h, this, text);
    _startButton->setStyle(Button::press);
    _startButton->setRelativePosition(Point(x, y));
    _startButton->loadAppearance(file);
    _startButton->clicked.connect(this, &MainWnd::startButtonClicked);
    _startButton->setTextColor(color | BIT(15));
    if (!ini.GetInt("start button", "show", 1))
        _startButton->hide();
    addChildWindow(_startButton);

    // // init brightness button
	
	    x = ini.GetInt("battery icon", "x", 238);
	    y = ini.GetInt("battery icon", "y", 172);
	    showBatt = ini.GetInt("battery icon", "show", 0);

	
	    if(showBatt)
	    {
            if(!ini.GetInt("battery icon", "screen", true))
            {
                _batteryIcon = new Button(x, y, w, h, this, "");
                _batteryIcon->setRelativePosition(Point(x,y));

                u8 batteryLevel = sys().batteryStatus();

				if (isDSiMode()) {
					if (batteryLevel & BIT(7)) {
						_batteryIcon->loadAppearance(SFN_BATTERY_CHARGE);
					} else if (batteryLevel == 0xF) {
						_batteryIcon->loadAppearance(SFN_BATTERY4);
					} else if (batteryLevel == 0xB) {
						_batteryIcon->loadAppearance(SFN_BATTERY3);
					} else if (batteryLevel == 0x7) {
						_batteryIcon->loadAppearance(SFN_BATTERY2);
					} else if (batteryLevel == 0x3 || batteryLevel == 0x1) {
						_batteryIcon->loadAppearance(SFN_BATTERY1);
					} else {
						_batteryIcon->loadAppearance(SFN_BATTERY_CHARGE);
					}
				} else {
					if (batteryLevel & BIT(0)) {
						_batteryIcon->loadAppearance(SFN_BATTERY1);
					} else {
						_batteryIcon->loadAppearance(SFN_BATTERY4);
					}
				}
        
                addChildWindow(_batteryIcon);
            }
	    }

    x = ini.GetInt("folderup btn", "x", 0);
    y = ini.GetInt("folderup btn", "y", 2);
    w = ini.GetInt("folderup btn", "w", 32);
    h = ini.GetInt("folderup btn", "h", 16);
    _folderUpButton = new Button(x, y, w, h, this, "");
    _folderUpButton->setRelativePosition(Point(x, y));
    _folderUpButton->loadAppearance(SFN_FOLDERUP_BUTTON);
    _folderUpButton->setSize(Size(w, h));
    _folderUpButton->pressed.connect(_mainList, &MainList::backParentDir);
    addChildWindow(_folderUpButton);

    x = ini.GetInt("folder text", "x", 20);
    y = ini.GetInt("folder text", "y", 2);
    w = ini.GetInt("folder text", "w", 160);
    h = ini.GetInt("folder text", "h", 16);
    _folderText = new StaticText(x, y, w, h, this, "");
    _folderText->setRelativePosition(Point(x, y));
    _folderText->setTextColor(ini.GetInt("folder text", "color", 0));
    addChildWindow(_folderText);
    
    if (!ms().showDirectories) {
        _folderText->hide();
    }
    // init startmenu
    _startMenu = new StartMenu(160, 40, 61, 108, this, "start menu");
    _startMenu->init();
    _startMenu->itemClicked.connect(this, &MainWnd::startMenuItemClicked);
    _startMenu->hide();
    _startMenu->setRelativePosition(_startMenu->position());
    addChildWindow(_startMenu);
    dbg_printf("startMenu %08x\n", _startMenu);

    arrangeChildren();
}

void MainWnd::draw()
{
    Form::draw();
}

void MainWnd::listSelChange(u32 i)
{
    // #ifdef DEBUG
    //     //dbg_printf( "main list item %d\n", i );
    //     DSRomInfo info;
    //     if (_mainList->getRomInfo(i, info))
    //     {
    //         char title[13] = {};
    //         memcpy(title, info.saveInfo().gameTitle, 12);
    //         char code[5] = {};
    //         memcpy(code, info.saveInfo().gameCode, 4);
    //         u16 crc = swiCRC16(0xffff, ((unsigned char *)&(info.banner())) + 32, 0x840 - 32);
    //         dbg_printf("%s %s %04x %d %04x/%04x\n",
    //                    title, code, info.saveInfo().gameCRC, info.isDSRom(), info.banner().crc, crc);
    //         //dbg_printf("sizeof banner %08x\n", sizeof( info.banner() ) );
    //     }
    // #endif //DEBUG
}

void MainWnd::startMenuItemClicked(s16 i)
{
    CIniFile ini(SFN_UI_SETTINGS);
    if(!ini.GetInt("start menu", "showFileOperations", true)) i += 4;
    
    dbg_printf("start menu item %d\n", i);

    // ------------------- Copy and Paste ---
    if (START_MENU_ITEM_COPY == i)
    {
        if (_mainList->getSelectedFullPath() == "")
            return;
        struct stat st;
        stat(_mainList->getSelectedFullPath().c_str(), &st);
        if (st.st_mode & S_IFDIR)
        {
            messageBox(this, LANG("no copy dir", "title"), LANG("no copy dir", "text"), MB_YES | MB_NO);
            return;
        }
        setSrcFile(_mainList->getSelectedFullPath(), SFM_COPY);
    }

    else if (START_MENU_ITEM_CUT == i)
    {
        if (_mainList->getSelectedFullPath() == "")
            return;
        struct stat st;
        stat(_mainList->getSelectedFullPath().c_str(), &st);
        if (st.st_mode & S_IFDIR)
        {
            messageBox(this, LANG("no copy dir", "title"), LANG("no copy dir", "text"), MB_YES | MB_NO);
            return;
        }
        setSrcFile(_mainList->getSelectedFullPath(), SFM_CUT);
    }

    else if (START_MENU_ITEM_PASTE == i)
    {
        bool ret = false;
        ret = copyOrMoveFile(_mainList->getCurrentDir());
        if (ret) // refresh current directory
            _mainList->enterDir(_mainList->getCurrentDir());
    }

    else if (START_MENU_ITEM_HIDE == i)
    {
        std::string fullPath = _mainList->getSelectedFullPath();
        if (fullPath != "")
        {
            bool ret = false;
            ret = hideFile(fullPath);
            if (ret)
                _mainList->enterDir(_mainList->getCurrentDir());
        }
    }

    else if (START_MENU_ITEM_DELETE == i)
    {
        std::string fullPath = _mainList->getSelectedFullPath();
        if (fullPath != "" && !ms().preventDeletion)
        {
            bool ret = false;
            ret = deleteFile(fullPath);
            if (ret)
                _mainList->enterDir(_mainList->getCurrentDir());
        }
    }

    if (START_MENU_ITEM_SETTING == i)
    {
        showSettings();
    }

    else if (START_MENU_ITEM_INFO == i)
    {
        showFileInfo();
    }
}

void MainWnd::startButtonClicked()
{
    if (_startMenu->isVisible())
    {
        _startMenu->hide();
    }
    else
    {
        _startMenu->show();
    }
}

Window &MainWnd::loadAppearance(const std::string &aFileName)
{
    return *this;
}

bool MainWnd::process(const Message &msg)
{
    if (_startMenu->isVisible())
        return _startMenu->process(msg);

    bool ret = false;

    ret = Form::process(msg);

    if (!ret)
    {
        if (msg.id() > Message::keyMessageStart && msg.id() < Message::keyMessageEnd)
        {
            ret = processKeyMessage((KeyMessage &)msg);
        }

        if (msg.id() > Message::touchMessageStart && msg.id() < Message::touchMessageEnd)
        {
            ret = processTouchMessage((TouchMessage &)msg);
        }
    }
    return ret;
}

bool MainWnd::processKeyMessage(const KeyMessage &msg)
{
    bool ret = false, isL = msg.shift() & KeyMessage::UI_SHIFT_L;
    if (msg.id() == Message::keyDown)
    {
        switch (msg.keyCode())
        {
        case KeyMessage::UI_KEY_DOWN:
            _mainList->selectNext();
            ret = true;
            break;
        case KeyMessage::UI_KEY_UP:
            _mainList->selectPrev();
            ret = true;
            break;

        case KeyMessage::UI_KEY_LEFT:
            _mainList->selectRow(_mainList->selectedRowId() - _mainList->visibleRowCount());
            ret = true;
            break;

        case KeyMessage::UI_KEY_RIGHT:
            _mainList->selectRow(_mainList->selectedRowId() + _mainList->visibleRowCount());
            ret = true;
            break;
        case KeyMessage::UI_KEY_A:
            onKeyAPressed();
            ret = true;
            break;
        case KeyMessage::UI_KEY_B:
            onKeyBPressed();
            ret = true;
            break;
        case KeyMessage::UI_KEY_Y:
            if (isL)
            {
                showSettings();
                _processL = false;
            }
            else
            {
                onKeyYPressed();
            }
            ret = true;
            break;

        case KeyMessage::UI_KEY_START:
            startButtonClicked();
            ret = true;
            break;
        case KeyMessage::UI_KEY_SELECT:
            if (isL)
            {
                _mainList->SwitchShowAllFiles();
                _processL = false;
            }
            else
            {
                _mainList->setViewMode((MainList::VIEW_MODE)((_mainList->getViewMode() + 1) % 3));
                ms().ak_viewMode = _mainList->getViewMode();
                ms().saveSettings();
            }
            ret = true;
            break;
        case KeyMessage::UI_KEY_L:
            _processL = true;
            ret = true;
            break;
        case KeyMessage::UI_KEY_R:
#ifdef DEBUG
            gdi().switchSubEngineMode();
            gdi().present(GE_SUB);
#endif //DEBUG
            ret = true;
            break;
        default:
        {
        }
        };
    }
    if (msg.id() == Message::keyUp)
    {
        switch (msg.keyCode())
        {
        case KeyMessage::UI_KEY_L:
            if (_processL)
            {
                _mainList->backParentDir();
                _processL = false;
            }
            ret = true;
            break;
        }
    }
    return ret;
}

bool MainWnd::processTouchMessage(const TouchMessage &msg)
{
    return false;
}

void MainWnd::onKeyYPressed()
{
    showFileInfo();
}

void MainWnd::onMainListSelItemClicked(u32 index)
{
    onKeyAPressed();
}

void MainWnd::onKeyAPressed()
{
    cwl();
    launchSelected();
}


void bootstrapBeforeSaveHandler()
{
    progressWnd().setPercent(0);
	progressWnd().setTipText("Now saving...");
    progressWnd().update();
}

void bootstrapSaveHandler()
{
	progressWnd().setTipText("Please wait...");
    progressWnd().setPercent(25);
    progressWnd().update();
}


void bootstrapCheatsHandler()
{
	progressWnd().setTipText("Writing cheats...");
    progressWnd().setPercent(50);
    progressWnd().update();
}


void bootstrapWidescreenHandler()
{
	progressWnd().setTipText("Enabling widescreen patches...");
    progressWnd().update();
}


void bootstrapWidescreenApplied()
{
	progressWnd().setTipText("Widescreen patches applied...");
    progressWnd().setPercent(75);
    progressWnd().update();
}

void bootstrapLaunchHandler()
{
    progressWnd().setPercent(90);
    progressWnd().update();
}

void MainWnd::bootArgv(DSRomInfo &rominfo)
{
    std::string fullPath = _mainList->getSelectedFullPath();
    std::string launchPath = fullPath;
    std::vector<const char *> cargv{};
    if (rominfo.isArgv())
    {
        ArgvFile argv(fullPath);
        launchPath = argv.launchPath();
        for (auto &string : argv.launchArgs())
            cargv.push_back(&string.front());
    }

    LoaderConfig config(fullPath, "");
    progressWnd().setTipText(LANG("game launch", "Please wait"));
    progressWnd().update();
    progressWnd().show();

    int err = config.launch(0, cargv.data());

    if (err)
    {
        std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
        messageBox(this, LANG("game launch", "ROM Start Error"), errorString, MB_OK);
        progressWnd().hide();
    }
}

void MainWnd::bootBootstrap(PerGameSettings &gameConfig, DSRomInfo &rominfo)
{
    dbg_printf("%s", _mainList->getSelectedShowName().c_str());
    std::string fileName = _mainList->getSelectedShowName();
    std::string fullPath = _mainList->getSelectedFullPath();

    BootstrapConfig config(fileName, 
		fullPath, 
		rominfo.saveInfo().gameCode,
		
		rominfo.saveInfo().gameSdkVersion, 
		rominfo.saveInfo().gameCRC, gameConfig.heapShrink);

    config.dsiMode(rominfo.isDSiWare() ? true : (gameConfig.dsiMode == PerGameSettings::EDefault ? ms().bstrap_dsiMode : (int)gameConfig.dsiMode))
		  .saveNo((int)gameConfig.saveNo)
		  .ramDiskNo((int)gameConfig.ramDiskNo)
		  .cpuBoost(gameConfig.boostCpu == PerGameSettings::EDefault ? ms().boostCpu : (bool)gameConfig.boostCpu)
		  .vramBoost(gameConfig.boostVram == PerGameSettings::EDefault ? ms().boostVram : (bool)gameConfig.boostVram)
		  .nightlyBootstrap(gameConfig.bootstrapFile == PerGameSettings::EDefault ? ms().bootstrapFile : (bool)gameConfig.bootstrapFile)
		  .wideScreen(gameConfig.wideScreen == PerGameSettings::EDefault ? ms().wideScreen : (bool)gameConfig.wideScreen);
	
	
	long cheatOffset; size_t cheatSize;
	if (CheatWnd::searchCheatData(GAME_CODE(rominfo.saveInfo().gameCode), 
			rominfo.saveInfo().headerCRC, cheatOffset, cheatSize)) {
		CheatWnd chtwnd((256)/2, (192)/2, 100, 100, NULL, fullPath);
		chtwnd.parse(fullPath);
		config.cheatData(chtwnd.getCheats());
	}
	
    // GameConfig is default, global is not default
    if (gameConfig.language == PerGameSettings::ELangDefault && ms().gameLanguage != TWLSettings::ELangDefault)
    {
        config.language(ms().gameLanguage);
    }
    // GameConfig is system, or global is defaut
    else if (gameConfig.language == PerGameSettings::ELangSystem || ms().gameLanguage == TWLSettings::ELangDefault)
    {
        config.language(-1);
    }
    else
    // gameConfig is not default
    {
        config.language(gameConfig.language);
    }

	if (!rominfo.isDSiWare()) {
		if (!config.checkCompatibility()) {
			int optionPicked = messageBox(this, LANG("game launch", "Compatibility Warning"), "This game is known to not run. If there's an nds-bootstrap version that fixes this, please ignore this message.", MB_OK | MB_CANCEL);
			progressWnd().hide();

			scanKeys();
			int pressed = keysHeld();

			if (pressed & KEY_B || optionPicked == ID_CANCEL)
			{
				return;
			}
		}
	}

	if (!rominfo.isDSiWare() && rominfo.requiresDonorRom()) {
		const char* pathDefine = "DONOR_NDS_PATH";
		const char* msg = "This game requires a donor ROM to run. Please switch the theme, and set an existing DS SDK5 game as a donor ROM.";
		if (rominfo.requiresDonorRom()==20) {
			pathDefine = "DONORE2_NDS_PATH";
			msg = "This game requires a donor ROM to run. Please switch the theme, and set an existing early SDK2 game as a donor ROM.";
		} else if (rominfo.requiresDonorRom()==2) {
			pathDefine = "DONOR2_NDS_PATH";
			msg = "This game requires a donor ROM to run. Please switch the theme, and set an existing late SDK2 game as a donor ROM.";
		} else if (rominfo.requiresDonorRom()==3) {
			pathDefine = "DONOR3_NDS_PATH";
			msg = "This game requires a donor ROM to run. Please switch the theme, and set an existing early SDK3 game as a donor ROM.";
		} else if (rominfo.requiresDonorRom()==51) {
			pathDefine = "DONORTWL_NDS_PATH";
			msg = "This game requires a donor ROM to run. Please switch the theme, and set an existing DSi-Enhanced game as a donor ROM.";
		}
		std::string donorRomPath;
		const char* bootstrapinipath = (sdFound() ? "sd:/_nds/nds-bootstrap.ini" : "fat:/_nds/nds-bootstrap.ini");
		CIniFile bootstrapini(bootstrapinipath);
		donorRomPath = bootstrapini.GetString("NDS-BOOTSTRAP", pathDefine, "");
		if (donorRomPath == "" || access(donorRomPath.c_str(), F_OK) != 0) {
			messageBox(this, LANG("game launch", "NDS Bootstrap Error"), msg, MB_OK);
			progressWnd().hide();
			return;
		}
	}

	if ((gameConfig.dsiMode == PerGameSettings::EDefault ? ms().bstrap_dsiMode : (int)gameConfig.dsiMode)
	 && !rominfo.isDSiWare() && !rominfo.hasExtendedBinaries()) {
		messageBox(this, LANG("game launch", "NDS Bootstrap Error"), "The DSi binaries are missing. Please get a clean dump of this ROM, or start in DS mode.", MB_OK);
		progressWnd().hide();
		return;
	}

	APFixType apType = config.hasAPFix();

	if (gameConfig.checkIfShowAPMsg() && !(apType == APFixType::EHasIPS || apType == APFixType::ENone)) {
		
        int optionPicked = 0;

		if (apType == APFixType::EMissingIPS)
		{
			optionPicked = messageBox(this, LANG("game launch", "ap warning"), LANG("game launch", "ap msg"), MB_OK | MB_HOLD_X | MB_CANCEL);
		}

		if (apType == APFixType::ERGFPatch)
		{
			optionPicked = messageBox(this, LANG("game launch", "ap warning"), LANG("game launch", "ap msg1"), MB_OK | MB_HOLD_X | MB_CANCEL);
		}

		scanKeys();
		int pressed = keysHeld();

		if (pressed & KEY_X || optionPicked == ID_HOLD_X)
		{
			gameConfig.dontShowAPMsgAgain();
		}

		if (pressed & KEY_B || optionPicked == ID_CANCEL || optionPicked == ID_NO)
		{
			return;
		}
	} 
	// Event handlers for progress window.
	config
        .onCheatsApplied(bootstrapCheatsHandler)
        .onWidescreenApply(bootstrapWidescreenHandler)
        .onWidescreenApply(bootstrapWidescreenApplied)
		.onBeforeSaveCreate(bootstrapBeforeSaveHandler)
		.onSaveCreated(bootstrapSaveHandler)
		.onConfigSaved(bootstrapLaunchHandler);

	progressWnd().setTipText(LANG("game launch", "Please wait"));
	progressWnd().update();
	progressWnd().show();

	int err = config.launch();
	if (err)
	{
		std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
		messageBox(this, LANG("game launch", "NDS Bootstrap Error"), errorString, MB_OK);
		progressWnd().hide();
	}
}

void MainWnd::bootFlashcard(const std::string &ndsPath, bool usePerGameSettings)
{
    int err = loadGameOnFlashcard(ndsPath.c_str(), usePerGameSettings);
    if (err)
    {
        std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
        messageBox(this, LANG("game launch", "Flashcard Error"), errorString, MB_OK);
    }
}

void MainWnd::bootFile(const std::string &loader, const std::string &fullPath)
{
    LoaderConfig config(loader, "");
    std::vector<const char *> argv{};
    argv.emplace_back(loader.c_str());
    argv.emplace_back(fullPath.c_str());
    int err = config.launch(argv.size(), argv.data());
    if (err)
    {
        std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
        messageBox(this, LANG("game launch", "Launch Error"), errorString, MB_OK);
        progressWnd().hide();
    }
}

void MainWnd::launchSelected()
{
    cwl();
    dbg_printf("Launch.");
    std::string fullPath = _mainList->getSelectedFullPath();

    cwl();
    if (fullPath[fullPath.size() - 1] == '/')
    {
        cwl();
        _mainList->enterDir(fullPath);
        return;
    }

	if (!ms().gotosettings && ms().consoleModel < 2 && ms().previousUsedDevice && bothSDandFlashcard()) {
		if (access("sd:/_nds/TWiLightMenu/tempDSiWare.dsi", F_OK) == 0) {
			remove("sd:/_nds/TWiLightMenu/tempDSiWare.dsi");
		}
		if (access("sd:/_nds/TWiLightMenu/tempDSiWare.pub", F_OK) == 0) {
			remove("sd:/_nds/TWiLightMenu/tempDSiWare.pub");
		}
		if (access("sd:/_nds/TWiLightMenu/tempDSiWare.prv", F_OK) == 0) {
			remove("sd:/_nds/TWiLightMenu/tempDSiWare.prv");
		}
	}

    ms().setCurrentRomFolder(_mainList->getCurrentDir());
    ms().previousUsedDevice = ms().secondaryDevice;
    ms().romPath[ms().secondaryDevice] = fullPath;

	// compatiblity with dsimenu theme. + 1 to account for back folder
	ms().cursorPosition[ms().secondaryDevice] = (_mainList->selectedRowId() + 1) % 40;
	ms().pagenum[ms().secondaryDevice] = (_mainList->selectedRowId() + 1) / 40;

    ms().slot1Launched = false;
    ms().saveSettings();

    if (ms().updateRecentlyPlayedList)
    {
        played().updateRecentlyPlayed(_mainList->getCurrentDir(), _mainList->getSelectedShowName());
        played().incrementTimesPlayed(_mainList->getCurrentDir(), _mainList->getSelectedShowName());
    }

    DSRomInfo rominfo;
    if (!_mainList->getRomInfo(_mainList->selectedRowId(), rominfo)) {
        return;
	}

	chdir(_mainList->getCurrentDir().c_str());

    // Launch DSiWare
    if (!rominfo.isHomebrew() && rominfo.isDSiWare() && (isDSiMode() || sdFound()) && ms().consoleModel == 0 && !ms().dsiWareBooter)
    {
        // Unlaunch boot here....
        UnlaunchBoot unlaunch(fullPath, rominfo.saveInfo().dsiPubSavSize, rominfo.saveInfo().dsiPrvSavSize);

        // Roughly associated with 50%, 90%
        unlaunch.onPrvSavCreated(bootstrapSaveHandler)
            .onPubSavCreated(bootstrapLaunchHandler);

            
        progressWnd().setPercent(0);
        progressWnd().setTipText(LANG("game launch", "Preparing Unlaunch Boot"));
        progressWnd().update();
        progressWnd().show();

        if (unlaunch.prepare())
        {
			progressWnd().hide();
            messageBox(this, LANG("game launch", "unlaunch boot"), LANG("game launch", "unlaunch instructions"), MB_OK);
        }
        ms().launchType[ms().secondaryDevice] = TWLSettings::EDSiWareLaunch;
        ms().saveSettings();
        progressWnd().hide();
        unlaunch.launch();
    }

    if (rominfo.isDSRom())
    {
        PerGameSettings gameConfig(_mainList->getSelectedShowName());
        // Direct Boot for homebrew.
        if (rominfo.isDSiWare() || (gameConfig.directBoot && rominfo.isHomebrew()))
        {
			std::string fileName = _mainList->getSelectedShowName();
			std::string fullPath = _mainList->getSelectedFullPath();

			ms().homebrewHasWide = (rominfo.saveInfo().gameCode[0] == 'W' || rominfo.version() == 0x57);
			ms().launchType[ms().secondaryDevice] = TWLSettings::ESDFlashcardDirectLaunch;
			ms().saveSettings();
			WidescreenConfig widescreen(fileName);
			widescreen
				.isHomebrew(true)
				.enable((gameConfig.wideScreen == PerGameSettings::EDefault ? ms().wideScreen : (bool)gameConfig.wideScreen))
				.apply();
            bootArgv(rominfo);
            return;
        }

        else if (ms().useBootstrap || !ms().secondaryDevice)
        {
			ms().launchType[ms().secondaryDevice] = TWLSettings::ESDFlashcardLaunch;
			ms().saveSettings();
            bootBootstrap(gameConfig, rominfo);
            return;
        }
        else
        {
			ms().launchType[ms().secondaryDevice] = TWLSettings::ESDFlashcardLaunch;
			ms().saveSettings();
            dbg_printf("Flashcard Launch: %s\n", fullPath.c_str());
            bootFlashcard(fullPath, true);
            return;
        }
    }

    std::string extension;
    size_t lastDotPos = fullPath.find_last_of('.');
    if (fullPath.npos != lastDotPos)
        extension = fullPath.substr(lastDotPos);

    // DSTWO Plugin Launch
    if (extension == ".plg" && ms().secondaryDevice && memcmp(io_dldi_data->friendlyName, "DSTWO(Slot-1)", 0xD) == 0)
    {
        ms().launchType[ms().secondaryDevice] = TWLSettings::ESDFlashcardLaunch;
        ms().saveSettings();

		// Print .plg path without "fat:" at the beginning
		char ROMpathDS2[256];
		for (int i = 0; i < 252; i++) {
			ROMpathDS2[i] = fullPath[4+i];
			if (fullPath[4+i] == '\x00') break;
		}

		CIniFile dstwobootini( "fat:/_dstwo/twlm.ini" );
		dstwobootini.SetString("boot_settings", "file", ROMpathDS2);
		dstwobootini.SaveIniFile( "fat:/_dstwo/twlm.ini" );

        bootFile(BOOTPLG_SRL, fullPath);
	}

	const char *ndsToBoot;

    // RVID Launch
    if (extension == ".rvid")
    {
        ms().homebrewArg = fullPath;
        ms().launchType[ms().secondaryDevice] = TWLSettings::ERVideoLaunch;
        ms().saveSettings();

		ndsToBoot = RVIDPLAYER_SD;
		if(!isDSiMode() || access(ndsToBoot, F_OK) != 0) {
			ndsToBoot = RVIDPLAYER_FC;
		}

        bootFile(ndsToBoot, fullPath);
    }

    // MPEG4 Launch
    if (extension == ".mp4")
    {
        ms().homebrewArg = fullPath;
        ms().launchType[ms().secondaryDevice] = TWLSettings::EMPEG4Launch;
        ms().saveSettings();

		ndsToBoot = MPEG4PLAYER_SD;
		if(!isDSiMode() || access(ndsToBoot, F_OK) != 0) {
			ndsToBoot = MPEG4PLAYER_FC;
		}

        bootFile(ndsToBoot, fullPath);
    }

    // GBA Launch
    if (extension == ".gba")
	{
        ms().homebrewArg = fullPath;
        ms().launchType[ms().secondaryDevice] = TWLSettings::ESDFlashcardLaunch;
        ms().saveSettings();
		if (ms().secondaryDevice)
        {
			if (REG_SCFG_EXT != 0)
			{
				ndsToBoot = ms().consoleModel>0 ? GBARUNNER_3DS : GBARUNNER_DSI;
				if(access(ndsToBoot, F_OK) != 0) {
					ndsToBoot = ms().consoleModel>0 ? GBARUNNER_3DS_FC : GBARUNNER_DSI_FC;
				}
			}
			else
			{
				ndsToBoot = ms().gbar2DldiAccess ? GBARUNNER_A7_SD : GBARUNNER_A9_SD;
				if(access(ndsToBoot, F_OK) != 0) {
					ndsToBoot = ms().gbar2DldiAccess ? GBARUNNER_A7 : GBARUNNER_A9;
				}
			}

            bootFile(ndsToBoot, fullPath);
		}
		else
		{
			std::string bootstrapPath = (ms().bootstrapFile ? BOOTSTRAP_NIGHTLY_HB : BOOTSTRAP_RELEASE_HB);

			std::vector<char*> argarray;
			argarray.push_back(strdup(bootstrapPath.c_str()));
			argarray.at(0) = (char*)bootstrapPath.c_str();

			const char* gbar2Path = ms().consoleModel>0 ? GBARUNNER_3DS : GBARUNNER_DSI;
			if (sys().arm7SCFGLocked()) {
				gbar2Path = ms().consoleModel>0 ? GBARUNNER_3DS_NODSP : GBARUNNER_DSI_NODSP;
			}

			LoaderConfig gen(bootstrapPath, BOOTSTRAP_INI);
			gen.option("NDS-BOOTSTRAP", "NDS_PATH", gbar2Path)
			   .option("NDS-BOOTSTRAP", "HOMEBREW_ARG", fullPath)
			   .option("NDS-BOOTSTRAP", "RAM_DRIVE_PATH", "")
			   .option("NDS-BOOTSTRAP", "LANGUAGE", ms().gameLanguage)
			   .option("NDS-BOOTSTRAP", "DSI_MODE", 0)
			   .option("NDS-BOOTSTRAP", "BOOST_CPU", 1)
			   .option("NDS-BOOTSTRAP", "BOOST_VRAM", 0);
			if (int err = gen.launch(argarray.size(), (const char **)&argarray[0], false))
			{
				std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
				messageBox(this, LANG("game launch", "nds-bootstrap error"), errorString, MB_OK);
			}
		}
	}

    // A26 Launch
    if (extension == ".a26")
    {
        ms().homebrewArg = fullPath;
        ms().launchType[ms().secondaryDevice] = TWLSettings::EStellaDSLaunch;
        ms().saveSettings();

		ndsToBoot = STELLADS_SD;
		if(!isDSiMode() || access(ndsToBoot, F_OK) != 0) {
			ndsToBoot = STELLADS_FC;
		}

        bootFile(ndsToBoot, fullPath);
    }

    // NES Launch
    if (extension == ".nes" || extension == ".fds")
    {
        ms().homebrewArg = fullPath;
        ms().launchType[ms().secondaryDevice] = TWLSettings::ENESDSLaunch;
        ms().saveSettings();

		ndsToBoot = (ms().secondaryDevice ? NESDS_SD : NESTWL_SD);
		if(!isDSiMode() || access(ndsToBoot, F_OK) != 0) {
			ndsToBoot = NESDS_FC;
		}

        bootFile(ndsToBoot, fullPath);
    }

    // GB Launch
    if (extension == ".gb" || extension == ".gbc")
    {
        ms().homebrewArg = fullPath;
        ms().launchType[ms().secondaryDevice] = TWLSettings::EGameYobLaunch;
        ms().saveSettings();

		ndsToBoot = GAMEYOB_SD;
		if(!isDSiMode() || access(ndsToBoot, F_OK) != 0) {
			ndsToBoot = GAMEYOB_FC;
		}

        bootFile(ndsToBoot, fullPath);
    }

    // SMS/GG Launch
    if (extension == ".sms" || extension == ".gg")
    {
		mkdir(ms().secondaryDevice ? "fat:/data" : "sd:/data", 0777);
		mkdir(ms().secondaryDevice ? "fat:/data/s8ds" : "sd:/data/s8ds", 0777);

		ms().homebrewArg = fullPath;
		if (!ms().secondaryDevice && !sys().arm7SCFGLocked() && ms().smsGgInRam)
		{
			ms().launchType[ms().secondaryDevice] = TWLSettings::ESDFlashcardLaunch;
			ms().saveSettings();

			std::string bootstrapPath = (ms().bootstrapFile ? BOOTSTRAP_NIGHTLY_HB : BOOTSTRAP_RELEASE_HB);

			std::vector<char*> argarray;
			argarray.push_back(strdup(bootstrapPath.c_str()));
			argarray.at(0) = (char*)bootstrapPath.c_str();

			LoaderConfig gen(bootstrapPath, BOOTSTRAP_INI);
			gen.option("NDS-BOOTSTRAP", "NDS_PATH", S8DS07_ROM)
			   .option("NDS-BOOTSTRAP", "HOMEBREW_ARG", "")
			   .option("NDS-BOOTSTRAP", "RAM_DRIVE_PATH", fullPath)
			   .option("NDS-BOOTSTRAP", "LANGUAGE", ms().gameLanguage)
			   .option("NDS-BOOTSTRAP", "DSI_MODE", 0)
			   .option("NDS-BOOTSTRAP", "BOOST_CPU", 1)
			   .option("NDS-BOOTSTRAP", "BOOST_VRAM", 0);
			if (int err = gen.launch(argarray.size(), (const char **)&argarray[0], false))
			{
				std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
				messageBox(this, LANG("game launch", "nds-bootstrap error"), errorString, MB_OK);
			}
		}
		else
		{
			ms().launchType[ms().secondaryDevice] = TWLSettings::ES8DSLaunch;
			ms().saveSettings();

			ndsToBoot = S8DS_ROM;
			if(!isDSiMode() || access(ndsToBoot, F_OK) != 0) {
				ndsToBoot = S8DS_FC;
			}

			bootFile(ndsToBoot, fullPath);
		}
    }
	
    // GEN Launch
    if (extension == ".gen")
	{
		bool usePicoDrive = ((isDSiMode() && sdFound() && sys().arm7SCFGLocked())
			|| ms().showMd==2 || (ms().showMd==3 && getFileSize(fullPath) > 0x300000));
        ms().homebrewArg = fullPath;
        ms().launchType[ms().secondaryDevice] = (usePicoDrive ? TWLSettings::EPicoDriveTWLLaunch : TWLSettings::ESDFlashcardLaunch);
        ms().saveSettings();
		if (usePicoDrive || ms().secondaryDevice)
        {
			ndsToBoot = usePicoDrive ? PICODRIVETWL_ROM : JENESISDS_ROM;
			if(!isDSiMode() || access(ndsToBoot, F_OK) != 0) {
				ndsToBoot = usePicoDrive ? PICODRIVETWL_FC : JENESISDS_FC;
			}

            bootFile(ndsToBoot, fullPath);
		}
		else
		{
			std::string bootstrapPath = (ms().bootstrapFile ? BOOTSTRAP_NIGHTLY_HB : BOOTSTRAP_RELEASE_HB);

			std::vector<char*> argarray;
			argarray.push_back(strdup(bootstrapPath.c_str()));
			argarray.at(0) = (char*)bootstrapPath.c_str();

			LoaderConfig gen(bootstrapPath, BOOTSTRAP_INI);
			gen.option("NDS-BOOTSTRAP", "NDS_PATH", JENESISDS_ROM)
			   .option("NDS-BOOTSTRAP", "HOMEBREW_ARG", "fat:/ROM.BIN")
			   .option("NDS-BOOTSTRAP", "RAM_DRIVE_PATH", fullPath)
			   .option("NDS-BOOTSTRAP", "LANGUAGE", ms().gameLanguage)
			   .option("NDS-BOOTSTRAP", "DSI_MODE", 0)
			   .option("NDS-BOOTSTRAP", "BOOST_CPU", 1)
			   .option("NDS-BOOTSTRAP", "BOOST_VRAM", 0);
			if (int err = gen.launch(argarray.size(), (const char **)&argarray[0], false))
			{
				std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
				messageBox(this, LANG("game launch", "nds-bootstrap error"), errorString, MB_OK);
			}
		}
	}

    // SNES Launch
    if (extension == ".smc" || extension == ".sfc")
	{
        ms().homebrewArg = fullPath;
        ms().launchType[ms().secondaryDevice] = TWLSettings::ESDFlashcardLaunch;
        ms().saveSettings();
		if (ms().secondaryDevice)
        {
			ndsToBoot = SNEMULDS_ROM;
			if(!isDSiMode() || access(ndsToBoot, F_OK) != 0) {
				ndsToBoot = SNEMULDS_FC;
			}

            bootFile(ndsToBoot, fullPath);
		}
		else
		{
			std::string bootstrapPath = (ms().bootstrapFile ? BOOTSTRAP_NIGHTLY_HB : BOOTSTRAP_RELEASE_HB);

			std::vector<char*> argarray;
			argarray.push_back(strdup(bootstrapPath.c_str()));
			argarray.at(0) = (char*)bootstrapPath.c_str();

			LoaderConfig snes(bootstrapPath, BOOTSTRAP_INI);
			snes.option("NDS-BOOTSTRAP", "NDS_PATH", SNEMULDS_ROM)
				.option("NDS-BOOTSTRAP", "HOMEBREW_ARG", "fat:/snes/ROM.SMC")
				.option("NDS-BOOTSTRAP", "RAM_DRIVE_PATH", fullPath)
			    .option("NDS-BOOTSTRAP", "LANGUAGE", ms().gameLanguage)
			    .option("NDS-BOOTSTRAP", "DSI_MODE", 0)
				.option("NDS-BOOTSTRAP", "BOOST_CPU", 0)
			    .option("NDS-BOOTSTRAP", "BOOST_VRAM", 0);
			if (int err = snes.launch(argarray.size(), (const char **)&argarray[0], false))
			{
				std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
				messageBox(this, LANG("game launch", "nds-bootstrap error"), errorString, MB_OK);
			}
		}
	}

    // PCE Launch
    if (extension == ".pce")
	{
        ms().homebrewArg = fullPath;
        ms().launchType[ms().secondaryDevice] = TWLSettings::ESDFlashcardLaunch;
        ms().saveSettings();
		if (ms().secondaryDevice)
        {
			ndsToBoot = NITROGRAFX_ROM;
			if(access(ndsToBoot, F_OK) != 0) {
				ndsToBoot = NITROGRAFX_FC;
			}

            bootFile(ndsToBoot, fullPath);
		}
		else
		{
			std::string bootstrapPath = (ms().bootstrapFile ? BOOTSTRAP_NIGHTLY_HB : BOOTSTRAP_RELEASE_HB);

			std::vector<char*> argarray;
			argarray.push_back(strdup(bootstrapPath.c_str()));
			argarray.at(0) = (char*)bootstrapPath.c_str();

			LoaderConfig snes(bootstrapPath, BOOTSTRAP_INI);
			snes.option("NDS-BOOTSTRAP", "NDS_PATH", NITROGRAFX_ROM)
			   .option("NDS-BOOTSTRAP", "HOMEBREW_ARG", fullPath)
			   .option("NDS-BOOTSTRAP", "RAM_DRIVE_PATH", "")
			    .option("NDS-BOOTSTRAP", "LANGUAGE", ms().gameLanguage)
			    .option("NDS-BOOTSTRAP", "DSI_MODE", 0)
				.option("NDS-BOOTSTRAP", "BOOST_CPU", 1)
			    .option("NDS-BOOTSTRAP", "BOOST_VRAM", 0);
			if (int err = snes.launch(argarray.size(), (const char **)&argarray[0], false))
			{
				std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
				messageBox(this, LANG("game launch", "nds-bootstrap error"), errorString, MB_OK);
			}
		}
	}
}

void MainWnd::onKeyBPressed()
{
    _mainList->backParentDir();
}

void MainWnd::showSettings(void)
{
    dbg_printf("Launch settings...");
	if (!isDSiMode()) {
		chdir("fat:/");
	} else if (sdFound()) {
		chdir("sd:/");
	}
    LoaderConfig settingsLoader(DSIMENUPP_SETTINGS_SRL, DSIMENUPP_INI);

    if (int err = settingsLoader.launch())
    {
        std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
        messageBox(this, LANG("game launch", "NDS Bootstrap Error"), errorString, MB_OK);
    }
}

void MainWnd::showManual(void)
{
    dbg_printf("Launch manual...");
	if (sdFound()) {
		chdir("sd:/");
	}
    LoaderConfig manualLoader(TWLMENUPP_MANUAL_SRL, DSIMENUPP_INI);

    if (int err = manualLoader.launch())
    {
        std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
        messageBox(this, LANG("game launch", "NDS Bootstrap Error"), errorString, MB_OK);
    }
}

void MainWnd::bootSlot1(void)
{
    dbg_printf("Launch Slot1..\n");
    ms().slot1Launched = true;
    ms().saveSettings();

    if (ms().slot1LaunchMethod==0 || sys().arm7SCFGLocked())
    {
        cardLaunch();
        return;
    }
	else if (ms().slot1LaunchMethod==2)
	{
        // Unlaunch boot here....
        UnlaunchBoot unlaunch("cart:", 0, 0);

        // Roughly associated with 50%, 90%
        unlaunch.onPrvSavCreated(bootstrapSaveHandler)
            .onPubSavCreated(bootstrapLaunchHandler);

            
        progressWnd().setPercent(0);
        progressWnd().setTipText(LANG("game launch", "Preparing Unlaunch Boot"));
        progressWnd().update();
        progressWnd().show();

        unlaunch.prepare();
		progressWnd().hide();
        unlaunch.launch();
	}

	sNDSHeader ndsCart;
	// Reset Slot-1 to allow reading card header
	sysSetCardOwner (BUS_OWNER_ARM9);
	disableSlot1();
	for(int i = 0; i < 25; i++) { swiWaitForVBlank(); }
	enableSlot1();
	for(int i = 0; i < 15; i++) { swiWaitForVBlank(); }

	cardReadHeader((uint8*)&ndsCart);

	char game_TID[5];
	memcpy(game_TID, ndsCart.gameCode, 4);
	game_TID[4] = 0;

	WidescreenConfig widescreen;
	widescreen
		.enable(ms().wideScreen)
		.gamePatch(game_TID, ndsCart.headerCRC16)
		.apply();

	if (sdFound()) {
		chdir("sd:/");
	}
    LoaderConfig slot1Loader(SLOT1_SRL, DSIMENUPP_INI);
    if (int err = slot1Loader.launch())
    {
        std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
        messageBox(this, LANG("game launch", "nds-bootstrap error"), errorString, MB_OK);
    }
}

void MainWnd::bootGbaRunner(void)
{
    if (ms().secondaryDevice && ms().useGbarunner)
    {
		if (ms().useBootstrap)
		{
			if (isDSiMode())
			{
				bootFile(ms().consoleModel>0 ? GBARUNNER_3DS_FC : GBARUNNER_DSI_FC, "");
			}
			else
			{
				bootFile(ms().gbar2DldiAccess ? GBARUNNER_A7 : GBARUNNER_A9, "");
			}
		}
		else
		{
			if (isDSiMode())
			{
				bootFlashcard(ms().consoleModel>0 ? GBARUNNER_3DS_FC : GBARUNNER_DSI_FC, false);
			}
			else
			{
				bootFlashcard(ms().gbar2DldiAccess ? GBARUNNER_A7 : GBARUNNER_A9, false);
			}
		}
        return;
    }

    if (!isDSiMode() && !ms().useGbarunner)
    {
        gbaSwitch();
        return;
    }

	std::string bootstrapPath = (ms().bootstrapFile ? BOOTSTRAP_NIGHTLY_HB : BOOTSTRAP_RELEASE_HB);

	std::vector<char*> argarray;
	argarray.push_back(strdup(bootstrapPath.c_str()));
	argarray.at(0) = (char*)bootstrapPath.c_str();

    LoaderConfig gbaRunner(bootstrapPath, BOOTSTRAP_INI);
	gbaRunner.option("NDS-BOOTSTRAP", "NDS_PATH", ms().consoleModel>0 ? GBARUNNER_3DS : GBARUNNER_DSI)
			 .option("NDS-BOOTSTRAP", "HOMEBREW_ARG", "")
			 .option("NDS-BOOTSTRAP", "RAM_DRIVE_PATH", "")
			 .option("NDS-BOOTSTRAP", "LANGUAGE", ms().gameLanguage)
			 .option("NDS-BOOTSTRAP", "DSI_MODE", 0)
			 .option("NDS-BOOTSTRAP", "BOOST_CPU", 1)
			 .option("NDS-BOOTSTRAP", "BOOST_VRAM", 0);
    if (int err = gbaRunner.launch(argarray.size(), (const char **)&argarray[0], false))
    {
        std::string errorString = formatString(LANG("game launch", "error").c_str(), err);
        messageBox(this, LANG("game launch", "NDS Bootstrap Error"), errorString, MB_OK);
    }
}

void MainWnd::showFileInfo()
{
    DSRomInfo rominfo;
    if (!_mainList->getRomInfo(_mainList->selectedRowId(), rominfo))
    {
        return;
    }

    dbg_printf("show '%s' info\n", _mainList->getSelectedFullPath().c_str());

    CIniFile ini(SFN_UI_SETTINGS); //(256-)/2,(192-128)/2, 220, 128
    u32 w = 240;
    u32 h = 144;
    w = ini.GetInt("rom info window", "w", w);
    h = ini.GetInt("rom info window", "h", h);

    RomInfoWnd *romInfoWnd = new RomInfoWnd((256 - w) / 2, (192 - h) / 2, w, h, this, LANG("rom info", "title"));
    std::string showName = _mainList->getSelectedShowName();
    std::string fullPath = _mainList->getSelectedFullPath();
    romInfoWnd->setFileInfo(fullPath, showName);
    romInfoWnd->setRomInfo(rominfo);
    romInfoWnd->doModal();
    rominfo = romInfoWnd->getRomInfo();
    _mainList->setRomInfo(_mainList->selectedRowId(), rominfo);

    delete romInfoWnd;
}

void MainWnd::onFolderChanged()
{
    resetInputIdle();
    std::string dirShowName = _mainList->getCurrentDir();


    if (!strncmp(dirShowName.c_str(), "^*::", 2))
    {

        if (dirShowName == SPATH_TITLEANDSETTINGS)
        {
            showSettings();
        }

        if (dirShowName == SPATH_MANUAL)
        {
            showManual();
        }

        if (dirShowName == SPATH_SLOT1)
        {
            bootSlot1();
        }

        if (dirShowName == SPATH_GBARUNNER)
        {
            bootGbaRunner();
        }

        if (dirShowName == SPATH_SYSMENU)
        {
            dsiSysMenuLaunch();
        }

        if (dirShowName == SPATH_SYSTEMSETTINGS)
        {
            dsiLaunchSystemSettings();
        }
        dirShowName.clear();
    }

    dbg_printf("%s\n", _mainList->getSelectedFullPath().c_str());

    _folderText->setText(dirShowName);
}

void MainWnd::onAnimation(bool &anAllow)
{
    if (_startMenu->isVisible())
        anAllow = false;
    else if (windowManager().currentWindow() != this)
        anAllow = false;
}

Window *MainWnd::windowBelow(const Point &p)
{
    Window *wbp = Form::windowBelow(p);
    if (_startMenu->isVisible() && wbp != _startButton)
        wbp = _startMenu;
    return wbp;
}