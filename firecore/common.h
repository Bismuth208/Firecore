#ifndef _COMMON_H
#define _COMMON_H

#include "types.h"

#ifdef __cplusplus
extern "C"{
#endif

//---------------------------------------------------------------------------//

#define TFT_W    160
#define TFT_H    128
//---------------------------------------------------------------------------//

#define MAX_GAME_TASKS        18

#define STAR_STEP             6
#define MAX_STARS             20      // how much stars we see on screen
#define MAX_PEW_PEW           45      // Crysis, nanosuit voice: - "Maximum pew".


#define PLAYER_ROCKET_SPEED   16  //(16/difficult)     // in future will be based on ship type
#define PLAYER_ROCKET_COST    15  //(5*difficult)      // in future will be based on ship type
#define PLAYER_ROCKET_REFILL  10  //(4/difficult)      //
#define ALIEN_HEALTH          90  //(30*difficult)     // if d = 4 then shoots = 10 to kill
#define ALIEN_BOSS_HEALTH    400  //
#define SHIP_HEALTH          180  // Left it as is
#define DEATH_RAY_SPEED       10  //(5*difficult)      // 
#define DAMAGE_TO_ALIEN       32  //(64/difficult)     //
#define DAMAGE_TO_BOSS        10  //
#define DAMAGE_TO_SHIP        20  //(20*difficult)     //

#define DENGER_HEALTH_LVL     30

#define MAX_DIFFICULT_INCREMENT 4

#define SCORE_VAL             20
#define BOSS_SCORE_VAL       200

#define RN             randNum()
#define RAND_CODE      tftSetCP437(RN % 2);
#define SLOW_CPU       setMainFreq(0x04);
#define DC(a)          tftDrawChar(a)

// this macros remove monstro constructions...
#define getConstCharPtr(a, b) (const uint8_t*)pgm_read_word(&(a[b]))
#define getConstWordPtr(a, b) (const uint16_t*)pgm_read_word(&(a[b]))

#define RND_POS_X ((RN % 26) * 6)
#define RND_POS_Y ((RN % 16) * 8)

// from (10 to 38) / x seconds; The higher the difficulty the more fires
// where x - (1 sec / (time to call checkInVadersRay()))
// for example if time = 500, when x = 2; if time = 250, when x = 4;
#define RAND_SHOOT_TIME (RN % 5 + 3)

//from (1 to 5)  seconds;
#define RAND_RESPAWN_TIME (RN % 4 + 1)

#define continue() {CHECK_RULE}

#define ADD_SOUND 0
//---------------------------------------------------------------------------//

#define BUTTON_A   SW_BTN_4_MUX
#define BUTTON_B   SW_BTN_1_MUX
#define BUTTON_X   SW_BTN_2_MUX
#define BUTTON_Y   SW_BTN_3_MUX
#define buttonIsPressed(a)  readSwitchButton(a)

#define LINE_X     X_J_MUX_VAL
#define LINE_Y     Y_J_MUX_VAL
#define getStickVal(a)  getJoyStickValue(a)
//---------------------------------------------------------------------------//

#define MIN_MENU_ITEM  0
#define MAX_MENU_ITEM  2


#define ROCKET_OFFSET_X  18
#define ROCKET_OFFSET_Y  6
//---------------------------------------------------------------------------//

#define SHIP_MIN_POS_X   0
#define SHIP_MIN_POS_Y   0

#define SHIP_MAX_POS_X   TFT_W - SHIP_PIC_W
#define SHIP_MAX_POS_Y   TFT_H - SHIP_PIC_H

#define SHIP_BASE_SPEED  5

#define SHIP_ENERGY_POS_X  112
#define SHIP_ENERGY_POS_Y  125
#define SHIP_ENERGY_W      MAX_PEW_PEW
#define SHIP_ENERGY_H      2

#define SHIP_TITLE_POS_X  60
#define SHIP_TITLE_POS_Y  64

#define SHIP_GAME_POS_X  0
#define SHIP_GAME_POS_Y  64
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
#define ALIEN_MOVE_ZONE_Y_MAX 100

#define ALIEN_MOVE_ZONE_X_MIN 0
#define ALIEN_MOVE_ZONE_X_MAX 125

#define ALIEN_DEFAULT_POS_X   125
#define ALIEN_DEFAULT_POS_Y   20

#define ALIEN_BOSS_MOVE_ZONE_Y_MIN 0
#define ALIEN_BOSS_MOVE_ZONE_Y_MAX 98

// 20 is the most interesting and hardcore!
#define ALIEN_DENSITY         30     // It cannot be less invader1_height (i.e. ! < 18 px)

#define MAX_ALIENS            4

#define ALIEN_KILLS_TO_BOSS   15

#define ALIEN_BOSS_SPEED_MOVE  2
#define ALIEN_BOSS_ROCKET_SPEED_MOVE  2
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
extern inVader_t alienBoss;
extern inVader_t alien[MAX_ALIENS];
extern rocket_t playeRockets[MAX_PEW_PEW];
extern btnStatus_t btnStates;
extern stars_t stars[MAX_STARS];
extern hudStatus_t hudStatus;
extern uint8_t someCount;

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
void drawStory(void);

void gameOver(void);
void levelClear(void);
void victory(void);
void drawSomeGUI(void);

void drawLevelSelect(void);

void waitOk(void);
void waitEnd(void);
//---------------------------------------------------------------------------//

void initInvaders(void);
void moveInVaders(void);
void checkInVadersRespawn(void);
void checkInVadersRay(void);
void checkInVaders(void);

void setInvaderValue(inVader_t *pAlien, bool state);
void setDeathRayState(inVader_t *pAlien, bool state);
//----------------------------//

void addBossTasks(void);

void moveBossVertical(void);
void drawBoss(void);
void checkBossDamage(void);
void checkBossFire(void);
void bossDie(void);

//---------------------------------------------------------------------------//

void initShip(void);
//---------------------------------------------------------------------------//

// core graphics
void drawTextWindow(const uint8_t *text, const uint8_t *btnText);
void drawStart(void);
void drawStars(void);
void drawShip(void);
void drawInVaders(void);
void drawPlayerRockets(void);
void drawTitleText(void);
void drawRows(void);
void screenSliderEffect(uint16_t color);

void drawBMP_RLE_P(int16_t x, int16_t y, uint8_t w, uint8_t h,
                              const uint8_t *pPic, int16_t sizePic);

void rocketEpxlosion(rocket_t *pRocket);
//---------------------------------------------------------------------------//

// Helpfull functions
void getBtnStates(void);
void resetBtnStates(void);

void checkShipPosition(uint16_t *pos, uint16_t max, uint16_t min);

bool checkNewPosition(position_t *objOne, position_t *objTwo);
void applyNewPosition(position_t *objOne, position_t *objTwo, uint16_t picW, uint16_t picH);
void movePicture(position_t *objOne, position_t *objTwo, uint16_t picW, uint16_t picH);

uint8_t randNum(void);
void setMainFreq(uint8_t ps);

bool checkCollision(position_t *pObjOne, uint16_t objOneW, uint16_t objOneH,
                    position_t *pObjTwo, uint16_t objTwoW, uint16_t objTwoH);
//---------------------------------------------------------------------------//

void addGameTasks(void);
void addTitleTasks(void);
void pauseWindow(void);
//---------------------------------------------------------------------------//

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*_COMMON_H*/
