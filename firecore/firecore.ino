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
 * Program size:  25,078 (*)
 * Free RAM:      1888 bytes
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
 *
 * (*) if 0 set in: DEBUG, USE_MEM_PANIC, USE_DYNAMIC_MEM
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
  0x3C,
  0x01,
  0x04,
  0x1C,
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
  return (newValuePin * 9 / 1024) + 48;
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
  tftFillRect(objOne->x, objOne->y, picW, picH, currentBackGroundColor);
  // store new position
  *objOne = *objTwo;
}

void movePicture(position_t *objOne, position_t *objTwo, uint16_t picW, uint16_t picH)
{
  // is position changed?
  if((objOne->x != objTwo->x) || (objOne->y != objTwo->y)) {
    // clear previos position
    tftFillRect(objOne->x, objOne->y, picW, picH, currentBackGroundColor);
    // store new position
    *objOne = *objTwo;
  }
}

void checkShipPosition(int16_t *pos, int16_t max, int16_t min)
{ 
  if(*pos < min) {
    *pos = min;
  } else if(*pos > max) {
    *pos = max;
  }
}

void applyShipDirection(uint16_t *pos, uint16_t valOne, uint8_t line)
{
  uint16_t newValueXY = getStickVal(line);
  if(newValueXY != valOne) {
    if(newValueXY < valOne) {
      *pos += ship.speed;
    } else {
      *pos -= ship.speed;
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
  if(btnStates.aBtn) {
    btnStates.aBtn = false;
    if(ship.weapon.overHeated) return;
      
      if((ship.weapon.rocketsLeft -= PLAYER_ROCKET_COST) < 0) {
        ship.weapon.overHeated = true;
        tftFillRect(SHIP_ENERGY_POS_X, SHIP_ENERGY_POS_Y,
                             SHIP_ENERGY_W, SHIP_ENERGY_H, COLOR_RED);
      } else {
        // store to local registers
        rocket_t *pRokets = &playeRockets[ship.weapon.rocketsLeft];
        if(pRokets->onUse == false) {
#if ADD_SOUND
          if(soundEnable) toneBuzz(500, 10);
#endif    
          pRokets->onUse = true;
          pRokets->pos.x = ship.posBase.x + ROCKET_OFFSET_X;
          pRokets->pos.y = ship.posBase.y + ROCKET_OFFSET_Y;
        }
      }
      hudStatus.updPew = true;
  }
}

void checkOverHeatGun(void)
{
  if(hudStatus.updPew) {
    if(ship.weapon.overHeated) {
      ship.weapon.rocketsLeft += PLAYER_ROCKET_REFILL;
#if 0
      if(soundEnable) {
        toneBuzz(200+10*ship.weapon.rocketsLeft, 10);
      }
#endif
    } else {  // Global cooldown
      ship.weapon.rocketsLeft += 2;
      tftFillRect(SHIP_ENERGY_POS_X, SHIP_ENERGY_POS_Y, 
                       SHIP_ENERGY_W, SHIP_ENERGY_H, currentBackGroundColor);
    }

    if(ship.weapon.rocketsLeft >= MAX_PEW_PEW) {
      ship.weapon.rocketsLeft = MAX_PEW_PEW;
      ship.weapon.overHeated = false;
      hudStatus.updPew = false;
    }

    tftFillRect(SHIP_ENERGY_POS_X, SHIP_ENERGY_POS_Y,
                       ship.weapon.rocketsLeft, SHIP_ENERGY_H, COLOR_WHITE);
  }
}

//---------------------------------------------------------------------------//
void applyShipDamage(inVader_t *pAlien)
{
  rocketEpxlosion(&pAlien->deathRay);
  pAlien->timeToShoot = RAND_SHOOT_TIME;

  // clear previous level
  tftFillRect(0, SHIP_ENERGY_POS_Y, ship.health/4 - 4, SHIP_ENERGY_H, COLOR_RED);
  ship.health -= DAMAGE_TO_SHIP;   // absorb damage
  hudStatus.updLife = true;        // update new life status later
}

void checkShipDamage(void)  //check damage from deathRay
{
  for(uint8_t countV=0; countV < MAX_ALIENS; countV++) {
    if(alien[countV].deathRay.onUse) { // is it shoot already?
      if(checkCollision(&alien[countV].deathRay.pos, ALIEN_ROCKET_PIC_W, ALIEN_ROCKET_PIC_H,
           &ship.posBase, SHIP_PIC_W, SHIP_PIC_H)) {
        applyShipDamage(&alien[countV]);        
      }
    }
  }
}

void checkShipBossDamage(void)  //check damage from boss deathRay
{
  if(alienBoss.deathRay.onUse) { // is it shoot already?
    if(checkCollision(&alienBoss.deathRay.pos, ALIEN_ROCKET_PIC_W, ALIEN_ROCKET_PIC_H,
          &ship.posBase, SHIP_PIC_W, SHIP_PIC_H)) {
      applyShipDamage(&alienBoss);
    }
  }
}
//---------------------------------------------------------------------------//

void checkShipHealth(void)
{
  if(ship.health <  DENGER_HEALTH_LVL) {
    lowHealthState = !lowHealthState;
  }
  setLEDValue(LED_R, lowHealthState); // yyess... every time set this...

  if(ship.health <= 0) {
    replaceTask(checkShipDamage, gameOver, 0, true);  // GameOver!
  }
}

void moveShip(void)
{
  uint16_t newValueXY =0; // store temp value

  newValueXY = getStickVal(LINE_X);
  if(newValueXY != calJoysticX) {
    if(newValueXY < calJoysticX) {
      ship.posNew.x += ship.speed;
    } else {
      ship.posNew.x -= ship.speed;
    } 
  }
  //applyShipDirection(&ship.posNew.x, calJoysticX, LINE_X);

  newValueXY = getStickVal(LINE_Y);
  if(newValueXY != calJoysticY) {
    if(newValueXY > calJoysticY) {
      ship.posNew.y += ship.speed;
    } else {
      ship.posNew.y -= ship.speed;
    } 
  }
  //applyShipDirection(&ship.posNew.y, calJoysticY, LINE_Y); 

  if(ship.posNew.x < SHIP_MIN_POS_X) {
    ship.posNew.x = SHIP_MIN_POS_X;
  } else if(ship.posNew.x > SHIP_MAX_POS_X) {
    ship.posNew.x = SHIP_MAX_POS_X;
  }
  //checkShipPosition(&ship.posNew.x, SHIP_MIN_POS_X, SHIP_MAX_POS_X);

  if(ship.posNew.y < SHIP_MIN_POS_Y) {
    ship.posNew.y = SHIP_MIN_POS_Y;
  } else if(ship.posNew.y > SHIP_MAX_POS_Y) {
    ship.posNew.y = SHIP_MAX_POS_Y;
  }
  //checkShipPosition(&ship.posNew.y, SHIP_MIN_POS_Y, SHIP_MAX_POS_Y);
}

void shipHyperJump(void)
{
  const uint8_t *pic = (shipState ? shipBaseHi : shipBaseLow);
  uint16_t picSize = (shipState ? SHIP_BASE_HI_PIC_SIZE : SHIP_BASE_LOW_PIC_SIZE);
  uint16_t posX;

  for(posX = ship.posBase.x; posX < SHIP_MAX_POS_X; posX++) {
    drawBMP_RLE_P(posX, ship.posBase.y, 
                     SHIP_PIC_W, SHIP_PIC_H, pic, picSize);
  }

  ship.posBase.x = posX;
  movePicture(&ship.posBase, &ship.posNew, SHIP_PIC_W, SHIP_PIC_H);
}

//---------------------------------------------------------------------------//
void moveGift(void)
{
  if((gift.posNew.x -= GIFT_MOVE_SPEED) <= GIFT_MIN_POS_X) {
    movePicture(&gift.posBase, &gift.posNew, GIFT_PIC_W, GIFT_PIC_H);
    createNextLevel();
  }
}

void drawGift(void)
{
  gift.state = !gift.state;
  const uint8_t *pic = (gift.state ? giftPicHi : giftPicLow);

  movePicture(&gift.posBase, &gift.posNew, GIFT_PIC_W, GIFT_PIC_H);
  drawBMP_RLE_P(gift.posBase.x, gift.posBase.y, GIFT_PIC_W, GIFT_PIC_H, pic, GIFT_PIC_SIZE);
}

void checkGift(void)
{
  int8_t countR;

  for(countR =0; countR < MAX_PEW_PEW; countR++) {
    if(playeRockets[countR].onUse) {
      if(checkCollision(&playeRockets[countR].pos, ROCKET_W, ROCKET_H,
              &gift.posBase, GIFT_PIC_W, GIFT_PIC_H)) {

        rocketEpxlosion(&playeRockets[countR]);

        score += GIFT_BONUS_SCORE;

        if((ship.health += GIFT_HEALTH_RESTORE) > SHIP_HEALTH) {
          ship.health = SHIP_HEALTH;
        }
        createNextLevel();
      }
    }
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

void addTitleTasks(void)
{
  addTask(titleAction, 200, true);
  addTask(getBtnStates, 50, true);
  addTask(drawStars, 50, true);
  addTask(drawShip, 70, true);
  addTask(drawStart, 500, true);
  //addTask(updateRows, 1 SEC, true);
  addTask(drawTitleText, 1 SEC, true);
}

void addGameTasks(void)
{
  addTask(drawStars, 50, true);
  addTask(moveShip, 70, true);
  addTask(drawShip, 70, true);
  addTask(checkShipDamage, 180, true);

  addTask(getBtnStates, 50, true);
  addTask(checkFireButton, 75, true);
  //addTask(checkSwitchWeapon, 150, true);
  addTask(pauseMenu, 400, true);
  addTask(drawPlayerRockets, 40, true);
  addTask(checkOverHeatGun, 100, true);
  addTask(checkShipHealth, 500, true);

  addTask(drawSomeGUI, 500, true);

  addTask(checkInVaders, 140, true);
  addTask(drawInVaders, 150, true);
  addTask(moveInVaders, 160, true);  // this is must be more than drawAliens()
  addTask(checkInVadersRespawn, 1 SEC, true);
  addTask(checkInVadersRay, 200, true);

  ship.posNew.x = SHIP_GAME_POS_X;
  ship.posNew.y = SHIP_GAME_POS_Y;
}

void addBossTasks(void)
{
  addTask(moveShip, 70, true);
  addTask(drawShip, 70, true);
  addTask(checkShipBossDamage, 130, true);

  addTask(getBtnStates, 50, true);
  addTask(checkFireButton, 75, true);
  //addTask(checkSwitchWeapon, 150, true);
  addTask(pauseMenu, 400, true);
  addTask(drawPlayerRockets, 40, true);
  addTask(checkOverHeatGun, 100, true);
  addTask(checkShipHealth, 500, true);

  addTask(drawSomeGUI, 500, true);

  addTask(drawBoss, 130, true);
  addTask(moveBossVertical, 150, true);
  addTask(checkBossDamage, 140, true);
  addTask(checkBossFire, 150, true);
}

void dropGift(void)
{
  // drop gift from the boss
  gift.posNew.x = GIFT_BASE_POS_X;
  gift.posNew.y = GIFT_BASE_POS_Y;
  gift.state = false;

  deleteAllTasks();
  addTask(moveShip, 70, true);
  addTask(drawShip, 70, true);
  addTask(getBtnStates, 50, true);
  addTask(checkFireButton, 75, true);
  addTask(drawPlayerRockets, 40, true);
  addTask(checkOverHeatGun, 100, true);

  addTask(moveGift, 120, true);
  addTask(drawGift, 150, true);
  addTask(checkGift, 80, true);
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
  uint8_t varCheck = eeprom_read_byte((const uint8_t*) EE_ADDR_MARK);

  if(varCheck != HI_SCORE_MARK ) {
    eeprom_write_word( (uint16_t*) EE_ADDR_SCORE, score);
    eeprom_write_byte( ( uint8_t*) EE_ADDR_MARK, HI_SCORE_MARK);
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
  ship.posNew.x = SHIP_TITLE_POS_X;
  ship.posNew.y = SHIP_TITLE_POS_Y;
  ship.health = SHIP_HEALTH;
  ship.speed = SHIP_BASE_SPEED;
  //ship.power = ;
  //ship.durability = ;
  ship.weapon.rocketsLeft = MAX_PEW_PEW;
  ship.weapon.overHeated = false;

  for(uint8_t count =0; count < MAX_PEW_PEW; count++) {
    playeRockets[count].onUse = false;
  }
}

void initStars(void)
{
  for(uint8_t count =0; count < MAX_STARS; count++) {
    stars[count].pos.x = RN % TFT_W;
    stars[count].pos.y = RN % (TFT_H-4);
    stars[count].color = RN % 80;
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
  tftFillScreen(currentBackGroundColor);

  initTasksArr(&taskArr, &pArr[0], MAX_GAME_TASKS);
  addTask(drawRows, 10, true);
}

//------------------------- yep, here's how it all began... -------------------//
int main(void)
{
  initSys();
  runTasks(); 
  return 0;
}
//-----------------------------------------------------------------------------//
