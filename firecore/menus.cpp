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

int8_t currentShip = SHIP_TYPE_POWER;
int8_t previousShip =0;
uint8_t dogeDialogs =0;
uint8_t menuSwitchSelectState =0;
uint8_t creditsSwitchState =0;

bool iconState = false;
bool rndFlag = false;

pFunc_t pCallBackWaitEvent = nullptr;

// -------------------------- Main menu -------------------------- //
void drawShipSelectionMenu(void)
{
  uint8_t posX = CHARACTER_ICON_OFFSET_X;
  uint8_t characterIconStep = (TFT_W /(CHARACTER_ICON_NUM+gameSaveData.bonusUnlocked));
  screenSliderEffect(getAlphaReplaceColorId());

  for(uint8_t count=0; count<(CHARACTER_ICON_NUM+gameSaveData.bonusUnlocked); count++) {
    drawPico_DIC_P(posX, CHARACTER_ICON_OFFSET_Y, getPicPtr(catsPics, count));
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
  uint8_t posX = characterIconStep*currentShip + CHARACTER_ICON_OFFSET_X;
  uint16_t color = iconState ? COLOR_WHITE : COLOR_BLACK;
  tftDrawRect(posX, CHARACTER_ICON_OFFSET_Y, CHARACTER_FRAME_WH, CHARACTER_FRAME_WH, color);

  if(currentShip != previousShip) {
#if ADD_SOUND
    sfxPlayTick();
#endif

    // update ship pic
    ship.type = currentShip;
    ship.sprite.pPic = getPicPtr(shipsPics, ship.type);

    posX = characterIconStep*previousShip + CHARACTER_ICON_OFFSET_X;
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
    case SHIP_TYPE_SPEED: {
      state = {SHIP_BASE_SPEED, SHIP_BASE_DAMAGE-10, SHIP_BASE_DURAB-20};
    } break;

    case SHIP_TYPE_POWER: {
      state = {SHIP_BASE_SPEED-1, SHIP_BASE_DAMAGE, SHIP_BASE_DURAB-10};
    } break;

    case SHIP_TYPE_DURAB: {
      state = {SHIP_BASE_SPEED-2, SHIP_BASE_DAMAGE-5, SHIP_BASE_DURAB};
    } break;

    case SHIP_TYPE_BONUS: { // unclock after saving the galaxy
      state = {SHIP_BASE_SPEED, SHIP_BASE_DAMAGE, SHIP_BASE_DURAB};
    } break;
  }

  ship.states = state;
}
//---------------------------------------------------------------------------//

void pauseMenu(void)
{
  if(getBtnState(BUTTON_Y)) {
    if(!pauseState) { // is it already stopped?
      pauseState = !pauseState;
      vTSMDisableAllTasks();
      vTSMEnableTask(playMusic);
      vTSMEnableTask(updateBtnStates);
      vTSMEnableTask(pauseMenu);
      pauseWindow();
    }
    
    if(getBtnState(BUTTON_B)) {
      resetBtnStates();
#if ADD_SOUND
      sfxPlayOK();
#endif
      pauseState = !pauseState;
      vTSMEnableAllTasks();
      disableWeaponGift(); // fix glitch
      // remove "Pause" text
      //fillRectFast(PAUSE_TEXT_POS_X, PAUSE_TEXT_POS_Y, PAUSE_TEXT_W, PAUSE_TEXT_H);
      tftFillRect(PAUSE_TEXT_POS_X, PAUSE_TEXT_POS_Y, PAUSE_TEXT_W, PAUSE_TEXT_H, currentBackGroundColor);
      disablePause();
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
    // addGameModeSelectTasks();
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

// bool gameModeSelectAction(void)
// {
//   bool state = false;
//   if(getBtnState(BUTTON_A)) {
//     resetBtnStates();
//     endlessAsteroidField = false;

//     swhitch(currentModeVal)
//     {
//       case MODE_NEW_GAME: {
//         addHistoryTasks();
//         drawGalaxyAt(0);
//         state = true;
//       } break;

//       case MODE_ENDLESS_GAME: {
//         addAsteroidsTasks();
//         endlessAsteroidField = true;
//       } break;

//       case MODE_CREDITS: {
//         addCreditsTasks();
//       } break;

//       default: break;
//     }
//   }

//   return state;
// }

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
    
    text_t *pText = getConstCharPtr(dogePA, dogeDialogs);

    if(pText) {
      drawTextWindow(pText, buttonB);

      switch(++dogeDialogs) {
        case 5: {
          vTSMEnableTask(drawStaticNoise);
        } break;
        case 7: {
          vTSMDisableTask(drawRandomDoge);
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
    
    uint8_t colorId = getPicByte(lvlColors + curretLevel);
    screenSliderEffect(colorId);

    switch(curretLevel) {
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
// We made for you tiny state machine which called by state machine, so, you can switch states
void menuSwitchSelect(void)
{
  bool switchSate = false;

  switch(menuSwitchSelectState)
  {
  case M_SWITCH_TITLE: {
    switchSate = titleAction();
  } break;

  // case M_SWITCH_MODE_SELECT: {
  //   switchSate = gameModeSelectAction();
  // } break;

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
  screenSliderEffect(COLOR_ID_BLACK);
  drawPico_DIC_P(GALAXY_PIC_POS_X, y, galaxyPic);
}

void drawGalaxy(void)
{
  setNewBackgroundColor();
  drawGalaxyAt(GALAXY_PIC_POS_Y);
}

//---------------------------------------------------------------------------//
void baseStory(void)
{
  drawGalaxy();
  drawTextWindow(getConstCharPtr(dogePA, dogeDialogs++), buttonB);
  tftDrawRect(6, 32, 52, 53, COLOR_WHITE); // Frame Dodge

  curretLevel = HOME_PLANET_ID;
  addStoryTasks();
}

void drawRandomDoge(void)
{
  rndFlag = !rndFlag;
  drawPico_DIC_P(7, 33+rndFlag, cityDogePic);

  uint16_t *ptr = (uint16_t*)0x0000; // base adress for random
  uint8_t dataSize = (RN & 31)+1;

  do {
    ptr += RN; // make offset for random data
    tftSetAddrPixel(7+(RN%DOGE_PIC_W), 33+(RN%DOGE_PIC_W));
    pushColorFast(pgm_read_word(ptr));
  } while(--dataSize);

  vTSMUpdateTaskTimeCheck(drawRandomDoge, (RN & 63) + 40);
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
  auto tmpData = getPicSize(lvlCoordinates, curretLevel<<1);

  if(tmpData.wData) { // not asteroids?
    iconState = !iconState;  // reuse
    tftDrawCircle(tmpData.u8Data1, tmpData.u8Data2, CIRCLE_PONITER_MAP_SIZE, (iconState ? COLOR_RED : COLOR_WHITE));
  }
}

//---------------------------------------------------------------------------//
void createNextLevel(void)
{
  shipHyperJump();
  setGameTasks(waitCallBackTasksArr);
  vTSMUpdateTaskTimeCheck(checkFireButton, playerFireCheck);

  if((++curretLevel) >= MAX_WORLDS) { // is it was final boss?
    victory();
  } else {
    if(++difficultyIncrement > MAX_DIFFICULT_INCREMENT) { // increase speed of all each lvl
      difficultyIncrement = MAX_DIFFICULT_INCREMENT;
    }
    levelClear();
  }
}

void levelBaseInit(void)
{
  ship.lowHealthState = false;
  setLEDValue(LED_R, ship.lowHealthState);
  initShip();
  initInvaders();
}

void prepareLevelSelect(void)
{
  int8_t i = curretLevel+1;

  drawGalaxy();
  // add level select tasks
  setGameTasks(levelSelectTasksArr);
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
void drawCreditWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h, pic_t *pic)
{
  tftFillRect(x-1, y-1, w, h, COLOR_WHITE);
  drawPico_DIC_P(x, y, pic);

  // auto tmpData = getPicSize(pic, 0);
  // tftFillRect(x-1, y-1, tmpData.u8Data1+2, tmpData.u8Data1+3, COLOR_WHITE);
  // setShakingAvatar(x, y, pPic);
}

void drawCredits(void)
{ 
  tftFillScreen(COLOR_BLACK);
  drawPico_DIC_P(GALAXY_PIC_POS_X, GALAXY_PIC_POS_Y, galaxyPic);
  drawTextWindow(getConstCharPtr(creditsPA, creditsSwitchState), emptyText);

  switch(creditsSwitchState) {
    case 0: case 1: {
      drawCreditWindow(7, 33, 52, 53, cityDogePic); // creditPicQR
    } break;

    // case 2: {
    //   drawCreditWindow(7, 33, 52, 53, cityDogePic);
    // } break;

    case 2: case 3: {
      drawCreditWindow(7, 33, 52, 53, creditPicOne);
    } break;

    case 4: case 5: case 6: case 7: {
      drawCreditWindow(102, 33, 52, 53, creditPicTwo);
    } break;

    default: break;
  }

  if(getConstCharPtr(creditsPA, creditsSwitchState++) == 0) {
    creditsSwitchState =0;
    baseTitleTask();
  }
}

//---------------------------------------------------------------------------//
void waitScreen(void)
{
  if(getBtnState(BUTTON_B)) {
    resetBtnStates();
#if ADD_SOUND
    sfxPlayCancel();
#endif
    pCallBackWaitEvent();
  }
}

void printScore(void)
{
  char buf[SCORE_DIGITS];

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

  setSaveData(SAVE_DATA_BLOCK, &gameSaveData.rawData[0], sizeof(saveData_t));
}

void done(text_t *text) // fantasy end, bad name for function...
{
  drawText(20, 40, 2, text);
  levelBaseInit();
}

void gameOver(void)
{
  curretLevel =0;
  menuSwitchSelectState = 0;
  pCallBackWaitEvent = baseTitleTask;

  done(gameOverP);
  printScore();

  resetBtnStates();
  setNewBackgroundColor();

  // add game over tasks
  setGameTasks(waitCallBackTasksArr);
  vTSMAddTask_P(T(&drawShipExplosion));
}

void levelClear(void)
{
  done(levelClearP);
  vTSMAddTask_P(T(&printDialogeText));
  vTSMDisableTask(printDialogeText);

  pCallBackWaitEvent = prepareLevelSelect;
}

void victory(void)
{
  curretLevel =0;
  menuSwitchSelectState = 0;
  gameSaveData.bonusUnlocked =1;
  pCallBackWaitEvent = addCreditsTasks;

  drawText(20, 40, 2, victoryP);
  printScore();
}
