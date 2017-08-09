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

#include <esploraAPI.h>

#include "taskmanager.h"

#include "pics.h"
#include "textProg.h"
#include "common.h"

uint8_t galaxySaved = 0;
int8_t currentShip = 2;
int8_t previousShip =0;
uint8_t dogeDialogs =0;
bool iconState = false;

// -------------------------- Main menu -------------------------- //
void drawShipSelectionMenu(void)
{
  uint16_t posX = CHARACTER_ICON_OFFSET_X;
  uint8_t characterIconStep = (TFT_W /(CHARACTER_ICON_NUM+galaxySaved));
  screenSliderEffect(currentBackGroundColor);

  for(uint8_t count=0; count<(CHARACTER_ICON_NUM+galaxySaved); count++) {
    drawFrame(posX, CHARACTER_ICON_OFFSET_Y,
                CHARACTER_FRAME_WH, CHARACTER_FRAME_WH, INDIGO_COLOR, COLOR_WHITE);

    drawBMP_RLE_P(posX, CHARACTER_ICON_OFFSET_Y+1, getConstCharPtr(catsPics, count));

    posX += characterIconStep;
  }
  
  // make tiny frame for ship
  tftDrawRect(SHIP_SELECT_POS_X-5, SHIP_SELECT_POS_Y-4, SHIP_PIC_W+10, SHIP_PIC_H+10, COLOR_WHITE);
  drawTextWindow(selectShipP, buttonA);
  addShipSelectTasks();
}

void drawCurrentShipSelection(void)
{
  iconState = !iconState;

  uint8_t characterIconStep = (TFT_W /(CHARACTER_ICON_NUM+galaxySaved));
  uint16_t posX = characterIconStep*(currentShip-1) + CHARACTER_ICON_OFFSET_X;
  uint16_t color = iconState ? COLOR_WHITE : COLOR_BLACK;
  tftDrawRect(posX, CHARACTER_ICON_OFFSET_Y, CHARACTER_FRAME_WH, CHARACTER_FRAME_WH, color);

  if(currentShip != previousShip) {
#if ADD_SOUND
    sfxPlayTick();
#endif

    // update ship pic
    ship.type = currentShip-1;
    ship.pBodyPic = getConstCharPtr(shipsPics, ship.type);

    posX = characterIconStep*(previousShip-1) + CHARACTER_ICON_OFFSET_X;
    tftDrawRect(posX, CHARACTER_ICON_OFFSET_Y, CHARACTER_FRAME_WH, CHARACTER_FRAME_WH, COLOR_BLACK);

    drawFrame(0, 55, 100, 30, INDIGO_COLOR, COLOR_WHITE); // frame for ship's params

    previousShip = currentShip;
    updateShipStates();

    uint8_t lineSize = mapVal(ship.states.speed, 0, SHIP_BASE_SPEED, 0, 40);
    tftDrawFastHLine(BASE_STATS_POS_X, BASE_STATS_POS_Y, lineSize, COLOR_WHITE);
    tftPrintAt_P(4, BASE_STATS_POS_Y-2, (const char*)shipSpeedStatP);

    lineSize = mapVal(ship.states.power, 0, SHIP_BASE_DAMAGE, 0, 40);
    tftDrawFastHLine(BASE_STATS_POS_X, BASE_STATS_POS_Y+8, lineSize, COLOR_WHITE);
    tftPrintAt_P(4, BASE_STATS_POS_Y+6, (const char*)shipPowerStatP);

    lineSize = mapVal(ship.states.durability, 0, SHIP_BASE_DURAB, 0, 40);
    tftDrawFastHLine(BASE_STATS_POS_X, BASE_STATS_POS_Y+16, lineSize, COLOR_WHITE);
    tftPrintAt_P(4, BASE_STATS_POS_Y+14, (const char*)shipDurabStatP);
  }
}

void getShipItem(void)
{
  uint16_t newValueXY = getStickVal(LINE_X);
  if(newValueXY != calJoysticY) {
    if(newValueXY < calJoysticY) {
      ++currentShip;
    } else {
      --currentShip;
    }
  }
  
  if(currentShip < MIN_SHIP_ITEM) {
    currentShip = MIN_SHIP_ITEM;
  } else if(currentShip > (MAX_SHIP_ITEM +galaxySaved)) {
    currentShip = MAX_SHIP_ITEM +galaxySaved;
  }
}

void updateShipStates(void)
{
  switch(currentShip) {
    case 1: { // speed
      ship.states.speed = SHIP_BASE_SPEED;
      ship.states.power = SHIP_BASE_DAMAGE-10;
      ship.states.durability = SHIP_BASE_DURAB-20;
    } break;

    case 2: { // power
      ship.states.speed = SHIP_BASE_SPEED-1;
      ship.states.power = SHIP_BASE_DAMAGE;
      ship.states.durability = SHIP_BASE_DURAB-10;
    } break;

    case 3: { // durability
      ship.states.speed = SHIP_BASE_SPEED-2;
      ship.states.power = SHIP_BASE_DAMAGE-5;
      ship.states.durability = SHIP_BASE_DURAB;
    } break;

    case 4: { // unclock after saving the galaxy
      ship.states.speed = SHIP_BASE_SPEED;
      ship.states.power = SHIP_BASE_DAMAGE;
      ship.states.durability = SHIP_BASE_DURAB;
    } break;
  }
}

void checkShipSelect(void)
{
  if(getBtnState(BUTTON_A)) {
    resetBtnStates();
#if ADD_SOUND
    sfxPlayOK();
#endif
    updateShipStates();
    previousShip =0;
    baseStory();
  }
}
//---------------------------------------------------------------------------//

void action(){SLOW_CPU;for(;;){RAND_CODE;DC(RNDCLR(RND_POS_X,RND_POS_Y));}}

void pauseMenu(void)
{
  if(getBtnState(BUTTON_Y)) {
    if(!pauseState) { // is it already stopped?
      pauseState = !pauseState;
      disableAllTasks();
      updateTaskStatus(updateBtnStates, true);
      updateTaskStatus(pauseMenu, true);
      pauseWindow();
    }
    
    if(getBtnState(BUTTON_B)) {
      resetBtnStates();
#if ADD_SOUND
      sfxPlayOK();
#endif
      pauseState = !pauseState;
      enableAllTasks();
      disableWeaponGift(); // fix glitch
      // remove "Pause" text
      fillRectFast(PAUSE_TEXT_POS_X, PAUSE_TEXT_POS_Y, PAUSE_TEXT_W, PAUSE_TEXT_H);
      continue();
    }
  }
}

void pauseWindow(void)
{
  tftSetTextSize(3);
  tftPrintAt_P(PAUSE_TEXT_POS_X, PAUSE_TEXT_POS_Y, (const char*)pauseP0);
}

//---------------------------------------------------------------------------//
// switch from title to main menu
void titleAction(void)
{
  if(getBtnState(BUTTON_A)) {
    resetBtnStates();
#if ADD_SOUND
    sfxPlayOK();
#endif
    addHistoryTasks();
    shipHyperJump();
    screenSliderEffect(COLOR_BLACK);
    // for text
    tftSetTextColor(COLOR_WHITE);
    tftSetTextSize(1);
    tftSetCursor(0,0);
  }

  if(getStickVal(LR_OK)) {
    if(getBtnState(BUTTON_Y) && getBtnState(BUTTON_X)) {
      resetBtnStates();
      resetScore();
#if ADD_SOUND
      sfxPlayCancel();
#endif
    }
  }
}

void historyAction(void)
{
  if(getBtnState(BUTTON_B)) {
    resetBtnStates();
    drawShipSelectionMenu();
  }
}

//---------------------------------------------------------------------------//
void drawGalaxy(void)
{
  screenSliderEffect(COLOR_BLACK);
  drawBMP_RLE_P(GALAXY_PIC_POS_X, GALAXY_PIC_POS_Y, galaxyPic);

  currentBackGroundColorId = getPicByte(lvlColors + curretLevel);
  currentBackGroundColor = getPicWord(nesPalette_ext, currentBackGroundColorId);
}

//---------------------------------------------------------------------------//
void baseStory(void)
{
  drawGalaxy();
  drawTextWindow(emptyText, buttonB);
  
  tftDrawRect(6, 35, 52, 52, COLOR_WHITE); // Frame Dodge
  drawBMP_RLE_P(7, 36, cityDogePic);

  curretLevel = HOME_PLANET_ID;
  addStoryTasks();
}

void drawStaticNoise(void)
{
  uint16_t *ptr = (uint16_t*)0x0000; // base adress for random
  uint16_t dataSize = 2500; // 50x50 == 2500

  tftSetAddrWindow(7, 36, 6+DOGE_PIC_W, 35+DOGE_PIC_H);

  while(dataSize--) {
    ptr += RN; // make offset for random data
    pushColorFast(pgm_read_word(ptr));
  }
}

void drawStory(void)
{
  if(getBtnState(BUTTON_B)) {
#if ADD_SOUND
    sfxPlayCancel();
#endif
    resetBtnStates();
    if(dogeDialogs < STORY_DOGE_TEXT_SIZE) {
      drawTextWindow(getConstCharPtr(dogePA, dogeDialogs), buttonB);
      if((++dogeDialogs) == 6) {
        updateTaskStatus(drawStaticNoise, true);
      }
    } else {
      curretLevel =0;
      dogeDialogs =0;
      prepareLevelSelect();
    }
  }
}

//---------------------------------------------------------------------------//
void blinkLevelPointer(void)
{
  uint8_t posX = getPicByte(lvlCoordinates + curretLevel*2);
  uint8_t posY = getPicByte(lvlCoordinates + curretLevel*2 + 1);

  iconState = !iconState;  // reuse
  tftDrawCircle(posX, posY, CIRCLE_PONITER_MAP_SIZE, (iconState ? COLOR_RED : COLOR_WHITE));
}

//---------------------------------------------------------------------------//

void prepareLevelSelect(void)
{
  drawGalaxy();
  // add level select tasks
  addTasksArray(levelSelectTasksArr, LVL_SEL_TASKS_COUNT);
  drawTextWindow(getConstCharPtr(worldPA, curretLevel), buttonA); // name of current world

  uint8_t i=0;
  uint8_t posX, posY;
  do {
    posX = getPicByte(lvlCoordinates + i*2);
    posY = getPicByte(lvlCoordinates + i*2 + 1);
    tftDrawCircle(posX, posY, CIRCLE_PONITER_MAP_SIZE, COLOR_WHITE);
    ++i;
  } while (i < curretLevel);
  
  resetBtnStates();
}

void drawLevelSelect(void)
{
  if(getBtnState(BUTTON_A)) {
    resetBtnStates();
#if ADD_SOUND
    sfxPlayOK();
#endif
    
    screenSliderEffect(currentBackGroundColor);
    drawBMP_RLE_P(0, 119, hudGuiPic);
    addGameTasks();
  }
}
//---------------------------------------------------------------------------//

void drawSomeGUI(void)
{
  if(hudStatus.updScore) {
    hudStatus.updScore = false;
    char buf[5]; // i'm pretty sure what 5 bytes will be enough...
    tftSetTextSize(1);
    tftFillRect(SCORE_POS_X, SCORE_POS_Y, 20, 7, currentBackGroundColor);
    tftPrintAt(SCORE_POS_X, SCORE_POS_Y, itoa(score, buf, 10));
  }

  if(hudStatus.updLife) {
    hudStatus.updLife = false;
    tftFillRect(SHIP_ENERGY_POS_X, SHIP_ENERGY_POS_Y, (ship.health>>2) - 4, SHIP_ENERGY_H, COLOR_WHITE);
  }
}

void waitEnd(void)
{
  if(getBtnState(BUTTON_B)) {
    resetBtnStates();
#if ADD_SOUND
    sfxPlayCancel();
#endif
    baseTitleTask();
  }
}

void waitOk(void)
{
  if(getBtnState(BUTTON_B)) {
    resetBtnStates();
#if ADD_SOUND
    sfxPlayCancel();
#endif
    prepareLevelSelect();
  }
}

void printMessage(const char *text)
{
  tftSetTextSize(2);
  tftSetTextColor(COLOR_WHITE);
  tftPrintAt_P(20, 40, (const char *)text);
}

void printScore(void)
{
  char buf[5];

  tftSetTextSize(1);
  tftPrintAt_P(40, 60, (const char *)scoreP);
  tftPrintAt(80, 60, itoa(score, buf, 10));

  // draw hi score
  uint16_t hiScore = readSaveData(EE_ADDR_SCORE);
  if(score > hiScore) {
    writeSaveData(EE_ADDR_SCORE, score);  // save new score
    hiScore = score;
  }
  tftPrintAt_P(40, 70, (const char *)maxScoreP);
  tftPrintAt(80, 70, itoa(hiScore, buf, 10));
  score = 0;
}

void done(const uint8_t *text) // fantasy end, bad name for function...
{
  tftSetTextSize(2);
  tftSetTextColor(COLOR_WHITE);
  tftPrintAt_P(20, 40, (const char *)text);

  levelBaseInit();
}

void gameOver(void)
{
  done(gameOverP);
  printScore();

  // add game over tasks
  addTasksArray(gameOverTasksArr, GAME_OVER_TASKS_COUNT);
  resetBtnStates();

  curretLevel =0;
  initBaseGameParams();
}

void levelClear(void)
{
  done(levelClearP);
}

void victory(void)
{
  done(victoryP);
  printScore();
  curretLevel =0;
  galaxySaved =1;
}
