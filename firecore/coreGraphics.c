#include <esploraAPI.h>

#include "taskmanager.h"

#include "pics.h"
#include "textProg.h"
#include "common.h"

//---------------------------------------------------------------------------//
// to make cool unfold animation
uint8_t titleRowRPosX = PIC_TITLE_R_BASE_X;
uint8_t titleRowLPosX = PIC_TITLE_L_BASE_X;

//uint8_t maxStars = MAX_STARS;
stars_t stars[MAX_STARS];
bool startState = true;

//inVader_t *pAlien;
//uint8_t maxAliens;
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
  
  tftPrintAt_P(TEXT_OK_X, TEXT_OK_Y, btnText);
}

void drawStars(void)
{
  uint8_t tmpPosX, tmpPosY;
  
  // Arrrrrghh!!!!
  // How to make it faster?!?!

  // draw stars and blow your mind, if still not
  for(uint8_t count=0; count < MAX_STARS; count++) {
    tmpPosX = stars[count].pos.x;
    tmpPosY = stars[count].pos.y;
    
    fillRectFast(tmpPosX, tmpPosY, 1, 1); // clear previous star
    
    // now move them
    if((tmpPosX -= STAR_STEP) > TFT_W) {
      tmpPosX = TFT_W;
      stars[count].pos.y = RN % (TFT_H-4);
      stars[count].color = RN % 80;
    }
    
    tftDrawPixel(tmpPosX, tmpPosY, getPicWord(nesPalette_ext, stars[count].color));
    
    // now apply back what we change
    stars[count].pos.x = tmpPosX;
  }
}
// --------------------------------------------------------------- //

void rocketEpxlosion(rocket_t *pRocket)
{
  // Please, don`t say anything about this...
  // generate delay about 20 ms

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
  movePicture(&ship.pos, SHIP_PIC_W, SHIP_PIC_H);
  drawBMP_RLE_P(ship.pos.Base.x, ship.pos.Base.y, SHIP_PIC_W, SHIP_PIC_H, pic, picSize);
}

void drawPlayerRockets(void)
{
  rocket_t *pRocket = &playeRockets[0];

  for(uint8_t count =0; count < MAX_PEW_PEW; count++) {
    if(pRocket->onUse) {
      // remove previous rocket image
      fillRectFast(pRocket->pos.x, pRocket->pos.y, ROCKET_W, ROCKET_H);
      
      if((pRocket->pos.x += PLAYER_ROCKET_SPEED) < TFT_W) {
        drawBMP_RLE_P(pRocket->pos.x, pRocket->pos.y,
                                  ROCKET_W, ROCKET_H, rocketPic, ROCKET_PIC_SIZE);
      } else {
        pRocket->onUse = false;
      }
    }
    ++pRocket;
  }
}
// --------------------------------------------------------------- //

void drawInVaders(void)
{
  inVader_t *pAlien = &alien[0];

  for(uint8_t count=0; count < MAX_ALIENS; count++) {
    if(pAlien->alive) { // ALIIIVEE! IT`S ALIIVEEE!

      const uint8_t *pic = (pAlien->state ? alienShipHi : alienShipLow);
      uint16_t picSize = (pAlien->state ? ALIEN_SHIP_HI_PIC_SIZE : ALIEN_SHIP_LOW_PIC_SIZE);
      
      pAlien->state = !pAlien->state;
      movePicture(&pAlien->pos, ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H);
      
      drawBMP_RLE_P(pAlien->pos.Base.x, pAlien->pos.Base.y,
                       ALIEN_SHIP_PIC_W, ALIEN_SHIP_PIC_H, pic, picSize);
    }
    ++pAlien;
  }
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
  tftPrintAt_P(60, 100, pressAtext);

  tftSetTextColor(0x020C);
  tftPrintAt_P(0, 120, versionP0);
  tftPrintAt_P(64, 120, creditP0);
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
  tftDrawFastVLine(TFT_W, 0, TFT_H, color);
  for(int16_t i = TFT_W; i >= 0; i--) {
    tftScrollSmooth(1, i, 4);
    tftDrawFastVLine(TFT_W-i-1, 0, TFT_H, color);
  }
  tftDrawFastVLine(0, 0, TFT_H, color);
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
