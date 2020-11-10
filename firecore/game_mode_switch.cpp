#include "pics.h"
#include "textProg.h"
#include "common.h"

//---------------------------------------------------------------------------//
__attribute__ ((optimize("O2"))) void setGameTasks(tasksArr_t *pTasks)
{
  vTSMAddTasksArray_P(pTasks);

  // this two tasks always every where
  vTSMAddTask_P(T(&updateBtnStates));
  vTSMAddTask_P(T(&playMusic));
}

__attribute__ ((optimize("O2"))) void addCreditsTasks(void)
{
  // setShakingAvatar(7, 33, cityDogePic); // creditPicQR
  screenSliderEffect(COLOR_ID_BLACK);
  setGameTasks(creditsTasksArr);
}

__attribute__ ((optimize("O2"))) void addAsteroidsTasks(void)
{
  ship.sprite.pos.New = {SHIP_GAME_POS_X, SHIP_GAME_POS_Y};

  initAsteroids();
  setGameTasks(asteroidFieldTasksArr);
}

__attribute__ ((optimize("O2"))) void addGameTasks(void)
{
  setGameTasks(gameTasksArr);

  ship.sprite.pos.New = {SHIP_GAME_POS_X, SHIP_GAME_POS_Y};
  gift.bezLine = {GIFT_MOVE_ID, 0};
  moveBezierCurve(&gift.sprite.pos.New, &gift.bezLine);
  vTSMUpdateTaskTimeCheck(dropWeaponGift, RAND_GIFT_SPAWN_TIME);
}

__attribute__ ((optimize("O2"))) void addBossTasks(void)
{
  bossInit();
  tftFillScreen(currentBackGroundColor);

  setGameTasks(bossTasksArr);
}

// drop gift from the boss
__attribute__ ((optimize("O2"))) void addGiftTasks(void)
{
  weaponGift = false;
  gift.bezLine = {GIFT_MOVE_ID, 0};
  gift.sprite.pPic = giftHeartPic;
  moveBezierCurve(&gift.sprite.pos.New, &gift.bezLine);

  setGameTasks(giftTasksArr);
}

__attribute__ ((optimize("O2"))) void addShipSelectTasks(void)
{
  ship.sprite.pos.New = {SHIP_SELECT_POS_X, SHIP_SELECT_POS_Y};
  setGameTasks(shipSelTasksArr);
}

__attribute__ ((optimize("O2"))) void addStoryTasks(void)
{
  setGameTasks(storyTasksArr);
  vTSMDisableTask(drawStaticNoise);
}

__attribute__ ((optimize("O2"))) void addHistoryTasks(void)
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

__attribute__ ((optimize("O2"))) void addTitleTasks(void)
{
  setGameTasks(titleTasksArr);
}

__attribute__ ((optimize("O2"))) void baseTitleTask(void)
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
