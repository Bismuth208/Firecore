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
  pAlien->pos.New.x = ALIEN_DEFAULT_POS_X;
  pAlien->pos.New.y = RN % ALIEN_MOVE_ZONE_Y_MAX;
  pAlien->health = ALIEN_HEALTH;
  pAlien->timeToShoot = RAND_SHOOT_TIME;
  pAlien->respawnTime = RAND_RESPAWN_TIME;
  pAlien->alive = state;
}

void setDeathRayState(inVader_t *pAlien, bool state)
{
  pAlien->deathRay.pos.x = pAlien->pos.New.x;
  pAlien->deathRay.pos.y = pAlien->pos.New.y+5; // +5 is offset to make center align
  pAlien->deathRay.onUse = state;
  pAlien->deathRay.state = ((RN % 2) ? false : true);
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
  inVader_t *pAlien = &alien[0];

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(pAlien->alive) {

      if(pAlien->state) { // move every second move...
        if(ship.pos.Base.y > pAlien->pos.New.y) {
          pAlien->pos.New.y += difficultyIncrement + 1;
        } else if(ship.pos.Base.y < pAlien->pos.New.y) {
          pAlien->pos.New.y -= difficultyIncrement + 1;
        }
      }

      if((--pAlien->pos.New.x) > TFT_W) {
        pAlien->pos.New.x = ALIEN_MOVE_ZONE_X_MIN;
        pAlien->alive = false;
        fillRectFast(pAlien->pos.New.x, pAlien->pos.New.y, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H);
      }
    }
    ++pAlien;
  }
}

void checkInVadersRespawn(void)
{
  inVader_t *pAlien = &alien[0];

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(pAlien->alive == false) {
      if((pAlien->respawnTime--) == 0) {
        setInvaderValue(pAlien, true); // make it alive, and update params          
      }
    }
    ++pAlien;
  }
}

void checkAliveAliens(void)
{
  uint8_t deadAliens=0;
  inVader_t *pAlien = &alien[0];

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(pAlien->alive == false) {
      ++deadAliens;
    }
    ++pAlien;
  }

  if(deadAliens == MAX_ALIENS) {
    bossInit(); // No more respawns left, all army defeated
  }
}

void checkInVadersRay(void)
{
  inVader_t *pAlien = &alien[0];

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(pAlien->deathRay.onUse) { // is it shoot already?
      // clear previous shoot
      fillRectFast(pAlien->deathRay.pos.x, pAlien->deathRay.pos.y, DEATHRAY_PIC_W, DEATHRAY_PIC_H);
        
      if((pAlien->deathRay.pos.x -= (DEATH_RAY_SPEED+difficultyIncrement)) < TFT_W) { 

        const uint8_t *pic = (pAlien->deathRay.state ? deathRayHiPic : deathRayLowPic);
        uint16_t picSize = (pAlien->deathRay.state ? DEATHRAY_HI_PIC_SIZE : DEATHRAY_LOW_PIC_SIZE);
        
        pAlien->deathRay.state = !pAlien->deathRay.state;
         
        drawBMP_RLE_P(pAlien->deathRay.pos.x, pAlien->deathRay.pos.y,
                              DEATHRAY_PIC_W, DEATHRAY_PIC_H, pic, picSize);


      } else {
        // oooh, we don`t shoot the player, inVeder sooo saaad :(
        pAlien->timeToShoot = RAND_SHOOT_TIME;
        pAlien->deathRay.onUse = false;
      }
    } else {
      if(pAlien->alive) {
        if((pAlien->timeToShoot--) == 0) {  // decrease timeout to shoot; Captain Obvious :)
          setDeathRayState(pAlien, true);
#if ADD_SOUND
          if(soundEnable) toneBuzz(ALIEN_SOUND_FREQ, ALIEN_SOUND_LONG);
#endif
        }
      }
    }
    ++pAlien;
  }
}

void checkInVaders(void)
{
  // One of the most difficult tasks in all game
  // recalc all collisions between player's rockets and each alien
  // i.e. rounds = MAX_PEW_PEW x MAX_ALIENS (in worst case of course)
  
  uint8_t countV, countR;
  inVader_t *pAlien = &alien[0];
  rocket_t *pRocket = &playeRockets[0];

  for(countR =0; countR < MAX_PEW_PEW; countR++) {
    if(pRocket->onUse) {
      for(countV=0; countV < MAX_ALIENS; countV++) {
        if(pAlien->alive) {
          if(checkCollision(&pRocket->pos,ROCKET_W, ROCKET_H,
                       &pAlien->pos.Base, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H)) {

            rocketEpxlosion(pRocket);
            
            // alien absorb damage
            pAlien->health -= ship.states.power;
            
            if(pAlien->health <= 0) {
              // if it dead, remove body from battleground
              fillRectFast(pAlien->pos.Base.x, pAlien->pos.Base.y, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H);
 
              pAlien->alive = false;    // actually now it dead
              score += SCORE_VAL;           // get cookies
              //hudStatus.updScore = true;    // update score later
              // check total respawns
              if(--totalRespawns <= 0) { // No more respawns left, all army defeated
                totalRespawns =0;
                replaceTask(checkInVadersRespawn, checkAliveAliens, 4 SEC, true);
              }
            }
          }
        }
        ++pAlien;
      }
    }
    ++pRocket;
  }
}


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

void bossInit(void)
{
  setInvaderValue(&alienBoss, true);
  setDeathRayState(&alienBoss, false);
  alienBoss.health = ALIEN_BOSS_HEALTH;

  addBossTasks();
}

void moveBossVertical(void)
{
  //if(alienBoss.state) { // move every second move...
    if(ship.pos.Base.y > alienBoss.pos.New.y) {
      alienBoss.pos.New.y += (ALIEN_BOSS_SPEED_MOVE + difficultyIncrement);
    } else if(ship.pos.Base.y < alienBoss.pos.New.y) {
      alienBoss.pos.New.y -= (ALIEN_BOSS_SPEED_MOVE + difficultyIncrement);
    }
  //}
}

void drawBoss(void)
{
  if(alienBoss.alive) { // ALIIIVEE! IT`S ALIIVEEE!
    movePicture(&alienBoss.pos, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H);
      
    drawBMP_RLE_P(alienBoss.pos.Base.x, alienBoss.pos.Base.y,
                       ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H, bossShip,
                       ALIEN_SHIP_BOSS_PIC_SIZE);
  }    
}

void drawBossExplosion(void)
{
  for(uint8_t i=0; i<ALIEN_BOSS_EXPLOSIONS; i++) {
    // reuse death ray
    alienBoss.deathRay.pos.x = RN % ALIEN_SHIP_BOSS_PIC_W + alienBoss.pos.Base.x;
    alienBoss.deathRay.pos.y = RN % ALIEN_SHIP_BOSS_PIC_H + alienBoss.pos.Base.y;

    rocketEpxlosion(&alienBoss.deathRay);
  }
}

void checkBossDamage(void)
{
  int8_t countR;
  rocket_t *pRocket = &playeRockets[0];

  if(alienBoss.alive) {
    for(countR =0; countR < MAX_PEW_PEW; countR++) {
      if(pRocket->onUse) {
        if(checkCollision(&pRocket->pos, ROCKET_W, ROCKET_H,
              &alienBoss.pos.Base, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H)) {

          rocketEpxlosion(pRocket);
            
          // alien absorb damage
          alienBoss.health -= ship.states.power;
            
          if(alienBoss.health <= 0) {
            // if it dead, remove body from battleground
            fillRectFast(alienBoss.pos.Base.x, alienBoss.pos.Base.y, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H);
 
            alienBoss.alive = false;      // actually now it dead
            score += BOSS_SCORE_VAL;      // get cookies
            //hudStatus.updScore = true;    // update score later
            bossDie();                    // was it easy no?
          }
        }
      }
      ++pRocket;
    }
  }
}

void checkBossFire(void)
{
  if(alienBoss.deathRay.onUse) { // is it shoot already?
    // clear previous shoot
    fillRectFast(alienBoss.deathRay.pos.x, alienBoss.deathRay.pos.y, ALIEN_ROCKET_PIC_W, ALIEN_ROCKET_PIC_H);

    //if(alienBoss.state) { // move every second move...
      if(ship.pos.Base.y > alienBoss.deathRay.pos.y) {
        alienBoss.deathRay.pos.y += (ALIEN_BOSS_ROCKET_SPEED_MOVE + difficultyIncrement);
      } else if(ship.pos.Base.y < alienBoss.deathRay.pos.y) {
        alienBoss.deathRay.pos.y -= (ALIEN_BOSS_ROCKET_SPEED_MOVE + difficultyIncrement);
      }
    //}
        
    if((alienBoss.deathRay.pos.x -= DEATH_RAY_SPEED) < TFT_W) { 
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
        if(soundEnable) toneBuzz(ALIEN_SOUND_FREQ, ALIEN_SOUND_LONG);
#endif
      }
    }
  }
}

void bossDie(void)
{
  drawBossExplosion();
  dropGift();
}
