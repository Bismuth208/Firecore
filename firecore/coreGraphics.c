#include <esploraAPI.h>

#include "taskmanager.h"

#include "pics.h"
#include "textProg.h"
#include "common.h"

//---------------------------------------------------------------------------//
// to make cool unfold animation
uint8_t titleRowRPosX = PIC_TITLE_R_BASE_X;
uint8_t titleRowLPosX = PIC_TITLE_L_BASE_X;

stars_t stars[MAX_STARS];
bool startState = true;

//---------------------------------------------------------------------------//
void printTextSlow(const uint8_t *text)
{
  uint8_t tmpChar;
  while((tmpChar=pgm_read_byte(text++)) != '\0') {
    tftPrintChar(tmpChar);
#if ADD_SOUND
    if(soundEnable) toneBuzz(1200, 20);
#endif
    _delayMS(40); // make retro effect
  }
}

void drawTextWindow(const uint8_t *text, const uint8_t *btnText)
{
  tftSetTextSize(1);
  tftSetTextColor(COLOR_WHITE);
  tftSetCursor(TEXT_WINDOW_X, TEXT_WINDOW_Y);

  drawFrame(TEXT_FRAME_X, TEXT_FRAME_Y,  TEXT_FRAME_W, TEXT_FRAME_H, INDIGO_COLOR, COLOR_WHITE);
  printTextSlow(text); // It is so epic retro !!!!
  
  tftPrintAt_P(TEXT_OK_X, TEXT_OK_Y, (const char *)btnText);
}
//---------------------------------------------------------------------------//

void drawFrame(uint16_t posX, uint16_t posY, uint8_t w, uint8_t h, uint16_t clr1, uint16_t clr2)
{
  tftFillRect(posX, posY, w, h, clr1);          // Frame 0
  tftDrawRect(posX+1, posY+1, w-2, h-2, clr2);  // Frame 1
}
//---------------------------------------------------------------------------//

void drawStars(void)
{
  // Arrrrrghh!!!!
  // How to make it faster?!?!

  stars_t *pStars = &stars[0];

  // draw stars and blow your mind, if still not
  for(uint8_t count=0; count < MAX_STARS; count++) {
    fillRectFast(pStars->pos.x, pStars->pos.y, 1, 1); // clear previous star
    
    // now move them
    if((pStars->pos.x -= pStars->speed) < TFT_W) {
      drawPixelFast(pStars->pos.x, pStars->pos.y, pStars->color);
    } else {
      pStars->pos.x = TFT_W;
      pStars->pos.y = RN % STARS_MAX_POS_Y;
      pStars->color = RAND_STAR_CLR;
      pStars->speed = RN % STAR_STEP + 1;
    }

    ++pStars;
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
#if ADD_SOUND
    if(soundEnable) toneBuzz(200*i, 2);
#endif
  }

  for(uint8_t i = 5; i > 0; i--) { // something ?
    tftDrawCircle(posX, posY, i*2, COLOR_YELLOW);
#if ADD_SOUND
    if(soundEnable)  toneBuzz(100*i, 2);
#endif
  }

  for(uint8_t i = 0; i < 7; i++) { // remove smoke
    tftFillCircle(posX, posY, i*2, currentBackGroundColor);
#if ADD_SOUND
    if(soundEnable) toneBuzz(60*i, 2);
#endif
  }
}
// --------------------------------------------------------------- //

void drawShip(void)
{
  const uint8_t *pic = (shipState ? shipBaseHi : shipBaseLow);
  uint16_t picSize = (shipState ? SHIP_BASE_HI_PIC_SIZE : SHIP_BASE_LOW_PIC_SIZE);
  
  shipState = !shipState;
  drawEnemy(&ship.pos, SHIP_PIC_W, SHIP_PIC_H, pic, picSize);
}

void shipHyperJump(void)
{
  while((ship.pos.Base.x++) < SHIP_MAX_POS_X) {
    // this pic used for left red track on screen
    drawBMP_RLE_P(ship.pos.Base.x, ship.pos.Base.y, SHIP_PIC_W, SHIP_PIC_H, 
                                          shipBaseLow, SHIP_BASE_LOW_PIC_SIZE);
  }
  movePicture(&ship.pos, SHIP_PIC_W, SHIP_PIC_H); // remove ship from screen
}

void drawShipExplosion(void)
{
  rocket_t *pRocket = pRocketGlobal; // reuse

  pRocket->pos.x = RN % SHIP_PIC_W + ship.pos.Base.x;
  pRocket->pos.y = RN % SHIP_PIC_H + ship.pos.Base.y;
  
  rocketEpxlosion(pRocket);
}

void drawPlayerRockets(void)
{
  rocket_t *pRocket = pRocketGlobal;

  for(uint8_t count =0; count < MAX_PEW_PEW; count++) {
    if(pRocket->onUse) {
      // remove previous rocket image
      fillRectFast(pRocket->pos.x, pRocket->pos.y, LASER_PIC_W, LASER_PIC_H);
      
      if(((pRocket->pos.x += PLAYER_ROCKET_SPEED) + LASER_PIC_W) <= TFT_W) {
        drawBMP_RLE_P(pRocket->pos.x, pRocket->pos.y,
                                  LASER_PIC_W, LASER_PIC_H,
                                  ship.weapon.pPic, ship.weapon.picSize);
      } else {
        pRocket->onUse = false;
      }
    }
    ++pRocket;
  }
}
// --------------------------------------------------------------- //

void drawGift(void)
{
  drawEnemy(&gift.pos, GIFT_PIC_W, GIFT_PIC_H, gift.pPic, gift.picSize);
}
// --------------------------------------------------------------- //

void drawStart(void)
{
  if(startState) {
    tftSetTextColor(COLOR_WHITE);
  } else {
    tftSetTextColor(currentBackGroundColor);
  }
  startState = !startState;
  tftPrintAt_P(START_TEXT_POS_X, START_TEXT_POS_Y, (const char *)pressAtext);

  tftSetTextColor(0x020C);
  tftPrintAt_P(0, 120, (const char *)versionP0);
  tftPrintAt_P(64, 120, (const char *)creditP0);
}

void drawTitleText(void)
{
  drawBMP_RLE_P(TITLE_PIC_POS_X, TITLE_PIC_POS_Y,
                  TEXT_TITLE_HI_W, TEXT_TITLE_HI_H, titleTextPic, TEXT_TITLE_HI_SIZE);
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
    drawTitleText();
  } else {
    drawBMP_RLE_P(titleRowLPosX, PIC_ROW_L_POS_Y,
                    PIC_TITLE_ROW_WH, PIC_TITLE_ROW_WH,
                      rowsLeftPic, ROWS_L_DATA_PIC_SIZE);
    
    drawBMP_RLE_P(titleRowRPosX, PIC_ROW_R_POS_Y,
                    PIC_TITLE_ROW_WH, PIC_TITLE_ROW_WH,
                      rowsRightPic, ROWS_R_DATA_PIC_SIZE);
  }
}
// --------------------------------------------------------------- //

// hardware scrolling; blocks everything
void screenSliderEffect(uint16_t color)
{
  for(int16_t i = TFT_W; i >= 0; i--) {
    tftScrollSmooth(1, i-1, 4);
    tftDrawFastVLine(TFT_W-i-1, 0, TFT_H, color);
  }
}

// --------------------------------------------------------------- //
void drawEnemy(objPosition_t *pEnemy, uint8_t w, uint8_t h, const uint8_t *pPic, uint16_t picSize)
{
  movePicture(pEnemy, w, h);
  drawBMP_RLE_P(pEnemy->Base.x, pEnemy->Base.y, w, h, pPic, picSize);
}
// --------------------------------------------------------------- //
void fillRectFast(int16_t x, int16_t y, uint8_t w, uint8_t h)
{
  // -1 == convert to display addr size
  tftSetAddrWindow(x, y, x+w-1, y+h-1);
  uint16_t dataSize = w*h;

  while(dataSize--) {
    pushColorFast(currentBackGroundColor);
  }
}

void drawPixelFast(int16_t x, int16_t y, uint8_t colorId)
{
  tftSetAddrWindow(x, y, x+1, y+1);
  pushColorFast(getPicWord(nesPalette_ext, colorId));
}

void drawBMP_RLE_P(int16_t x, int16_t y, uint8_t w, uint8_t h,
                              const uint8_t *pPic, int16_t sizePic)
{
  // This is used
  // when need maximum pic compression,
  // and you can waste some CPU resources for it;
  // It use very simple RLE compression;
  // Also draw background color;
  
  uint16_t repeatColor;
  uint8_t tmpInd, repeatTimes;
  
  tftSetAddrWindow(x, y, x+w-1, y+h-1); // -1 == convert to display addr size
  
  while(sizePic--) {  // compressed pic size!
    // get color index or repeat times
    tmpInd = getPicByte(pPic);
    
    if(~tmpInd & 0x80) { // is it color index?
      repeatTimes = 1;
    } else {   // nope, just repeat color
      repeatTimes = tmpInd - 0x80;
      // get previous color index to repeat
      tmpInd = getPicByte(pPic-1);
    }
    
    // get color from colorTable by tmpInd color index
    repeatColor = getPicWord(nesPalette_ext, tmpInd);
    if(repeatColor == replaceColor) {
      repeatColor = currentBackGroundColor;
    }

    do {
      --repeatTimes;
      pushColorFast(repeatColor);
    } while(repeatTimes);

    ++pPic;
  }
}
