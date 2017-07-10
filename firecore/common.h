#ifndef _COMMON_H
#define _COMMON_H

#include <stdbool.h>
#include <stdlib.h>
#include <avr/eeprom.h>

#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif

//---------------------------------------------------------------------------//

#define TFT_W    160
#define TFT_H    128
//---------------------------------------------------------------------------//

#define MAX_GAME_TASKS        18

#define STAR_STEP              6  // move speed for stars
#define MAX_STARS             40  // how much stars we see on screen
#define MAX_PEW_PEW            5  // Crysis, nanosuit voice: - "Maximum pew".


#define PLAYER_ROCKET_SPEED   16  //(16/difficult)     // in future will be based on ship type
#define PLAYER_ROCKET_COST     1  //(5*difficult)      // in future will be based on ship type
#define PLAYER_ROCKET_REFILL  10  //(4/difficult)      //
#define ALIEN_HEALTH          90  //(30*difficult)     // if d = 4 then shoots = 10 to kill
#define ALIEN_BOSS_HEALTH    400  //
#define SHIP_HEALTH          180  // Left it as is
#define DEATH_RAY_SPEED       10  //(5*difficult)      // 
#define DAMAGE_TO_ALIEN       32  //(64/difficult)     //
#define DAMAGE_TO_BOSS        10  //
#define DAMAGE_TO_SHIP        20  //(20*difficult)     //

#define RAND_GIFT_SPAWN_TIME ((RN % (15 SEC) + 5 SEC))

#define PLAYER_ROCKET_CD_REFILL 2  // Global cooldown

#define DENGER_HEALTH_LVL     30  // when RGB LED  start to blink

#define MAX_DIFFICULT_INCREMENT 2

#define SCORE_VAL             20
#define BOSS_SCORE_VAL       100
#define GIFT_BONUS_SCORE     150

#define RN             randNum()
#define RAND_CODE      tftSetCP437(RN % 2);
#define SLOW_CPU       setMainFreq(0x04);
#define DC(a)          tftDrawChar(a)


#define STARS_MAX_POS_Y (TFT_H-9)
#define RAND_STAR_CLR  (((RN % 3)+1)<<4) | (RN % 0x8)

// this macros remove monstro constructions...
#define getConstCharPtr(a, b) (const uint8_t*)pgm_read_word(&(a[b]))
#define getConstWordPtr(a, b) (const uint16_t*)pgm_read_word(&(a[b]))

#define RND_POS_X ((RN % 26) * 6)
#define RND_POS_Y ((RN % 16) * 8)

// from (10 to 38) / x seconds; The higher the difficulty the more fires
// where x - (1 sec / (time to call checkInVadersRay()))
// for example if time = 500, when x = 2; if time = 250, when x = 4;
#define RAND_SHOOT_TIME (RN % 4 + 3)

//from (1 to 5)  seconds;
#define RAND_RESPAWN_TIME (RN % 4 + 1)

#define continue() {CHECK_RULE}

#define ADD_SOUND 1
//---------------------------------------------------------------------------//

#define EE_ADDR_SCORE         0x00 //((difficult-1)*2)  // adrr: 0x00 - 0x07 
#define EE_ADDR_MARK          0x08
#define HI_SCORE_MARK         0x41

#define getSaveDataMark(addr)     eeprom_read_byte((const uint8_t*)addr)
#define readSaveData(addr)        eeprom_read_word((const uint16_t*)addr)
#define writeSaveData(addr, val)  eeprom_write_word((uint16_t*)addr, val)
//---------------------------------------------------------------------------//

#define BUTTON_A   SW_BTN_4_MUX
#define BUTTON_B   SW_BTN_1_MUX
#define BUTTON_X   SW_BTN_2_MUX
#define BUTTON_Y   SW_BTN_3_MUX
#define buttonIsPressed(a)  readSwitchButton(a)

#define LINE_X     X_J_MUX_VAL
#define LINE_Y     Y_J_MUX_VAL
#define LR_OK      SW_J_MUX_VAL
#define getStickVal(a)  getJoyStickValue(a)
//---------------------------------------------------------------------------//

#define BASE_STATS_POS_X  50
#define BASE_STATS_POS_Y  60

#define MIN_SHIP_ITEM  1
#define MAX_SHIP_ITEM  3
//---------------------------------------------------------------------------//

#define SCORE_POS_X 90
#define SCORE_POS_Y 120

//---------------------------------------------------------------------------//

#define ROCKET_OFFSET_X  18
#define ROCKET_OFFSET_Y  4
//---------------------------------------------------------------------------//


#define PAUSE_TEXT_POS_X 50
#define PAUSE_TEXT_POS_Y 40

#define PAUSE_TEXT_W  90 
#define PAUSE_TEXT_H  25 

//---------------------------------------------------------------------------//

#define GIFT_HEALTH_RESTORE  SHIP_HEALTH>>1 // >>1 same as /2; he he he, add half-life :) 

#define GIFT_MIN_POS_X   0

#define GIFT_BASE_POS_X  140
#define GIFT_BASE_POS_Y  RN % 100

#define GIFT_MOVE_SPEED  2

//---------------------------------------------------------------------------//

#define WEAPON_ROCKET_DMG 10 // additional damage from rockets
#define MAX_WEAPON_LVL    4
#define WEAPON_GIFT_BONUS 5

//---------------------------------------------------------------------------//

#define SHIP_BASE_SPEED  6
#define SHIP_BASE_DAMAGE 35
#define SHIP_BASE_DURAB  50

//---------------------------------------------------------------------------//

#define SHIP_MIN_POS_X   0
#define SHIP_MIN_POS_Y   0

#define SHIP_MAX_POS_X   TFT_W - SHIP_PIC_W
#define SHIP_MAX_POS_Y   TFT_H - SHIP_PIC_H - 9


#define SHIP_ENERGY_POS_X  2
#define SHIP_ENERGY_POS_Y  123
#define SHIP_ENERGY_W      MAX_PEW_PEW
#define SHIP_ENERGY_H      2

#define SHIP_TITLE_POS_X  60
#define SHIP_TITLE_POS_Y  64

#define SHIP_GAME_POS_X  0
#define SHIP_GAME_POS_Y  64

#define SHIP_SELECT_POS_X  110
#define SHIP_SELECT_POS_Y  60
//---------------------------------------------------------------------------//

#define PIC_ROWS_STEP    1

#define PIC_ROW_L_POS_X  8
#define PIC_ROW_L_POS_Y  24

#define PIC_ROW_R_POS_X  138
#define PIC_ROW_R_POS_Y  24

#define PIC_TITLE_ROS_BASE_X  PIC_ROW_L_POS_X+PIC_TITLE_ROW_WH-1

#define PIC_TITLE_L_BASE_X 66
#define PIC_TITLE_R_BASE_X 78
//---------------------------------------------------------------------------//

#define TITLE_PIC_POS_X    22
#define TITLE_PIC_POS_Y    15
//---------------------------------------------------------------------------//

#define GALAXY_PIC_POS_X   0
#define GALAXY_PIC_POS_Y   5
//---------------------------------------------------------------------------//

#define START_TEXT_POS_X 60
#define START_TEXT_POS_Y 100

//---------------------------------------------------------------------------//

#define TEXT_WINDOW_X  6
#define TEXT_WINDOW_Y  94

#define TEXT_FRAME_X   0
#define TEXT_FRAME_Y   88
#define TEXT_FRAME_W   159
#define TEXT_FRAME_H   40

#define TEXT_OK_X      138
#define TEXT_OK_Y      116
//---------------------------------------------------------------------------//

#define CIRCLE_PONITER_MAP_SIZE  5
#define CHECK_RULE if(++someCount>10) action();
//---------------------------------------------------------------------------//

#define ALIEN_MOVE_ZONE_Y_MIN 0
#define ALIEN_MOVE_ZONE_Y_MAX (TFT_H-ALIEN_SHIP_PIC_H-8)

#define ALIEN_MOVE_ZONE_X_MIN 0
#define ALIEN_MOVE_ZONE_X_MAX 125

#define ALIEN_DEFAULT_POS_X   125
#define ALIEN_DEFAULT_POS_Y   20

#define ALIEN_BOSS_MOVE_ZONE_Y_MIN 0
#define ALIEN_BOSS_MOVE_ZONE_Y_MAX (TFT_H-ALIEN_SHIP_BOSS_PIC_H-8)

// 20 is the most interesting and hardcore!
#define ALIEN_DENSITY         30     // It cannot be less invader1_height (i.e. ! < 18 px)

#define MAX_ALIENS            4

#define ALIEN_KILLS_TO_BOSS   15

#define ALIEN_BOSS_SPEED_MOVE  2
#define ALIEN_BOSS_ROCKET_SPEED_MOVE  2

#define ALIEN_SOUND_FREQ   2000
#define ALIEN_SOUND_LONG   10

#define ALIEN_BOSS_EXPLOSIONS  15

#define ALIEN_BOSS_DEATH_RAYS  4
//---------------------------------------------------------------------------//

// worlds on galaxy map
#define WORLD_0_POS_X   7
#define WORLD_0_POS_Y   10

#define WORLD_1_POS_X   30
#define WORLD_1_POS_Y   13

#define WORLD_2_POS_X   29
#define WORLD_2_POS_Y   40

#define WORLD_3_POS_X   35
#define WORLD_3_POS_Y   56

#define WORLD_4_POS_X   57
#define WORLD_4_POS_Y   57

#define WORLD_5_POS_X   71
#define WORLD_5_POS_Y   51

#define WORLD_6_POS_X   86
#define WORLD_6_POS_Y   54

#define WORLD_7_POS_X   113
#define WORLD_7_POS_Y   53

#define WORLD_8_POS_X   145
#define WORLD_8_POS_Y   65

#define MAX_WORLDS      9
//---------------------------------------------------------------------------//

extern uint16_t calJoysticX;
extern uint16_t calJoysticY;
extern uint16_t currentBackGroundColor;
extern uint16_t replaceColor;

extern int8_t menuItem;
extern uint8_t dogeDialogs;
//extern uint8_t maxStars;

extern bool pauseState;
extern bool shipState;
extern bool lowHealthState;
#if ADD_SOUND
extern bool soundEnable;
#endif

extern ship_t ship;
extern gift_t gift;
extern inVaderBoss_t alienBoss;
extern inVader_t alien[MAX_ALIENS];
extern rocket_t *pRocketGlobal;
extern btnStatus_t btnStates;
extern stars_t stars[MAX_STARS];
extern hudStatus_t hudStatus;
extern uint8_t someCount;

extern int8_t totalRespawns;

extern uint16_t score;

extern uint8_t difficultyIncrement;

extern uint8_t curretLevel;
extern const uint8_t lvlCoordinates[];
extern const uint8_t lvlColors[];
//---------------------------------------------------------------------------//

// Core GUI
void getMenuItem(void);
void drawMenuItemSelector(void);
void menuAction(void);
void selectionMenu(void);
void pauseMenu(void);
void titleAction(void);
void baseStory(void);
void drawStory(void);

void gameOver(void);
void levelClear(void);
void victory(void);
void drawSomeGUI(void);

void waitOk(void);
void waitEnd(void);

void drawStaticNoise(void);

//---------------------------------------------------------------------------//
void dropWeaponGift(void);
void disableWeaponGift(void);
void checkGift(void);
void moveGift(void);
void drawGift(void);

void initBaseGameParams(void);

void levelBaseInit(void);
void createNextLevel(void);
void prepareLevelSelect(void);
void drawLevelSelect(void);
//---------------------------------------------------------------------------//

void initInvaders(void);
void moveInVaders(void);
void drawInVaders(void);
void checkInVadersRespawn(void);
void checkInVadersRay(void);
void checkInVaders(void);

void setInvaderValue(inVader_t *pAlien, bool state);
void setDeathRayState(rocket_t *pAlien, bool state);
void setDeathRayPos(rocket_t *deathRay, position_t *pPos);
//----------------------------//

void addBossTasks(void);

void bossInit(void);

void moveBossVertical(void);
void drawBoss(void);
void checkBossDamage(void);
void checkBossFire(void);
void checkBossRays(void);
void bossDie(void);

void checkShipBossDamage(void);

//---------------------------------------------------------------------------//
void addShipSelectTasks(void);
void drawShipSelectionMenu(void);
void drawCurrentShipSelection(void);
void getShipItem(void);
void getShipStates(shipStats_t *pShipStates);
void checkShipSelect(void);

//---------------------------------------------------------------------------//

void shipHyperJump(void);
void drawShipExplosion(void);

void initShip(void);
void moveShip(void);
void drawShip(void);
void checkShipDamage(void);

void drawPlayerRockets(void);

void checkFireButton(void);
void checkShipHealth(void);
//---------------------------------------------------------------------------//

// core graphics
void drawTextWindow(const uint8_t *text, const uint8_t *btnText);
void drawStart(void);
void drawStars(void);

void pauseWindow(void);

void drawTitleText(void);
void drawRows(void);
void screenSliderEffect(uint16_t color);

void drawFrame(uint16_t posX, uint16_t posY,
	            uint8_t w, uint8_t h, uint16_t clr1, uint16_t clr2);

void drawBMP_RLE_P(int16_t x, int16_t y, uint8_t w, uint8_t h,
                              const uint8_t *pPic, int16_t sizePic);

void fillRectFast(int16_t x, int16_t y, uint8_t w, uint8_t h);
void drawPixelFast(int16_t x, int16_t y, uint8_t colorId);

void rocketEpxlosion(rocket_t *pRocket);
//---------------------------------------------------------------------------//

// Helpfull functions
void updateBtnStates(void);
bool getBtnState(uint8_t btn);
void resetBtnStates(void);
uint8_t getJoyStickValue(uint8_t pin);

void readEEpromScore(void);
void resetScore(void);

void checkShipPosition(uint16_t *pos, uint16_t max, uint16_t min);

bool checkNewPosition(position_t *objOne, position_t *objTwo);
void applyNewPosition(position_t *objOne, position_t *objTwo, uint16_t picW, uint16_t picH);
void movePicture(objPosition_t *pObj, uint16_t picW, uint16_t picH);

void drawEnemy(objPosition_t *pEnemy, uint8_t w, uint8_t h, const uint8_t *pPic, uint16_t picSize);

uint8_t randNum(void);
void setMainFreq(uint8_t ps);

void applyShipDamage(rocket_t *pWeapon);

bool checkCollision(position_t *pObjOne, uint8_t objOneW, uint8_t objOneH,
                    position_t *pObjTwo, uint8_t objTwoW, uint8_t objTwoH);

int32_t mapVal(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);

void memset_F(void *pvDest, uint8_t src, size_t size);

void playMusic(void);
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

#define T(a) a##Task
#define TASK_N(a)    const taskParams_t T(a)
#define TASK(a,b)    TASK_N(a) PROGMEM = {a, b}
#define TASK_P(a)    (taskParams_t*)&T(a)
#define TASK_ARR(a)  tasksArr_t a##TasksArr[] PROGMEM

//---------------------------------------------------------------------------//

extern TASK_N(drawRows);
extern TASK_N(updateBtnStates);
extern TASK_N(waitEnd);
extern TASK_N(waitOk);
extern TASK_N(drawStory);
extern TASK_N(drawStaticNoise);
extern TASK_N(drawLevelSelect);
extern TASK_N(drawShipExplosion);

//---------------------------------------------------------------------------//

void addTasksArray(tasksArr_t *pArr, uint8_t size);
void addTitleTasks(void);
void addGameTasks(void);
void addBossTasks(void);
void addGiftTasks(void);
void addShipSelectTasks(void);
void baseTitleTask(void);
//---------------------------------------------------------------------------//

#define TITLE_TASKS_COUNT     6
#define SHIP_SEL_TASKS_COUNT  5
#define GAME_TASKS_COUNT     18
#define BOSS_TASKS_COUNT     13
#define GIFT_TASKS_COUNT      8


extern tasksArr_t titleTasksArr[];
extern tasksArr_t gameTasksArr[];
extern tasksArr_t bossTasksArr[];
extern tasksArr_t giftTasksArr[];
extern tasksArr_t shipSelTasksArr[];

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*_COMMON_H*/
