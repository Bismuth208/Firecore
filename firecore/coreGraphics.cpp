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

//---------------------------------------------------------------------------//
// to make cool unfold animation
uint8_t titleRowRPosX = PIC_TITLE_R_BASE_X;
uint8_t titleRowLPosX = PIC_TITLE_L_BASE_X;

star_t stars[MAX_STARS];
bool startState = true;

text_t *pTextDialoge = nullptr;
uint8_t textDialogePosX =0;
uint8_t textHistoryPosX =0;

//---------------------------------------------------------------------------//
void printHistory(void)
{
  uint8_t tmpChar = pgm_read_byte(historyTextP + textHistoryPosX);
  if(tmpChar != '\0') {
    tftPrintChar(tmpChar);
    ++textHistoryPosX;
#if ADD_SOUND
    sfxPlayPattern(beepPattern, SFX_CH_0);
#endif
  } else {
    disableTask(printHistory);
  }
}

void printDialogeText(void)
{
  uint8_t tmpChar = pgm_read_byte(pTextDialoge);
  if(tmpChar != '\0') {
    ++pTextDialoge;
    // 6 is width of font is size will == 1
    //tftPrintCharAt(TEXT_WINDOW_X + (textDialogePosX*6), TEXT_WINDOW_Y, tmpChar);
    tftPrintChar(tmpChar);
    ++textDialogePosX;
#if ADD_SOUND
    sfxPlayPattern(beepPattern, SFX_CH_0);
#endif
  } else {
    // all text printed
    disableTask(printDialogeText);
    pTextDialoge = nullptr;
    textDialogePosX =0;
  }
}

void drawTextWindow(text_t *text, text_t *btnText)
{
  // drawFrame(TEXT_FRAME_X, TEXT_FRAME_Y,  TEXT_FRAME_W, TEXT_FRAME_H, INDIGO_COLOR, COLOR_WHITE);
  drawPico_DIC_P(TEXT_FRAME_X, TEXT_FRAME_Y, textWindowPic);
  drawText(TEXT_OK_X, TEXT_OK_Y, 1, btnText);

  pTextDialoge = text; // draw this text later
  textDialogePosX =0;  // reset position
  tftSetCursor(TEXT_WINDOW_X, TEXT_WINDOW_Y);
  enableTask(printDialogeText); // It is so epic retro !!!!
}

//---------------------------------------------------------------------------//
// Arrrrrghh!!!!
// How to make it faster?!?!
void drawStars(void)
{
  // draw stars and blow your mind, if still not
  for(auto &star : stars) {
    drawPixelFast(&star.pos, getAlphaReplaceColorId());
    
    // now move them
    if((star.pos.x -= star.speed) < TFT_W) {
      drawPixelFast(&star.pos, star.color);
    } else {
      star.pos = {TFT_W, RN % STARS_MAX_POS_Y};
      star.color = RAND_STAR_CLR;
      star.speed = RN % STAR_STEP + 1;
    }
  }
}
// --------------------------------------------------------------- //

void drawSomeGUI(void)
{
  drawPico_DIC_P(0, 119, hudGuiPic);
  
  char buf[5]; // i'm pretty sure what 5 bytes will be enough...
  tftSetTextSize(1);
  tftFillRect(SCORE_POS_X, SCORE_POS_Y, 20, 7, currentBackGroundColor);
  tftPrintAt(SCORE_POS_X, SCORE_POS_Y, itoa(score, buf, 10));

  if(ship.health) {
    tftFillRect(SHIP_ENERGY_POS_X, SHIP_ENERGY_POS_Y, (ship.health>>2) - 4, SHIP_ENERGY_H, COLOR_WHITE);
  }
}
// --------------------------------------------------------------- //

void rocketEpxlosion(rocket_t *pRocket)
{
  // Please, don`t say anything about this...

  pRocket->onUse = false;
  uint8_t posX = pRocket->sprite.pos.Old.x;
  uint8_t posY = pRocket->sprite.pos.Old.y;

  for(uint8_t i = 0; i < 5; i++) { // base formation
    tftFillCircle(posX, posY, i*2, COLOR_WHITE);
    tftDrawCircle(posX, posY, i*2, COLOR_ORANGE);
  }

  for(uint8_t i = 5; i > 0; i--) { // something ?
    tftDrawCircle(posX, posY, i*2, COLOR_YELLOW);
  }

  for(uint8_t i = 0; i < 7; i++) { // remove smoke
    tftFillCircle(posX, posY, i*2, currentBackGroundColor);
  }

  //drawPico_DIC_P(posX, posY, explosion1);
#if ADD_SOUND
  sfxPlayPattern(enemyHitPattern, SFX_CH_1); // veeery tyny delay
#endif
  //drawPico_DIC_P(posX, posY, explosion2);

  //fillRectFast(posX, posY, EXPLOSION_PIC_WH, EXPLOSION_PIC_WH);
}

// void rocketEpxlosion(rocket_t *pRocket)
// {
//   // Please, don`t say anything about this...

//   pRocket->onUse = false;
//   uint8_t posX = pRocket->sprite.pos.Old.x;
//   uint8_t posY = pRocket->sprite.pos.Old.y;

//   for(uint8_t i = 0; i < 10; i++) { // base formation
//     drawPico_DIC_P(posX, posY, explosion1);
//   }

// #if ADD_SOUND
//   sfxPlayPattern(enemyHitPattern, SFX_CH_1); // veeery tyny delay
// #endif

//   for(uint8_t i = 20; i > 0; i--) { // something ?
//     drawPico_DIC_P(posX, posY, explosion2);
//   }

//   fillRectFast(&pRocket->sprite.pos.Old, explosion2);
// }
// --------------------------------------------------------------- //

// draws in ~3.6 - 4.7ms
void drawShip(void)
{
  ship.flameState = !ship.flameState;

  updateSprite(&ship.sprite);
  drawPico_DIC_P(ship.sprite.pos.Old.x, ship.sprite.pos.Old.y+SHIP_FLAME_OFFSET_Y,
                    (ship.flameState ? flameFireHiPic : flameFireLowPic));
}

void shipHyperJump(void)
{
  auto sprite = &ship.sprite;

  while((sprite->pos.Old.x++) < SHIP_MAX_POS_X) {
    // this pic used to left red track on screen
    drawSprite(sprite);
    drawPico_DIC_P(sprite->pos.Old.x, sprite->pos.Old.y+SHIP_FLAME_OFFSET_Y, flameFireHiPic);
  }
  moveSprite(sprite); // remove ship from screen
}

void drawShipExplosion(void)
{
  auto pLaser = &ship.weapon.lasers[0]; // reuse

  pLaser->sprite.pos.Old.x = (RN & (SHIP_PIC_W-1)) + ship.sprite.pos.Old.x;
  pLaser->sprite.pos.Old.y = (RN & (SHIP_PIC_H-1)) + ship.sprite.pos.Old.y;
  
  rocketEpxlosion(pLaser);
#if ADD_SOUND
  sfxPlayPattern(playerDestroyPattern, SFX_CH_0);
#endif
}

void drawPlayerWeapon(void)
{
  for(auto &laser : ship.weapon.lasers) {
    if(laser.onUse) {
      // remove previous rocket image
      removeSprite(&laser.sprite);
      
      if(((laser.sprite.pos.Old.x += PLAYER_ROCKET_SPEED) + LASER_PIC_W) <= TFT_W) {
        drawSprite(&laser.sprite);
      } else {
        laser.onUse = false;
      }
    }
  }
}
// --------------------------------------------------------------- //

void drawGift(void)
{
  updateSprite(&gift.sprite);
}
// --------------------------------------------------------------- //

void drawStart(void)
{
  startState = !startState;
  tftSetTextColor(startState ? COLOR_WHITE : currentBackGroundColor);
  tftPrintAt_P(START_TEXT_POS_X, START_TEXT_POS_Y, (const char *)pressAtext);

  tftSetTextColor(0x020C);
  tftPrintAt_P(0, 120, (const char *)versionP0);
  tftPrintAt_P(64, 120, (const char *)creditP0);
}

void drawTitleText(void)
{
  drawPico_DIC_P(TITLE_PIC_POS_X, TITLE_PIC_POS_Y, titleTextPic);
}

// make unfold animation
void drawRows(void)
{
  titleRowLPosX -= PIC_ROWS_STEP;
  titleRowRPosX += PIC_ROWS_STEP;
  
  if((titleRowLPosX <= PIC_ROW_L_POS_X ) || ( titleRowRPosX >= PIC_ROW_R_POS_X )) {
    
    titleRowLPosX = PIC_TITLE_L_BASE_X;
    titleRowRPosX = PIC_TITLE_R_BASE_X;
    
    addTitleTasks();
  } else {
    drawPico_DIC_P(titleRowLPosX, PIC_ROW_L_POS_Y, rowsLeftPic);
    drawPico_DIC_P(titleRowRPosX, PIC_ROW_R_POS_Y, rowsRightPic);
  }
}
// ------------------------------------------ //

void moveBezierCurve(position_t *pPos, bezierLine_t *pItemLine)
{
  auto pLine = &bezierLine;

  // init Bezier line
  getBezierCurve(pItemLine->id);

  if((++pItemLine->step) > pLine->totalSteps) { // curent point position
    pItemLine->step = 0;
  }

  // P0 - start point
  // P1 - angle-distance point
  // P2 - end point
  // t  - number of step betwen P0 and P3
  // B = ((1.0 - t)^2)P0 + 2t(1.0 - t)P2 + (t^2)P3
  // t [>= 0 && <= 1]
  float t = ((float)pItemLine->step)/((float)pLine->totalSteps);
  pPos->x = (1.0 - t)*(1.0 - t)*pLine->P0.x + 2*t*(1.0 - t)*pLine->P1.x + t*t*pLine->P2.x;
  pPos->y = (1.0 - t)*(1.0 - t)*pLine->P0.y + 2*t*(1.0 - t)*pLine->P1.y + t*t*pLine->P2.y;
}

void fixPosition(position_t *pPos)
{
  // reduce power of move effect
  pPos->x -= (pPos->x >> 4);
}

void getBezierCurve(uint8_t line)
{
  memcpy_P(&bezierLine, &lineCurves[line*sizeof(bezier_t)], sizeof(bezier_t));
}
