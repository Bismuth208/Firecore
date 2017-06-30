/*
 * Simple game.
 * Defend whole galaxy from Aliens.
 * 
 * Game end when ship's life will empty or main boss defeated
 * 
 *                    
 * 
 * Main IDE:      Sublime Text 2
 *  Arduino IDE:  1.6.6   (as plugin and compiler)
 * Board(CPU):    Arduino Esplora (ATmega32u4)
 * CPU speed:     16 MHz
 * Program size:  24,184
 *  pics:         10,132
 *  code:         14,022
 * Used RAM:      487 bytes
 * Free RAM:      2073 bytes
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

uint16_t score;

ship_t ship;
gift_t gift;
rocket_t playeRockets[MAX_PEW_PEW];
hudStatus_t hudStatus = {1,1}; // need update all
btnStatus_t btnStates = {0};

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
  0x10,
  0x20,
  0x30,
  0x01,
  0x04,
  0x10,
  0x04
};
//---------------------------------------------------------------------------//

uint8_t randNum(void)
{
  nextInt ^= (nextInt << 3);
  nextInt ^= (nextInt >> 5);
  nextInt ^= (randNumA++ >> 2);
  return nextInt;
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// ------------------------ Poll constols ------------------------ //
void getBtnStates(void)
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

bool checkCollision(position_t *pObjOne, uint16_t objOneW, uint16_t objOneH,
                    position_t *pObjTwo, uint16_t objTwoW, uint16_t objTwoH)
{
  /* ----------- Check X position ----------- */
  //uint16_t objOnePosEndX = (pObjOne->x + objOneW);
  //bool xCollision = false;

  if((pObjOne->x + objOneW) >= pObjTwo->x) { // objOnePosEndX
    //uint16_t objTwoPosEndX = (pObjTwo->x + objTwoW);
    if(pObjOne->x >= (pObjTwo->x + objTwoW)) { // objTwoPosEndX
      return false; // nope, different X positions
    }
    // ok, objects on same X lines; Go next...
    //xCollision = true;
  } else {
    return false; // nope, absolutelly different X positions
  }

  /* ---------------------------------------- */
  /* ----------- Check Y position ----------- */
  //uint16_t objOnePosEndY = (pObjOne->y + objOneH);
  //bool yCollision = false;
  
  if((pObjOne->y + objOneH) >= pObjTwo->y) { // objOnePosEndY
    //uint16_t objTwoPosEndY = (pObjTwo->y + objTwoH);
    if(pObjOne->y <= (pObjTwo->y + objTwoH)) { // objTwoPosEndY
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

// --------------------------------------------------------------- //

void checkFireButton(void)
{
  rocket_t *pRokets;

  if(btnStates.aBtn) {
    btnStates.aBtn = false;
    if(ship.weapon.overHeated) {
      ship.weapon.rocketsLeft += PLAYER_ROCKET_REFILL;
      if(ship.weapon.rocketsLeft > MAX_PEW_PEW) {
         ship.weapon.rocketsLeft = MAX_PEW_PEW;
         ship.weapon.overHeated = false;
      }
      return;
    }
      
    if((ship.weapon.rocketsLeft -= PLAYER_ROCKET_COST) < MAX_PEW_PEW) {
      // store to local registers
      pRokets = &playeRockets[ship.weapon.rocketsLeft];
      if(pRokets->onUse == false) {
#if ADD_SOUND
        if(soundEnable) toneBuzz(500, 10);
#endif    
        pRokets->onUse = true;
        pRokets->pos.x = ship.pos.Base.x + ROCKET_OFFSET_X; // \__ start position
        pRokets->pos.y = ship.pos.Base.y + ROCKET_OFFSET_Y; // /
      }
    } else {
      ship.weapon.rocketsLeft = 0;
      ship.weapon.overHeated = true;
    }
  }
}

//---------------------------------------------------------------------------//
void applyShipDamage(inVader_t *pAlien)
{
  rocketEpxlosion(&pAlien->deathRay);
  pAlien->timeToShoot = RAND_SHOOT_TIME;

  // clear previous level
  tftFillRect(0, SHIP_ENERGY_POS_Y, (ship.health>>2) - 4, SHIP_ENERGY_H, COLOR_RED);
  ship.health -= DAMAGE_TO_SHIP;   // absorb damage
  hudStatus.updLife = true;        // update new life status later
}

void checkShipDamage(void)  // check damage from deathRay
{
  inVader_t *pAlien = &alien[0];
  for(uint8_t countV=0; countV < MAX_ALIENS; countV++) {
    if(pAlien->deathRay.onUse) { // is it shoot already?
      if(checkCollision(&pAlien->deathRay.pos, DEATHRAY_PIC_W, DEATHRAY_PIC_H,
                                    &ship.pos.Base, SHIP_PIC_W, SHIP_PIC_H)) {
        applyShipDamage(pAlien);
      }
    }
    ++pAlien;
  }
}

void checkShipBossDamage(void)  // check damage from boss deathRay
{
  if(alienBoss.deathRay.onUse) { // is it shoot already?
    if(checkCollision(&alienBoss.deathRay.pos, ALIEN_ROCKET_PIC_W, ALIEN_ROCKET_PIC_H,
                                    &ship.pos.Base, SHIP_PIC_W, SHIP_PIC_H)) {
      applyShipDamage(&alienBoss);
    }
  }
}
//---------------------------------------------------------------------------//

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

void shipHyperJump(void)
{
  while((ship.pos.Base.x++) < SHIP_MAX_POS_X) {
    // this pic used to left red track on screen
    drawBMP_RLE_P(ship.pos.Base.x, ship.pos.Base.y, SHIP_PIC_W, SHIP_PIC_H, 
                                          shipBaseLow, SHIP_BASE_LOW_PIC_SIZE);
  }
  movePicture(&ship.pos, SHIP_PIC_W, SHIP_PIC_H); // remove ship from screen
}

//---------------------------------------------------------------------------//
void moveGift(void)
{
  if((gift.pos.New.x -= GIFT_MOVE_SPEED) >= TFT_W) {
    movePicture(&gift.pos, GIFT_PIC_W, GIFT_PIC_H); // remove gift from screen
    createNextLevel();
  }
}

void drawGift(void)
{
  movePicture(&gift.pos, GIFT_PIC_W, GIFT_PIC_H); // clear previous position
  drawBMP_RLE_P(gift.pos.Base.x, gift.pos.Base.y, GIFT_PIC_W, GIFT_PIC_H, giftPic, GIFT_PIC_SIZE);
}

void checkGift(void)
{
  rocket_t *pRocket = &playeRockets[0];

  for(uint8_t countR =0; countR < MAX_PEW_PEW; countR++) {
    if(pRocket->onUse) {
      if(checkCollision(&pRocket->pos, ROCKET_W, ROCKET_H, 
                        &gift.pos.Base, GIFT_PIC_W, GIFT_PIC_H)) {

        rocketEpxlosion(pRocket);
        score += GIFT_BONUS_SCORE;

        if((ship.health += GIFT_HEALTH_RESTORE) > SHIP_HEALTH) {
          ship.health = SHIP_HEALTH;
        }
        createNextLevel();
      }
    }
    ++pRocket;
  }
}

// --------------------------------------------------------------- //

void createNextLevel(void)
{
  shipHyperJump();
  deleteAllTasks();
  addTask(getBtnStates, 50, true);

  if((++curretLevel) >= MAX_WORLDS) { // is it was final boss?
    victory();
    addTask(waitEnd, 400, true);
  } else {
    totalRespawns = ALIEN_KILLS_TO_BOSS + difficultyIncrement;
    if(++difficultyIncrement > MAX_DIFFICULT_INCREMENT) { // increase speed of all each lvl
      difficultyIncrement = MAX_DIFFICULT_INCREMENT;
    }
    levelClear();
    addTask(waitOk, 400, true);
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
  const taskParams_t *pTask;
  deleteAllTasks();
  setMaxTasks(size); // less tasks more perfomance

  while(size--) {
    pTask = pgm_read_word(pArr++); // get ponter to task structure
    // get params from structure
    addTask(pgm_read_word(&pTask->task), pgm_read_word(&pTask->timeout), true);
  }
}

void addTitleTasks(void)
{
  addTasksArray(titleTasksArr, TITLE_TASKS_COUNT);
}

void addGameTasks(void)
{
  ship.pos.New.x = SHIP_GAME_POS_X;
  ship.pos.New.y = SHIP_GAME_POS_Y;

  addTasksArray(gameTasksArr, GAME_TASKS_COUNT);
}

void addBossTasks(void)
{
  ship.states.power = DAMAGE_TO_BOSS;
  addTasksArray(bossTasksArr, BOSS_TASKS_COUNT);
}

void dropGift(void)
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
  addTask(drawRows, 10, true);
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
void readEEpromScore(void)
{
  uint8_t varCheck = getSaveDataMark(EE_ADDR_MARK);

  if(varCheck != HI_SCORE_MARK ) { // no save data in EEPROM?
    writeSaveData(EE_ADDR_SCORE, score);
    writeSaveData(EE_ADDR_MARK, HI_SCORE_MARK);
  }
}

void calibrateJoyStic(void)
{
  calJoysticX = getStickVal(LINE_X);
  calJoysticY = getStickVal(LINE_Y);
}

void initGUI(void)
{
  //drawStr_P(TFT_W/3, 5, PGRW_CP(textPGM, 3)); // Cookies! Anom nom nom...
  //hudStatus.updScore = true;
}

void initShip(void)
{
  ship.pos.New.x = SHIP_TITLE_POS_X;
  ship.pos.New.y = SHIP_TITLE_POS_Y;
  ship.health = SHIP_HEALTH;
  ship.weapon.rocketsLeft = MAX_PEW_PEW;
  ship.weapon.overHeated = false;

  uint8_t *pRocket = (uint8_t*)&playeRockets[0];
  for(uint8_t count =0; count < 3*MAX_PEW_PEW; count++) { // 3 == sizeof(rocket_t)
    *pRocket++ = 0x0;
  }
}

void initStars(void)
{
  for(uint8_t count =0; count < MAX_STARS; count++) {
    stars[count].pos.x = RN % TFT_W;
    stars[count].pos.y = RN % (TFT_H-4);
    stars[count].color = RAND_STAR_CLR;
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
}

void initSys(void)
{
  initEsplora();
  initBaseGameParams();
  calibrateJoyStic();
  readEEpromScore();

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
