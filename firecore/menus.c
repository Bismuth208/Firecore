#include <esploraAPI.h>

#include "taskmanager.h"

#include "pics.h"
#include "textProg.h"
#include "common.h"

#ifndef NULL
#define NULL (void *)0
#endif

// -------------------------- Main menu -------------------------- //
void getMenuItem(void)
{
  uint16_t newValueXY =0;
  
  newValueXY = getJoyStickValue(Y_J_MUX_VAL);
  if(newValueXY != calJoysticY) {
    if(newValueXY > calJoysticY) {
      ++menuItem;
    } else {
      --menuItem;
    }
  }
  
  if(menuItem < MIN_MENU_ITEM) {
    menuItem = MIN_MENU_ITEM;
  } else if(menuItem > MAX_MENU_ITEM) {
    menuItem = MAX_MENU_ITEM;
  }
}

void drawMenuItemSelector(void)
{
  uint8_t selectorPosY = menuItem*50;
  drawBMP_RLE_P(selectorPosY, 5, ROCKET_W, ROCKET_H, rocketPic, ROCKET_PIC_SIZE);
}

void menuAction(void)
{
  if(btnStates.aBtn) {
    btnStates.aBtn = false;
    pFunc_t fPtr = NULL;
    
    if(menuItem == 0) {
      fPtr = addGameTasks;
    }
#if 0
    if(menuItem == 1) {
      addEndlessTasks();
    }
    
    if(menuItem == 2) {
      addTitleTasks();
    }
#endif
    
    if(fPtr != NULL) {
      deleteAllTasks();
      resetBtnStates();
      fPtr();
    }
  }
}

void selectionMenu(void)
{
  uint8_t count, countY = 30;
  
  screenSliderEffect(currentBackGroundColor);
  
  // set F_CPU to 2 MHz by ps = 8 (0x03)
  setMainFreq(0x03);
  
  for(count=5; count < 7; count++) {
    // draw buttons
    tftFillRoundRect(26, countY, 120, 32, 5, COLOR_WHITE);
    tftDrawRoundRect(25, countY+1, 121, 30, 5, COLOR_BLACK);
    //drawButton(55, countY, 193, 32, 5, COLOR_WHITE, COLOR_BLACK);
    
    //tftPrint_P(TFT_W/5, countY+6, PGRW_CP(modePA, count-5));
    
    countY += 50;
  }
  
  // set F_CPU back to 16 MHz by ps = 0 (0x00)
  setMainFreq(0x00);
  
  deleteAllTasks();
  addTask(getMenuItem, 75, true);
  addTask(drawMenuItemSelector, 200, true);
  addTask(menuAction, 400, true);
}

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
      tftFillRect(TFT_W/3, TFT_H/3, 90, 25, currentBackGroundColor);
      //continue();
    }
  }
}

void pauseWindow(void)
{
  tftSetTextSize(3);
  tftSetCursor(TFT_W/3, TFT_H/3);
  tftPrint_P(pauseP0);
  tftSetTextSize(1);
}

//---------------------------------------------------------------------------//
// switch from title to main menu
void titleAction(void)
{
  if(btnStates.aBtn) {
    baseStory();
  }
}

//---------------------------------------------------------------------------//
void drawGalaxy(void)
{
  currentBackGroundColor = BACKGROUND_COLOR;
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
  resetBtnStates();
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
    deleteAllTasks();
    addGameTasks();
    resetBtnStates();
    screenSliderEffect(currentBackGroundColor);
  }
}
//---------------------------------------------------------------------------//

void runEndlessMode(void)
{
  deleteAllTasks();
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
    tftFillRect(0, SHIP_ENERGY_POS_Y, ship.health/4 - 4, SHIP_ENERGY_H, COLOR_WHITE);   // 180/4 - 4 = 43 ...
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
    deleteAllTasks();
    addTask(drawRows, 10, true);
    resetBtnStates();
    tftFillScreen(currentBackGroundColor);
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
  tftSetCursor(20, 40);
  tftSetTextColor(COLOR_WHITE);
  tftPrint_P(text);
  tftSetTextSize(1);
}

void printScore(void)
{
  char buf[10];
  tftSetCursor(40, 60); tftPrint_P(scoreP);
  tftSetCursor(80, 60); tftPrint(itoa(score, buf, 10));

  // draw hi score
  tftSetCursor(40, 70); tftPrint_P(maxScoreP);

  uint16_t hiScore = eeprom_read_word( (const uint16_t*) EE_ADDR_SCORE);
  if(score > hiScore) {
    eeprom_write_word( (uint16_t*) EE_ADDR_SCORE, score);   // save new score
    hiScore = score;
  }
  tftSetCursor(80, 70); tftPrint(itoa(hiScore, buf, 10));
  score = 0;
}

void gameOver(void)
{
  printMessage(gameOverP);
  printScore();

  deleteAllTasks();
  addTask(getBtnStates, 50, true);
  addTask(waitEnd, 250, true);
  resetBtnStates();

  curretLevel =0;
  levelBaseInit();
}

void levelClear(void)
{
  printMessage(levelClearP);
  levelBaseInit();
}

void victory(void)
{
  printMessage(victoryP);
  printScore();
  levelBaseInit();
  curretLevel =0;
}
