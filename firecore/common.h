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

#ifndef _COMMON_H
#define _COMMON_H

#include <stdbool.h>
#include <stdlib.h>
#include <avr/eeprom.h>

#include <esploraAPI.h>

#include "rleUnpack.h"
#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif

//---------------------------------------------------------------------------//

#define TFT_W    160
#define TFT_H    128
//---------------------------------------------------------------------------//

#define STAR_STEP                6  // move speed for stars
#define MAX_STARS               40  // how much stars we see on screen
#define MAX_PEW_PEW              4  // Crysis, nanosuit voice: - "Maximum pew".

#define PLAYER_ROCKET_SPEED     16  //(16/difficult)     // in future will be based on ship type
#define PLAYER_ROCKET_COST       1  //(5*difficult)      // in future will be based on ship type
#define PLAYER_ROCKET_REFILL    10  //(4/difficult)      //
#define SHIP_HEALTH            180  // Left it as is
#define DAMAGE_TO_SHIP          20  //(20*difficult)     //
#define RAND_GIFT_SPAWN_TIME    ((RN % (15 SEC) + 10 SEC))
#define PLAYER_ROCKET_CD_REFILL  2  // Global cooldown
#define DENGER_HEALTH_LVL       30  // when RGB LED  start to blink
#define MAX_DIFFICULT_INCREMENT  2

#define SCORE_VAL               20
#define BOSS_SCORE_VAL         100
#define GIFT_BONUS_SCORE       150

#define RN             fastRandNum()
#define RAND_CODE      tftSetCP437(RN & 1);
#define SLOW_CPU       setMainFreq(0x04);
#define SC             tftSetTextColorBG(TXTRNCLR)
#define RC             RNDCLR(RND_POS_X,RND_POS_Y)
#define TS             tftSetTextSize(1)
#define DC(a)          tftDrawCharInt(a)


#define STARS_MAX_POS_Y (TFT_H-9)
#define RAND_STAR_CLR   (((RN % 3)+1)<<4) | (RN & 0x7)

// this macros remove monstro constructions...
#define getConstCharPtr(a, b) (const uint8_t*)pgm_read_word(&(a[b]))
#define getConstWordPtr(a, b) (const uint16_t*)pgm_read_word(&(a[b]))

#define RND_POS_X ((RN % 26) * 6)
#define RND_POS_Y ((RN & 15) * 8)

#define continue() {CHECK_RULE}

//---------------------------------------------------------------------------//
#define ADD_SOUND             1
#define BEZIER_FIXED_MATH     0
//---------------------------------------------------------------------------//

#define M_SWITCH_TITLE        0
#define M_SWITCH_HISTORY      1
#define M_SWITCH_SHIP_SELECT  2
#define M_SWITCH_STORY        3
#define M_SWITCH_LVL_SELECT   4

//---------------------------------------------------------------------------//

// adrr range used: 0x00 - 0x08
#define EE_ADDR_SAVE_DATA  0x00
#define HI_SCORE_MARK      0x41

#define getSaveData(addr, val)  eeprom_read_block((void*)val, (const void*)addr, sizeof(saveData_t));
#define setSaveData(addr, val)  eeprom_update_block((void*)val, (const void*)addr, sizeof(saveData_t));
//---------------------------------------------------------------------------//

#define BASE_STATS_POS_X    50
#define BASE_STATS_POS_Y    60

#define MIN_SHIP_ITEM        1
#define MAX_SHIP_ITEM        3
//---------------------------------------------------------------------------//

#define SCORE_POS_X         90
#define SCORE_POS_Y        120
//---------------------------------------------------------------------------//

#define MAX_BEZIER_LINES     6
//---------------------------------------------------------------------------//

#define PAUSE_TEXT_POS_X    50
#define PAUSE_TEXT_POS_Y    40

#define PAUSE_TEXT_W        90
#define PAUSE_TEXT_H        25
//---------------------------------------------------------------------------//

#define GIFT_HEALTH_RESTORE  SHIP_HEALTH>>1 // >>1 same as /2; he he he, add half-life :) 

#define GIFT_MIN_POS_X       0
#define GIFT_BASE_POS_X    140
#define GIFT_BASE_POS_Y    RN % 100

#define GIFT_MOVE_SPEED      2
#define GIFT_MOVE_ID         8
//---------------------------------------------------------------------------//

#define ROCKET_OFFSET_X     20
#define ROCKET_OFFSET_Y      4

#define WEAPON_ROCKET_DMG   10 // additional damage from rockets
#define MAX_WEAPON_LVL       4
#define WEAPON_GIFT_BONUS    5
//---------------------------------------------------------------------------//

#define SHIP_BASE_SPEED      6
#define SHIP_BASE_DAMAGE    35
#define SHIP_BASE_DURAB     60
//---------------------------------------------------------------------------//

#define SHIP_FLAME_OFFSET_X  0
#define SHIP_FLAME_OFFSET_Y  6

#define SHIP_MIN_POS_X       0
#define SHIP_MIN_POS_Y       0

#define SHIP_MAX_POS_X     TFT_W - SHIP_PIC_W
#define SHIP_MAX_POS_Y     TFT_H - SHIP_PIC_H - 9

#define SHIP_ENERGY_POS_X    2
#define SHIP_ENERGY_POS_Y  123
#define SHIP_ENERGY_W      MAX_PEW_PEW
#define SHIP_ENERGY_H        2

#define SHIP_TITLE_POS_X    60
#define SHIP_TITLE_POS_Y    64

#define SHIP_GAME_POS_X      0
#define SHIP_GAME_POS_Y     64

#define SHIP_SELECT_POS_X  110
#define SHIP_SELECT_POS_Y   60
//---------------------------------------------------------------------------//

#define PIC_ROWS_STEP       1

#define PIC_ROW_L_POS_X     8
#define PIC_ROW_L_POS_Y    24

#define PIC_ROW_R_POS_X   138
#define PIC_ROW_R_POS_Y    24

#define PIC_TITLE_ROS_BASE_X  PIC_ROW_L_POS_X+PIC_TITLE_ROW_WH-1

#define PIC_TITLE_L_BASE_X 66
#define PIC_TITLE_R_BASE_X 78
//---------------------------------------------------------------------------//

#define TITLE_PIC_POS_X    22
#define TITLE_PIC_POS_Y    15
//---------------------------------------------------------------------------//

#define GALAXY_PIC_POS_X    0
#define GALAXY_PIC_POS_Y    5
//---------------------------------------------------------------------------//

#define START_TEXT_POS_X   60
#define START_TEXT_POS_Y  100
//---------------------------------------------------------------------------//

#define TEXT_WINDOW_X       6
#define TEXT_WINDOW_Y      94

#define TEXT_FRAME_X        0
#define TEXT_FRAME_Y       88
#define TEXT_FRAME_W      159
#define TEXT_FRAME_H       40

#define TEXT_OK_X         138
#define TEXT_OK_Y         116
//---------------------------------------------------------------------------//

#define CIRCLE_PONITER_MAP_SIZE  5
#define CHECK_RULE               if(++someCount>10) action();
//---------------------------------------------------------------------------//

#define ALIEN_MOVE_ZONE_Y_MIN         0
#define ALIEN_MOVE_ZONE_Y_MAX    (TFT_H-ALIEN_SHIP_PIC_H-8)

#define ALIEN_MOVE_ZONE_X_MIN         0
#define ALIEN_MOVE_ZONE_X_MAX       125

#define ALIEN_DEFAULT_POS_X         125
#define ALIEN_DEFAULT_POS_Y          20

#define ALIEN_BOSS_MOVE_ZONE_Y_MIN    0
#define ALIEN_BOSS_MOVE_ZONE_Y_MAX    (TFT_H-ALIEN_SHIP_BOSS_PIC_H-8)

// 20 is the most interesting and hardcore!
#define ALIEN_DENSITY                30  // It cannot be less invader1_height (i.e. ! < 18 px)
#define ALIEN_HEALTH                 90  //(30*difficult)     // if d = 4 then shoots = 10 to kill
#define ALIEN_BOSS_HEALTH           800  //
#define ALIEN_KILLS_TO_BOSS          15
#define ALIEN_BOSS_SPEED_MOVE         2
#define ALIEN_BOSS_ROCKET_SPEED_MOVE  2
#define ALIEN_BOSS_EXPLOSIONS        15
#define ALIEN_BOSS_DEATH_RAYS         4
#define ALIEN_BOSS_MOVE_UP_ID         6
#define ALIEN_BOSS_MOVE_DOWN_ID       7
#define ALIEN_RAND_SHOOT_TIME         ((RN & 3) + 3) // see p.s.1
#define ALIEN_RAND_RESPAWN_TIME       (RN % 5 + 1)   // from (1 to 6)  seconds;

#define MAX_ALIENS                    4
#define DEATH_RAY_SPEED              10  //(5*difficult)
#define DAMAGE_TO_ALIEN              32  //(64/difficult)
#define DAMAGE_TO_BOSS               10  //

// p.s.1 :
// from (10 to 38) / x seconds; The higher the difficulty the more fires
// where x - (1 sec / (time to call checkInVadersRay()))
// for example if time = 500, when x = 2; if time = 250, when x = 4;
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

#define WORLD_8_POS_X   150
#define WORLD_8_POS_Y   65

#define MAX_WORLDS      9

#define HOME_PLANET_ID  8
//---------------------------------------------------------------------------//

extern bool pauseState;

extern uint16_t calJoysticX;
extern uint16_t calJoysticY;

extern int8_t  menuItem;
extern uint8_t dogeDialogs;
extern uint8_t textHistoryPosX;

extern uint8_t someCount;
extern int8_t  totalRespawns;

extern uint16_t score;

extern uint8_t  difficultyIncrement;

extern uint8_t curretLevel;
extern const uint8_t lineCurves[];
extern const uint8_t lvlCoordinates[];

extern ship_t        ship;
extern gift_t        gift;
extern inVaderBoss_t alienBoss;
extern btnStatus_t   btnStates;
extern bezier_t      bezierLine;
extern saveData_t    gameSaveData;
extern stars_t       stars[MAX_STARS];
extern inVader_t     aliens[MAX_ALIENS];
extern rocket_t      playerLasers[MAX_PEW_PEW];
//---------------------------------------------------------------------------//

extern const uint16_t enemyShotPattern[];
extern const uint16_t enemyHitPattern[];
extern const uint16_t enemyDestroyPattern[];

extern const uint16_t playerDestroyPattern[];
extern const uint16_t playerSuperPattern[];
extern const uint16_t playerShotPattern[];

extern const uint16_t beepPattern[];
extern const uint16_t unfoldPattern[];
//---------------------------------------------------------------------------//

// Core GUI
void pauseMenu(void);
void baseStory(void);
void menuAction(void);
void getMenuItem(void);
void selectionMenu(void);
void drawMenuItemSelector(void);

void victory(void);
void gameOver(void);
void levelClear(void);
void drawSomeGUI(void);

void waitOk(void);
void waitEnd(void);

void drawStaticNoise(void);
void blinkLevelPointer(void);

void drawGalaxy(void);
void drawGalaxyAt(uint8_t y);
//---------------------------------------------------------------------------//
void moveGift(void);
void drawGift(void);
void checkGift(void);
void dropWeaponGift(void);
void disableWeaponGift(void);

void initBaseGameParams(void);

void levelBaseInit(void);
void createNextLevel(void);

void prepareLevelSelect(void);
//---------------------------------------------------------------------------//

uint8_t drawStory(void);
uint8_t titleAction(void);
uint8_t historyAction(void);
uint8_t drawLevelSelect(void);

void menuSwitchSelect(void);
//---------------------------------------------------------------------------//

void initInvaders(void);
void moveInVaders(void);
void drawInVaders(void);
void checkInVaders(void);
void checkInVadersRay(void);
void checkInVadersRespawn(void);

void setInvaderValue(inVader_t *pAlien, bool state);
void setDeathRayState(rocket_t *pAlien, position_t *pPos, bool state);
//----------------------------//

void bossDie(void);
void bossInit(void);
void drawBoss(void);
void checkBossFire(void);
void checkBossRays(void);
void checkBossDamage(void);
void moveBossVertical(void);
void drawBossExplosion(void);
void checkShipBossDamage(void);

//---------------------------------------------------------------------------//
void drawCurrentShipSelection(void);
void drawShipSelectionMenu(void);
void updateShipStates(void);
uint8_t checkShipSelect(void);
void getShipItem(void);

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
void printDialogeText(void);
void printHistory(void);
void drawStart(void);
void drawStars(void);

void pauseWindow(void);

void drawRows(void);
void drawTitleText(void);
void screenSliderEffect(uint16_t color);

void drawFrame(uint8_t posX, uint8_t posY,
	            uint8_t w, uint8_t h, uint16_t clr1, uint16_t clr2);

void fillRectFast(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void drawPixelFast(position_t *pPos, uint8_t colorId);

void rocketEpxlosion(rocket_t *pRocket);

void drawRandomDoge(void);
//---------------------------------------------------------------------------//

// Helpfull functions
void resetBtnStates(void);
void updateBtnStates(void);
bool getBtnState(uint8_t btn);
uint8_t getJoyStickValue(uint8_t pin);

void readEEpromScore(void);
void resetScore(void);

void moveBezierCurve(position_t *pPos, bezierLine_t *pItemLine);
void getBezierCurve(uint8_t line);
void fixPosition(position_t *pPos);

int8_t checkShipPosition(int8_t pos, uint8_t min, uint8_t max);

bool checkNewPosition(position_t *objOne, position_t *objTwo);
void applyNewPosition(position_t *objOne, position_t *objTwo, uint8_t picW, uint8_t picH);
void movePicture(objPosition_t *pObj, uint8_t picW, uint8_t picH);

void drawEnemy(objPosition_t *pEnemy, uint8_t w, uint8_t h, pic_t *pPic);

void setMainFreq(uint8_t ps);

void applyShipDamage(rocket_t *pWeapon);

bool checkCollision(position_t *pObjOne, uint8_t objOneW, uint8_t objOneH,
                    position_t *pObjTwo, uint8_t objTwoW, uint8_t objTwoH);

void playMusic(void);
void printDutyDebug(uint32_t duration);
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

void baseTitleTask(void);
void addTitleTasks(void);
void addHistoryTasks(void);
void addStoryTasks(void);
void addShipSelectTasks(void);
void addGameTasks(void);
void addBossTasks(void);
void addGiftTasks(void);

//---------------------------------------------------------------------------//

#define MAX_GAME_TASKS        19

#define TITLE_TASKS_COUNT      7
#define HISTORY_TASKS_COUNT    4
#define SHIP_SEL_TASKS_COUNT   7
#define STORY_TASKS_COUNT      7
#define LVL_SEL_TASKS_COUNT    5
#define GAME_TASKS_COUNT      16  // actually +3 for gift
#define BOSS_TASKS_COUNT      14
#define GIFT_TASKS_COUNT      10
#define GAME_OVER_TASKS_COUNT  4


extern tasksArr_t titleTasksArr[];
extern tasksArr_t historyTasksArr[];
extern tasksArr_t storyTasksArr[];
extern tasksArr_t shipSelTasksArr[];
extern tasksArr_t levelSelectTasksArr[];
extern tasksArr_t gameTasksArr[];
extern tasksArr_t bossTasksArr[];
extern tasksArr_t giftTasksArr[];
extern tasksArr_t gameOverTasksArr[];


extern TASK_N(printDialogeText);
extern TASK_N(updateBtnStates);
extern TASK_N(playMusic);
extern TASK_N(drawRows);
extern TASK_N(waitEnd);
extern TASK_N(waitOk);

extern TASK_N(moveGift);
extern TASK_N(drawGift);
extern TASK_N(checkGift);

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*_COMMON_H*/
