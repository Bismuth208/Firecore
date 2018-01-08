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
  pAlien->bezLine.step = 0; //RN % 10;
  pAlien->bezLine.id = RN % MAX_BEZIER_LINES;
  moveBezierCurve(&pAlien->pos.New, &pAlien->bezLine);

  //pAlien->pos.New.x = ALIEN_DEFAULT_POS_X;
  pAlien->weapon.timeToShoot = ALIEN_RAND_SHOOT_TIME;
  pAlien->respawnTime = ALIEN_RAND_RESPAWN_TIME;
  pAlien->health = ALIEN_HEALTH;
  pAlien->alive = state;
}

void setDeathRayState(rocket_t *deathRay, position_t *pPos, bool state)
{
  deathRay->onUse = state;
  deathRay->state = ((RN & 1) ? false : true); // ste of picture
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

void checkTimeToShoot(deathRay_t *pWeapon, position_t *pPos)
{
  if((pWeapon->timeToShoot--) == 0) {  // decrease timeout to shoot; Captain Obvious :)
    setDeathRayState(&pWeapon->deathRay, pPos, true);
#if ADD_SOUND
    sfxPlayPattern(enemyShotPattern, SFX_CH_0);
#endif
  }
}

void checkDeathRay(deathRay_t *pWeapon)
{
  rocket_t *pDeathRay = &pWeapon->deathRay;
  // check damage from deathRay
  if(checkCollision(&pDeathRay->pos, DEATHRAY_PIC_W, DEATHRAY_PIC_H,
                                    &ship.pos.Base, SHIP_PIC_W, SHIP_PIC_H)) {
    applyShipDamage(pDeathRay);
    pWeapon->timeToShoot = ALIEN_RAND_SHOOT_TIME;
  } else {
    // clear previous shoot
    fillRectFast(pDeathRay->pos.x, pDeathRay->pos.y, DEATHRAY_PIC_W, DEATHRAY_PIC_H);
        
    if((pDeathRay->pos.x -= (DEATH_RAY_SPEED+difficultyIncrement)) < TFT_W) {
      pDeathRay->state = !pDeathRay->state;
      drawBMP_ERLE_P(pDeathRay->pos.x, pDeathRay->pos.y,
                         (pDeathRay->state ? deathRayHiPic : deathRayLowPic));
    } else {
      // oooh, we don`t shoot the player, inVeder sooo saaad :(
      pWeapon->timeToShoot = ALIEN_RAND_SHOOT_TIME;
      pWeapon->deathRay.onUse = false;
    }
  }
}

//---------------------------------------------------------------------------//

void initInvaders(void)
{
  bool aliveState;
  inVader_t *pAlien = &alien[0];

  for(uint8_t count =0; count < MAX_ALIENS; count++) {  // Yeah, hate me for this!
    pAlien->state = ((RN & 1) ? false : true);
    aliveState = ((RN & 1) ? false : true);

    setInvaderValue(pAlien, aliveState);
    setDeathRayState(&pAlien->weapon.deathRay, &pAlien->pos.New, false);
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

      if(pAlien->bezLine.step == bezierLine.totalSteps) {
        pAlien->alive = false;
        pAlien->respawnTime = ALIEN_RAND_RESPAWN_TIME;
        fillRectFast(pAlien->pos.Base.x, pAlien->pos.Base.y, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H);
      }
    }
    ++pAlien;
  }
}

// each invader consume ~6ms in total for 4 = ~24ms
void drawInVaders(void)
{
  inVader_t *pAlien = &alien[0];

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(pAlien->alive) { // ALIIIVEE! IT`S ALIIVEEE!

      pAlien->state = !pAlien->state;
      drawEnemy(&pAlien->pos, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H, alienShipV2);
      drawBMP_ERLE_P(pAlien->pos.Base.x+28, pAlien->pos.Base.y+6,
                   (pAlien->state ? alienShipFireHi : alienShipFireLow));
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
    // make all alien shots to explode
    pAlien = &alien[0];
    for(uint8_t count=0; count < MAX_ALIENS; count++) {
      if(pAlien->weapon.deathRay.onUse) {
        rocketEpxlosion(&pAlien->weapon.deathRay);
      }
      ++pAlien;
    }

    bossInit(); // No more respawns left, all army defeated
  }
}

void checkInVadersRay(void)
{
  inVader_t *pAlien = &alien[0];

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(pAlien->weapon.deathRay.onUse) { // is it shoot already?
      checkDeathRay(&pAlien->weapon);
    } else {
      if(pAlien->alive) {
        checkTimeToShoot(&pAlien->weapon, &pAlien->pos.New);
      }
    }
    ++pAlien;
  }
}

// in worst case least ~120us; best case ~32us
void checkInVaders(void)
{
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
 
            pAlien->alive = false;     // actually now it dead
            score += SCORE_VAL;        // get cookies
            hudStatus.updScore = true; // update score later
            // check total respawns
            if(--totalRespawns <= 0) { // No more respawns left, all army defeated
              totalRespawns =0;
              replaceTask(checkInVadersRespawn, checkAliveAliens, 4 SEC, true);
            }
          }
          return; // because only one shot can be at one position!
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
  setDeathRayState(&alienBoss.base.weapon.deathRay, &alienBoss.base.pos.New, false);

  alienBoss.base.health = ALIEN_BOSS_HEALTH;
  // move up or move down
  alienBoss.base.bezLine.id = ((RN & 1) ? ALIEN_BOSS_MOVE_UP_ID : ALIEN_BOSS_MOVE_DOWN_ID);
  alienBoss.base.bezLine.step = 25; // start animation at centere
  moveBezierCurve(&alienBoss.base.pos.New, &alienBoss.base.bezLine);

  deathRay_t *pWeapons = &alienBoss.weapons[0];
  for(uint8_t count=0; count < ALIEN_BOSS_DEATH_RAYS; count++) {
    setDeathRayState(&pWeapons->deathRay, &alienBoss.base.pos.New, false);
    ++pWeapons;
  }

  addBossTasks();
}

void moveBossVertical(void)
{
  // move fully controlled by curve and no fix need
  moveBezierCurve(&alienBoss.base.pos.New, &alienBoss.base.bezLine);

  // reach end of move?
  if(alienBoss.base.bezLine.step == bezierLine.totalSteps) {
    // flip direction move
    if(alienBoss.base.bezLine.id == ALIEN_BOSS_MOVE_UP_ID) {
      alienBoss.base.bezLine.id = ALIEN_BOSS_MOVE_DOWN_ID;
    } else {
      alienBoss.base.bezLine.id = ALIEN_BOSS_MOVE_UP_ID;
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
  alienBoss.base.weapon.deathRay.pos.x = RN % ALIEN_SHIP_BOSS_PIC_W + alienBoss.base.pos.Base.x;
  alienBoss.base.weapon.deathRay.pos.y = RN % ALIEN_SHIP_BOSS_PIC_H + alienBoss.base.pos.Base.y;

  rocketEpxlosion(&alienBoss.base.weapon.deathRay);
}

void checkBossDamage(void)
{
  uint8_t countR;
  rocket_t *pRocket = &playerLasers[0];
  inVader_t *pBase = &alienBoss.base;

  if(pBase->alive) {
    for(countR =0; countR < MAX_PEW_PEW; countR++) {
      if(pRocket->onUse) {
        if(checkCollision(&pRocket->pos, LASER_PIC_W, LASER_PIC_H,
              &pBase->pos.Base, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H)) {

          rocketEpxlosion(pRocket);

          if((pBase->health -= ship.states.power) <= 0) {  // alien absorb damage
#if ADD_SOUND
            sfxPlayPattern(enemyDestroyPattern, SFX_CH_0);
#endif
            // if it dead, remove body from battleground
            fillRectFast(pBase->pos.Base.x, pBase->pos.Base.y, ALIEN_SHIP_BOSS_PIC_W, ALIEN_SHIP_BOSS_PIC_H);
 
            pBase->alive = false; // actually now it dead
            score += BOSS_SCORE_VAL;      // get cookies
            hudStatus.updScore = true;    // update score later
            bossDie();                    // was it easy no?
          }
          return;
        }
      }
      ++pRocket;
    }
  }
}

void checkBossFire(void)
{
  inVader_t *pBase = &alienBoss.base;

  if(pBase->weapon.deathRay.onUse) { // is it shoot already?
    if(checkCollision(&pBase->weapon.deathRay.pos, DEATHRAY_BOSS_PIC_W, DEATHRAY_BOSS_PIC_H,
                                    &ship.pos.Base, SHIP_PIC_W, SHIP_PIC_H)) {
      applyShipDamage(&pBase->weapon.deathRay);
      pBase->weapon.timeToShoot = ALIEN_RAND_SHOOT_TIME;
    } else {
      // clear previous shoot
      fillRectFast(pBase->weapon.deathRay.pos.x, pBase->weapon.deathRay.pos.y,
                                   DEATHRAY_BOSS_PIC_W, DEATHRAY_BOSS_PIC_H);
      moveEnemyV(&pBase->weapon.deathRay.pos, ALIEN_BOSS_ROCKET_SPEED_MOVE);
        
      if((pBase->weapon.deathRay.pos.x -= DEATH_RAY_SPEED) < TFT_W) { 
        drawBMP_ERLE_P(pBase->weapon.deathRay.pos.x, pBase->weapon.deathRay.pos.y, deathRayBossPic);
      } else {
        // oooh, we don`t shoot the player, inVeder sooo saaad :(
        pBase->weapon.timeToShoot = ALIEN_RAND_SHOOT_TIME;
        pBase->weapon.deathRay.onUse = false;
      }
    }
  } else {
    checkTimeToShoot(&pBase->weapon, &pBase->pos.New);
  }
}

void checkBossRays(void)
{
  deathRay_t *pWeapons = &alienBoss.weapons[0];

  for(uint8_t count=0; count < ALIEN_BOSS_DEATH_RAYS; count++) {
    if(pWeapons->deathRay.onUse) { // is it shoot already?
      checkDeathRay(pWeapons);
    } else {
      checkTimeToShoot(pWeapons, &alienBoss.base.pos.New);
    }
    ++pWeapons;
  }
}

void bossDie(void)
{
  // boss defeated and it's shot exploded. Why? Just because Simon say: "boom"!
  deathRay_t *pWeapons = &alienBoss.weapons[0];
  for(uint8_t count=0; count < ALIEN_BOSS_DEATH_RAYS; count++) {
    rocketEpxlosion(&pWeapons->deathRay);
    ++pWeapons;
  }
  
  rocketEpxlosion(&alienBoss.base.weapon.deathRay);

  addGiftTasks();
}
