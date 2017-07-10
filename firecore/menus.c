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
  uint16_t posX = CHARACTER_ICON_OFFSET_X;
  screenSliderEffect(currentBackGroundColor);

  const uint8_t *pPic;
  uint16_t picSize;

  for(uint8_t count=0; count<CHARACTER_ICON_NUM; count++) {
    pPic = getConstCharPtr(catsPics, count);
    picSize = getPicWord(catsPicsSizes, count);

    drawFrame(posX, CHARACTER_ICON_OFFSET_Y,
                CHARACTER_FRAME_WH, CHARACTER_FRAME_WH, INDIGO_COLOR, COLOR_WHITE);
    drawBMP_RLE_P(posX, CHARACTER_ICON_OFFSET_Y+1,
                                CHARACTER_ICON_W, CHARACTER_ICON_H, pPic, picSize);

    posX += CHARACTER_ICON_STEP;
  }
  
  // make tiny frame for ship
  tftDrawRect(SHIP_SELECT_POS_X-5, SHIP_SELECT_POS_Y-5,
                 SHIP_PIC_W+10, SHIP_PIC_H+10, COLOR_WHITE);
  drawTextWindow(selectShipP, buttonA);
  addShipSelectTasks();
}

void drawCurrentShipSelection(void)
{
  iconState = !iconState;

  uint16_t posX = CHARACTER_ICON_STEP*(currentShip-1) + CHARACTER_ICON_OFFSET_X;
  uint16_t color = iconState ? COLOR_WHITE : COLOR_BLACK;
  tftDrawRect(posX, CHARACTER_ICON_OFFSET_Y, CHARACTER_FRAME_WH, CHARACTER_FRAME_WH, color);

  if(currentShip != previousShip) {
    posX = CHARACTER_ICON_STEP*(previousShip-1) + CHARACTER_ICON_OFFSET_X;
    tftDrawRect(posX, CHARACTER_ICON_OFFSET_Y, CHARACTER_FRAME_WH, CHARACTER_FRAME_WH, COLOR_BLACK);

    drawFrame(0, 55, 100, 30, INDIGO_COLOR, COLOR_WHITE); // frame params
    tftPrintAt_P(4, 58, (const char*)shipSpeedStatP);
    tftPrintAt_P(4, 66, (const char*)shipPowerStatP);
    tftPrintAt_P(4, 74, (const char*)shipDurabStatP);

    previousShip = currentShip;
    getShipStates(&ship.states);

    uint8_t lineSize = mapVal(ship.states.speed, 0, SHIP_BASE_SPEED, 0, 40);
    tftDrawFastHLine(BASE_STATS_POS_X, BASE_STATS_POS_Y, lineSize, COLOR_WHITE);

    lineSize = mapVal(ship.states.power, 0, SHIP_BASE_DAMAGE, 0, 40);
    tftDrawFastHLine(BASE_STATS_POS_X, BASE_STATS_POS_Y+8, lineSize, COLOR_WHITE);

    lineSize = mapVal(ship.states.durability, 0, SHIP_BASE_DURAB, 0, 40);
    tftDrawFastHLine(BASE_STATS_POS_X, BASE_STATS_POS_Y+16, lineSize, COLOR_WHITE);
  }
}

void getShipItem(void)
{
  uint16_t newValueXY = getJoyStickValue(X_J_MUX_VAL);
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
  if(getBtnState(BUTTON_A)) {
    resetBtnStates();
    getShipStates(&ship.states);
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
      pauseState = !pauseState;
      enableAllTasks();
      // remove "Pause" text
      fillRectFast(PAUSE_TEXT_POS_X, PAUSE_TEXT_POS_Y, PAUSE_TEXT_W, PAUSE_TEXT_H);
      //continue();
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
    shipHyperJump();
    drawShipSelectionMenu();
  }

  if(getStickVal(LR_OK)) {
    if(getBtnState(BUTTON_Y) && getBtnState(BUTTON_X)) {
      resetBtnStates();
      resetScore();
#if ADD_SOUND
    if(soundEnable) toneBuzz(1200, 20);
#endif
    }
  }
}

//---------------------------------------------------------------------------//
void drawGalaxy(void)
{
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
  addTask_P(T(&updateBtnStates));
  addTask_P(T(&drawStory));
}

void drawStaticNoise(void)
{
  uint16_t *ptr = (uint16_t*)0x0000; // base adress for random
  uint16_t dataSize = 2500; // 50x50 == 2500

  tftSetAddrWindow(7, 36, 6+DOGE_PIC_W, 35+DOGE_PIC_H);

  while(dataSize--){
    ptr += RN; // make offset for random data
    pushColorFast(pgm_read_word(ptr));
  }
}

void drawStory(void)
{
  tftDrawCircle(WORLD_8_POS_X, WORLD_8_POS_Y, CIRCLE_PONITER_MAP_SIZE,
                       (shipState ? COLOR_RED : COLOR_WHITE )); // Home planet
  
  shipState = !shipState; // reuse
  
  if(getBtnState(BUTTON_B)) {
    resetBtnStates();
    if(dogeDialogs < STORY_DOGE_TEXT_SIZE) {
      drawTextWindow(getConstCharPtr(dogePA, dogeDialogs), buttonB);
      dogeDialogs++;
      if(dogeDialogs == 6) {
        addTask_P(T(&drawStaticNoise));
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
  addTask_P(T(&updateBtnStates));
  addTask_P(T(&drawLevelSelect));
  resetBtnStates();
}

void drawLevelSelect(void)
{
  uint8_t posX = getPicByte(lvlCoordinates + curretLevel*2);
  uint8_t posY = getPicByte(lvlCoordinates + curretLevel*2 + 1);

  shipState = !shipState;

  tftDrawCircle(posX, posY, CIRCLE_PONITER_MAP_SIZE,
                     (shipState ? COLOR_RED : COLOR_WHITE )); // Home planet
  
  if(getBtnState(BUTTON_A)) {
    resetBtnStates();
    addGameTasks();    
    screenSliderEffect(currentBackGroundColor);

    drawBMP_RLE_P(0, 119, HUD_GUI_PIC_W, HUD_GUI_PIC_H, hudGuiPic, HUD_GUI_PIC_SIZE);
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
  if(hudStatus.updScore) {
    hudStatus.updScore = false;
    char buf[5]; // i'm pretty sure what 5 bytes will be enough...
    tftSetTextSize(1);
    tftFillRect(SCORE_POS_X, SCORE_POS_Y, 20, 7, currentBackGroundColor);
    tftPrintAt(SCORE_POS_X, SCORE_POS_Y, itoa(score, buf, 10));
  }

  if(hudStatus.updLife) {
    hudStatus.updLife = false;
    tftFillRect(SHIP_ENERGY_POS_X, SHIP_ENERGY_POS_Y,
                 (ship.health>>2) - 4, SHIP_ENERGY_H, COLOR_WHITE);
  }
}

void waitEnd(void)
{
  if(getBtnState(BUTTON_B)) {
    resetBtnStates();
    deleteAllTasks();
    baseTitleTask();
  }
}

void waitOk(void)
{
  if(getBtnState(BUTTON_B)) {
    resetBtnStates();
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
  char buf[10];

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

  deleteAllTasks();
  addTask_P(T(&updateBtnStates));
  addTask_P(T(&waitEnd));
  addTask_P(T(&drawShipExplosion));
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
}
