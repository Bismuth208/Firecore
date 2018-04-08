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


asteroid_t asteroids[MAX_ASTEROIDS];
uint8_t asteroidsToDefeat =0;

//---------------------------------------------------------------------------//
void setAsteroidValue(asteroid_t &asteroid)
{
  asteroid.onUse = (RN & 1);
  asteroid.speed = RN % ASTEROID_STEP + 1;
  asteroid.sprite.pos.New = {TFT_W-10, RN % ASTEROID_MAX_POS_Y};
  asteroid.type = RN % 3;
  asteroid.sprite.pPic = getConstCharPtr(asteroidsPics, asteroid.type);
}

void initAsteroids(void)
{
  for(auto &asteroid : asteroids) {
    setAsteroidValue(asteroid);
  }

  asteroidsToDefeat = ASTEROIDS_TO_DEFEAT;
}

void moveAsteroids(void)
{
  for(auto &asteroid : asteroids) {
    if(asteroid.onUse) {
      updateSprite(&asteroid.sprite);

      if((asteroid.sprite.pos.New.x -= asteroid.speed) > TFT_W) {
        //asteroid.onUse = false;
        removeSprite(&asteroid.sprite); // moveSprite ?
        setAsteroidValue(asteroid);
      }
    }
  }
}

void respawnAsteroids(void)
{
  for(auto &asteroid : asteroids) {
    if(!asteroid.onUse) {
      if((--asteroidsToDefeat) == 0) {
        createNextLevel();
      } else {
        setAsteroidValue(asteroid);
      }
    }
  }
}

void checkAsteroids(void)
{
  bool collision = false;
  rocket_t hamster;

  for(auto &laser : ship.weapon.lasers) {
    for(auto &asteroid : asteroids) {
      if(asteroid.onUse) {

        if(laser.onUse) {
          if(checkSpriteCollision(&laser.sprite, &asteroid.sprite)) {
            collision = true;
            laser.onUse = false;
            removeSprite(&laser.sprite);
            score += (SCORE_VAL>>1);        // get half a cookie
          }
        } else {
          if(checkSpriteCollision(&ship.sprite, &asteroid.sprite)) {
            collision = true;          
            score -= SCORE_PENALTY;   // lost my treasure! Again!
            if(score < 0) score = 0;
          }
        }

        if(collision) {
          collision = false;
          hamster.sprite.pos.Old = asteroid.sprite.pos.Old;
          rocketEpxlosion(&hamster); // exploding hamster! Yay! \(^_^)/
          removeSprite(&asteroid.sprite);

          if(asteroid.type > 0) { // can devide by smaller piece?
            --asteroid.type;
            asteroid.sprite.pPic = getConstCharPtr(asteroidsPics, asteroid.type);
          } else { // nope it's already like a dust!
            asteroid.onUse = false;
          }

          break; // exit asteroids loop to next laser
        }
      }      
    }
  }
}
