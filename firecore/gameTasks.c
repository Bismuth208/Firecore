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

#include <esploraAPI.h>
#include "common.h"

// just do not try to understand!
// overvice mindblow can occur...
// and i really dont care about names, as i'll acess throw pointers

/*
#define T(a) a##Task
#define TASK_N(a)     const taskParams_t T(a)
#define TASK(a,b)     TASK_N(a) PROGMEM = {.pFunc=a, .timeOut=b}
#define TASK_P(a)     (taskParams_t*)&T(a)
#define TASK_ARR_N(a) const tasksArr_t a##TasksArr[]
#define TASK_ARR(a)   TASK_ARR_N(a) PROGMEM
#define TASK_END      NULL
*/

//---------------------------------------------------------------------------//
// Thanks to macro below, only one pointer copy of each task are possible

TASK(updateBtnStates, 25);
TASK(checkFireButton, PLAYER_FIRE_CHECK);

TASK(drawStars, 30);
TASK(drawSomeGUI, 1 SEC);
TASK(pauseMenu, 500);
TASK(drawTitleText, 1 SEC);
TASK(drawStart, 500);
TASK(drawCredits, 5 SEC);

TASK(drawShip, 40);
TASK(moveShip, 40);
TASK(drawPlayerWeapon, 60);
TASK(checkShipHealth, 400);
TASK(drawShipExplosion, 250);

TASK(drawCurrentShipSelection, 250);
TASK(getShipItem, 250);

TASK(checkInVaders, 150);
TASK(drawInVaders, 60);
TASK(moveInVaders, 70);  // this is must be more than drawInVaders()
TASK(checkInVadersRespawn, 1 SEC);
TASK(checkInVadersRay, 200);
TASK(checkInVadersCollision, 150);

TASK(drawBoss, 130);
TASK(moveBossVertical, 75);
TASK(checkBossDamage, 80);
TASK(checkBossFire, 150);
TASK(checkBossRays, 200);
TASK(drawBossExplosion, 250);

TASK(moveGift, 120);
TASK(drawGift, 150);
TASK(checkGift, 100);
TASK(dropWeaponGift, 10 SEC);

TASK(moveAsteroids, 60);
TASK(checkAsteroids, 80);
TASK(respawnAsteroids, 200);

TASK(playMusic, 50);

TASK(drawRows, 10);
TASK(waitScreen, 400);
TASK(printDialogeText, 40);
TASK(drawStaticNoise, 50);
TASK(blinkLevelPointer, 250);
TASK(drawRandomDoge, 80);

TASK(menuSwitchSelect, 400);

//---------------------------------------------------------------------------//

TASK_ARR( game ) = {
  TASK_P(drawStars),
  TASK_P(moveShip),
  TASK_P(drawShip),
  TASK_P(checkFireButton),
  TASK_P(pauseMenu),
  TASK_P(drawPlayerWeapon),
  TASK_P(checkShipHealth),
  TASK_P(drawSomeGUI),
  TASK_P(checkInVaders),
  TASK_P(drawInVaders),
  TASK_P(moveInVaders),
  TASK_P(checkInVadersRespawn),
  TASK_P(checkInVadersRay),
  TASK_P(checkInVadersCollision),
  TASK_P(dropWeaponGift),
  TASK_END
};
//---------------------------------------------------------------------------//

TASK_ARR( boss )= {
  TASK_P(moveShip),
  TASK_P(drawShip),
  TASK_P(checkFireButton),
  TASK_P(pauseMenu),
  TASK_P(drawPlayerWeapon),
  TASK_P(checkShipHealth),
  TASK_P(drawSomeGUI),
  TASK_P(drawBoss),
  TASK_P(moveBossVertical),
  TASK_P(checkBossDamage),
  TASK_P(checkBossFire),
  TASK_P(checkBossRays),
  TASK_END
};
//---------------------------------------------------------------------------//

TASK_ARR( gift ) = {
  TASK_P(moveShip),
  TASK_P(drawShip),
  TASK_P(checkFireButton),
  TASK_P(drawPlayerWeapon),
  TASK_P(drawBossExplosion),
  TASK_P(moveGift),
  TASK_P(drawGift),
  TASK_P(checkGift),
  TASK_END
};
//---------------------------------------------------------------------------//

TASK_ARR( asteroidField ) = {
  TASK_P(drawStars),
  TASK_P(moveShip),
  TASK_P(drawShip),
  TASK_P(checkFireButton),
  TASK_P(pauseMenu),
  TASK_P(drawPlayerWeapon),
  TASK_P(checkShipHealth),
  TASK_P(drawSomeGUI),
  TASK_P(respawnAsteroids),
  TASK_P(moveAsteroids),
  TASK_P(checkAsteroids),
  TASK_END
};
//---------------------------------------------------------------------------//

TASK_ARR( startup ) = {
  TASK_P(drawRows),
  TASK_END
};
//---------------------------------------------------------------------------//

TASK_ARR( title ) = {
  TASK_P(menuSwitchSelect),  
  TASK_P(drawStars),
  TASK_P(drawShip),
  TASK_P(drawStart),
  TASK_P(drawTitleText),
  TASK_END
};

TASK_ARR( history ) = {
  TASK_P(menuSwitchSelect),
  TASK_P(printDialogeText),
  TASK_END
};

TASK_ARR( shipSel ) = {
  TASK_P(menuSwitchSelect),
  TASK_P(drawShip),
  TASK_P(drawCurrentShipSelection),
  TASK_P(getShipItem),
  TASK_P(printDialogeText),
  TASK_END
};

TASK_ARR( story ) = {
  TASK_P(menuSwitchSelect), 
  TASK_P(blinkLevelPointer),
  TASK_P(printDialogeText),
  TASK_P(drawStaticNoise),
  TASK_P(drawRandomDoge),
  TASK_END
};

TASK_ARR( levelSelect ) = {
  TASK_P(menuSwitchSelect),
  TASK_P(blinkLevelPointer),
  TASK_P(printDialogeText),
  TASK_END
};
//---------------------------------------------------------------------------//

TASK_ARR( waitCallBack ) = {
  TASK_P(waitScreen),
  TASK_END
};

TASK_ARR( credits ) = {
  TASK_P(printDialogeText),
  TASK_P(drawCredits),
  TASK_END
};
