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

#include <avr/pgmspace.h>  // for progmem

#include "taskmanager.h"

#include "common.h"

// just do not try to understand!
// overvice mindblow can occur...
// and i really dont care about names, as i'll acess throw pointers

/*
#define T(a) a##Task
#define TASK_N(a)    const taskParams_t T(a)
#define TASK(a,b)    TASK_N(a) PROGMEM = {a, b}
#define TASK_P(a)    (taskParams_t*)&T(a)
#define TASK_ARR(a)  const tasksArr_t a##TasksArr[] PROGMEM
*/

//---------------------------------------------------------------------------//
// Thanks to macro below, only one pointer copy of each task are possible

TASK(updateBtnStates, 40);
TASK(checkFireButton, 200);

TASK(titleAction, 400);
TASK(historyAction, 400);

TASK(drawStars, 50);
TASK(drawSomeGUI, 500);
TASK(pauseMenu, 500);
TASK(drawTitleText, 1 SEC);
TASK(drawStart, 500);

TASK(drawShip, 70);
TASK(moveShip, 70);
TASK(drawPlayerRockets, 60);
TASK(checkShipHealth, 500);
TASK(drawShipExplosion, 250);

TASK(drawCurrentShipSelection, 250);
TASK(getShipItem, 250);
TASK(checkShipSelect, 400);

TASK(checkInVaders, 80);
TASK(drawInVaders, 150);
TASK(moveInVaders, 160);  // this is must be more than drawInVaders()
TASK(checkInVadersRespawn, 1 SEC);
TASK(checkInVadersRay, 180);

TASK(drawBoss, 130);
TASK(moveBossVertical, 150);
TASK(checkBossDamage, 80);
TASK(checkBossFire, 150);
TASK(checkBossRays, 200);
TASK(drawBossExplosion, 250);

TASK(moveGift, 120);
TASK(drawGift, 150);
TASK(checkGift, 80);
TASK(dropWeaponGift, 10 SEC);

TASK(playMusic, 50);

TASK(drawRows, 10);
TASK(waitEnd, 400);
TASK(waitOk, 400);
TASK(drawStory, 500);
TASK(printDialogeText, 40);
TASK(printHistory, 40);
TASK(drawStaticNoise, 50);
TASK(drawLevelSelect, 250);
TASK(blinkLevelPointer, 250);

//---------------------------------------------------------------------------//

TASK_ARR( title ) = {
  TASK_P(updateBtnStates),
  TASK_P(playMusic),
  TASK_P(titleAction),
  TASK_P(drawStars),
  TASK_P(drawShip),
  TASK_P(drawStart),
  TASK_P(drawTitleText)
};
//---------------------------------------------------------------------------//

TASK_ARR( game ) = {
  TASK_P(updateBtnStates),
  TASK_P(playMusic),
  TASK_P(drawStars),
  TASK_P(moveShip),
  TASK_P(drawShip),
  TASK_P(checkFireButton),
  TASK_P(pauseMenu),
  TASK_P(drawPlayerRockets),
  TASK_P(checkShipHealth),
  TASK_P(drawSomeGUI),
  TASK_P(checkInVaders),
  TASK_P(drawInVaders),
  TASK_P(moveInVaders),
  TASK_P(checkInVadersRespawn),
  TASK_P(checkInVadersRay),
  TASK_P(moveGift),
  TASK_P(drawGift),
  TASK_P(checkGift),
  TASK_P(dropWeaponGift)
};
//---------------------------------------------------------------------------//

TASK_ARR( boss )= {
  TASK_P(updateBtnStates),
  TASK_P(playMusic),
  TASK_P(moveShip),
  TASK_P(drawShip),
  TASK_P(checkFireButton),
  TASK_P(pauseMenu),
  TASK_P(drawPlayerRockets),
  TASK_P(checkShipHealth),
  TASK_P(drawSomeGUI),
  TASK_P(drawBoss),
  TASK_P(moveBossVertical),
  TASK_P(checkBossDamage),
  TASK_P(checkBossFire),
  TASK_P(checkBossRays)
};
//---------------------------------------------------------------------------//

TASK_ARR( gift ) = {
  TASK_P(updateBtnStates),
  TASK_P(playMusic),
  TASK_P(moveShip),
  TASK_P(drawShip),
  TASK_P(checkFireButton),
  TASK_P(drawPlayerRockets),
  TASK_P(drawBossExplosion),
  TASK_P(moveGift),
  TASK_P(drawGift),
  TASK_P(checkGift)
};
//---------------------------------------------------------------------------//

TASK_ARR( shipSel ) = {
  TASK_P(updateBtnStates),
  TASK_P(playMusic),
  TASK_P(drawShip),
  TASK_P(drawCurrentShipSelection),
  TASK_P(getShipItem),
  TASK_P(checkShipSelect),
  TASK_P(printDialogeText)
};

TASK_ARR( story ) = {
  TASK_P(updateBtnStates),
  TASK_P(playMusic),
  TASK_P(drawStory),
  TASK_P(blinkLevelPointer),
  TASK_P(printDialogeText),
  TASK_P(drawStaticNoise)
};

TASK_ARR( history ) = {
  TASK_P(updateBtnStates),
  TASK_P(playMusic),
  TASK_P(printHistory),
  TASK_P(historyAction)
};

TASK_ARR( levelSelect ) = {
  TASK_P(updateBtnStates),
  TASK_P(playMusic),
  TASK_P(drawLevelSelect),
  TASK_P(blinkLevelPointer),
  TASK_P(printDialogeText)
};

TASK_ARR( gameOver ) = {
  TASK_P(updateBtnStates),
  TASK_P(playMusic),
  TASK_P(waitEnd),
  TASK_P(drawShipExplosion)
};
