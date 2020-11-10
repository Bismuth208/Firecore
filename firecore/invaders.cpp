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

#include "pics.h"
#include "textProg.h"
#include "common.h"

int8_t  totalRespawns = 0;
uint8_t someCount = 0;
inVader_t aliens[MAX_ALIENS];
inVaderBoss_t alienBoss;

bool bossMoveDirect = false;
bool advancedBoss = false;

//---------------------------------- inVaders --------------------------------------------//
__attribute__ ((optimize("O2"))) void setInvaderValue(inVader_t *pAlien, bool state)
{
  pAlien->bezLine.step = 0; //RN % 10;
  pAlien->bezLine.id = RN % MAX_BEZIER_LINES;
  moveBezierCurve(&pAlien->sprite.pos.New, &pAlien->bezLine);

  pAlien->weapon.timeToShoot = ALIEN_RAND_SHOOT_TIME;
  pAlien->respawnTime = ALIEN_RAND_RESPAWN_TIME;
  pAlien->sprite.pPic = alienShipV2;
  pAlien->health = ALIEN_HEALTH;
  pAlien->alive = state;
}

__attribute__ ((optimize("O2"))) void setDeathRayState(rocket_t *deathRay, position_t *pPos, bool state)
{
  deathRay->onUse = state;
  //deathRay->state = ((RN & 1) ? false : true); // ste of picture
  deathRay->sprite.pos.Old = {pPos->x, pPos->y+5};// +5 is offset to make center align
}

__attribute__ ((optimize("O2"))) void checkTimeToShoot(deathRay_t &pWeapon, position_t &pPos)
{
  if((pWeapon.timeToShoot--) == 0) {  // decrease timeout to shoot; Captain Obvious :)
    setDeathRayState(&pWeapon.ray, &pPos, true);
#if ADD_SOUND
    sfxPlayPattern(enemyShotPattern, SFX_CH_0);
#endif
  }
}

// super fast respawn - aka magic
__attribute__ ((optimize("O2"))) void invadersMagicRespawn(void)
{
  for(auto &alien : aliens) {
    if(!alien.alive) {
      alien.respawnTime = 1;
    }
  }
}

__attribute__ ((optimize("O2"))) void checkDeathRay(deathRay_t &pWeapon)
{
  // check damage from deathRay
  if(checkSpriteCollision(&pWeapon.ray.sprite, &ship.sprite)) {
    applyShipDamage(&pWeapon.ray);
    pWeapon.timeToShoot = ALIEN_RAND_SHOOT_TIME;
  } else {
    // clear previous shoot
    removeSprite(&pWeapon.ray.sprite);
        
    if((pWeapon.ray.sprite.pos.Old.x -= (DEATH_RAY_SPEED+difficultyIncrement)) < TFT_W) {
      pWeapon.ray.sprite.pPic = ((RN & 1) ? deathRayHiPic : deathRayLowPic);

      drawSprite(&pWeapon.ray.sprite);
    } else {
      // oooh, we don`t shoot the player, inVeder sooo saaad :(
      pWeapon.timeToShoot = ALIEN_RAND_SHOOT_TIME;
      pWeapon.ray.onUse = false;
    }
  }
}

//---------------------------------------------------------------------------//
__attribute__ ((optimize("O2"))) void initInvaders(void)
{
  totalRespawns = ALIEN_KILLS_TO_BOSS + difficultyIncrement;

  for(auto &alien : aliens) {
    setInvaderValue(&alien, ((RN & 1) ? false : true));
    setDeathRayState(&alien.weapon.ray, &alien.sprite.pos.New, false);
  }
}
//---------------------------------------------------------------------------//

__attribute__ ((optimize("O2"))) void moveInVaders(void)
{
  for(auto &alien : aliens) {
    if(alien.alive) {
      auto sprite = &alien.sprite;
      moveBezierCurve(&sprite->pos.New, &alien.bezLine);
      fixPosition(&sprite->pos.New);

      if(sprite->pos.New.y > ALIEN_MOVE_ZONE_Y_MAX) {
        sprite->pos.New.y = ALIEN_MOVE_ZONE_Y_MAX;
      }

      if(alien.bezLine.step == bezierLine.totalSteps) {
        alien.alive = false;
        alien.respawnTime = ALIEN_RAND_RESPAWN_TIME;
        removeSprite(sprite);
      }
    }
  }
}

// each invader consume ~6ms
__attribute__ ((optimize("O2"))) void drawInVaders(void)
{
  for(auto &alien : aliens) {
    if(alien.alive) { // ALIIIVEE! IT`S ALIIVEEE!
      updateSprite(&alien.sprite);
      drawPico_DIC_P(alien.sprite.pos.Old.x+28, alien.sprite.pos.Old.y+6,
                   ((RN & 1) ? alienShipFireHi : alienShipFireLow));
    }
  }
}

__attribute__ ((optimize("O2"))) void checkInVadersRespawn(void)
{
  for(auto &alien : aliens) {
    if(!alien.alive)
      if((alien.respawnTime--) == 0)
        setInvaderValue(&alien, true); // make it alive, and update params
  }
}

//calc how much aliens on screen, if 0 then call Boss!
__attribute__ ((optimize("O2"))) void checkAliveAliens(void)
{
  uint8_t deadAliens=0;

  for(auto &alien : aliens) {
    if(!alien.alive) ++deadAliens;
  }

  if(deadAliens == MAX_ALIENS) {
    addBossTasks(); // No more respawns left, all army defeated
  }
}

__attribute__ ((optimize("O2"))) void checkInVadersRay(void)
{
  for(auto &alien : aliens) {
    if(alien.weapon.ray.onUse) { // is it shoot already?
      checkDeathRay(alien.weapon);
    } else {
      if(alien.alive) {
        checkTimeToShoot(alien.weapon, alien.sprite.pos.New);
      }
    }
  }
}

// in worst case least ~120us; best case ~32us
__attribute__ ((optimize("O2"))) void checkInVaders(void)
{
  // recalc all collisions between player's rockets and each alien
  // i.e. rounds = MAX_PEW_PEW x MAX_ALIENS (in worst case of course)
  for(auto &laser : ship.weapon.lasers) {
    if(laser.onUse) {
      for(auto &alien : aliens) {
        if(alien.alive) {
          if(checkSpriteCollision(&laser.sprite, &alien.sprite)) {

            rocketEpxlosion(&laser);
              
            if((alien.health -= ship.states.power) <= 0) { // alien absorb damage
  #if ADD_SOUND
              sfxPlayPattern(enemyDestroyPattern, SFX_CH_0);
  #endif
              // if it dead, remove body from battleground
              removeSprite(&alien.sprite);
   
              alien.alive = false;       // actually now it dead
              score += SCORE_VAL;        // get cookies
              // check total respawns
              if(--totalRespawns <= 0) { // No more respawns left, all army defeated
                totalRespawns =0;
                vTSMReplaceTask(checkInVadersRespawn, checkAliveAliens, 8 SEC, true);
                drawText(40, 40, 2, bossWarningP);
              }
            }
            break; // exit from aliens loop to ship.weapon.lasers loop
          }
        }
      }
    }
  }
}

//---------------------------------------------------------------------------//
__attribute__ ((optimize("O2"))) void checkInVadersCollision(void)
{
  decltype(aliens[0].weapon.ray) gopher; // Do you see it? No? But it's exist! 

  for(auto &alien : aliens) {
    if(alien.alive) {
      if(checkSpriteCollision(&ship.sprite, &alien.sprite)) {

        gopher.sprite.pos.Old = alien.sprite.pos.Old;
        rocketEpxlosion(&gopher); // now make gopher to explode \(^_^)/
        removeSprite(&alien.sprite);

        alien.alive = false;      // actually now it dead, again...
        score -= SCORE_PENALTY;   // lost my treasure!
        if(score < 0) score = 0;
      }
    }
  }
}

//---------------------------------------------------------------------------//
__attribute__ ((optimize("O2"))) void bossInit(void)
{
  auto pAlien = &alienBoss.base;

  setInvaderValue(pAlien, true);
  setDeathRayState(&pAlien->weapon.ray, &pAlien->sprite.pos.New, false);
  pAlien->weapon.ray.sprite.pPic = deathRayBossPic;

  pAlien->health = ALIEN_BOSS_HEALTH;
  pAlien->sprite.pPic = advancedBoss ? bossShip : ship_boss_v2;

  // move up or move down
  pAlien->bezLine.id = ((RN & 1) ? ALIEN_BOSS_MOVE_UP_ID : ALIEN_BOSS_MOVE_DOWN_ID);
  pAlien->bezLine.step = 25; // start animation at centere
  moveBezierCurve(&pAlien->sprite.pos.New, &pAlien->bezLine);

  advancedBoss = !advancedBoss; // change pic every 2nd lvl boss

  for(auto &weapon : alienBoss.weapons)
    setDeathRayState(&weapon.ray, &pAlien->sprite.pos.New, false);
}

__attribute__ ((optimize("O2"))) void moveBossVertical(void)
{
  auto pAlien = &alienBoss.base;

  // move fully controlled by curve and no fix need
  moveBezierCurve(&pAlien->sprite.pos.New, &pAlien->bezLine);

  // reach end of move?
  if(pAlien->bezLine.step == bezierLine.totalSteps) {
    // flip direction move
    if(pAlien->bezLine.id == ALIEN_BOSS_MOVE_UP_ID) {
      pAlien->bezLine.id = ALIEN_BOSS_MOVE_DOWN_ID;
    } else {
      pAlien->bezLine.id = ALIEN_BOSS_MOVE_UP_ID;
    }
    pAlien->bezLine.step = 0;
  }
}

__attribute__ ((optimize("O2"))) void drawBoss(void)
{
  if(alienBoss.base.alive) { // ALIIIVEE! IT`S ALIIVEEE!
    updateSprite(&alienBoss.base.sprite);
  }
}

__attribute__ ((optimize("O2"))) void drawBossExplosion(void)
{
  // reuse death ray
  auto pAlien = &alienBoss.base;
  auto pPos = &pAlien->weapon.ray.sprite.pos.Old;
  pPos->x = RN % ALIEN_SHIP_BOSS_PIC_W + pAlien->sprite.pos.Old.x;
  pPos->y = RN % ALIEN_SHIP_BOSS_PIC_H + pAlien->sprite.pos.Old.y;

  rocketEpxlosion(&pAlien->weapon.ray);
}

__attribute__ ((optimize("O2"))) void checkBossDamage(void)
{
  auto pAlien = &alienBoss.base;

  if(pAlien->alive) {
    for(auto &laser : ship.weapon.lasers) {
      if(laser.onUse) {
        if(checkSpriteCollision(&laser.sprite, &pAlien->sprite)) {

          rocketEpxlosion(&laser);

          if((pAlien->health -= ship.states.power) <= 0) {  // alien absorb damage
#if ADD_SOUND
            sfxPlayPattern(enemyDestroyPattern, SFX_CH_0);
#endif
            // if it dead, remove body from battleground
            removeSprite(&pAlien->sprite);
 
            pAlien->alive = false;     // actually now it dead
            score += BOSS_SCORE_VAL;   // get cookies
            bossDie();                 // was it easy no?
          }
          return;
        }
      }
    }
  }
}

__attribute__ ((optimize("O2"))) void checkBossFire(void)
{
  auto pAlien = &alienBoss.base;

  if(pAlien->weapon.ray.onUse) { // is it shoot already?
    if(checkSpriteCollision(&pAlien->weapon.ray.sprite, &ship.sprite)) {
      applyShipDamage(&pAlien->weapon.ray);
      pAlien->weapon.timeToShoot = ALIEN_RAND_SHOOT_TIME;
    } else {
      // clear previous shoot
      removeSprite(&pAlien->weapon.ray.sprite);
      moveEnemyV(&pAlien->weapon.ray.sprite.pos.Old, ALIEN_BOSS_ROCKET_SPEED_MOVE);
        
      if((pAlien->weapon.ray.sprite.pos.Old.x -= DEATH_RAY_SPEED) < TFT_W) { 
        drawSprite(&pAlien->weapon.ray.sprite);
      } else {
        // oooh, we don`t shoot the player, inVeder sooo saaad :(
        pAlien->weapon.timeToShoot = ALIEN_RAND_SHOOT_TIME;
        pAlien->weapon.ray.onUse = false;
      }
    }
  } else {
    checkTimeToShoot(pAlien->weapon, pAlien->sprite.pos.New);
  }
}

__attribute__ ((optimize("O2"))) void checkBossRays(void)
{
  for(auto &pWeapon : alienBoss.weapons) {
    if(pWeapon.ray.onUse) { // is it shoot already?
      checkDeathRay(pWeapon);
    } else {
      checkTimeToShoot(pWeapon, alienBoss.base.sprite.pos.New);
    }
  }
}

__attribute__ ((optimize("O2"))) void bossDie(void)
{
  bool invertState = true;

  for(uint8_t i=0; i<3; i++) {
    // boss defeated and it's shot exploded. Why? Just because Simon say: "boom"!
    for(auto &pWeapon : alienBoss.weapons) {
      tftSetInvertion(invertState);
      rocketEpxlosion(&pWeapon.ray);

      invertState = !invertState;
    }
    rocketEpxlosion(&alienBoss.base.weapon.ray);
  }
  tftSetInvertion(false);
  addGiftTasks();
}
