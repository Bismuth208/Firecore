/*
 * Simple game.
 * Defend whole galaxy from Aliens.
 * 
 * Game end when ship's life will empty or last level boss defeated
 * 
 *                    
 * 
 * Main IDE:      Sublime Text 2
 *  Arduino IDE:  1.8.1   (as plugin and compiler)
 * Board(CPU):    Arduino Esplora (ATmega32u4)
 * CPU speed:     16 MHz
 * Program size:  22,784
 *  pics:         5,233
 *  code:         17,551
 * Used RAM:      1,355 bytes
 * Free RAM:      1,205 bytes
 *
 * Language:      C and C++
 * 
 * Author: Antonov Alexandr (Bismuth208)
 * Date:   2 June, 2017
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
 * 
 */

#include "pics.h"
#include "textProg.h"
#include "common.h"

//---------------------------------------------------------------------------//
// super puper duper ultra extreemely main structures, DO NOT TOUCH THEM!
taskStates_t taskArr;
taskStatesArr_t pArr[MAX_GAME_TASKS];
//---------------------------------------------------------------------------//

bool lowHealthState = false;
bool pauseState = false;
bool weaponLasers = false;
bool weaponGift = false;

int8_t menuItem =0;
uint8_t curretLevel =0;
uint8_t difficultyIncrement =0;
uint16_t score =0;

uint16_t calJoysticX =0;
uint16_t calJoysticY =0;

ship_t ship;
gift_t gift;
rocket_t playerLasers[MAX_PEW_PEW];
hudStatus_t hudStatus = {1,1}; // need update all
btnStatus_t btnStates = {0};

saveData_t gameSaveData;

bezier_t bezierLine;

// for bezier line
// x1,y1, x2,y2, x3,y3, steps
const uint8_t lineCurves[] PROGMEM = {
  130, 40, 85,120,  0, 40,50,
  130, 30, 85, 50,  0, 64,50,
  130, 95,115,130,  0,  0,50,
  130,  0,115, 10,  0,120,50,
  130, 10, 85, 50,  0, 62,50,
  130, 50, 85, 70,  0, 10,50,
  // this one not counted in MAX_BEZIER_LINES
  135,  0, 90, 45,135, 90,40, // boss move down
  135, 90, 90, 45,135,  0,40, // boss move up
  144, 40, 85,120,  0, 40,50  // gift
};

const uint8_t lvlCoordinates[] PROGMEM = {
  WORLD_0_POS_X, WORLD_0_POS_Y,
  WORLD_1_POS_X, WORLD_1_POS_Y,
  WORLD_2_POS_X, WORLD_2_POS_Y,
  WORLD_3_POS_X, WORLD_3_POS_Y,
  WORLD_4_POS_X, WORLD_4_POS_Y,
  WORLD_5_POS_X, WORLD_5_POS_Y,
  WORLD_6_POS_X, WORLD_6_POS_Y,
  WORLD_7_POS_X, WORLD_7_POS_Y,
  WORLD_8_POS_X, WORLD_8_POS_Y
};

// ------------------------ Poll controls ------------------------ //
// poll periodically buttons states
void updateBtnStates(void)
{
  if(buttonIsPressed(BUTTON_A))
    btnStates.aBtn = true;
  if(buttonIsPressed(BUTTON_B))
    btnStates.bBtn = true;
  if(buttonIsPressed(BUTTON_X))
    btnStates.xBtn = true;
  if(buttonIsPressed(BUTTON_Y))
    btnStates.yBtn = true;
}

bool getBtnState(uint8_t btn)
{
  bool state = false;

  switch(btn)
  {
   case BUTTON_A: {
    state = btnStates.aBtn;
   } break;

   case BUTTON_B: {
    state = btnStates.bBtn;
   } break;

   case BUTTON_X: {
    state = btnStates.xBtn;
   } break;

   case BUTTON_Y: {
    state = btnStates.yBtn;
   } break;
  }

  return state;
}

void resetBtnStates(void)
{
  btnStates.zBtn = 0;
}

uint8_t getJoyStickValue(uint8_t pin)
{
  uint16_t newValuePin = readJoystic(pin);

  // approximate newValuePin data, and encode to ASCII 
  // (just because it works and i left it as is)
  return ((newValuePin * 9) >> 10) + 48; // '>>10' same as '/1024'
}

// --------------------------------------------------------------- //

void movePicture(objPosition_t *pObj, uint8_t picW, uint8_t picH)
{
  // is position changed?
  if((pObj->Base.x != pObj->New.x) || (pObj->Base.y != pObj->New.y)) {
    // clear previos position
    fillRectFast(pObj->Base.x, pObj->Base.y, picW, picH);
    // store new position
    pObj->Base = pObj->New;
  }
}

int8_t checkShipPosition(int8_t pos, uint8_t min, uint8_t max)
{ 
  if(pos < min) {
    pos = min;
  } else if(pos > max) {
    pos = max;
  }

  return pos;
}

void applyShipDirection(uint16_t *pos, uint16_t valOne, uint8_t line)
{
  uint16_t newValueXY = getStickVal(line);
  if(newValueXY != valOne) {
    if(newValueXY < valOne) {
      *pos += ship.states.speed;
    } else {
      *pos -= ship.states.speed;
    } 
  }  
}

bool checkCollision(position_t *pObjOne, uint8_t objOneW, uint8_t objOneH,
                    position_t *pObjTwo, uint8_t objTwoW, uint8_t objTwoH)
{
  /* ----------- Check X position ----------- */
  uint16_t objOnePosEndX = (pObjOne->x + objOneW);
  //bool xCollision = false;

  if(objOnePosEndX >= pObjTwo->x) {
    uint16_t objTwoPosEndX = (pObjTwo->x + objTwoW);
    if(pObjOne->x >= objTwoPosEndX) {
      return false; // nope, different X positions
    }
    // ok, objects on same X lines; Go next...
    //xCollision = true;
  } else {
    return false; // nope, absolutelly different X positions
  }

  /* ---------------------------------------- */
  /* ----------- Check Y position ----------- */
  uint16_t objOnePosEndY = (pObjOne->y + objOneH);
  //bool yCollision = false;
  
  if(objOnePosEndY>= pObjTwo->y) {
    uint16_t objTwoPosEndY = (pObjTwo->y + objTwoH);
    if(pObjOne->y <= objTwoPosEndY) {
      // ok, objects on same Y lines; Go next...
      //yCollision = true;
      
      // yep, if we are here
      // then, part of one object collide wthith another object
      return true;
    } else {
      return false; // nope, different Y positions
    }
  } else {
    return false; // nope, absolutelly different Y positions
  }
}

void playMusic(void)
{
#if ADD_SOUND
  // 1/20s or every 50ms
  sfxUpdateAll(); // update sound each frame, as sound engine is frame based
#endif
}

void printDutyDebug(uint32_t duration)
{
  char buf[10];

  tftSetTextSize(1);
  tftFillRect(0, 0, 36, 7, currentBackGroundColor);
  tftPrintAt(0, 0, itoa(duration, buf, 10));
}
// --------------------------------------------------------------- //
void checkFireButton(void)
{
  rocket_t *pRokets = &playerLasers[0];

  if(getBtnState(BUTTON_A)) {
    resetBtnStates();

    for(uint8_t i=0; i<MAX_PEW_PEW; i++) {
      if(!pRokets->onUse) {
#if ADD_SOUND
        sfxPlayPattern(playerShotPattern, SFX_CH_1);
#endif    
        pRokets->onUse = true;
        pRokets->pos.x = ship.pos.Base.x + ROCKET_OFFSET_X; // \__ start position
        pRokets->pos.y = ship.pos.Base.y + ROCKET_OFFSET_Y; // /
        break;
      }
      ++pRokets;
    }
  }
}

//---------------------------------------------------------------------------//
void applyShipDamage(rocket_t *pWeapon)
{
  rocketEpxlosion(pWeapon);

  // clear previous level
  tftFillRect(SHIP_ENERGY_POS_X, SHIP_ENERGY_POS_Y, (ship.health>>2) - 4, SHIP_ENERGY_H, COLOR_RED);

  // Calc damage as: MAX_DURAB - (0.5 * SHIP_DURAB) - DAMAGE_TO_SHIP
  uint8_t damage = SHIP_BASE_DURAB - (ship.states.durability/2 + DAMAGE_TO_SHIP);
  ship.health -= damage;     // absorb damage
  hudStatus.updLife = true;  // update new life status later
}

void checkShipHealth(void)
{
  if(ship.health <  DENGER_HEALTH_LVL) {
    lowHealthState = !lowHealthState;
    setLEDValue(LED_R, lowHealthState); // yyess... every time set this...

    if(ship.health <= 0) {
      gameOver();  // GameOver!
    }
  }
}
//---------------------------------------------------------------------------//

void moveShip(void)
{
  uint16_t newValueXY =0; // store temp value
  position_t pos = ship.pos.New;
  uint8_t speed = ship.states.speed;

  newValueXY = getStickVal(LINE_X);
  if(newValueXY != calJoysticX) {
    if(newValueXY < calJoysticX) {
      pos.x += speed;
    } else {
      pos.x -= speed;
    } 
  }
  //applyShipDirection(&ship.posNew.x, calJoysticX, LINE_X);

  newValueXY = getStickVal(LINE_Y);
  if(newValueXY != calJoysticY) {
    if(newValueXY > calJoysticY) {
      pos.y += speed;
    } else {
      pos.y -= speed;
    }
  }
  //applyShipDirection(&ship.posNew.y, calJoysticY, LINE_Y); 

  ship.pos.New.x = checkShipPosition(pos.x, SHIP_MIN_POS_X, SHIP_MAX_POS_X);
  ship.pos.New.y = checkShipPosition(pos.y, SHIP_MIN_POS_Y, SHIP_MAX_POS_Y);
}

//---------------------------------------------------------------------------//
void moveGift(void)
{
  moveBezierCurve(&gift.pos.New, &gift.bezLine);

  // reach end of move?
  if(gift.bezLine.step == bezierLine.totalSteps) {
    movePicture(&gift.pos, GIFT_PIC_W, GIFT_PIC_H); // remove gift from screen
    if(weaponGift) {
      disableWeaponGift();
    } else {
      createNextLevel();
    }    
  }
}

void giftDone(void)
{
  score += GIFT_BONUS_SCORE;
  hudStatus.updScore = true;    // update score later

  // remove gift from screen
  fillRectFast(gift.pos.Base.x, gift.pos.Base.y, GIFT_PIC_W, GIFT_PIC_H);
}

void checkGift(void)
{
  rocket_t *pRocket = &playerLasers[0];

  if(weaponGift) {
    for(uint8_t countR =0; countR < MAX_PEW_PEW; countR++) {
      if(pRocket->onUse) {
        if(checkCollision(&pRocket->pos, LASER_PIC_W, LASER_PIC_H, 
                        &gift.pos.Base, GIFT_PIC_W, GIFT_PIC_H)) {

          rocketEpxlosion(pRocket);
#if ADD_SOUND
          sfxPlayPattern(playerSuperPattern, SFX_CH_2);
#endif
          giftDone();

          ship.states.power += WEAPON_GIFT_BONUS;
          if((++ship.weapon.level) > MAX_WEAPON_LVL) {
            ship.weapon.level = MAX_WEAPON_LVL;
          }
          ship.weapon.pPic = getConstCharPtr(laserPics, ship.weapon.level);
          disableWeaponGift();
          return;
        }
      }
      ++pRocket;
    }
  } else { // heart and end of the level
    if(checkCollision(&ship.pos.Base, SHIP_PIC_W, SHIP_PIC_H, 
                        &gift.pos.Base, GIFT_PIC_W, GIFT_PIC_H)) {
      giftDone();

      if((ship.health += GIFT_HEALTH_RESTORE) > SHIP_HEALTH) {
        ship.health = SHIP_HEALTH;
      }
      createNextLevel();
    }
  }
}

void disableWeaponGift(void)
{
  updateTaskStatus(dropWeaponGift, false); // to be shure and pause fix

  updateTaskStatus(moveGift, false);
  updateTaskStatus(checkGift, false);
  updateTaskStatus(drawGift, false);
}

void dropWeaponGift(void)
{
  gift.pPic = giftWeaponPic;
  weaponGift = true;
  updateTaskStatus(dropWeaponGift, false);

  addTask_P(T(&moveGift));
  addTask_P(T(&drawGift));
  addTask_P(T(&checkGift));
}

// --------------------------------------------------------------- //

void createNextLevel(void)
{
  shipHyperJump();
  deleteAllTasks();
  addTask_P(T(&updateBtnStates));

  if((++curretLevel) >= MAX_WORLDS) { // is it was final boss?
    victory();
    addTask_P(T(&waitEnd));
  } else {
    totalRespawns = ALIEN_KILLS_TO_BOSS + difficultyIncrement;
    if(++difficultyIncrement > MAX_DIFFICULT_INCREMENT) { // increase speed of all each lvl
      difficultyIncrement = MAX_DIFFICULT_INCREMENT;
    }
    levelClear();
    addTask_P(T(&waitOk));
    addTask_P(T(&printDialogeText));
    updateTaskStatus(printDialogeText, false);
  }  
}

void levelBaseInit(void)
{
  lowHealthState = false;
  setLEDValue(LED_R, false);
  initShip();
  initInvaders();
}

// --------------------------------------------------------------- //
void addGameTasks(void)
{
  ship.pos.New.x = SHIP_GAME_POS_X;
  ship.pos.New.y = SHIP_GAME_POS_Y;

  addTasksArray_P(gameTasksArr, GAME_TASKS_COUNT);

  gift.bezLine.id = GIFT_MOVE_ID;
  gift.bezLine.step =0;
  moveBezierCurve(&gift.pos.New, &gift.bezLine);
  updateTaskTimeCheck(dropWeaponGift, RAND_GIFT_SPAWN_TIME);
}

void addBossTasks(void)
{
  addTasksArray_P(bossTasksArr, BOSS_TASKS_COUNT);
}

// drop gift from the boss
void addGiftTasks(void)
{
  weaponGift = false;
  gift.bezLine.id = GIFT_MOVE_ID;
  gift.bezLine.step =0;
  gift.pPic = giftHeartPic;
  moveBezierCurve(&gift.pos.New, &gift.bezLine);

  addTasksArray_P(giftTasksArr, GIFT_TASKS_COUNT);
}

void addShipSelectTasks(void)
{
  ship.pos.New.x = SHIP_SELECT_POS_X;
  ship.pos.New.y = SHIP_SELECT_POS_Y;
  addTasksArray_P(shipSelTasksArr, SHIP_SEL_TASKS_COUNT);
}

void addStoryTasks(void)
{
  addTasksArray_P(storyTasksArr, STORY_TASKS_COUNT);
  updateTaskStatus(drawStaticNoise, false);
}

void addHistoryTasks(void)
{
  textHistoryPosX = 0;
  addTasksArray_P(historyTasksArr, HISTORY_TASKS_COUNT);
}

void addTitleTasks(void)
{
  addTasksArray_P(titleTasksArr, TITLE_TASKS_COUNT);
}

void baseTitleTask(void)
{
  tftFillScreen(currentBackGroundColor);
  deleteAllTasks();
  addTask_P(T(&drawRows));
  addTask_P(T(&playMusic));

#if ADD_SOUND
  sfxPlayPattern(unfoldPattern, SFX_CH_0);
#endif
}

// --------------------------------------------------------------- //

void setMainFreq(uint8_t ps)
{
  // This function set prescaller,
  // which change main F_CPU freq
  // I have Atmega32U4 and F_CPU == 16 MHz!
  // at another freq will be another result!
  CLKPR = 0x80;  //enable change prascaller
  CLKPR = ps;    //change prescaller of F_CPU
}

// ------------- Init section ---------------------- //
void readScore(void)
{
  getSaveData(EE_ADDR_SAVE_DATA, &gameSaveData.rawData[0]);

  if(gameSaveData.saveDataMark != HI_SCORE_MARK ) { // no save data in EEPROM?
    resetScore();
  }
}

void resetScore(void)
{
  memset_F(&gameSaveData.rawData[0], 0x00, sizeof(saveData_t));
  gameSaveData.saveDataMark = HI_SCORE_MARK;

  setSaveData(EE_ADDR_SAVE_DATA, &gameSaveData.rawData[0]);
}

void calibrateJoystick(void)
{
  calJoysticX = getStickVal(LINE_X);
  calJoysticY = getStickVal(LINE_Y);
}

// caled every level
void initShip(void)
{
  ship.pos.New.x = SHIP_TITLE_POS_X;
  ship.pos.New.y = SHIP_TITLE_POS_Y;
  //ship.weapon.rocketsLeft = MAX_PEW_PEW;
  ship.weapon.overHeated = false;
  ship.weapon.pPic = getConstCharPtr(laserPics, ship.weapon.level);

  memset_F(playerLasers, 0x00, sizeof(rocket_t)*MAX_PEW_PEW);
}

void resetShip(void)
{
  ship.health = SHIP_HEALTH;
  ship.weapon.level = 1;
  ship.weapon.pPic = getConstCharPtr(laserPics, ship.weapon.level);
  ship.type = 3; // he he he
  ship.pBodyPic = getConstCharPtr(shipsPics, ship.type);
}

void initStars(void)
{
  for(uint8_t count =0; count < MAX_STARS; count++) {
    stars[count].pos.x = RN % TFT_W;
    stars[count].pos.y = RN % STARS_MAX_POS_Y;
    stars[count].color = RAND_STAR_CLR;
    stars[count].speed = RN % STAR_STEP + 1;
  }
}

void initRand(void)
{
  // yes, it real "random"!
  seedRndNum(readMic());
  seedRndNum(readTemp());
  seedRndNum(readLight());
}

void initBaseGameParams(void)
{
  initRand();
  initShip();
  initStars();
  initInvaders();

  // this one init only once
  resetShip();
}

void initSys(void)
{
  initEsplora();
  initBaseGameParams();
  calibrateJoystick();
  readScore();

  // place palette in RAM for faster access
  memcpy_P(&palette_RAM[0], palette_ext, PALETTE_SIZE);

  initTasksArr(&taskArr, &pArr[0], MAX_GAME_TASKS);
  baseTitleTask(); // at start moment need only this task
}

//------------------------- yep, here's how it all began... -------------------//
__attribute__((noreturn)) int main(void)
{
  initSys();
  runTasks();
}
//-----------------------------------------------------------------------------//
