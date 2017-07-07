#include <esploraAPI.h>

#include "taskmanager.h"

#include "pics.h"
#include "common.h"

int8_t  totalRespawns = ALIEN_KILLS_TO_BOSS;
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

void moveEnemyV(position_t *pPos, uint8_t moveSize)
{
  if(ship.pos.Base.y > pPos->y) {
    pPos->y += moveSize + difficultyIncrement;
  } else if(ship.pos.Base.y < pPos->y) {
    pPos->y -= moveSize + difficultyIncrement;
  }
}

void checkTimeToShoot(inVader_t *pAlien)
{
  if(pAlien->alive) {
    if((pAlien->timeToShoot--) == 0) {  // decrease timeout to shoot; Captain Obvious :)
      setDeathRayState(pAlien, true);
#if ADD_SOUND
      if(soundEnable) toneBuzz(ALIEN_SOUND_FREQ, ALIEN_SOUND_LONG);
#endif
    }
  }
}

//---------------------------------------------------------------------------//

void initInvaders(void)
{
  bool aliveState;
  inVader_t *pAlien = &alien[0];

  for(uint8_t count =0; count < MAX_ALIENS; count++) {  // Yeah, hate me for this!
    pAlien->state = ((RN % 2) ? false : true);
    aliveState = ((RN % 2) ? false : true);

    setInvaderValue(pAlien, aliveState);
    setDeathRayState(pAlien, false);
    ++pAlien;
  }
}
//---------------------------------------------------------------------------//
void moveInVaders(void)
{
  inVader_t *pAlien = &alien[0];

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(pAlien->alive) {

      if(pAlien->state) {
        moveEnemyV(&pAlien->pos.New, 1);
      }

      if((--pAlien->pos.New.x) > TFT_W) {
        pAlien->alive = false;
        pAlien->pos.New.x = ALIEN_MOVE_ZONE_X_MIN; // fix overflow for fillRectFast
        fillRectFast(pAlien->pos.New.x, pAlien->pos.New.y, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H);
      }
    }
    ++pAlien;
  }
}

void drawInVaders(void)
{
  inVader_t *pAlien = &alien[0];

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(pAlien->alive) { // ALIIIVEE! IT`S ALIIVEEE!

      const uint8_t *pic = (pAlien->state ? alienShipHi : alienShipLow);
      uint16_t picSize = (pAlien->state ? ALIEN_SHIP_HI_PIC_SIZE : ALIEN_SHIP_LOW_PIC_SIZE);
      
      pAlien->state = !pAlien->state;
      drawEnemy(&pAlien->pos, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H, pic, picSize);
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

// calc how much aliens on screen, if 0 then call Boss!
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
      // check damage from deathRay
      if(checkCollision(&pAlien->deathRay.pos, DEATHRAY_PIC_W, DEATHRAY_PIC_H,
                                    &ship.pos.Base, SHIP_PIC_W, SHIP_PIC_H)) {
        applyShipDamage(pAlien);
      } else {
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
      }
    } else {
      checkTimeToShoot(pAlien);
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
  rocket_t *pRocket = pRocketGlobal;

  for(countR =0; countR < MAX_PEW_PEW; countR++) {
    for(countV=0; countV < MAX_ALIENS; countV++) {
      if((pRocket->onUse) && (pAlien->alive)) {
        if(checkCollision(&pRocket->pos, LASER_PIC_W, LASER_PIC_H,
                       &pAlien->pos.Base, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H)) {

          rocketEpxlosion(pRocket);
            
          if((pAlien->health -= ship.states.power) <= 0) { // alien absorb damage
            // if it dead, remove body from battleground
            fillRectFast(pAlien->pos.Base.x, pAlien->pos.Base.y, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H);
 
            pAlien->alive = false;    // actually now it dead
            score += SCORE_VAL;           // get cookies
            hudStatus.updScore = true;    // update score later
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
    pAlien = &alien[0]; // reset pointer
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
  //alienBoss.state = true; // fix for vertical move

  addBossTasks();
}

void moveBossVertical(void)
{
  moveEnemyV(&alienBoss.pos.New, ALIEN_BOSS_SPEED_MOVE);
}

void drawBoss(void)
{
  if(alienBoss.alive) { // ALIIIVEE! IT`S ALIIVEEE!
    drawEnemy(&alienBoss.pos, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H,
                                           bossShip, ALIEN_SHIP_BOSS_PIC_SIZE);
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
  uint8_t countR;
  rocket_t *pRocket = pRocketGlobal;

  if(alienBoss.alive) {
    for(countR =0; countR < MAX_PEW_PEW; countR++) {
      if(pRocket->onUse) {
        if(checkCollision(&pRocket->pos, LASER_PIC_W, LASER_PIC_H,
              &alienBoss.pos.Base, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H)) {

          rocketEpxlosion(pRocket);

          if((alienBoss.health -= ship.states.power) <= 0) {  // alien absorb damage
            // if it dead, remove body from battleground
            fillRectFast(alienBoss.pos.Base.x, alienBoss.pos.Base.y, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H);
 
            alienBoss.alive = false;      // actually now it dead
            score += BOSS_SCORE_VAL;      // get cookies
            hudStatus.updScore = true;    // update score later
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
    if(checkCollision(&alienBoss.deathRay.pos, DEATHRAY_BOSS_PIC_W, DEATHRAY_BOSS_PIC_H,
                                    &ship.pos.Base, SHIP_PIC_W, SHIP_PIC_H)) {
      applyShipDamage(&alienBoss);
    } else {
      // clear previous shoot
      fillRectFast(alienBoss.deathRay.pos.x, alienBoss.deathRay.pos.y,
                                   DEATHRAY_BOSS_PIC_W, DEATHRAY_BOSS_PIC_H);
      moveEnemyV(&alienBoss.deathRay.pos, ALIEN_BOSS_ROCKET_SPEED_MOVE);
        
      if((alienBoss.deathRay.pos.x -= DEATH_RAY_SPEED) < TFT_W) { 

        drawBMP_RLE_P(alienBoss.deathRay.pos.x, alienBoss.deathRay.pos.y,
                       DEATHRAY_BOSS_PIC_W, DEATHRAY_BOSS_PIC_H,
                       deathRayBossPic, DEATHRAY_BOSS_PIC_SIZE);
      } else {
        // oooh, we don`t shoot the player, inVeder sooo saaad :(
        alienBoss.timeToShoot = RAND_SHOOT_TIME;
        alienBoss.deathRay.onUse = false;
      }
    }
  } else {
    checkTimeToShoot(&alienBoss);
  }
}

void bossDie(void)
{
  drawBossExplosion();
  addGiftTasks();
}
