/*
 * Author: Antonov Alexandr (Bismuth208)
 * e-mail: bismuth20883@gmail.com
 * 
 *  THIS PROJECT IS PROVIDED FOR EDUCATION/HOBBY USE ONLY
 *  NO PROTION OF THIS WORK CAN BE USED IN COMMERIAL
 *  APPLICATION WITHOUT WRITTEN PERMISSION FROM THE AUTHOR
 *  
 *  EVERYONE IS FREE TO POST/PUBLISH THIS ARTICLE IN
 *  PRINTED OR ELECTRONIC FORM IN FREE/PAID WEBSITES/MAGAZINES/BOOKS
 *  IF PROPER CREDIT TO ORIGINAL AUTHOR IS MENTIONED WITH LINKS TO
 *  ORIGINAL ARTICLE
 */

#include <avr/pgmspace.h>  // for progmem

#include "textProg.h"

//---------------------------------------------------------------------------//

text_t emptyText[] PROGMEM = {" "};
text_t pauseP0[] PROGMEM = {"PAUSE"};
text_t pressAtext[] PROGMEM = {"PRESS A"};
text_t versionP0[] PROGMEM = {"V0.5A"};
text_t scoreP[] PROGMEM = {"SCORE: "};
text_t maxScoreP[] PROGMEM = {"MAX: "};

text_t selectShipP[] PROGMEM = {"SELECT SHIP"};
text_t shipSpeedStatP[] PROGMEM = {"SPEED: "};
text_t shipPowerStatP[] PROGMEM = {"POWER: "};
text_t shipDurabStatP[] PROGMEM = {"ARMOR: "};


text_t gameOverP[] PROGMEM = {"GAME OOVER"};
text_t levelClearP[] PROGMEM = {"LEVEL CLEAR"};
text_t victoryP[] PROGMEM = {"VICTORY!"};

text_t bossWarningP[] PROGMEM = {"!WARNING!"};


text_t buttonA[] PROGMEM = {"'A'"};
text_t buttonB[] PROGMEM = {"'B'"};
text_t buttonX[] PROGMEM = {"'X'"};
text_t buttonY[] PROGMEM = {"'Y'"};
text_t * const buttonsAll[] PROGMEM = {buttonA, buttonB, buttonX, buttonY};


text_t dogeP1[] PROGMEM = {"WOOF WOOF, I'M DOGE, WOW!"};
text_t dogeP2[] PROGMEM = {"HERO, YOU MUST TO"};
text_t dogeP3[] PROGMEM = {"SAVE THE GALAXY!"};
text_t dogeP4[] PROGMEM = {"AND OUR HOME PLANET!"};
text_t dogeP5[] PROGMEM = {"'STATIC NOISE...'"};
text_t dogeP6[] PROGMEM = {"INVADERS ARE COMING..."};
text_t dogeP7[] PROGMEM = {"..WE...EED YO..R...ELP!"};
text_t * const dogePA[] PROGMEM = {
	dogeP1, dogeP2, dogeP3,
	dogeP4,	dogeP5, dogeP6,
	dogeP7, 0
};


text_t modeP1[] PROGMEM = {" STORY"};
text_t modeP2[] PROGMEM = {"ENDLESS"};
text_t modeP3[] PROGMEM = {"CREDITS"};
text_t * const modePA[] PROGMEM = {modeP1, modeP2, modeP3};

text_t historyTextP[] PROGMEM = {"IN FAR FAR AWAY GALAXY,\nAFTER LAST HOLYWAR, WHAT\nWAS THOUSANDS YEARS AGO,\nTWO SPECIES LIVES IN\nDIFFERENT PARTS OF\nTHE GALAXY.\nIN SILENCE AND PEACE.\a\a\a\a\a\a\a\a\a\a\a\r\fUNTIL NOW..."};
text_t victoryTextP[] PROGMEM = {""};

text_t worldP0[] PROGMEM = {"ASTEROIDS!!!"};
text_t worldP1[] PROGMEM = {"NAUTREM 3"};
text_t worldP2[] PROGMEM = {"RHAOD"};
text_t worldP3[] PROGMEM = {"UMIGOGAN"};
text_t worldP4[] PROGMEM = {"ILICON 5"};
text_t worldP5[] PROGMEM = {"ARCEUS 10"};
text_t worldP6[] PROGMEM = {"VULKAN"};
text_t worldP7[] PROGMEM = {"MYRI"};
text_t worldP8[] PROGMEM = {"UNICLYA"};
text_t worldP9[] PROGMEM = {"HOME WORLD"};
text_t * const worldPA[] PROGMEM = {
	worldP1, worldP2,
  worldP0,
  worldP3, worldP4,
  worldP0,
	worldP5, worldP6,
  worldP0,
  worldP7, worldP8,
  worldP0,
	worldP9
};

text_t creditP0[] PROGMEM = {"2017 @BISMUTH208"};

text_t creditsTextP0[] PROGMEM = {"THANK YOU\n\tFOR PLAYING\n\t\tTHIS GAME!"};
text_t creditsTextP1[] PROGMEM = {"BONUS SHIP\n\tIS NOW\n\t\tUNLOCKED!"};
//text_t creditsTextP2[] PROGMEM = {"ORIGINAL\n\tGAME LOCATION\n\t\t\tIS HERE"};
text_t creditsTextP3[] PROGMEM = {"PROGRAMER:\n\tME (@BISMUTH208)"};
text_t creditsTextP4[] PROGMEM = {"ARTIST1:\n\tME (@BISMUTH208)"};
text_t creditsTextP5[] PROGMEM = {"ARTIST2:\n\tMY WIFE (@VOKALOID18)"};
text_t creditsTextP6[] PROGMEM = {"SPECIAL\n\tTHANKS TO:\n\t\tMY WIFE!"};
text_t creditsTextP7[] PROGMEM = {"WHO\n\tWAS MY\n\t\tBETA TESTER,"};
text_t creditsTextP8[] PROGMEM = {"AND\n\tWHO INSPIRE ME\n\t\tALL THIS TIME!"};
text_t * const creditsPA[] PROGMEM = {
  creditsTextP0, creditsTextP1,
  /*creditsTextP2,*/ creditsTextP3,
  creditsTextP4, creditsTextP5,
  creditsTextP6, creditsTextP7,
  creditsTextP8, 0
};
//---------------------------------------------------------------------------//
