/*
 * Simple game.
 * Defend whole galaxy from Aliens.
 * 
 * Game end when ship's life will empty or last level boss defeated
 * 
 *                    
 * 
 * Main IDE:      Sublime Text 3
 *  Arduino IDE:  1.8.5   (as plugin and compiler)
 * Board(CPU):    Arduino Esplora (ATmega32u4)
 * CPU speed:     16 MHz
 * Program size:  22,490
 *  pics:         5,202
 *  code:         17,288
 * Used RAM:      1,422 bytes
 * Free RAM:      1,138 bytes
 *
 * Language:      C and C++
 * 
 * Author: Antonov Alexandr (Bismuth208)
 * Date:   2 June, 2017
 * Last:   17 Mar, 2018
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
tasksContainer_t taskArr;
taskFunc_t pArr[MAX_GAME_TASKS];
//---------------------------------------------------------------------------//

bool lowHealthState = false;
bool pauseState = false;
bool weaponLasers = false;
bool weaponGift = false;

int8_t menuItem =0;
uint8_t curretLevel =0;
uint8_t difficultyIncrement =0;
int16_t score =0;

uint16_t calJoysticX =0;
uint16_t calJoysticY =0;

ship_t ship;
gift_t gift;
btnStatus_t btnStates;
saveData_t gameSaveData;
bezier_t bezierLine;

// for bezier line
// x1,y1, x2,y2, x3,y3, steps
const uint8_t lineCurves[] PROGMEM = {
  130, 40, 85,120,  0, 40,50,
  130, 95,115,130,  0,  0,50,
  130, 50, 85, 70,  0, 10,50,
  130,  0,115, 10,  0,120,50,
  130, 10, 85, 50,  0, 62,50,
  130, 30, 85, 50,  0, 64,50,
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
int8_t checkShipPosition(int8_t pos, uint8_t min, uint8_t max)
{ 
  return (pos < min) ? min : (pos > max) ? max : pos;
}

void applyShipDirection(uint16_t pos, uint16_t valOne, uint8_t line)
{
  uint16_t newValueXY = getStickVal(line);
  if(newValueXY != valOne) {
    pos += (newValueXY < valOne) ? ship.states.speed : -ship.states.speed;
  }  
}

bool checkSpriteCollision(sprite_t *pSprOne, sprite_t *pSprTwo)
{
  auto tmpDataOne = getPicSize(pSprOne->pPic, 0);
  auto tmpDataTwo = getPicSize(pSprTwo->pPic, 0);

  /* ----------- Check X position ----------- */
  uint8_t objOnePosEndX = (pSprOne->pos.Old.x + tmpDataOne.u8Data1);

  if(objOnePosEndX >= pSprTwo->pos.Old.x) {
    uint8_t objTwoPosEndX = (pSprTwo->pos.Old.x + tmpDataTwo.u8Data1);
    if(pSprOne->pos.Old.x >= objTwoPosEndX) {
      return false; // nope, different X positions
    }
    // ok, objects on same X lines; Go next...
  } else {
    return false; // nope, absolutelly different X positions
  }

  /* ---------------------------------------- */
  /* ----------- Check Y position ----------- */
  uint8_t objOnePosEndY = (pSprOne->pos.Old.y + tmpDataOne.u8Data2);
  
  if(objOnePosEndY>= pSprTwo->pos.Old.y) {
    uint8_t objTwoPosEndY = (pSprTwo->pos.Old.y + tmpDataTwo.u8Data2);
    if(pSprOne->pos.Old.y <= objTwoPosEndY) {
      // ok, objects on same Y lines; Go next...
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
  sfxUpdateAll(); // update sound each frame, as sound engine is frame Oldd
#endif
}

// --------------------------------------------------------------- //
void checkFireButton(void)
{
  if(getBtnState(BUTTON_A)) {
    resetBtnStates();

    for(auto &laser : ship.weapon.lasers) {
      if(!laser.onUse) {
#if ADD_SOUND
        sfxPlayPattern(playerShotPattern, SFX_CH_1);
#endif
        laser.onUse = true;
        //start position
        laser.sprite.pos.Old.x = ship.sprite.pos.Old.x + ROCKET_OFFSET_X;
        laser.sprite.pos.Old.y = ship.sprite.pos.Old.y + ROCKET_OFFSET_Y;
        laser.sprite.pPic = getConstCharPtr(laserPics, ship.weapon.level);
        break;
      }
    }
  }
}

//---------------------------------------------------------------------------//
void applyShipDamage(rocket_t *pWeapon)
{
  rocketEpxlosion(pWeapon);

  if(ship.health) { // this rule preserv us from GUI overflow glitch
    // clear previous level
    tftFillRect(SHIP_ENERGY_POS_X, SHIP_ENERGY_POS_Y, (ship.health>>2) - 4, SHIP_ENERGY_H, COLOR_RED);

    // Calc damage as: MAX_DURAB - (0.5 * SHIP_DURAB) - DAMAGE_TO_SHIP
    uint8_t damage = SHIP_BASE_DURAB - (ship.states.durability/2 + DAMAGE_TO_SHIP);
    ship.health -= damage;     // absorb damage
  }
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
  uint8_t speed = ship.states.speed;
  auto pos = ship.sprite.pos.New;

  newValueXY = getStickVal(LINE_X);
  if(newValueXY != calJoysticX) {
    if(newValueXY < calJoysticX) {
      pos.x += speed;
    } else {
      pos.x -= speed;
    } 
  }
  //applyShipDirection(ship.posNew.x, calJoysticX, LINE_X);

  newValueXY = getStickVal(LINE_Y);
  if(newValueXY != calJoysticY) {
    if(newValueXY > calJoysticY) {
      pos.y += speed;
    } else {
      pos.y -= speed;
    }
  }
  //applyShipDirection(ship.posNew.y, calJoysticY, LINE_Y); 

  ship.sprite.pos.New.x = checkShipPosition(pos.x, SHIP_MIN_POS_X, SHIP_MAX_POS_X);
  ship.sprite.pos.New.y = checkShipPosition(pos.y, SHIP_MIN_POS_Y, SHIP_MAX_POS_Y);
}

//---------------------------------------------------------------------------//
void moveGift(void)
{
  moveBezierCurve(&gift.sprite.pos.New, &gift.bezLine);

  // reach end of move?
  if(gift.bezLine.step == bezierLine.totalSteps) {
    moveSprite(&gift.sprite); // remove gift from screen
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
  removeSprite(&gift.sprite); //remove gift from screen
}

void checkGift(void)
{
  if(weaponGift) {
    for(auto &laser : ship.weapon.lasers) {
      if(laser.onUse) {
        if(checkSpriteCollision(&laser.sprite, &gift.sprite)) {

          rocketEpxlosion(&laser);
#if ADD_SOUND
          sfxPlayPattern(playerSuperPattern, SFX_CH_2);
#endif
          giftDone();

          ship.states.power += WEAPON_GIFT_BONUS;
          if((++ship.weapon.level) > MAX_WEAPON_LVL) {
            ship.weapon.level = MAX_WEAPON_LVL;
          }
          disableWeaponGift();
          return;
        }
      }
    }
  } else { // heart and end of the level
    if(checkSpriteCollision(&ship.sprite, &gift.sprite)) {
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
  disableTask(dropWeaponGift); // to be shure and pause fix
  disableTask(moveGift);
  disableTask(checkGift);
  disableTask(drawGift);
}

void dropWeaponGift(void)
{
  gift.sprite.pPic = giftWeaponPic;
  weaponGift = true;
  disableTask(dropWeaponGift);

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
    addTask_P(T(&playMusic));
    addTask_P(T(&printDialogeText));
    disableTask(printDialogeText);
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
  addTasksArray_P(gameTasksArr);

  ship.sprite.pos.New = {SHIP_GAME_POS_X, SHIP_GAME_POS_Y};
  gift.bezLine = {GIFT_MOVE_ID, 0};
  moveBezierCurve(&gift.sprite.pos.New, &gift.bezLine);
  updateTaskTimeCheck(dropWeaponGift, RAND_GIFT_SPAWN_TIME);
}

void addBossTasks(void)
{
  bossInit();
  tftFillScreen(currentBackGroundColor);

  addTasksArray_P(bossTasksArr);
}

// drop gift from the boss
void addGiftTasks(void)
{
  weaponGift = false;
  gift.bezLine = {GIFT_MOVE_ID, 0};
  gift.sprite.pPic = giftHeartPic;
  moveBezierCurve(&gift.sprite.pos.New, &gift.bezLine);

  addTasksArray_P(giftTasksArr);
}

void addShipSelectTasks(void)
{
  ship.sprite.pos.New = {SHIP_SELECT_POS_X, SHIP_SELECT_POS_Y};
  addTasksArray_P(shipSelTasksArr);
}

void addStoryTasks(void)
{
  addTasksArray_P(storyTasksArr);
  disableTask(drawStaticNoise);
}

void addHistoryTasks(void)
{
  textHistoryPosX = 0;
  addTasksArray_P(historyTasksArr);
}

void addTitleTasks(void)
{
  addTasksArray_P(titleTasksArr);
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

// call every new level
void initShip(void)
{
  ship.sprite.pos.New = {SHIP_TITLE_POS_X, SHIP_TITLE_POS_Y};

  memset_F(ship.weapon.lasers, 0x00, sizeof(rocket_t)*MAX_PEW_PEW);
}

void resetShip(void)
{
  ship.health = SHIP_HEALTH;
  ship.weapon.level = 0;
  ship.type = 3; // he he he
  ship.sprite.pPic = getConstCharPtr(shipsPics, ship.type);
 
  for(auto &laser : ship.weapon.lasers) {
    laser.sprite.pPic = getConstCharPtr(laserPics, ship.weapon.level);
  }
}

void initStars(void)
{
  // init only speed as next drawStars() call will reinit all params
  for(auto &star : stars)
    star.speed = RN % STAR_STEP + 1;
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
