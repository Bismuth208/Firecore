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

#include <esploraAPI.h>

//#include "tinySM_config.h"
#include "types.h"
#include "textProg.h"

#ifdef __cplusplus
extern "C"{
#endif

//---------------------------------------------------------------------------//

#define STAR_STEP                6  // move speed for stars
#define MAX_STARS               40  // how much stars we see on screen
#define MAX_STARS_WARP          20
#define STARS_MAX_POS_Y         (TFT_H-9)
#define RAND_STAR_CLR           (((RN % 3)+1)<<4) | (RN & 0x7)
//---------------------------------------------------------------------------//

#define ASTEROID_STEP           4  // same as stars
#define MAX_ASTEROIDS           16  // same as stars
#define ASTEROID_MAX_POS_Y      (TFT_H-20)
#define ASTEROIDS_TO_DEFEAT     200
#define ASTEROID_SPEED_MOVE     1
#define ASTEROID_SHIP_DAMAGE    30 // actually its inversed...
//---------------------------------------------------------------------------//

#define SAVE_DATA_BLOCK          0

#define MAX_DIFFICULT_INCREMENT  2

#define SCORE_VAL               20
#define SCORE_PENALTY           10
#define BOSS_SCORE_VAL         100
#define GIFT_BONUS_SCORE       150

#define RN             fastRandNum()
#define RAND_CODE      tftSetCP437(RN & 1);
#define SLOW_CPU       setMainFreq(0x04);
#define SC             tftSetTextColorBG(TXTRNCLR)
#define RC             RNDCLR(RND_POS_X,RND_POS_Y)
#define TS             tftSetTextSize(1)
#define DC(a)          tftDrawCharInt(a)

#define RND_POS_X ((RN % 26) * 6)
#define RND_POS_Y ((RN & 15) * 8)

#define disablePause() {CHECK_RULE}

//---------------------------------------------------------------------------//
#define BEZIER_FIXED_MATH     0
//---------------------------------------------------------------------------//

#define M_SWITCH_TITLE        0
#define M_SWITCH_HISTORY      1
#define M_SWITCH_SHIP_SELECT  2
#define M_SWITCH_STORY        3
#define M_SWITCH_LVL_SELECT   4

//---------------------------------------------------------------------------//

#define HI_SCORE_MARK      0x41

//---------------------------------------------------------------------------//

#define SCORE_POS_X         90
#define SCORE_POS_Y        120

#define SCORE_DIGITS         6 // now, i'm pretty sure, what 6 bytes will be enough...
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

#define RAND_GIFT_SPAWN_TIME    ((RN % (15 SEC) + 10 SEC))
//---------------------------------------------------------------------------//

#define MIN_SHIP_ITEM        0
#define MAX_SHIP_ITEM        2

#define WEAPON_ROCKET_DMG   10 // additional damage from rockets
#define MAX_WEAPON_LVL       4
#define WEAPON_GIFT_BONUS    5

#define SHIP_TYPE_SPEED      0
#define SHIP_TYPE_POWER      1
#define SHIP_TYPE_DURAB      2
#define SHIP_TYPE_BONUS      3

#define SHIP_BASE_SPEED      6
#define SHIP_BASE_DAMAGE    35
#define SHIP_BASE_DURAB     60

#define BASE_STATS_POS_X    50
#define BASE_STATS_POS_Y    60

#define PLAYER_ROCKET_CD_REFILL  2  // Global cooldown
#define PLAYER_ROCKET_SPEED     16  //(16/difficult)     // in future will be based on ship type
#define PLAYER_ROCKET_COST       1  //(5*difficult)      // in future will be based on ship type
#define PLAYER_ROCKET_REFILL    10  //(4/difficult)      //
#define SHIP_HEALTH            180  // Left it as is
#define DAMAGE_TO_SHIP          20  //(20*difficult)     //
#define DENGER_HEALTH_LVL       30  // when RGB LED  start to blink

#define ROCKET_OFFSET_X     20
#define ROCKET_OFFSET_Y      4

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
#define PIC_ROW_R_POS_X   138

#define PIC_TITLE_ROS_BASE_X  PIC_ROW_L_POS_X+PIC_TITLE_ROW_WH-1

#define PIC_TITLE_L_BASE_X 66
#define PIC_TITLE_L_BASE_Y 24

#define PIC_TITLE_R_BASE_X 78
#define PIC_TITLE_R_BASE_Y 24

#define TITLE_PIC_POS_X    22
#define TITLE_PIC_POS_Y    15


#define MENU_SELECT_ROW_STEP     20

#define MENU_SELECT_ROW_L_POS_X 112
#define MENU_SELECT_ROW_L_POS_Y  40

#define MENU_SELECT_ROW_R_POS_X 134
#define MENU_SELECT_ROW_R_POS_Y  40
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
#define TEXT_FRAME_H       39

#define TEXT_OK_X         138
#define TEXT_OK_Y         116
//---------------------------------------------------------------------------//

#define CIRCLE_PONITER_MAP_SIZE  5
#define CHECK_RULE               if(++someCount>10) action();
#define MT                       tftPrintAt_P(32, 58, (const char *)creditP0);
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
#define ALIEN_BOSS_HEALTH          1600  //
#define ALIEN_KILLS_TO_BOSS          20
#define ALIEN_BOSS_SPEED_MOVE         2
#define ALIEN_BOSS_ROCKET_SPEED_MOVE  2
#define ALIEN_BOSS_EXPLOSIONS        15
#define ALIEN_BOSS_MOVE_UP_ID         6
#define ALIEN_BOSS_MOVE_DOWN_ID       7
#define ALIEN_RAND_SHOOT_TIME         ((RN & 7) + 1) // see p.s.1
#define ALIEN_RAND_RESPAWN_TIME       (RN % 5 + 1)   // from (1 to 6)  seconds;
#define invadersMagicRespawn          action(){TS;for(;;){RAND_CODE;SC;DC(RC);MT}} a

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

#define WORLD_E_POS_X   0
#define WORLD_E_POS_Y   0

#define MAX_WORLDS      13

#define HOME_PLANET_ID  12
//---------------------------------------------------------------------------//

extern bool pauseState;
extern bool weaponGift;

extern uint16_t calJoysticX;
extern uint16_t calJoysticY;

extern int8_t  menuItem;
extern uint8_t dogeDialogs;
extern text_t *pTextDialoge;

extern uint8_t someCount;

extern int16_t score;

extern uint8_t  difficultyIncrement;
extern uint16_t playerFireCheck;

extern uint8_t curretLevel;
extern const uint8_t lineCurves[];
extern const uint8_t lvlCoordinates[];

extern ship_t        ship;
extern gift_t        gift;
extern sprite_t      rowL;
extern sprite_t      rowR;
extern inVaderBoss_t alienBoss;
extern saveData_t    gameSaveData;
extern star_t        stars[MAX_STARS];
extern inVader_t     aliens[MAX_ALIENS];
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

extern pFunc_t pCallBackWaitEvent;
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
void drawCredits(void);

void waitScreen(void);
void action(void);

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
bool drawNewLevel(void);
//---------------------------------------------------------------------------//

void menuSwitchSelect(void);
//---------------------------------------------------------------------------//

void initInvaders(void);
void moveInVaders(void);
void drawInVaders(void);
void checkInVaders(void);
void checkInVadersRay(void);
void checkInVadersRespawn(void);
void checkInVadersCollision(void);

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
void getShipItem(void);

//---------------------------------------------------------------------------//

void shipHyperJump(void);
void drawShipExplosion(void);

void initShip(void);
void moveShip(void);
void drawShip(void);
void checkShipDamage(void);

void drawPlayerWeapon(void);

void checkFireButton(void);
void checkShipHealth(void);

void drawHealthStatusBar(uint8_t colorId);
//---------------------------------------------------------------------------//


void initAsteroids(void);
void moveAsteroids(void);
void respawnAsteroids(void);
void checkAsteroids(void);
//---------------------------------------------------------------------------//

// core graphics
void drawTextWindow(text_t *text, text_t *btnText);
void printDialogeText(void);
void updateWindowTextPos(void);

void drawStart(void);
void drawStars(void);

void pauseWindow(void);

void drawRows(void);
// void drawAnimUnfoldRows(void);
// void drawAnimFoldRows(void);

void drawTitleText(void);

void rocketEpxlosion(rocket_t *pRocket);

void drawRandomDoge(void);
void setShakingAvatar(uint8_t posX, uint8_t posY, pic_t *pPic);
void drawShankingAvatar(void);

void setNewBackgroundColor(void);
//---------------------------------------------------------------------------//

// Helpfull functions
void readEEpromScore(void);
void resetScore(void);

void moveBezierCurve(position_t *pPos, bezierLine_t *pItemLine);
void getBezierCurve(uint8_t line);
void fixPosition(position_t *pPos);

int8_t checkShipPosition(int8_t pos, uint8_t min, uint8_t max);

bool checkNewPosition(position_t *objOne, position_t *objTwo);
void applyNewPosition(position_t *objOne, position_t *objTwo, uint8_t picW, uint8_t picH);

void done(text_t *text);

void makeHorribleMagic(uint8_t magicValue);
void applyShipDamage(rocket_t *pWeapon);

void moveEnemyV(position_t *pPos, uint8_t moveSize);

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

void baseTitleTask(void);
void addTitleTasks(void);
// void addGameModeSelect(void);
void addHistoryTasks(void);
void addStoryTasks(void);
void addShipSelectTasks(void);
void addGameTasks(void);
void addBossTasks(void);
void addGiftTasks(void);
void addAsteroidsTasks(void);
void addCreditsTasks(void);

void setGameTasks(tasksArr_t *pTasks);
//---------------------------------------------------------------------------//

// don't forget to setup correct number of tasks in TSM_CONFIG_NUM_WORK_TASKS
// in "tinySM_config.h"
#define TITLE_TASKS_COUNT      7
#define HISTORY_TASKS_COUNT    4
#define SHIP_SEL_TASKS_COUNT   7
#define STORY_TASKS_COUNT      7
#define LVL_SEL_TASKS_COUNT    5
#define GAME_TASKS_COUNT      17  // actually +3 for gift
#define BOSS_TASKS_COUNT      14
#define GIFT_TASKS_COUNT      10
#define GAME_OVER_TASKS_COUNT  4

#define PLAYER_FIRE_CHECK      260
#define PLAYER_FIRE_CHECK_COST 15

extern TASK_ARR_N( startup );
extern TASK_ARR_N( title );
extern TASK_ARR_N( history );
extern TASK_ARR_N( story );
extern TASK_ARR_N( shipSel );
extern TASK_ARR_N( levelSelect );
extern TASK_ARR_N( game );
extern TASK_ARR_N( boss );
extern TASK_ARR_N( gift );
extern TASK_ARR_N( waitCallBack );
extern TASK_ARR_N( asteroidField );
extern TASK_ARR_N( credits );


extern TASK_N(printDialogeText);
extern TASK_N(updateBtnStates);
extern TASK_N(playMusic);
extern TASK_N(drawRows);
// extern TASK_N(drawAnimUnfoldRows);
// extern TASK_N(drawAnimFoldRows);

extern TASK_N(waitScreen);
extern TASK_N(drawCredits);
extern TASK_N(drawShipExplosion);

extern TASK_N(moveGift);
extern TASK_N(drawGift);
extern TASK_N(checkGift);

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*_COMMON_H*/
