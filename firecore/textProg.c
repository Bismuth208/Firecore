#include <avr/pgmspace.h>  // for progmem

#include "textProg.h"

//---------------------------------------------------------------------------//

const uint8_t emptyText[] PROGMEM = {" "};
const uint8_t pauseP0[] PROGMEM = {"Pause"};
const uint8_t pressAtext[] PROGMEM = {"Prees A"};
const uint8_t versionP0[] PROGMEM = {"V0.1a"};
const uint8_t creditP0[] PROGMEM = {"2017 @Bismuth208"};
const uint8_t scoreP[] PROGMEM = {"Score: "};
const uint8_t maxScoreP[] PROGMEM = {"Max: "};

const uint8_t selectShipP[] PROGMEM = {"Select ship"};
const uint8_t shipSpeedStatP[] PROGMEM = {"Speed: "};
const uint8_t shipPowerStatP[] PROGMEM = {"Power: "};
const uint8_t shipDurabStatP[] PROGMEM = {"Armor: "};


const uint8_t gameOverP[] PROGMEM = {"Game Over"};
const uint8_t levelClearP[] PROGMEM = {"Level clear"};
const uint8_t victoryP[] PROGMEM = {"Victory!"};


const uint8_t buttonA[] PROGMEM = {"'A'"};
const uint8_t buttonB[] PROGMEM = {"'B'"};
const uint8_t buttonX[] PROGMEM = {"'X'"};
const uint8_t buttonY[] PROGMEM = {"'Y'"};
const uint8_t * const buttonsAll[] PROGMEM = {buttonA, buttonB, buttonX, buttonY};


const uint8_t dogeP1[] PROGMEM = {"WOOF WOOF, I'M DOGE, WOW!"};
const uint8_t dogeP2[] PROGMEM = {"YOU MUST TO..."};
const uint8_t dogeP3[] PROGMEM = {"SAVE THE GALAXY..."};
const uint8_t dogeP4[] PROGMEM = {"AND OUR HOME PLANET..."};
const uint8_t dogeP5[] PROGMEM = {"INVADERS ARE COMING..."};
const uint8_t dogeP6[] PROGMEM = {"'STATIC NOISE...'"};
const uint8_t * const dogePA[] PROGMEM = {dogeP1, dogeP2, dogeP3, dogeP4, dogeP5, dogeP6};


const uint8_t modeP1[] PROGMEM = {"Story"};
const uint8_t modeP2[] PROGMEM = {"Endless"};
const uint8_t * const modePA[] PROGMEM = {modeP1, modeP2};


const uint8_t worldP1[] PROGMEM = {"Nautrem 3"};
const uint8_t worldP2[] PROGMEM = {"Rhaod"};
const uint8_t worldP3[] PROGMEM = {"Umigogan"};
const uint8_t worldP4[] PROGMEM = {"Tilicien"};
const uint8_t worldP5[] PROGMEM = {"Arceus 10"};
const uint8_t worldP6[] PROGMEM = {"Viliviel"};
const uint8_t worldP7[] PROGMEM = {"Myri"};
const uint8_t worldP8[] PROGMEM = {"Uniclya"};
const uint8_t worldP9[] PROGMEM = {"Home world"};
const uint8_t * const worldPA[] PROGMEM = {
	worldP1, worldP2, worldP3, worldP4,
	worldP5, worldP6, worldP7, worldP8,
	worldP9
};

//---------------------------------------------------------------------------//
