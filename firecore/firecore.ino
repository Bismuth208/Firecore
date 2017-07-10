/*
 * Simple game.
 * Defend whole galaxy from Aliens.
 * 
 * Game end when ship's life will empty or last level boss defeated
 * 
 *                    
 * 
 * Main IDE:      Sublime Text 2
 *  Arduino IDE:  1.6.6   (as plugin and compiler)
 * Board(CPU):    Arduino Esplora (ATmega32u4)
 * CPU speed:     16 MHz
 * Program size:  26,600
 *  pics:         10,324
 *  code:         14,072
 * Used RAM:      514 bytes
 * Free RAM:      2046 bytes
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

#include <esploraAPI.h>

#include "taskmanager.h"

#include "pics.h"
#include "textProg.h"
#include "common.h"
#include "notes.h"

//---------------------------------------------------------------------------//
// super puper duper ultra extreemely main structures, DO NOT TOUCH THEM!
taskStates_t taskArr;
taskStatesArr_t pArr[MAX_GAME_TASKS];
//---------------------------------------------------------------------------//

bool lowHealthState = false;
bool shipState = false;
bool pauseState = false;
#if ADD_SOUND
bool soundEnable = true;
#endif

bool weaponLasers = false;
bool weaponGift = false;

int8_t menuItem =0;
uint8_t dogeDialogs =0;
uint8_t curretLevel =0;

uint8_t nextInt =1;
uint8_t randNumA =0;
uint16_t currentBackGroundColor = BACKGROUND_COLOR;
uint16_t replaceColor = 0x01F4;

uint16_t calJoysticX =0;
uint16_t calJoysticY =0;

uint8_t difficultyIncrement =0;

uint16_t score =0;

uint16_t thisNote = 0;

ship_t ship;
gift_t gift;
rocket_t playerRockets[MAX_PEW_PEW];
rocket_t playerLasers[MAX_PEW_PEW];
hudStatus_t hudStatus = {1,1,0}; // need update all
btnStatus_t btnStates = {0};

rocket_t *pRocketGlobal;

soundSample_t music;

const uint8_t lvlCoordinates[] PROGMEM = {
  WORLD_0_POS_X, WORLD_0_POS_Y,
  WORLD_1_POS_X, WORLD_1_POS_Y,
  WORLD_2_POS_X, WORLD_2_POS_Y,
  WORLD_3_POS_X, WORLD_3_POS_Y,
  WORLD_4_POS_X, WORLD_4_POS_Y,
  WORLD_5_POS_X, WORLD_5_POS_Y,
  WORLD_6_POS_X, WORLD_6_POS_Y,
  WORLD_7_POS_X, WORLD_7_POS_Y,
  WORLD_8_POS_X, WORLD_8_POS_Y,
};

const uint8_t lvlColors[] PROGMEM = {
  0x1E,
  0x01,
  0x1E,
  0x2E,
  0x3E,
  0x01,
  0x04,
  0x00,
  0x04
};

const uint16_t mainTheme[] PROGMEM = {
  // still nothing here
};
//---------------------------------------------------------------------------//

uint8_t randNum(void)
{
  nextInt ^= (nextInt << 3);
  nextInt ^= (nextInt >> 5);
  nextInt ^= (randNumA++ >> 2);
  return nextInt;
}

int32_t mapVal(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// ------------------------ Poll constols ------------------------ //
// poll periodically buttons states
void updateBtnStates(void)
{
  if(buttonIsPressed(BUTTON_A)) {
    btnStates.aBtn = true;
  }

  if(buttonIsPressed(BUTTON_B)) {
    btnStates.bBtn = true;
  }

  if(buttonIsPressed(BUTTON_X)) {
    btnStates.xBtn = true;
  }

  if(buttonIsPressed(BUTTON_Y)) {
    btnStates.yBtn = true;
  }
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

void waitReleaseBtn(uint8_t btn)
{
  while(!buttonIsPressed(btn));
  while(buttonIsPressed(btn));
}
// --------------------------------------------------------------- //

bool checkNewPosition(position_t *objOne, position_t *objTwo)
{
  bool state = false;
  // is position changed?
  if((objOne->x != objTwo->x) || (objOne->y != objTwo->y)) {
    state = true;
  }
  return state;
}

void applyNewPosition(position_t *objOne, position_t *objTwo, uint16_t picW, uint16_t picH)
{
  // clear previos position
  fillRectFast(objOne->x, objOne->y, picW, picH);
  // store new position
  *objOne = *objTwo;
}

void movePicture(objPosition_t *pObj, uint16_t picW, uint16_t picH)
{
  // is position changed?
  if((pObj->Base.x != pObj->New.x) || (pObj->Base.y != pObj->New.y)) {
    // clear previos position
    fillRectFast(pObj->Base.x, pObj->Base.y, picW, picH);
    // store new position
    pObj->Base = pObj->New;
  }
}

int16_t checkShipPosition(int16_t pos, int16_t min, int16_t max)
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

// fast and low cost memset
void memset_F(void *pvDest, uint8_t src, size_t size)
{
  uint8_t *dest = (uint8_t*)pvDest;
  while(size--) {
    *dest++ = src;
  }
}

void playMusic(void)
{
  uint16_t freq = pgm_read_word(&music.pCurrent[music.currentNote]);
  uint16_t noteDuration = pgm_read_word(&music.pCurrent[music.currentNote+1]);

  if(freq) { // silense or not?
    toneBuzz(freq, noteDuration);
  }
  updateTaskTimeCheck(playMusic, noteDuration);

  if((music.currentNote+=2) > music.notesNum) {
    music.currentNote = 0;
  }
}
// --------------------------------------------------------------- //

void checkFireButton(void)
{
  rocket_t *pRokets;

  if(getBtnState(BUTTON_A)) {
    resetBtnStates();
    if((ship.weapon.rocketsLeft -= PLAYER_ROCKET_COST) < MAX_PEW_PEW) {
      // store to local registers
      pRokets = &pRocketGlobal[ship.weapon.rocketsLeft];
      if(pRokets->onUse == false) {
#if ADD_SOUND
        if(soundEnable) toneBuzz(500, 10);
#endif    
        pRokets->onUse = true;
        pRokets->pos.x = ship.pos.Base.x + ROCKET_OFFSET_X; // \__ start position
        pRokets->pos.y = ship.pos.Base.y + ROCKET_OFFSET_Y; // /
      }
    } else {
      ship.weapon.rocketsLeft = MAX_PEW_PEW;
    }
  }
}

//---------------------------------------------------------------------------//
void applyShipDamage(rocket_t *pWeapon)
{
  rocketEpxlosion(pWeapon);

  // clear previous level
  tftFillRect(SHIP_ENERGY_POS_X, SHIP_ENERGY_POS_Y,
                    (ship.health>>2) - 4, SHIP_ENERGY_H, COLOR_RED);
  ship.health -= DAMAGE_TO_SHIP;   // absorb damage
  hudStatus.updLife = true;        // update new life status later
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
  int16_t posX = ship.pos.New.x;
  int16_t posY = ship.pos.New.y;
  uint8_t speed = ship.states.speed;

  newValueXY = getStickVal(LINE_X);
  if(newValueXY != calJoysticX) {
    if(newValueXY < calJoysticX) {
      posX += speed;
    } else {
      posX -= speed;
    } 
  }
  //applyShipDirection(&ship.posNew.x, calJoysticX, LINE_X);

  newValueXY = getStickVal(LINE_Y);
  if(newValueXY != calJoysticY) {
    if(newValueXY > calJoysticY) {
      posY += speed;
    } else {
      posY -= speed;
    } 
  }
  //applyShipDirection(&ship.posNew.y, calJoysticY, LINE_Y); 

  ship.pos.New.x = checkShipPosition(posX, SHIP_MIN_POS_X, SHIP_MAX_POS_X);
  ship.pos.New.y = checkShipPosition(posY, SHIP_MIN_POS_Y, SHIP_MAX_POS_Y);
}

void swapWeapon(void)
{
  if(getBtnState(BUTTON_X)) {
    resetBtnStates();
    if(weaponLasers) {
      pRocketGlobal = &playerLasers[0];
      ship.states.power -= WEAPON_ROCKET_DMG;
      ship.weapon.pPic = getConstCharPtr(laserPics, ship.weapon.level); 
      ship.weapon.picSize = pgm_read_byte(laserPicsSize + ship.weapon.level);
    } else {
      pRocketGlobal = &playerRockets[0];
      ship.states.power += WEAPON_ROCKET_DMG;
      ship.weapon.pPic = rocketPic;
      ship.weapon.picSize = ROCKET_PIC_SIZE;
    }
    weaponLasers = !weaponLasers;
  }
}

//---------------------------------------------------------------------------//
void moveGift(void)
{
  if((gift.pos.New.x -= GIFT_MOVE_SPEED) >= TFT_W) {
    movePicture(&gift.pos, GIFT_PIC_W, GIFT_PIC_H); // remove gift from screen
    if(weaponGift) {
      disableWeaponGift();
    } else {
      createNextLevel();
    }    
  }
}

void checkGift(void)
{
  rocket_t *pRocket = pRocketGlobal;

  for(uint8_t countR =0; countR < MAX_PEW_PEW; countR++) {
    if(pRocket->onUse) {
      if(checkCollision(&pRocket->pos, LASER_PIC_W, LASER_PIC_H, 
                        &gift.pos.Base, GIFT_PIC_W, GIFT_PIC_H)) {

        rocketEpxlosion(pRocket);
        score += GIFT_BONUS_SCORE;
        hudStatus.updScore = true;    // update score later

        movePicture(&gift.pos, GIFT_PIC_W, GIFT_PIC_H); // remove gift from screen

        if(weaponGift) {
          ship.states.power += WEAPON_GIFT_BONUS;
          if((++ship.weapon.level) >= MAX_WEAPON_LVL) {
            ship.weapon.level = MAX_WEAPON_LVL;
          }
          ship.weapon.pPic = getConstCharPtr(laserPics, ship.weapon.level); 
          ship.weapon.picSize = getPicByte(laserPicsSize + ship.weapon.level);
          disableWeaponGift();
        } else {
          if((ship.health += GIFT_HEALTH_RESTORE) > SHIP_HEALTH) {
            ship.health = SHIP_HEALTH;
          }
          createNextLevel();
        }
      }
    }
    ++pRocket;
  }
}

void disableWeaponGift(void)
{
  gift.pPic = giftHeartPic;
  gift.picSize = GIFT_HEART_PIC_SIZE;

  weaponGift = false;
  updateTaskStatus(moveGift, false);
  updateTaskStatus(checkGift, false);
  updateTaskStatus(drawGift, false);
}

void dropWeaponGift(void)
{
  gift.pPic = giftWeaponPic;
  gift.picSize = GIFT_WEAPON_PIC_SIZE;
  gift.pos.New.x = GIFT_BASE_POS_X;
  gift.pos.New.y = GIFT_BASE_POS_Y;

  weaponGift = true;
  updateTaskStatus(dropWeaponGift, false);
  updateTaskStatus(moveGift, true);
  updateTaskStatus(drawGift, true);
  updateTaskStatus(checkGift, true);
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

// mother of God, what i've done...
void addTasksArray(tasksArr_t *pArr, uint8_t size)
{
  deleteAllTasks();
  setMaxTasks(size); // less tasks more perfomance

  while(size--) {
    // get ponter to task structure and load params from structure
    addTask_P(pgm_read_word(pArr++));
  }
}

void addTitleTasks(void)
{
  //music.pCurrent = mainTheme;
  //music.notesNum = 17;
  //music.currentNote =0;

  addTasksArray(titleTasksArr, TITLE_TASKS_COUNT);
}

void addGameTasks(void)
{
  ship.pos.New.x = SHIP_GAME_POS_X;
  ship.pos.New.y = SHIP_GAME_POS_Y;

  //music.pCurrent = gameTheme;
  //music.notesNum = X;
  //music.currentNote =0;

  addTasksArray(gameTasksArr, GAME_TASKS_COUNT);

  updateTaskTimeCheck(dropWeaponGift, RAND_GIFT_SPAWN_TIME);
  updateTaskStatus(moveGift, false);
  updateTaskStatus(drawGift, false);
  updateTaskStatus(checkGift, false);
}

void addBossTasks(void)
{
  ship.states.power = DAMAGE_TO_BOSS;

  //music.pCurrent = bossTheme;
  //music.notesNum = X;
  //music.currentNote =0;

  addTasksArray(bossTasksArr, BOSS_TASKS_COUNT);
}

void addGiftTasks(void)
{
  // drop gift from the boss
  gift.pos.New.x = GIFT_BASE_POS_X;
  gift.pos.New.y = GIFT_BASE_POS_Y;
  //gift.state = false;

  addTasksArray(giftTasksArr, GIFT_TASKS_COUNT);
}

void addShipSelectTasks(void)
{
  ship.pos.New.x = SHIP_SELECT_POS_X;
  ship.pos.New.y = SHIP_SELECT_POS_Y;
  addTasksArray(shipSelTasksArr, SHIP_SEL_TASKS_COUNT);
}

void baseTitleTask(void)
{
  tftFillScreen(currentBackGroundColor);
  addTask_P(T(&drawRows));
}
// --------------------------------------------------------------- //

void setMainFreq(uint8_t ps)
{
  // This function set prescaller,
  // which change main F_CPU freq
  // I have Atmega328P and F_CPU = 16 MHz!
  // at another freq will be another result!
  CLKPR = 0x80;  //enable change prascaller
  CLKPR = ps;    //change prescaller of F_CPU
}

// ------------- Init section ---------------------- //
void readScore(void)
{
  uint8_t varCheck = getSaveDataMark(EE_ADDR_MARK);

  if(varCheck != HI_SCORE_MARK ) { // no save data in EEPROM?
    resetScore();
  }
}

void resetScore(void)
{
  writeSaveData(EE_ADDR_SCORE, score);
  writeSaveData(EE_ADDR_MARK, HI_SCORE_MARK);
}

void calibrateJoyStic(void)
{
  calJoysticX = getStickVal(LINE_X);
  calJoysticY = getStickVal(LINE_Y);
}

// caled every level
void initShip(void)
{
  ship.pos.New.x = SHIP_TITLE_POS_X;
  ship.pos.New.y = SHIP_TITLE_POS_Y;
  ship.weapon.rocketsLeft = MAX_PEW_PEW;
  ship.weapon.overHeated = false;

  ship.weapon.pPic = getConstCharPtr(laserPics, ship.weapon.level); 
  ship.weapon.picSize = getPicByte(laserPicsSize + ship.weapon.level);

  memset_F(playerLasers, 0x00, sizeof(rocket_t)*MAX_PEW_PEW);
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
  nextInt += readMic();
  nextInt += readTemp();
  nextInt += readLight();
}

void initBaseGameParams(void)
{
  initRand();
  initShip();
  initStars();
  initInvaders();

  // this one init only once
  ship.health = SHIP_HEALTH;
  ship.weapon.level = 0;
  pRocketGlobal = &playerLasers[0];
}

void initSys(void)
{
  initEsplora();
  initBaseGameParams();
  calibrateJoyStic();
  readScore();

  tftSetRotation(1);

  initTasksArr(&taskArr, &pArr[0], 1); // at start moment need only 1 task
  baseTitleTask(); // this task
}

//------------------------- yep, here's how it all began... -------------------//
__attribute__((OS_main)) int main(void)
{
  initSys();
  runTasks();
}
//-----------------------------------------------------------------------------//
