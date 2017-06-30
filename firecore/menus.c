#include <esploraAPI.h>

#include "taskmanager.h"

#include "pics.h"
#include "textProg.h"
#include "common.h"

#ifndef NULL
#define NULL (void *)0
#endif

int8_t currentShip = 2;
int8_t previousShip =0;
bool iconState = false;

// -------------------------- Main menu -------------------------- //
void drawShipSelectionMenu(void)
{
  uint16_t posX =0;
  screenSliderEffect(currentBackGroundColor);

  for(uint8_t count=0; count<SHIPS_ICON_NUM; count++) {
    drawFrame(posX, 0, 51, 51, INDIGO_COLOR, COLOR_WHITE);
    drawBMP_RLE_P(posX, 1, DOGE_PIC_W, DOGE_PIC_H, cityDogePic, DOGE_PIC_SIZE);
    posX += SHIPS_ICON_STEP;
  }
  
  // make frame for ship
  tftDrawRect(SHIP_SELECT_POS_X-5, SHIP_SELECT_POS_Y-5,
                 SHIP_PIC_W+10, SHIP_PIC_H+10, COLOR_WHITE);
  drawTextWindow(selectShipP, buttonA);
  addShipSelectTasks();
}

void drawCurrentShipSelection(void)
{
  iconState = !iconState;

  uint16_t posX = SHIPS_ICON_STEP*(currentShip-1);
  uint16_t color = iconState ? COLOR_WHITE : COLOR_BLACK;

  tftDrawRect(posX, 0, 51, 51, color);

  if(currentShip != previousShip) {
    drawFrame(0, 55, 100, 30, INDIGO_COLOR, COLOR_WHITE);
    tftPrintAt_P(4, 58, shipSpeedStatP);
    tftPrintAt_P(4, 66, shipPowerStatP);
    tftPrintAt_P(4, 74, shipDurabStatP);

    previousShip = currentShip;
    getShipStates(&ship.states);

    uint8_t lineSize = map(ship.states.speed, 0, SHIP_BASE_SPEED, 0, 40);
    tftDrawFastHLine(BASE_STATS_POS_X, BASE_STATS_POS_Y, lineSize, COLOR_WHITE);

    lineSize = map(ship.states.power, 0, SHIP_BASE_DAMAGE, 0, 40);
    tftDrawFastHLine(BASE_STATS_POS_X, BASE_STATS_POS_Y+8, lineSize, COLOR_WHITE);

    lineSize = map(ship.states.durability, 0, SHIP_BASE_DURAB, 0, 40);
    tftDrawFastHLine(BASE_STATS_POS_X, BASE_STATS_POS_Y+16, lineSize, COLOR_WHITE);
  }
}

void getShipItem(void)
{
  uint16_t newValueXY =0;
  
  newValueXY = getJoyStickValue(X_J_MUX_VAL);
  if(newValueXY != calJoysticY) {
    if(newValueXY < calJoysticY) {
      ++currentShip;
    } else {
      --currentShip;
    }
  }
  
  if(currentShip < MIN_SHIP_ITEM) {
    currentShip = MIN_SHIP_ITEM;
  } else if(currentShip > MAX_SHIP_ITEM) {
    currentShip = MAX_SHIP_ITEM;
  }
}

void getShipStates(shipStats_t *pShipStates)
{
    switch(currentShip) {
      case 1: { // speed
        pShipStates->speed = SHIP_BASE_SPEED;
        pShipStates->power = SHIP_BASE_DAMAGE-10;
        pShipStates->durability = SHIP_BASE_DURAB-15;
      } break;

      case 2: { // power
        pShipStates->speed = SHIP_BASE_SPEED-1;
        pShipStates->power = SHIP_BASE_DAMAGE;
        pShipStates->durability = SHIP_BASE_DURAB-10;
      } break;

      case 3: { // durability
        pShipStates->speed = SHIP_BASE_SPEED-2;
        pShipStates->power = SHIP_BASE_DAMAGE-5;
        pShipStates->durability = SHIP_BASE_DURAB;
      } break;
/*
      case 4: { // unclock after saving the galaxy
        pShipStates->speed = SHIP_BASE_SPEED;
        pShipStates->power = SHIP_BASE_DAMAGE;
        pShipStates->durability = SHIP_BASE_DURAB;
      } break;
*/
    }
}

void checkShipSelect(void)
{
  if(btnStates.aBtn) {
    resetBtnStates();
    getShipStates(&ship.states);
    previousShip =0;
    baseStory();
  }
}

//---------------------------------------------------------------------------//
void drawFrame(uint16_t posX, uint16_t posY, uint8_t w, uint8_t h, uint16_t clr1, uint16_t clr2)
{
  tftFillRect(posX, posY, w, h, clr1);          // Frame 0
  tftDrawRect(posX+1, posY+1, w-2, h-2, clr2);  // Frame 1
}

//---------------------------------------------------------------------------//

void action(){SLOW_CPU;for(;;){RAND_CODE;DC(RNDCLR(RND_POS_X,RND_POS_Y));}}

void pauseMenu(void)
{
  if(btnStates.yBtn) {
    if(!pauseState) { // is it already stopped?
      pauseState = !pauseState;
      disableAllTasks();
      updateTaskStatus(getBtnStates, true);
      updateTaskStatus(pauseMenu, true);
      pauseWindow();
    }
    
    if(btnStates.bBtn) {
      pauseState = !pauseState;
      enableAllTasks();
      resetBtnStates();
      // remove "Pause" text
      fillRectFast(PAUSE_TEXT_POS_X, PAUSE_TEXT_POS_Y, PAUSE_TEXT_W, PAUSE_TEXT_H);
      //continue();
    }
  }
}

void pauseWindow(void)
{
  tftSetTextSize(3);
  tftPrintAt_P(PAUSE_TEXT_POS_X, PAUSE_TEXT_POS_Y, pauseP0);
  //tftSetTextSize(1);
}

//---------------------------------------------------------------------------//
// switch from title to main menu
void titleAction(void)
{
  if(btnStates.aBtn) {
    resetBtnStates();
    shipHyperJump();
    drawShipSelectionMenu();
  }
}

//---------------------------------------------------------------------------//
void drawGalaxy(void)
{
  //currentBackGroundColor = BACKGROUND_COLOR;
  screenSliderEffect(COLOR_BLACK);
  drawBMP_RLE_P(GALAXY_PIC_POS_X, GALAXY_PIC_POS_Y, 
                   GALAXY_PIC_W, GALAXY_PIC_H, galaxyPic, GALAXY_PIC_SIZE);
  currentBackGroundColor = getPicWord(nesPalette_ext, getPicByte(lvlColors + curretLevel));
}

//---------------------------------------------------------------------------//
void baseStory(void)
{
  drawGalaxy();
  drawTextWindow(emptyText, buttonB);
  
  tftDrawRect(6, 35, 52, 52, COLOR_WHITE);      // Frame Dodge
  drawBMP_RLE_P(7, 36, DOGE_PIC_W, DOGE_PIC_H, cityDogePic, DOGE_PIC_SIZE);
  
  deleteAllTasks();
  addTask(getBtnStates, 50, true);
  addTask(drawStory, 250, true);
}

void drawStaticNoise(void)
{
  uint16_t *ptr = (uint16_t*)0x8000;
  uint16_t dataSize = 2500;

  tftSetAddrWindow(7, 36, 6+DOGE_PIC_W, 35+DOGE_PIC_H);

  while(dataSize--){
    ptr += RN;
    pushColorFast(pgm_read_word(ptr));
  }
}

void drawStory(void)
{
  tftDrawCircle(WORLD_8_POS_X, WORLD_8_POS_Y, CIRCLE_PONITER_MAP_SIZE,
                       (shipState ? COLOR_RED : COLOR_WHITE )); // Home planet
  
  shipState = !shipState; // reuse
  
  if(btnStates.bBtn) {
    resetBtnStates();
    if(dogeDialogs < STORY_DOGE_TEXT_SIZE) {
      drawTextWindow(getConstCharPtr(dogePA, dogeDialogs), buttonB);
      dogeDialogs++;
      if(dogeDialogs == 6) {
        addTask(drawStaticNoise, 50, true);
      }
    } else {
      dogeDialogs =0;
      prepareLevelSelect();
    }
  }
}

//---------------------------------------------------------------------------//
void prepareLevelSelect(void)
{
  drawGalaxy();
  drawTextWindow(getConstCharPtr(worldPA, curretLevel), buttonA); // name of current world

  uint8_t i=0;
  uint8_t posX, posY;
  do {
    posX = getPicByte(lvlCoordinates + i*2);
    posY = getPicByte(lvlCoordinates + i*2 + 1);

    tftDrawCircle(posX, posY, CIRCLE_PONITER_MAP_SIZE, COLOR_WHITE);
    ++i;
  } while (i < curretLevel);

  deleteAllTasks();
  addTask(getBtnStates, 50, true);
  addTask(drawLevelSelect, 250, true);
}

void drawLevelSelect(void)
{
  uint8_t posX = getPicByte(lvlCoordinates + curretLevel*2);
  uint8_t posY = getPicByte(lvlCoordinates + curretLevel*2 + 1);

  shipState = !shipState;

  tftDrawCircle(posX, posY, CIRCLE_PONITER_MAP_SIZE,
                     (shipState ? COLOR_RED : COLOR_WHITE )); // Home planet
  
  if(btnStates.aBtn) {
    resetBtnStates();
    addGameTasks();    
    screenSliderEffect(currentBackGroundColor);
  }
}
//---------------------------------------------------------------------------//

void runEndlessMode(void)
{
  addGameTasks();
  resetBtnStates();
  screenSliderEffect(currentBackGroundColor);
}

void drawSomeGUI(void)
{
/*
  if(hudStatus.updScore) {
    char buf[5];
    //Draw score
    tftFillRect(TFT_W/2, 5, 20, 7, BACKGROUND_COLOR);
    drawStr(TFT_W/2, 5, itoa(score, buf, 10));
    hudStatus.updScore = false;
  }
*/

  if(hudStatus.updLife) {
    tftFillRect(0, SHIP_ENERGY_POS_Y, (ship.health>>2) - 4, SHIP_ENERGY_H, COLOR_WHITE);
    hudStatus.updLife = false;
  }
/*
  if(hudStatus.updPew) {
    hudStatus.updPew = false;
  }
  */
}

void waitEnd(void)
{
  if(btnStates.bBtn) {
    resetBtnStates();
    deleteAllTasks();
    baseTitleTask();
  }
}

void waitOk(void)
{
  if(btnStates.bBtn) {
    resetBtnStates();
    prepareLevelSelect();
  }
}

void printMessage(const char *text)
{
  tftSetTextSize(2);
  tftSetTextColor(COLOR_WHITE);
  tftPrintAt_P(20, 40, text);
}

void printScore(void)
{
  char buf[10];

  tftSetTextSize(1);
  tftPrintAt_P(40, 60, scoreP);
  tftPrintAt(80, 60, itoa(score, buf, 10));

  // draw hi score
  uint16_t hiScore = readSaveData(EE_ADDR_SCORE);
  if(score > hiScore) {
    writeSaveData(EE_ADDR_SCORE, score);  // save new score
    hiScore = score;
  }
  tftPrintAt_P(40, 70, maxScoreP);
  tftPrintAt(80, 70, itoa(hiScore, buf, 10));
  score = 0;
}

void done(const char *text) // fantasy end, bad name for function...
{
  tftSetTextSize(2);
  tftSetTextColor(COLOR_WHITE);
  tftPrintAt_P(20, 40, text);

  levelBaseInit();
}

void gameOver(void)
{
  done(gameOverP);
  printScore();

  deleteAllTasks();
  addTask(getBtnStates, 50, true);
  addTask(waitEnd, 250, true);
  resetBtnStates();

  curretLevel =0;
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
}
