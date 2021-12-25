
#include "myDSiMode.h"
#include "common/inifile.h"
#include "common/bootstrappaths.h"
#include "bootstrapsettings.h"
#include <string.h>

BootstrapSettings::BootstrapSettings()
{
    b4dsMode = 0;
    cacheFatTable = false;
    debug = false;
	logging = false;
	romreadled = BootstrapSettings::ELEDNone;
	dmaromreadled = BootstrapSettings::ELEDSame;
	consoleModel = -1;
}

void BootstrapSettings::loadSettings()
{
    CIniFile bootstrapini(BOOTSTRAP_INI);

    // UI settings.
   	debug = bootstrapini.GetInt("NDS-BOOTSTRAP", "DEBUG", debug);
	logging = bootstrapini.GetInt("NDS-BOOTSTRAP", "LOGGING", logging);
	cacheFatTable = bootstrapini.GetInt("NDS-BOOTSTRAP", "CACHE_FAT_TABLE", cacheFatTable);
	if (dsiFeatures()) {
		b4dsMode = bootstrapini.GetInt("NDS-BOOTSTRAP", "B4DS_MODE", b4dsMode);
		romreadled = bootstrapini.GetInt("NDS-BOOTSTRAP", "ROMREAD_LED", romreadled);
		dmaromreadled = bootstrapini.GetInt("NDS-BOOTSTRAP", "DMA_ROMREAD_LED", dmaromreadled);
	}
	consoleModel = bootstrapini.GetInt( "NDS-BOOTSTRAP", "CONSOLE_MODEL", consoleModel);
}

void BootstrapSettings::saveSettings()
{
    CIniFile bootstrapini(BOOTSTRAP_INI);

    // UI settings.
    bootstrapini.SetInt("NDS-BOOTSTRAP", "DEBUG", debug);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "LOGGING", logging);
	bootstrapini.SetInt("NDS-BOOTSTRAP", "CACHE_FAT_TABLE", cacheFatTable);
	if (dsiFeatures()) {
		bootstrapini.SetInt("NDS-BOOTSTRAP", "B4DS_MODE", b4dsMode);
		bootstrapini.SetInt("NDS-BOOTSTRAP", "ROMREAD_LED", romreadled);
		bootstrapini.SetInt("NDS-BOOTSTRAP", "DMA_ROMREAD_LED", dmaromreadled);
	}
	bootstrapini.SetInt( "NDS-BOOTSTRAP", "CONSOLE_MODEL", consoleModel);
    bootstrapini.SaveIniFile(BOOTSTRAP_INI);
}
