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
 * Program size:  24,390
 *  pics:         5,202
 *  code:         17,288
 * Used RAM:      1,544 bytes
 * Free RAM:      1,016 bytes
 *
 * Language:      C and C++
 * 
 * Author: Antonov Alexandr (Bismuth208)
 * Created:   2 June, 2017
 * Last edit: 23 June, 2018
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
tasksContainer_t tasksContainer;
taskFunc_t tasksArr[MAX_GAME_TASKS];
//---------------------------------------------------------------------------//

bool pauseState = false;
bool weaponGift = false;

int8_t menuItem =0;
uint8_t curretLevel =0;
uint8_t difficultyIncrement =0;
int16_t score =0;

uint16_t calJoysticX =0;
uint16_t calJoysticY =0;

uint16_t playerFireCheck = 0;

ship_t ship;
gift_t gift;
saveData_t gameSaveData;
bezier_t bezierLine;

// for bezier line
// x1,y1, x2,y2, x3,y3, steps
// x1,y1 - start point
// x2,y2 - is imaginary for angle
// x3,y3 - end point
// steps - number of points to draw line
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
  WORLD_E_POS_X, WORLD_E_POS_X, // ASTEROIDS!!!

  WORLD_2_POS_X, WORLD_2_POS_Y,
  WORLD_3_POS_X, WORLD_3_POS_Y,
  WORLD_E_POS_X, WORLD_E_POS_X, // ASTEROIDS!!!

  WORLD_4_POS_X, WORLD_4_POS_Y,
  WORLD_5_POS_X, WORLD_5_POS_Y,
  WORLD_E_POS_X, WORLD_E_POS_X, // ASTEROIDS!!!

  WORLD_6_POS_X, WORLD_6_POS_Y,
  WORLD_7_POS_X, WORLD_7_POS_Y,
  WORLD_E_POS_X, WORLD_E_POS_X, // ASTEROIDS!!!

  WORLD_8_POS_X, WORLD_8_POS_Y
};

// ------------------------ Poll controls ------------------------ //


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

void moveEnemyV(position_t *pPos, uint8_t moveSize)
{
  pPos->y += (int8_t)((moveSize)*((ship.sprite.pos.Old.y > pPos->y) ? 1 : -1));
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
        laser.sprite.pPic = getPicPtr(laserPics, ship.weapon.level);
        break;
      }
    }
  }
}

//---------------------------------------------------------------------------//
void makeHorribleMagic(uint8_t magicValue)
{
  if(ship.health) { // this rule preserv us from GUI overflow glitch
    // clear previous level
    drawHealthStatusBar(COLOR_ID_RED);

    // Calc damage as: MAX_DURAB - (0.5 * SHIP_DURAB) - magicValue
    uint8_t damage = SHIP_BASE_DURAB - (ship.states.durability>>1) - magicValue;
    ship.health -= damage;     // absorb damage
  }
}

void applyShipDamage(rocket_t *pWeapon)
{
  rocketEpxlosion(pWeapon);
  makeHorribleMagic(DAMAGE_TO_SHIP);
}

void checkShipHealth(void)
{
  if(ship.health <= DENGER_HEALTH_LVL) {
    ship.lowHealthState = !ship.lowHealthState;
    setLEDValue(LED_R, ship.lowHealthState);

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
            playerFireCheck -= PLAYER_FIRE_CHECK_COST;
            updateTaskTimeCheck(checkFireButton, playerFireCheck);
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
void setGameTasks(tasksArr_t *pTasks)
{
  addTasksArray_P(pTasks);

  // this two tasks always every where
  addTask_P(T(&updateBtnStates));
  addTask_P(T(&playMusic));
}

void addCreditsTasks(void)
{
  // setShakingAvatar(7, 33, cityDogePic); // creditPicQR
  screenSliderEffect(COLOR_ID_BLACK);
  setGameTasks(creditsTasksArr);
}

void addAsteroidsTasks(void)
{
  ship.sprite.pos.New = {SHIP_GAME_POS_X, SHIP_GAME_POS_Y};

  initAsteroids();
  setGameTasks(asteroidFieldTasksArr);
}

void addGameTasks(void)
{
  setGameTasks(gameTasksArr);

  ship.sprite.pos.New = {SHIP_GAME_POS_X, SHIP_GAME_POS_Y};
  gift.bezLine = {GIFT_MOVE_ID, 0};
  moveBezierCurve(&gift.sprite.pos.New, &gift.bezLine);
  updateTaskTimeCheck(dropWeaponGift, RAND_GIFT_SPAWN_TIME);
}

void addBossTasks(void)
{
  bossInit();
  tftFillScreen(currentBackGroundColor);

  setGameTasks(bossTasksArr);
}

// drop gift from the boss
void addGiftTasks(void)
{
  weaponGift = false;
  gift.bezLine = {GIFT_MOVE_ID, 0};
  gift.sprite.pPic = giftHeartPic;
  moveBezierCurve(&gift.sprite.pos.New, &gift.bezLine);

  setGameTasks(giftTasksArr);
}

void addShipSelectTasks(void)
{
  ship.sprite.pos.New = {SHIP_SELECT_POS_X, SHIP_SELECT_POS_Y};
  setGameTasks(shipSelTasksArr);
}

void addStoryTasks(void)
{
  setGameTasks(storyTasksArr);
  disableTask(drawStaticNoise);
}

void addHistoryTasks(void)
{
  // for history text
  pTextDialoge = historyTextP;
  tftSetTextColor(COLOR_WHITE);
  tftSetTextSize(1);
  tftSetCursor(0, 72);
  updateWindowTextPos();

  setGameTasks(historyTasksArr);
}

// void addGameModeSelectTasks(void)
// {
//   tftFillScreen(currentBackGroundColor);

//   rowL.pos.New = {MENU_SELECT_ROW_L_POS_X, MENU_SELECT_ROW_L_POS_Y};
//   rowR.pos.New = {MENU_SELECT_ROW_R_POS_X, MENU_SELECT_ROW_R_POS_Y};

//   // drawMenuGameMode();

//   // addTask_P(T(&rowsUnfold));
//   // updateTaskTimeCheck(rowsUnfold, 5);
//   // pCallBackWaitEvent = rowsAnimDone;
// }

void addTitleTasks(void)
{
  setGameTasks(titleTasksArr);
}

void baseTitleTask(void)
{
  tftFillScreen(currentBackGroundColor);

  initBaseGameParams();
  setGameTasks(startupTasksArr);
  // pCallBackWaitEvent = addTitleTasks;
  playerFireCheck = PLAYER_FIRE_CHECK;

  rowL.pos.New = {PIC_TITLE_L_BASE_X, PIC_TITLE_L_BASE_Y};
  rowL.pos.Old = rowL.pos.New;
  rowL.pPic = rowsLeftPic;

  rowR.pos.New = {PIC_TITLE_R_BASE_X, PIC_TITLE_R_BASE_Y};
  rowR.pos.Old = rowR.pos.New;
  rowR.pPic = rowsRightPic;

#if ADD_SOUND
  sfxPlayPattern(unfoldPattern, SFX_CH_0);
#endif
}

// ------------- Init section ---------------------- //
void readScore(void)
{
  getSaveData(SAVE_DATA_BLOCK, &gameSaveData.rawData[0], sizeof(saveData_t));

  if(gameSaveData.saveDataMark != HI_SCORE_MARK ) { // no save data in EEPROM?
    resetScore();
  }
}

void resetScore(void)
{
  memset_F(&gameSaveData.rawData[0], 0x00, sizeof(saveData_t));
  gameSaveData.saveDataMark = HI_SCORE_MARK;

  setSaveData(SAVE_DATA_BLOCK, &gameSaveData.rawData[0], sizeof(saveData_t));
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
  ship.type = SHIP_TYPE_BONUS; // he he he
  ship.sprite.pPic = getPicPtr(shipsPics, ship.type);
 
  for(auto &laser : ship.weapon.lasers) {
    laser.sprite.pPic = getPicPtr(laserPics, ship.weapon.level);
  }
}

void initStars(void)
{
  // init only speed as next drawStars() call will reinit all params
  for(auto &star : stars)
    star.speed = RN % STAR_STEP + 1;
}

void initBaseGameParams(void)
{
  initShip();
  initStars();
  initInvaders();
  setAlphaReplaceColorId(0x01);

  // this one init only once
  resetShip();
}

void initSys(void)
{
  initEsplora();
  initEsploraGame();
  calibrateJoystick();
  readScore();

  initTasksArr(&tasksContainer, &tasksArr[0], MAX_GAME_TASKS);
  baseTitleTask(); // at start moment need only this task
}

//------------------------- yep, here's how it all began... -------------------//
__attribute__((noreturn)) int main(void)
{
  initSys();
  runTasks();
}
//-----------------------------------------------------------------------------//
