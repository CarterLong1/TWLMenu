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
#pragma once
void iconTitleInit();
void loadConsoleIcons();
void getGameInfo(bool isDir, const char* name);
void iconUpdate(bool isDir, const char* name);
void titleUpdate(bool isDir, const char* name);
void drawIcon(int Xpos, int Ypos);
void drawIconFolder(int Xpos, int Ypos);
void drawIconPlg(int Xpos, int Ypos);
void drawIconGBA(int Xpos, int Ypos);
void drawIconGB(int Xpos, int Ypos);
void drawIconGBC(int Xpos, int Ypos);
void drawIconNES(int Xpos, int Ypos);
void drawIconSMS(int Xpos, int Ypos);
void drawIconGG(int Xpos, int Ypos);
void drawIconMD(int Xpos, int Ypos);
void drawIconSNES(int Xpos, int Ypos);
