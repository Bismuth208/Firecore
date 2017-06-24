#include <esploraAPI.h>

#include "taskmanager.h"

#include "pics.h"
#include "common.h"

uint8_t  totalRespawns = ALIEN_KILLS_TO_BOSS;
inVader_t alien[MAX_ALIENS];
inVader_t alienBoss;
uint8_t someCount = 0;

bool bossMoveDirect = false;

//---------------------------------- inVaders --------------------------------------------//
void setInvaderValue(inVader_t *pAlien, bool state)
{
  pAlien->posNew.x = ALIEN_DEFAULT_POS_X;
  pAlien->posNew.y = RN % ALIEN_MOVE_ZONE_Y_MAX;
  pAlien->health = ALIEN_HEALTH;
  pAlien->timeToShoot = RAND_SHOOT_TIME;
  pAlien->respawnTime = RAND_RESPAWN_TIME;
  pAlien->alive = state;
}

void setDeathRayState(inVader_t *pAlien, bool state)
{
  pAlien->deathRay.pos.x = pAlien->posNew.x;
  pAlien->deathRay.pos.y = pAlien->posNew.y+5; // +5 is offset to make center align
  pAlien->deathRay.onUse = state;
}

void initInvaders(void)
{
  bool aliveState;

  for(uint8_t count =0; count < MAX_ALIENS; count++) {  // Yeah, hate me for this!
    alien[count].state = ((RN % 2) ? false : true);
    aliveState = ((RN % 2) ? false : true);

    setInvaderValue(&alien[count], aliveState);
    setDeathRayState(&alien[count], false);
  }
}

void moveInVaders(void)
{
  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(alien[count].alive) {

      if(alien[count].state) { // move every second move...
        if(ship.posBase.y > alien[count].posNew.y) {
          alien[count].posNew.y += difficultyIncrement + 1;
        } else if(ship.posBase.y < alien[count].posNew.y) {
          alien[count].posNew.y -= difficultyIncrement + 1;
        }
      }

      if((--alien[count].posNew.x) <= ALIEN_MOVE_ZONE_X_MIN) {
        tftFillRect(alien[count].posNew.x, alien[count].posNew.y,
                  ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H, currentBackGroundColor);

        alien[count].alive = false;
      }
    }    
  }
}

void checkInVadersRespawn(void)
{
  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(alien[count].alive == false) {
      if((alien[count].respawnTime--) == 0) {
        setInvaderValue(&alien[count], true); // make it alive, and update params          
      }
    }
  }
}

void checkAliveAliens(void)
{
  uint8_t deadAliens=0;

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(alien[count].alive == false) {
      ++deadAliens;
    }
  }

  if(deadAliens == MAX_ALIENS) {
    bossInit(); // No more respawns left, all army defeated
  }
}

void checkInVadersRay(void)
{
  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(alien[count].deathRay.onUse) { // is it shoot already?
      // clear previous shoot
      tftFillRect(alien[count].deathRay.pos.x, alien[count].deathRay.pos.y,
                    ALIEN_ROCKET_PIC_W, ALIEN_ROCKET_PIC_H, currentBackGroundColor);
        
      if((alien[count].deathRay.pos.x -= (DEATH_RAY_SPEED+difficultyIncrement)) > 0) { 
        drawBMP_RLE_P(alien[count].deathRay.pos.x, alien[count].deathRay.pos.y,
                       ALIEN_ROCKET_PIC_W, ALIEN_ROCKET_PIC_H,
                       alienRocketPic, ALIEN_ROCKET_PIC_SIZE);
      } else {
        // oooh, we don`t shoot the player, inVeder sooo saaad :(
        alien[count].timeToShoot = RAND_SHOOT_TIME;
        alien[count].deathRay.onUse = false;
      }
    } else {
      if(alien[count].alive) {
        if((alien[count].timeToShoot--) == 0) {  // decrease timeout to shoot; Captain Obvious :)
          setDeathRayState(&alien[count], true);
#if ADD_SOUND
          if(soundEnable) toneBuzz(500, 10);
#endif
        }
      }
    }
  }
}

void checkInVaders(void)
{
  // One of the most difficult tasks in all game
  // recalc all collisions between player's rockets and each alien
  // i.e. rounds = MAX_PEW_PEW x MAX_ALIENS (in worst case of course)
  
  uint8_t countV, countR;

  for(countR =0; countR < MAX_PEW_PEW; countR++) {
    if(playeRockets[countR].onUse) {
      for(countV=0; countV < MAX_ALIENS; countV++) {
        if(alien[countV].alive) {
          if(checkCollision(&playeRockets[countR].pos,ROCKET_W, ROCKET_H,
              &alien[countV].posBase, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H)) {

            rocketEpxlosion(&playeRockets[countR]);
            
            // alien absorb damage
            alien[countV].health -= DAMAGE_TO_ALIEN; // More difficult less damage!
            
            if(alien[countV].health < 0) {
              // if it dead, remove body from battleground
              tftFillRect(alien[countV].posBase.x, alien[countV].posBase.y,
                                   ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H, currentBackGroundColor); 
 
              alien[countV].alive = false;    // actually now it dead
              //score += SCORE_VAL;           // get cookies
              //hudStatus.updScore = true;    // update score later
              // check total respawns
              if(--totalRespawns <= 0) { // No more respawns left, all army defeated
                totalRespawns =0;
                replaceTask(checkInVadersRespawn, checkAliveAliens, 4 SEC, true);
              }
            }
          }
        }
      }
    }
  }
}


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

void bossInit(void)
{
  setInvaderValue(&alienBoss, true);
  setDeathRayState(&alienBoss, false);
  alienBoss.health = ALIEN_BOSS_HEALTH;

  deleteAllTasks();
  addBossTasks();
}

void moveBossVertical(void)
{
  //if(alienBoss.state) { // move every second move...
    if(ship.posBase.y > alienBoss.posNew.y) {
      alienBoss.posNew.y += (ALIEN_BOSS_SPEED_MOVE + difficultyIncrement);
    } else if(ship.posBase.y < alienBoss.posNew.y) {
      alienBoss.posNew.y -= (ALIEN_BOSS_SPEED_MOVE + difficultyIncrement);
    }
  //}
}

void drawBoss(void)
{
  if(alienBoss.alive) { // ALIIIVEE! IT`S ALIIVEEE!
    movePicture(&alienBoss.posBase, &alienBoss.posNew, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H);
      
    drawBMP_RLE_P(alienBoss.posBase.x, alienBoss.posBase.y,
                       ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H, bossShip,
                       ALIEN_SHIP_BOSS_PIC_SIZE);
  }    
}

void checkBossDamage(void)
{
  int8_t countR;

  if(alienBoss.alive) {
    for(countR =0; countR < MAX_PEW_PEW; countR++) {
      if(playeRockets[countR].onUse) {
        if(checkCollision(&playeRockets[countR].pos,ROCKET_W, ROCKET_H,
              &alienBoss.posBase, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H)) {

          rocketEpxlosion(&playeRockets[countR]);
            
          // alien absorb damage
          alienBoss.health -= DAMAGE_TO_BOSS; // More difficult less damage!
            
          if(alienBoss.health < 0) {
            // if it dead, remove body from battleground
            tftFillRect(alienBoss.posBase.x, alienBoss.posBase.y,
                                 ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H,
                                 currentBackGroundColor);
 
            alienBoss.alive = false;      // actually now it dead
            bossDie();                    // was it easy no?
            //score += BOSS_SCORE_VAL;           // get cookies
            //hudStatus.updScore = true;    // update score later
          }
        }
      }
    }
  }
}

void checkBossFire(void)
{
  if(alienBoss.deathRay.onUse) { // is it shoot already?
    // clear previous shoot
    tftFillRect(alienBoss.deathRay.pos.x, alienBoss.deathRay.pos.y,
                    ALIEN_ROCKET_PIC_W, ALIEN_ROCKET_PIC_H, currentBackGroundColor);

    //if(alienBoss.state) { // move every second move...
      if(ship.posBase.y > alienBoss.deathRay.pos.y) {
        alienBoss.deathRay.pos.y += (ALIEN_BOSS_ROCKET_SPEED_MOVE + difficultyIncrement);
      } else if(ship.posBase.y < alienBoss.deathRay.pos.y) {
        alienBoss.deathRay.pos.y -= (ALIEN_BOSS_ROCKET_SPEED_MOVE + difficultyIncrement);
      }
    //}
        
    if((alienBoss.deathRay.pos.x -= DEATH_RAY_SPEED) > 0) { 
        drawBMP_RLE_P(alienBoss.deathRay.pos.x, alienBoss.deathRay.pos.y,
                       ALIEN_ROCKET_PIC_W, ALIEN_ROCKET_PIC_H,
                       alienRocketPic, ALIEN_ROCKET_PIC_SIZE);
    } else {
      // oooh, we don`t shoot the player, inVeder sooo saaad :(
      alienBoss.timeToShoot = RAND_SHOOT_TIME;
      alienBoss.deathRay.onUse = false;
    }
  } else {
    if(alienBoss.alive) {
      if((alienBoss.timeToShoot--) == 0) {  // decrease timeout to shoot; Captain Obvious :)
        setDeathRayState(&alienBoss, true);
#if ADD_SOUND
        if(soundEnable) toneBuzz(500, 10);
#endif
      }
    }
  }
}

void bossDie(void)
{
  deleteAllTasks();
  if((++curretLevel) >= MAX_WORLDS) { // is it was final boss?
    victory();
    addTask(getBtnStates, 50, true);
    addTask(waitEnd, 250, true);
  } else {
    totalRespawns = ALIEN_KILLS_TO_BOSS;
    if(++difficultyIncrement > MAX_DIFFICULT_INCREMENT) { // increase speed of all each lvl
      difficultyIncrement = MAX_DIFFICULT_INCREMENT;
    }

    //currentBackGroundColor = getPicWord(nesPalette_ext, getPicByte(lvlColors + curretLevel));
  
    levelClear();
    addTask(getBtnStates, 50, true);
    addTask(waitOk, 400, true);
  }
}
