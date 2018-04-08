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

#include "pics.h"
#include "textProg.h"
#include "common.h"

int8_t currentShip = 2;
int8_t previousShip =0;
uint8_t dogeDialogs =0;
uint8_t menuSwitchSelectState =0;
uint8_t creditsSwitchState =0;

bool iconState = false;
bool rndFlag = false;

// -------------------------- Main menu -------------------------- //
void drawShipSelectionMenu(void)
{
  uint8_t posX = CHARACTER_ICON_OFFSET_X;
  uint8_t characterIconStep = (TFT_W /(CHARACTER_ICON_NUM+gameSaveData.bonusUnlocked));
  screenSliderEffect(currentBackGroundColor);

  for(uint8_t count=0; count<(CHARACTER_ICON_NUM+gameSaveData.bonusUnlocked); count++) {
    drawBMP_ERLE_P(posX, CHARACTER_ICON_OFFSET_Y, getConstCharPtr(catsPics, count));
    tftDrawRect(posX, CHARACTER_ICON_OFFSET_Y, CHARACTER_FRAME_WH, CHARACTER_FRAME_WH, COLOR_BLACK);

    posX += characterIconStep;
  }
  
  // make tiny frame for ship
  tftDrawRect(SHIP_SELECT_POS_X-5, SHIP_SELECT_POS_Y-4, SHIP_PIC_W+10, SHIP_PIC_H+10, COLOR_WHITE);
  drawTextWindow(selectShipP, buttonA);

  addShipSelectTasks();
}

void drawShipStats(uint8_t yPerk, uint8_t yText, uint8_t perk, uint8_t maxPerkVal, text_t *perkText)
{
  uint8_t lineSize = mapVal(perk, 0, maxPerkVal, 0, 40);
  tftDrawFastHLine(BASE_STATS_POS_X, yPerk, lineSize, COLOR_WHITE);
  tftPrintAt_P(4, yText, (const char*)perkText);
}

void drawCurrentShipSelection(void)
{
  iconState = !iconState;

  uint8_t characterIconStep = (TFT_W /(CHARACTER_ICON_NUM+gameSaveData.bonusUnlocked));
  uint8_t posX = characterIconStep*(currentShip-1) + CHARACTER_ICON_OFFSET_X;
  uint16_t color = iconState ? COLOR_WHITE : COLOR_BLACK;
  tftDrawRect(posX, CHARACTER_ICON_OFFSET_Y, CHARACTER_FRAME_WH, CHARACTER_FRAME_WH, color);

  if(currentShip != previousShip) {
#if ADD_SOUND
    sfxPlayTick();
#endif

    // update ship pic
    ship.type = currentShip-1;
    ship.sprite.pPic = getConstCharPtr(shipsPics, ship.type);

    posX = characterIconStep*(previousShip-1) + CHARACTER_ICON_OFFSET_X;
    tftDrawRect(posX, CHARACTER_ICON_OFFSET_Y, CHARACTER_FRAME_WH, CHARACTER_FRAME_WH, COLOR_BLACK);

    drawFrame(0, 55, 100, 30, INDIGO_COLOR, COLOR_WHITE); // frame for ship's params

    previousShip = currentShip;
    updateShipStates();

    drawShipStats(BASE_STATS_POS_Y, BASE_STATS_POS_Y-2, ship.states.speed, SHIP_BASE_SPEED, shipSpeedStatP);
    drawShipStats(BASE_STATS_POS_Y+8, BASE_STATS_POS_Y+6, ship.states.power, SHIP_BASE_DAMAGE, shipPowerStatP);
    drawShipStats(BASE_STATS_POS_Y+16, BASE_STATS_POS_Y+14, ship.states.durability, SHIP_BASE_DURAB, shipDurabStatP);
  }
}

void getShipItem(void)
{
  uint16_t newValueXY = getStickVal(LINE_X);
  if(newValueXY != calJoysticY) {
    currentShip += (newValueXY < calJoysticY) ? 1 : -1;
  }
  
  if(currentShip < MIN_SHIP_ITEM) {
    currentShip = MIN_SHIP_ITEM;
  } else if(currentShip > (MAX_SHIP_ITEM + gameSaveData.bonusUnlocked)) {
    currentShip = MAX_SHIP_ITEM + gameSaveData.bonusUnlocked;
  }
}

void updateShipStates(void)
{
  decltype(ship.states) state;

  switch(currentShip) {
    case 1: { // speed
      state = {SHIP_BASE_SPEED, SHIP_BASE_DAMAGE-10, SHIP_BASE_DURAB-20};
    } break;

    case 2: { // power
      state = {SHIP_BASE_SPEED-1, SHIP_BASE_DAMAGE, SHIP_BASE_DURAB-10};
    } break;

    case 3: { // durability
      state = {SHIP_BASE_SPEED-2, SHIP_BASE_DAMAGE-5, SHIP_BASE_DURAB};
    } break;

    case 4: { // unclock after saving the galaxy
      state = {SHIP_BASE_SPEED, SHIP_BASE_DAMAGE, SHIP_BASE_DURAB};
    } break;
  }

  ship.states = state;
}
//---------------------------------------------------------------------------//

void action(){TS;for(;;){RAND_CODE;SC;DC(RC);}}

void pauseMenu(void)
{
  if(getBtnState(BUTTON_Y)) {
    if(!pauseState) { // is it already stopped?
      pauseState = !pauseState;
      disableAllTasks();
      enableTask(playMusic);
      enableTask(updateBtnStates);
      enableTask(pauseMenu);
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
      //fillRectFast(PAUSE_TEXT_POS_X, PAUSE_TEXT_POS_Y, PAUSE_TEXT_W, PAUSE_TEXT_H);
      tftFillRect(PAUSE_TEXT_POS_X, PAUSE_TEXT_POS_Y, PAUSE_TEXT_W, PAUSE_TEXT_H, currentBackGroundColor);
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
bool titleAction(void)
{
  uint8_t playOkSound = 0;

  if(getBtnState(BUTTON_A)) {
    resetBtnStates();
    playOkSound = 1;

    addHistoryTasks();
    shipHyperJump();
    drawGalaxyAt(0);
    // for history text
    tftSetTextColor(COLOR_WHITE);
    tftSetTextSize(1);
    tftSetCursor(0, 72);
    return true;
  }

  if(getStickVal(LR_OK)) {
    if(getBtnState(BUTTON_Y) && getBtnState(BUTTON_X)) {
      resetBtnStates();
      resetScore();
      playOkSound = 1;
    }

#if 0
    if(getBtnState(BUTTON_A)) {
      sfxSetVolume(sfxGetGlobalVolume()+1);
      playOkSound = 1;
    }

    if(getBtnState(BUTTON_B)) {
      sfxSetVolume(sfxGetGlobalVolume()-1);
      playOkSound = 1;
    }
#endif
  }

#if ADD_SOUND
  if(playOkSound) sfxPlayOK();
#endif
  return false;
}

bool historyAction(void)
{
  if(getBtnState(BUTTON_B)) {
    resetBtnStates();
    drawShipSelectionMenu();
    return true;
  }
  return false;
}

bool checkShipSelect(void)
{
  if(getBtnState(BUTTON_A)) {
    resetBtnStates();
#if ADD_SOUND
    sfxPlayOK();
#endif
    updateShipStates();
    previousShip =0;
    baseStory();
    return true;
  }
  return false;
}

bool drawStory(void)
{
  if(getBtnState(BUTTON_B)) {
    resetBtnStates();
#if ADD_SOUND
    sfxPlayCancel();
#endif
    if(dogeDialogs < STORY_DOGE_TEXT_SIZE) {
      drawTextWindow(getConstCharPtr(dogePA, dogeDialogs), buttonB);

      switch(++dogeDialogs) {
        case 5: {
          enableTask(drawStaticNoise);
        } break;
        case 7: {
          disableTask(drawRandomDoge);
        } break;
        default: break;
      }
    } else {
      curretLevel =0;
      dogeDialogs =0;
      prepareLevelSelect();
      return true;
    }
  }
  return false;
}

bool drawNewLevel(void)
{
  if(getBtnState(BUTTON_A)) {
    resetBtnStates();
#if ADD_SOUND
    sfxPlayOK();
#endif
    
    screenSliderEffect(currentBackGroundColor);

    switch(curretLevel) {
      case 0: case 1: 
      case 3: case 4:
      case 6: case 7:
      case 9: case 10:
      case 12: {
        addGameTasks();
      } break;

      case 2: case 5:
      case 8: case 11: {
        addAsteroidsTasks();
      } break;

      default: {
        addGameTasks();
      } break;
    }
  }
  return false;
}

//---------------------------------------------------------------------------//
// tiny state machine
void menuSwitchSelect(void)
{
  bool switchSate = false;

  switch(menuSwitchSelectState)
  {
  case M_SWITCH_TITLE: {
    switchSate = titleAction();
  } break;

  case M_SWITCH_HISTORY: {
    switchSate = historyAction();
  } break;

  case M_SWITCH_SHIP_SELECT: {
    switchSate = checkShipSelect();
  } break;

  case M_SWITCH_STORY: {
    switchSate = drawStory();
  } break;

  case M_SWITCH_LVL_SELECT: {
    switchSate = drawNewLevel();
  } break;  

  default: break; // do nothing
  }

  if(switchSate) {
    if(++menuSwitchSelectState > M_SWITCH_LVL_SELECT) {
      menuSwitchSelectState = 0;
    }
  }  
}

//---------------------------------------------------------------------------//
void drawGalaxyAt(uint8_t y)
{
  screenSliderEffect(COLOR_BLACK);
  drawBMP_ERLE_P(GALAXY_PIC_POS_X, y, galaxyPic);
}

void drawGalaxy(void)
{
  drawGalaxyAt(GALAXY_PIC_POS_Y);
  currentBackGroundColorId = getPicByte(lvlColors + curretLevel);
  currentBackGroundColor = palette_RAM[currentBackGroundColorId];
}

//---------------------------------------------------------------------------//
void baseStory(void)
{
  drawGalaxy();
  drawTextWindow(emptyText, buttonB);
  tftDrawRect(6, 32, 52, 53, COLOR_WHITE); // Frame Dodge

  curretLevel = HOME_PLANET_ID;
  addStoryTasks();
}

void drawRandomDoge(void)
{
  rndFlag = !rndFlag;
  drawBMP_ERLE_P(7, 33+rndFlag, cityDogePic);

  uint16_t *ptr = (uint16_t*)0x0000; // base adress for random
  uint8_t dataSize = (RN & 31)+1;

  do {
    ptr += RN; // make offset for random data
    tftSetAddrPixel(7+(RN%DOGE_PIC_W), 33+(RN%DOGE_PIC_W));
    pushColorFast(pgm_read_word(ptr));
  } while(--dataSize);

  updateTaskTimeCheck(drawRandomDoge, (RN & 63) + 40);
}

void drawStaticNoise(void)
{
  uint16_t *ptr = (uint16_t*)0x0000; // base adress for random
  uint16_t dataSize = 2500; // 50x50 == 2500

  tftSetAddrWindow(7, 33, 6+DOGE_PIC_W, 35+DOGE_PIC_H);

  do {
    ptr += RN; // make offset for random data
    pushColorFast(pgm_read_word(ptr));
  } while(--dataSize);
}

//---------------------------------------------------------------------------//
void blinkLevelPointer(void)
{
  auto tmpData = getPicSize(lvlCoordinates, curretLevel*2);

  if(tmpData.wData) { // not asteroids?
    iconState = !iconState;  // reuse
    tftDrawCircle(tmpData.u8Data1, tmpData.u8Data2, CIRCLE_PONITER_MAP_SIZE, (iconState ? COLOR_RED : COLOR_WHITE));
  }
}

//---------------------------------------------------------------------------//

void prepareLevelSelect(void)
{
  int8_t i = curretLevel+1;

  drawGalaxy();
  // add level select tasks
  addTasksArray_P(levelSelectTasksArr);
  drawTextWindow(getConstCharPtr(worldPA, i-1), buttonA); // name of current world

  do {
    --i;
    auto tmpData = getPicSize(lvlCoordinates, i*2);
    if(tmpData.wData) {  // not asteroids?
      tftDrawCircle(tmpData.u8Data1, tmpData.u8Data2, CIRCLE_PONITER_MAP_SIZE, COLOR_WHITE);
    }    
  } while(i);
  
  resetBtnStates();
}
//---------------------------------------------------------------------------//

void drawCredits(void)
{  
  drawBMP_ERLE_P(GALAXY_PIC_POS_X, GALAXY_PIC_POS_Y, galaxyPic);
  drawTextWindow(getConstCharPtr(creditsPA, creditsSwitchState), emptyText);

  if(creditsSwitchState == 5) { // fix for qr code...
    tftFillRect(6, 32, 52, 53, COLOR_BLACK);
    drawBMP_ERLE_P(GALAXY_PIC_POS_X, GALAXY_PIC_POS_Y, galaxyPic);
  }

  switch(creditsSwitchState) {
    case 0: case 1: {
      drawBMP_ERLE_P(7, 33, cityDogePic);
    } break;

    case 2: {
      tftFillRect(6, 32, 52, 53, COLOR_WHITE);
      tftDrawXBitmap(7, 33, creditPicQR, 50, 50, COLOR_BLACK);
    } break;

    case 3: case 4: {
      tftFillRect(6, 32, 52, 53, COLOR_WHITE);
      drawBMP_ERLE_P(7, 33, creditPicOne);
    } break;

    case 5: case 6: case 7: case 8: {
      tftFillRect(104, 32, 52, 53, COLOR_WHITE);
      drawBMP_ERLE_P(105, 33, creditPicTwo);
    } break;

    default: break;
  }

  if(getConstCharPtr(creditsPA, creditsSwitchState++) == 0) {
    creditsSwitchState =0;
    baseTitleTask();
  }
}

//---------------------------------------------------------------------------//

void drawSomeGUI(void)
{
  drawBMP_ERLE_P(0, 119, hudGuiPic);
  
  char buf[5]; // i'm pretty sure what 5 bytes will be enough...
  tftSetTextSize(1);
  tftFillRect(SCORE_POS_X, SCORE_POS_Y, 20, 7, currentBackGroundColor);
  tftPrintAt(SCORE_POS_X, SCORE_POS_Y, itoa(score, buf, 10));

  tftFillRect(SHIP_ENERGY_POS_X, SHIP_ENERGY_POS_Y, (ship.health>>2) - 4, SHIP_ENERGY_H, COLOR_WHITE);
}

void waitEnd(void)
{
  if(getBtnState(BUTTON_B)) {
    resetBtnStates();
#if ADD_SOUND
    sfxPlayCancel();
#endif
    addCreditsTasks();
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

void printScore(void)
{
  char buf[5];

  tftSetTextSize(1);
  tftPrintAt_P(40, 60, (const char *)scoreP);
  tftPrintAt(80, 60, itoa(score, buf, 10));

  // draw hi score
  int16_t hiScore = gameSaveData.score;
  if(score > hiScore) {
    gameSaveData.score = score;
    hiScore = score;
  }
  tftPrintAt_P(40, 70, (const char *)maxScoreP);
  tftPrintAt(80, 70, itoa(hiScore, buf, 10));
  score = 0;

  setSaveData(EE_ADDR_SAVE_DATA, &gameSaveData.rawData[0]);
}

void done(const uint8_t *text) // fantasy end, bad name for function...
{
  drawText(20, 40, 2, text);
  levelBaseInit();
}

void gameOver(void)
{
  done(gameOverP);
  printScore();

  // add game over tasks
  addTasksArray_P(gameOverTasksArr);
  resetBtnStates();

  curretLevel =0;
  menuSwitchSelectState = 0;
  initBaseGameParams();
}

void levelClear(void)
{
  done(levelClearP);
}

void victory(void)
{
  curretLevel =0;
  menuSwitchSelectState = 0;
  gameSaveData.bonusUnlocked =1;

  drawText(20, 40, 2, victoryP);
  printScore();
}
