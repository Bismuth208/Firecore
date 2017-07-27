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
 
#include <esploraAPI.h>

#include "taskmanager.h"

#include "pics.h"
#include "common.h"

int8_t  totalRespawns = ALIEN_KILLS_TO_BOSS;
inVader_t alien[MAX_ALIENS];
inVaderBoss_t alienBoss;
uint8_t someCount = 0;

bool bossMoveDirect = false;

//---------------------------------- inVaders --------------------------------------------//
void setInvaderValue(inVader_t *pAlien, bool state)
{
  //pAlien->pos.New.y = RN % ALIEN_MOVE_ZONE_Y_MAX;
  pAlien->bezLine.step = 0; //RN % 10;
  pAlien->bezLine.id = RN % MAX_BEZIER_LINES;
  moveBezierCurve(&pAlien->pos.New, &pAlien->bezLine);
  //fixPosition(&pAlien->pos.New);
  pAlien->pos.New.x = ALIEN_DEFAULT_POS_X;
  pAlien->health = ALIEN_HEALTH;
  pAlien->timeToShoot = RAND_SHOOT_TIME;
  pAlien->respawnTime = RAND_RESPAWN_TIME;
  pAlien->alive = state;
}

void setDeathRayState(rocket_t *deathRay, position_t *pPos, bool state)
{
  deathRay->onUse = state;
  deathRay->state = ((RN % 2) ? false : true);
  deathRay->pos.x = pPos->x;
  deathRay->pos.y = pPos->y+5; // +5 is offset to make center align
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
      setDeathRayState(&pAlien->deathRay, &pAlien->pos.New, true);
#if ADD_SOUND
      sfxPlayPattern(enemyShotPattern, SFX_CH_0);
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
    setDeathRayState(&pAlien->deathRay, &pAlien->pos.New, false);
    ++pAlien;
  }
}
//---------------------------------------------------------------------------//

void moveInVaders(void)
{
  inVader_t *pAlien = &alien[0];

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(pAlien->alive) {

      moveBezierCurve(&pAlien->pos.New, &pAlien->bezLine);
      fixPosition(&pAlien->pos.New);

      if(pAlien->pos.New.y > ALIEN_MOVE_ZONE_Y_MAX) {
        pAlien->pos.New.y = ALIEN_MOVE_ZONE_Y_MAX;
      }

      if(pAlien->pos.New.x > TFT_W) { // reach end of screen
        pAlien->alive = false;
        pAlien->respawnTime = RAND_RESPAWN_TIME;
        pAlien->pos.New.x = ALIEN_MOVE_ZONE_X_MIN; // fix overflow for fillRectFast
        fillRectFast(pAlien->pos.Base.x, pAlien->pos.Base.y, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H);
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

      pAlien->state = !pAlien->state;
      drawEnemy(&pAlien->pos, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H,
                      (pAlien->state ? alienShipHi : alienShipLow));
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
        applyShipDamage(&pAlien->deathRay);
        pAlien->timeToShoot = RAND_SHOOT_TIME;
      } else {
        // clear previous shoot
        fillRectFast(pAlien->deathRay.pos.x, pAlien->deathRay.pos.y, DEATHRAY_PIC_W, DEATHRAY_PIC_H);
        
        if((pAlien->deathRay.pos.x -= (DEATH_RAY_SPEED+difficultyIncrement)) < TFT_W) { 
          pAlien->deathRay.state = !pAlien->deathRay.state;
          drawBMP_RLE_P(pAlien->deathRay.pos.x, pAlien->deathRay.pos.y, 
                        (pAlien->deathRay.state ? deathRayHiPic : deathRayLowPic));
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
  rocket_t *pRocket = &playerLasers[0];

  for(countR =0; countR < MAX_PEW_PEW; countR++) {
    for(countV=0; countV < MAX_ALIENS; countV++) {
      if((pRocket->onUse) && (pAlien->alive)) {
        if(checkCollision(&pRocket->pos, LASER_PIC_W, LASER_PIC_H,
                       &pAlien->pos.Base, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H)) {

          rocketEpxlosion(pRocket);
            
          if((pAlien->health -= ship.states.power) <= 0) { // alien absorb damage
#if ADD_SOUND
            sfxPlayPattern(enemyDestroyPattern, SFX_CH_0);
#endif
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
  setInvaderValue(&alienBoss.base, true);
  setDeathRayState(&alienBoss.base.deathRay, &alienBoss.base.pos.New, false);

  alienBoss.base.bezLine.id = 6; // move up
  alienBoss.base.bezLine.step = 25; // start animation at centere
  moveBezierCurve(&alienBoss.base.pos.New, &alienBoss.base.bezLine);

  rocket_t *pDeathRay = &alienBoss.deathRays[0];
  for(uint8_t count=0; count < ALIEN_BOSS_DEATH_RAYS; count++) {
    setDeathRayState(pDeathRay, &alienBoss.base.pos.New, false);
    ++pDeathRay;
  }
  alienBoss.base.health = ALIEN_BOSS_HEALTH;

  addBossTasks();
}

void moveBossVertical(void)
{
  // move fully controlled by curve and no fix need
  moveBezierCurve(&alienBoss.base.pos.New, &alienBoss.base.bezLine);

  // reach end of move?
  if(alienBoss.base.bezLine.step == bezierLine.totalSteps) {
    // flip direction move
    if(alienBoss.base.bezLine.id == 6) {
      alienBoss.base.bezLine.id = 7;
    } else {
      alienBoss.base.bezLine.id = 6;
    }
    alienBoss.base.bezLine.step = 0;
  }
}

void drawBoss(void)
{
  if(alienBoss.base.alive) { // ALIIIVEE! IT`S ALIIVEEE!
    drawEnemy(&alienBoss.base.pos, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H, bossShip);
  }    
}

void drawBossExplosion(void)
{
  // reuse death ray
  alienBoss.base.deathRay.pos.x = RN % ALIEN_SHIP_BOSS_PIC_W + alienBoss.base.pos.Base.x;
  alienBoss.base.deathRay.pos.y = RN % ALIEN_SHIP_BOSS_PIC_H + alienBoss.base.pos.Base.y;

  rocketEpxlosion(&alienBoss.base.deathRay);
}

void checkBossDamage(void)
{
  uint8_t countR;
  rocket_t *pRocket = &playerLasers[0];

  if(alienBoss.base.alive) {
    for(countR =0; countR < MAX_PEW_PEW; countR++) {
      if(pRocket->onUse) {
        if(checkCollision(&pRocket->pos, LASER_PIC_W, LASER_PIC_H,
              &alienBoss.base.pos.Base, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H)) {

          rocketEpxlosion(pRocket);

          if((alienBoss.base.health -= ship.states.power) <= 0) {  // alien absorb damage
#if ADD_SOUND
            sfxPlayPattern(enemyDestroyPattern, SFX_CH_0);
#endif
            // if it dead, remove body from battleground
            fillRectFast(alienBoss.base.pos.Base.x, alienBoss.base.pos.Base.y, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H);
 
            alienBoss.base.alive = false; // actually now it dead
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
  if(alienBoss.base.deathRay.onUse) { // is it shoot already?
    if(checkCollision(&alienBoss.base.deathRay.pos, DEATHRAY_BOSS_PIC_W, DEATHRAY_BOSS_PIC_H,
                                    &ship.pos.Base, SHIP_PIC_W, SHIP_PIC_H)) {
      applyShipDamage(&alienBoss.base.deathRay);
      alienBoss.base.timeToShoot = RAND_SHOOT_TIME;
    } else {
      // clear previous shoot
      fillRectFast(alienBoss.base.deathRay.pos.x, alienBoss.base.deathRay.pos.y,
                                   DEATHRAY_BOSS_PIC_W, DEATHRAY_BOSS_PIC_H);
      moveEnemyV(&alienBoss.base.deathRay.pos, ALIEN_BOSS_ROCKET_SPEED_MOVE);
        
      if((alienBoss.base.deathRay.pos.x -= DEATH_RAY_SPEED) < TFT_W) { 
        drawBMP_RLE_P(alienBoss.base.deathRay.pos.x, alienBoss.base.deathRay.pos.y, deathRayBossPic);
      } else {
        // oooh, we don`t shoot the player, inVeder sooo saaad :(
        alienBoss.base.timeToShoot = RAND_SHOOT_TIME;
        alienBoss.base.deathRay.onUse = false;
      }
    }
  } else {
    checkTimeToShoot(&alienBoss.base);
  }
}

void checkBossRays(void)
{
  rocket_t *pDeathRay = &alienBoss.deathRays[0];

  for(uint8_t count=0; count < ALIEN_BOSS_DEATH_RAYS; count++) {
    if(pDeathRay->onUse) { // is it shoot already?
      // check damage from deathRay
      if(checkCollision(&pDeathRay->pos, DEATHRAY_PIC_W, DEATHRAY_PIC_H,
                                    &ship.pos.Base, SHIP_PIC_W, SHIP_PIC_H)) {
        applyShipDamage(pDeathRay);
        alienBoss.timeToShoot[count] = RAND_SHOOT_TIME;
      } else {
        // clear previous shoot
        fillRectFast(pDeathRay->pos.x, pDeathRay->pos.y, DEATHRAY_PIC_W, DEATHRAY_PIC_H);
        
        if((pDeathRay->pos.x -= (DEATH_RAY_SPEED+difficultyIncrement)) < TFT_W) {
          pDeathRay->state = !pDeathRay->state;
          drawBMP_RLE_P(pDeathRay->pos.x, pDeathRay->pos.y,
                         (pDeathRay->state ? deathRayHiPic : deathRayLowPic));
        } else {
          // oooh, we don`t shoot the player, inVeder sooo saaad :(
          alienBoss.timeToShoot[count] = RAND_SHOOT_TIME;
          pDeathRay->onUse = false;
        }
      }
    } else {
      if((alienBoss.timeToShoot[count]--) == 0) {  // decrease timeout to shoot; Captain Obvious :)
        setDeathRayState(pDeathRay, &alienBoss.base.pos.New, true);
#if ADD_SOUND
        sfxPlayPattern(enemyShotPattern, SFX_CH_0);
#endif
      }
    }
    ++pDeathRay;
  }
}

void bossDie(void)
{
  addGiftTasks();
}
