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

star_t stars[MAX_STARS] = {{0, 0}, 0, 0};
bool startState = true;

const uint8_t *pTextDialoge = NULL;
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
    updateTaskStatus(printHistory, false);
  }
}

void printDialogeText(void)
{
  uint8_t tmpChar = pgm_read_byte(pTextDialoge);
  if(tmpChar != '\0') {
    ++pTextDialoge;
    // 6 is width of font is size will == 1
    tftPrintCharAt(TEXT_WINDOW_X + (textDialogePosX*6), TEXT_WINDOW_Y, tmpChar);
    ++textDialogePosX;
#if ADD_SOUND
    sfxPlayPattern(beepPattern, SFX_CH_0);
#endif
  } else {
    // all text printed
    updateTaskStatus(printDialogeText, false);
    pTextDialoge = NULL;
    textDialogePosX =0;
  }
}

void drawTextWindow(const uint8_t *text, const uint8_t *btnText)
{
  drawFrame(TEXT_FRAME_X, TEXT_FRAME_Y,  TEXT_FRAME_W, TEXT_FRAME_H, INDIGO_COLOR, COLOR_WHITE);
  //drawBMP_ERLE_P(TEXT_FRAME_X, TEXT_FRAME_Y, textWindowPic);
  drawText(TEXT_OK_X, TEXT_OK_Y, 1, btnText);

  pTextDialoge = text; // draw this text later
  textDialogePosX =0;  // reset position
  updateTaskStatus(printDialogeText, true); // It is so epic retro !!!!
}


//---------------------------------------------------------------------------//
void drawText(uint8_t posX, uint8_t posY, uint8_t textSize, const uint8_t *pText)
{
  tftSetTextSize(textSize);
  tftSetTextColor(COLOR_WHITE);
  tftPrintAt_P(posX, posY, (const char *)pText);
}
//---------------------------------------------------------------------------//

void drawFrame(uint8_t posX, uint8_t posY, uint8_t w, uint8_t h, uint16_t clr1, uint16_t clr2)
{
  tftFillRect(posX, posY, w, h, clr1);          // Frame 0
  tftDrawRect(posX+1, posY+1, w-2, h-2, clr2);  // Frame 1
}
//---------------------------------------------------------------------------//
// Arrrrrghh!!!!
// How to make it faster?!?!
void drawStars(void)
{
  // draw stars and blow your mind, if still not
  for(auto &pStar: stars) {
    drawPixelFast(&pStar.pos, currentBackGroundColorId);
    
    // now move them
    if((pStar.pos.x -= pStar.speed) < TFT_W) {
      drawPixelFast(&pStar.pos, pStar.color);
    } else {
      pStar.pos = {TFT_W, RN % STARS_MAX_POS_Y};
      pStar.color = RAND_STAR_CLR;
      pStar.speed = RN % STAR_STEP + 1;
    }
  }
}
// --------------------------------------------------------------- //

void rocketEpxlosion(rocket_t *pRocket)
{
  // Please, don`t say anything about this...

  pRocket->onUse = false;
  uint16_t posX = pRocket->pos.x;
  uint16_t posY = pRocket->pos.y;

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

  //drawBMP_ERLE_P(posX, posY, explosion1);
#if ADD_SOUND
  sfxPlayPattern(enemyHitPattern, SFX_CH_1); // veeery tyny delay
#endif
  //drawBMP_ERLE_P(posX, posY, explosion2);

  //fillRectFast(posX, posY, EXPLOSION_PIC_WH, EXPLOSION_PIC_WH);
}
// --------------------------------------------------------------- //

// draws in ~3.6 - 4.7ms
void drawShip(void)
{
  ship.flameState = !ship.flameState;

  drawEnemy(&ship.pos, ship.pBodyPic);
  drawBMP_ERLE_P(ship.pos.Base.x, ship.pos.Base.y+SHIP_FLAME_OFFSET_Y,
                    (ship.flameState ? flameFireHiPic : flameFireLowPic));
}

void shipHyperJump(void)
{
  while((ship.pos.Base.x++) < SHIP_MAX_POS_X) {
    // this pic used to left red track on screen
    drawBMP_ERLE_P(ship.pos.Base.x, ship.pos.Base.y, ship.pBodyPic);
    drawBMP_ERLE_P(ship.pos.Base.x, ship.pos.Base.y+SHIP_FLAME_OFFSET_Y, flameFireHiPic);
  }
  movePicture(&ship.pos, ship.pBodyPic); // remove ship from screen
}

void drawShipExplosion(void)
{
  auto pRocket = &ship.lasers[0]; // reuse

  pRocket->pos.x = (RN & (SHIP_PIC_W-1)) + ship.pos.Base.x;
  pRocket->pos.y = (RN & (SHIP_PIC_H-1)) + ship.pos.Base.y;
  
  rocketEpxlosion(pRocket);
#if ADD_SOUND
  sfxPlayPattern(playerDestroyPattern, SFX_CH_0);
#endif
}

void drawPlayerRockets(void)
{
  for(auto &pRocket: ship.lasers) {
    if(pRocket.onUse) {
      // remove previous rocket image
      fillRectFast(pRocket.pos.x, pRocket.pos.y, LASER_PIC_W, LASER_PIC_H);
      
      if(((pRocket.pos.x += PLAYER_ROCKET_SPEED) + LASER_PIC_W) <= TFT_W) {
        drawBMP_ERLE_P(pRocket.pos.x, pRocket.pos.y, ship.weapon.pPic);
      } else {
        pRocket.onUse = false;
      }
    }
  }
}
// --------------------------------------------------------------- //

void drawGift(void)
{
  drawEnemy(&gift.pos, gift.pPic);
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
  drawBMP_ERLE_P(TITLE_PIC_POS_X, TITLE_PIC_POS_Y, titleTextPic);
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
    drawBMP_ERLE_P(titleRowLPosX, PIC_ROW_L_POS_Y, rowsLeftPic);
    drawBMP_ERLE_P(titleRowRPosX, PIC_ROW_R_POS_Y, rowsRightPic);
  }
}
// --------------------------------------------------------------- //

// hardware scrolling; blocks everything
void screenSliderEffect(uint16_t color)
{
  for(int16_t i = TFT_W; i >= 0; i--) {
    tftScrollSmooth(1, i-1, 2); // 2 - is delay in ms
    tftDrawFastVLine(TFT_W-i-1, 0, TFT_H, color);
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

// --------------------------------------------------------------- //
void drawEnemy(objPosition_t *pEnemy, pic_t *pPic)
{
  movePicture(pEnemy, pPic);
  drawBMP_ERLE_P(pEnemy->Base.x, pEnemy->Base.y, pPic);
}
// --------------------------------------------------------------- //
void fillRectFast(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  // -1 == convert to display addr size
  tftSetAddrWindow(x, y, x+w-1, y+h-1);
  uint16_t dataSize = w*h;

  do {
#ifdef __AVR__  // really dirt trick... but... FOR THE PERFOMANCE!
    SPDR_t in = {.val = currentBackGroundColor};
    SPDR = in.msb;
    SPDR_TX_WAIT;
    SPDR = in.lsb;
    SPDR_TX_WAIT;
#else
    pushColorFast(currentBackGroundColor);
#endif
  } while(--dataSize);
}

void drawPixelFast(position_t *pPos, uint8_t colorId)
{
  tftSetAddrPixel(pPos->x, pPos->y);
  pushColorFast(palette_RAM[colorId]);
}
