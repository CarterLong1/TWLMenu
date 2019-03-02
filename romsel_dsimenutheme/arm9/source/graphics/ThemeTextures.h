#pragma once
#ifndef __DSIMENUPP_THEME_TEXTURES__
#define __DSIMENUPP_THEME_TEXTURES__
#include "common/gl2d.h"
#include "common/singleton.h"
#include "GritTexture.h"
#include "BmpTexture.h"
#include <memory>
#include <string>
#include <algorithm>


#define BG_BUFFER_PIXELCOUNT 256 * 192

using std::unique_ptr;
using std::min;

class ThemeTextures
{

public:
  ThemeTextures();
  virtual ~ThemeTextures() = default;

public:
  void loadDSiTheme();
  void load3DSTheme();

  void reloadPalDialogBox();

  static unsigned short convertToDsBmp(unsigned short val);

private:
  void loadVolumeTextures();
  void loadBatteryTextures();
  void loadUITextures();
  void loadIconTextures();

public:
  static unsigned short *beginSubModify();
  static void commitSubModify();  
  static void commitSubModifyAsync();

  void drawTopBg();
  void drawTopBgAvoidingShoulders();

  void drawProfileName();
  void drawBottomBubbleBg();
  void drawBottomMovingBg();
  void drawBottomBg();

  void drawBoxArt(const char* filename);

  void drawVolumeImage(int volumeLevel);
  void drawVolumeImageCached();

  void drawBatteryImage(int batteryLevel, bool drawDSiMode, bool isRegularDS);
  void drawBatteryImageCached();

  void drawShoulders(bool showLShoulder, bool showRshoulder) ;
  void drawDateTime(const char* date, const int posX, const int posY, const int drawCount, int *hourWidthPointer);

  void clearTopScreen();
  static void videoSetup();
private:
  void applyGrayscaleToAllGrfTextures();

  void loadBubbleImage(const GritTexture& tex, int sprW, int sprH);
  void loadProgressImage(const GritTexture& tex);
  void loadDialogboxImage(const GritTexture& tex);
  void loadBipsImage(const GritTexture& tex);
  void loadScrollwindowImage(const GritTexture& tex);
  void loadButtonarrowImage(const GritTexture& tex);
  void loadMovingarrowImage(const GritTexture& tex);
  void loadLaunchdotImage(const GritTexture& tex);
  void loadStartImage(const GritTexture& tex);
  void loadStartbrdImage(const GritTexture& tex, int sprH);
  void loadBraceImage(const GritTexture& tex);
  void loadSettingsImage(const GritTexture& tex);
  void loadBoxfullImage(const GritTexture& tex);
  void loadBoxemptyImage(const GritTexture& tex);
  void loadFolderImage(const GritTexture& tex);
  void loadCornerButtonImage(const GritTexture& tex, int arraysize,
											int sprW, int sprH);
  void loadSmallCartImage(const GritTexture& tex);
  void loadWirelessIcons(const GritTexture& tex);

  void loadBackgrounds();

  void loadDateFont(const unsigned short *bitmap);

  static unsigned int getTopFontSpriteIndex(const u16 character);
  static unsigned int getDateTimeFontSpriteIndex(const u16 character);

  static int getVolumeLevel();
  static int getBatteryLevel();

private:

  /**
   * Allocates space for and loads a glTexture into memory, returning a 
   * unique_ptr to the glImage array, and sets textureId to the ID of the
   * loaded texture.
   * 
   * arraySize is the size of the glImage array.
   */
  unique_ptr<glImage[]> loadTexture(
            int *textureId, const GritTexture& texture,
						unsigned int arraySize,
						int sprW, int sprH);

public:
  const glImage *bubbleImage() { return _bubbleImage.get(); }
  const glImage *progressImage() { return _progressImage.get(); }
  const glImage *dialogboxImage() { return _dialogboxImage.get(); }
  const glImage *bipsImage() { return _bipsImage.get(); }
  const glImage *scrollwindowImage() { return _scrollwindowImage.get(); }
  const glImage *buttonarrowImage() { return _buttonarrowImage.get(); }
  const glImage *movingArrowImage() { return _movingarrowImage.get(); }
  const glImage *launchdotImage() { return _launchdotImage.get(); }
  const glImage *startImage() { return _startImage.get(); }
  const glImage *startbrdImage() { return _startbrdImage.get(); }
  const glImage *braceImage() { return _braceImage.get(); }
  const glImage *settingsImage() { return _settingsImage.get(); }
  const glImage *boxfullImage() { return _boxfullImage.get(); }
  const glImage *boxemptyImage() { return _boxemptyImage.get(); }
  const glImage *folderImage() { return _folderImage.get(); }
  const glImage *cornerButtonImage() { return _cornerButtonImage.get(); }
  const glImage *smallCartImage() { return _smallCartImage.get(); }
  const glImage *wirelessIcons() { return _wirelessIcons.get(); }


  const GritTexture *iconGBTexture() { return _iconGBTexture.get(); }
  const GritTexture *iconGBATexture() { return _iconGBATexture.get(); }
  const GritTexture *iconGBAModeTexture() { return _iconGBAModeTexture.get(); }
  const GritTexture *iconGGTexture() { return _iconGGTexture.get(); }
  const GritTexture *iconMDTexture() { return _iconMDTexture.get(); }
  const GritTexture *iconNESTexture() { return _iconNESTexture.get(); }
  const GritTexture *iconSMSTexture() { return _iconSMSTexture.get(); }
  const GritTexture *iconSNESTexture() { return _iconSNESTexture.get(); }
  const GritTexture *iconUnknownTexture() { return _iconUnknownTexture.get(); }
  
  const BmpTexture *topBackgroundTexture() { return _topBackgroundTexture.get(); }

  const BmpTexture *dateTimeFontTexture() { return _dateTimeFontTexture.get(); }
  const BmpTexture *leftShoulderTexture() { return _leftShoulderTexture.get(); }
  const BmpTexture *rightShoulderTexture() { return _rightShoulderTexture.get(); }
  const BmpTexture *leftShoulderGreyedTexture() { return _leftShoulderGreyedTexture.get(); }
  const BmpTexture *rightShoulderGreyedTexture() { return _rightShoulderGreyedTexture.get(); }

  static u16* bmpImageBuffer();

  const BmpTexture *volumeTexture(int texture) { 
    switch(texture) {
      case 4:
        return _volume4Texture.get();
      case 3:
        return _volume3Texture.get();
      case 2:
        return _volume2Texture.get();
      case 1:
        return _volume1Texture.get();
      case 0:
      default:
        return _volume0Texture.get();
    }
  }

  
  const BmpTexture *batteryTexture(int texture, bool dsiMode, bool regularDS) { 
    if (dsiMode) {
      switch(texture) {
        case 7:
          return _batterychargeTexture.get();
        case 4:
          return _battery4Texture.get();
        case 3:
          return _battery3Texture.get();
        case 2:
          return _battery2Texture.get();
        case 1:
          return _battery1Texture.get();
        case 0:
        default:
          return _battery1Texture.get();
      }
    } else {
      switch (texture)
      {
        case 1:
          return _batterylowTexture.get();
        case 0:
        default:
          return regularDS ? _batteryfullDSTexture.get() : _batteryfullTexture.get();
      }
    }
  }

private:
  unique_ptr<glImage[]> _progressImage;
  unique_ptr<glImage[]> _dialogboxImage;
  unique_ptr<glImage[]> _bipsImage;
  unique_ptr<glImage[]> _scrollwindowImage;
  unique_ptr<glImage[]> _buttonarrowImage;
  unique_ptr<glImage[]> _movingarrowImage;
  unique_ptr<glImage[]> _launchdotImage;
  unique_ptr<glImage[]> _startImage;
  unique_ptr<glImage[]> _startbrdImage;
  unique_ptr<glImage[]> _braceImage;
  unique_ptr<glImage[]> _settingsImage;
  unique_ptr<glImage[]> _boxfullImage;
  unique_ptr<glImage[]> _boxemptyImage;
  unique_ptr<glImage[]> _folderImage;
  unique_ptr<glImage[]> _cornerButtonImage;
  unique_ptr<glImage[]> _smallCartImage;
  unique_ptr<glImage[]> _wirelessIcons;
  unique_ptr<glImage[]> _bubbleImage;

  unique_ptr<GritTexture> _bipsTexture;
  unique_ptr<GritTexture> _boxTexture;
  unique_ptr<GritTexture> _braceTexture;
  unique_ptr<GritTexture> _bubbleTexture;
  unique_ptr<GritTexture> _buttonArrowTexture;
  unique_ptr<GritTexture> _cornerButtonTexture;
  unique_ptr<GritTexture> _dialogBoxTexture;
  unique_ptr<GritTexture> _folderTexture;
  unique_ptr<GritTexture> _launchDotTexture;
  unique_ptr<GritTexture> _movingArrowTexture;
  unique_ptr<GritTexture> _progressTexture;
  unique_ptr<GritTexture> _scrollWindowTexture;
  unique_ptr<GritTexture> _smallCartTexture;
  unique_ptr<GritTexture> _startBorderTexture;
  unique_ptr<GritTexture> _startTextTexture;
  unique_ptr<GritTexture> _wirelessIconsTexture;
  unique_ptr<GritTexture> _settingsIconTexture;

  unique_ptr<GritTexture> _boxFullTexture;
  unique_ptr<GritTexture> _boxEmptyTexture;

  unique_ptr<GritTexture> _iconGBTexture;
  unique_ptr<GritTexture> _iconGBATexture;
  unique_ptr<GritTexture> _iconGBAModeTexture;
  unique_ptr<GritTexture> _iconGGTexture;
  unique_ptr<GritTexture> _iconMDTexture;
  unique_ptr<GritTexture> _iconNESTexture;
  unique_ptr<GritTexture> _iconSMSTexture;
  unique_ptr<GritTexture> _iconSNESTexture;
  unique_ptr<GritTexture> _iconUnknownTexture;

  unique_ptr<BmpTexture> _volume0Texture;
  unique_ptr<BmpTexture> _volume1Texture;
  unique_ptr<BmpTexture> _volume2Texture;
  unique_ptr<BmpTexture> _volume3Texture;
  unique_ptr<BmpTexture> _volume4Texture;

  unique_ptr<BmpTexture> _battery1Texture;
  unique_ptr<BmpTexture> _battery2Texture;
  unique_ptr<BmpTexture> _battery3Texture;
  unique_ptr<BmpTexture> _battery4Texture;
  unique_ptr<BmpTexture> _batterychargeTexture;
  unique_ptr<BmpTexture> _batteryfullTexture;
  unique_ptr<BmpTexture> _batteryfullDSTexture;
  unique_ptr<BmpTexture> _batterylowTexture;

  unique_ptr<BmpTexture> _topBackgroundTexture;
  unique_ptr<BmpTexture> _dateTimeFontTexture;
  unique_ptr<BmpTexture> _leftShoulderTexture;
  unique_ptr<BmpTexture> _rightShoulderTexture;
  unique_ptr<BmpTexture> _leftShoulderGreyedTexture;
  unique_ptr<BmpTexture> _rightShoulderGreyedTexture;

  unique_ptr<u16[]> _bottomMovingBgImage;

  unique_ptr<u16[]> _dateFontImage;

private:
  int bubbleTexID;
  int bipsTexID;
  int scrollwindowTexID;
  int buttonarrowTexID;
  int movingarrowTexID;
  int launchdotTexID;
  int startTexID;
  int startbrdTexID;
  int settingsTexID;
  int braceTexID;
  int boxfullTexID;
  int boxemptyTexID;
  int folderTexID;
  int cornerButtonTexID;
  int smallCartTexID;

  int progressTexID;
  int dialogboxTexID;
  int wirelessiconTexID;

private:
  int _cachedVolumeLevel;
  int _cachedBatteryLevel;
};


//   xbbbbbgggggrrrrr according to http://problemkaputt.de/gbatek.htm#dsvideobgmodescontrol


typedef singleton<ThemeTextures> themeTextures_s;
inline ThemeTextures &tex() { return themeTextures_s::instance(); }

#endif