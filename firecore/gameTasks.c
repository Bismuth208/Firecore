#include <avr/pgmspace.h>  // for progmem

#include "taskmanager.h"

#include "common.h"

// just do not try to understand!
// overvice mindblow can occur...
// and i really dont care about names, as i'll acess throw pointers

/*
#define T(a) a##Task
#define TASK_N(a)    const taskParams_t T(a)[]
#define TASK(a,b)    TASK_N(a) PROGMEM = {a, b}
#define TASK_P(a)    (taskParams_t*)&T(a)
#define TASK_ARR(a)  tasksArr_t a##TasksArr[] PROGMEM
*/

//---------------------------------------------------------------------------//
// Thanks to macro below, only one pointer copy of each task are possible

TASK(updateBtnStates, 40);
TASK(checkFireButton, 200);

TASK(titleAction, 400);
TASK(drawStars, 50);
TASK(drawSomeGUI, 500);
TASK(pauseMenu, 500);
TASK(drawTitleText, 1 SEC);
TASK(drawStart, 500);

TASK(drawShip, 70);
TASK(moveShip, 70);
TASK(drawPlayerRockets, 60);
TASK(checkShipHealth, 500);

TASK(checkInVaders, 80);
TASK(drawInVaders, 150);
TASK(moveInVaders, 160);  // this is must be more than drawAliens()
TASK(checkInVadersRespawn, 1 SEC);
TASK(checkInVadersRay, 180);

TASK(drawBoss, 130);
TASK(moveBossVertical, 150);
TASK(checkBossDamage, 80);
TASK(checkBossFire, 150);

TASK(moveGift, 120);
TASK(drawGift, 150);
TASK(checkGift, 80);

TASK(drawCurrentShipSelection, 250);
TASK(getShipItem, 250);
TASK(checkShipSelect, 400);



TASK(drawRows, 10);
TASK(waitEnd, 400);
TASK(waitOk, 400);
TASK(drawStory, 250);
TASK(drawStaticNoise, 50);
TASK(drawLevelSelect, 250);
TASK(drawShipExplosion, 250);

//TASK(swapWeapon, 120);
//---------------------------------------------------------------------------//

TASK_ARR( title ) = {
  TASK_P(titleAction),
  TASK_P(updateBtnStates),
  TASK_P(drawStars),
  TASK_P(drawShip),
  TASK_P(drawStart),
  TASK_P(drawTitleText)
};
//---------------------------------------------------------------------------//

TASK_ARR( game ) = {
  TASK_P(drawStars),
  TASK_P(moveShip),
  TASK_P(drawShip),
  TASK_P(updateBtnStates),
  TASK_P(checkFireButton),
  TASK_P(pauseMenu),
  TASK_P(drawPlayerRockets),
  TASK_P(checkShipHealth),
  TASK_P(drawSomeGUI),
  TASK_P(checkInVaders),
  TASK_P(drawInVaders),
  TASK_P(moveInVaders),
  TASK_P(checkInVadersRespawn),
  TASK_P(checkInVadersRay)
  //TASK_P(moveGift),
  //TASK_P(drawGift, 100),
  //TASK_P(checkGift, 100),
  //TASK_P(dropGift, RAND_GIFT_SPAWN_TIME)
};
//---------------------------------------------------------------------------//

TASK_ARR( boss )= {
  TASK_P(moveShip),
  TASK_P(drawShip),
  TASK_P(updateBtnStates),
  TASK_P(checkFireButton),
  TASK_P(pauseMenu),
  TASK_P(drawPlayerRockets),
  TASK_P(checkShipHealth),
  TASK_P(drawSomeGUI),
  TASK_P(drawBoss),
  TASK_P(moveBossVertical),
  TASK_P(checkBossDamage),
  TASK_P(checkBossFire)
};
//---------------------------------------------------------------------------//

TASK_ARR( gift ) = {
  TASK_P(moveShip),
  TASK_P(drawShip),
  TASK_P(updateBtnStates),
  TASK_P(checkFireButton),
  TASK_P(drawPlayerRockets),
  TASK_P(moveGift),
  TASK_P(drawGift),
  TASK_P(checkGift)
};
//---------------------------------------------------------------------------//

TASK_ARR( shipSel ) = {
  TASK_P(updateBtnStates),
  TASK_P(drawShip),
  TASK_P(drawCurrentShipSelection),
  TASK_P(getShipItem),
  TASK_P(checkShipSelect)
};
