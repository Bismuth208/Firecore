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

const uint8_t emptyText[] PROGMEM = {" "};
const uint8_t pauseP0[] PROGMEM = {"PAUSE"};
const uint8_t pressAtext[] PROGMEM = {"PRESS A"};
const uint8_t versionP0[] PROGMEM = {"V0.2A"};
const uint8_t creditP0[] PROGMEM = {"2017 @BISMUTH208"};
const uint8_t scoreP[] PROGMEM = {"SCORE: "};
const uint8_t maxScoreP[] PROGMEM = {"MAX: "};

const uint8_t selectShipP[] PROGMEM = {"SELECT SHIP"};
const uint8_t shipSpeedStatP[] PROGMEM = {"SPEED: "};
const uint8_t shipPowerStatP[] PROGMEM = {"POWER: "};
const uint8_t shipDurabStatP[] PROGMEM = {"ARMOR: "};


const uint8_t gameOverP[] PROGMEM = {"GAME OOVER"};
const uint8_t levelClearP[] PROGMEM = {"LEVEL CLEAR"};
const uint8_t victoryP[] PROGMEM = {"VICTORY!"};


const uint8_t buttonA[] PROGMEM = {"'A'"};
const uint8_t buttonB[] PROGMEM = {"'B'"};
const uint8_t buttonX[] PROGMEM = {"'X'"};
const uint8_t buttonY[] PROGMEM = {"'Y'"};
const uint8_t * const buttonsAll[] PROGMEM = {buttonA, buttonB, buttonX, buttonY};


const uint8_t dogeP1[] PROGMEM = {"WOOF WOOF, I'M DOGE, WOW!"};
const uint8_t dogeP2[] PROGMEM = {"HERO, YOU MUST TO"};
const uint8_t dogeP3[] PROGMEM = {"SAVE THE GALAXY!"};
const uint8_t dogeP4[] PROGMEM = {"AND OUR HOME PLANET!"};
const uint8_t dogeP5[] PROGMEM = {"'STATIC NOISE...'"};
const uint8_t dogeP6[] PROGMEM = {"INVADERS ARE COMING..."};
const uint8_t dogeP7[] PROGMEM = {"..WE...EED YO..R...ELP!"};
const uint8_t * const dogePA[] PROGMEM = {
	dogeP1, dogeP2, dogeP3,
	dogeP4,	dogeP5, dogeP6,
	dogeP7
};


const uint8_t modeP1[] PROGMEM = {"STOY"};
const uint8_t modeP2[] PROGMEM = {"ENDLESS"};
const uint8_t * const modePA[] PROGMEM = {modeP1, modeP2};

const uint8_t historyTextP[] PROGMEM = {"IN FAR FAR AWAY GALAXY,\nAFTER LAST HOLYWAR, WHAT\nWAS THOUSANDS YEARS AGO,\nTWO SPECIES LIVES IN\nDIFFERENT PARTS OF\nTHE GALAXY.\nIN SILENCE AND PEACE.\s\s\s\s\s\s\s\s\s\s\s\r\fUNTIL NOW..."};

const uint8_t worldP1[] PROGMEM = {"NAUTREM 3"};
const uint8_t worldP2[] PROGMEM = {"RHAOD"};
const uint8_t worldP3[] PROGMEM = {"UMIGOGAN"};
const uint8_t worldP4[] PROGMEM = {"ILICON 5"};
const uint8_t worldP5[] PROGMEM = {"ARCEUS 10"};
const uint8_t worldP6[] PROGMEM = {"VULKAN"};
const uint8_t worldP7[] PROGMEM = {"MYRI"};
const uint8_t worldP8[] PROGMEM = {"UNICLYA"};
const uint8_t worldP9[] PROGMEM = {"HOME WORLD"};
const uint8_t * const worldPA[] PROGMEM = {
	worldP1, worldP2, worldP3, worldP4,
	worldP5, worldP6, worldP7, worldP8,
	worldP9
};

//---------------------------------------------------------------------------//
