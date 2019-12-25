#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <sys/stat.h>
#include <unistd.h>

extern int guiLanguage;
extern int bstrap_language;
int setLanguage = 0;
int setGameLanguage = 0;

/**
 * Initialize translations.
 * Uses the language ID specified in settings.ui.language.
 */
void langInit(void)
{
	if (guiLanguage == -1) {
		setLanguage = PersonalData->language;
	} else {
		setLanguage = guiLanguage;
	}

	if (bstrap_language == -1) {
		setGameLanguage = PersonalData->language;
	} else {
		setGameLanguage = bstrap_language;
	}
}