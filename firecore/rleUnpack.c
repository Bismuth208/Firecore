/*
 * This is used when need maximum pic compression,
 * and you can waste some CPU resources for it;
 * It use very simple RLE compression;
 *
 * Where are two versions:
 *  - drawBMP_RLE_P;
 *  - drawBMP_ERLE_P;
 *
 * First use as simple as possible RLE compresseion;
 * Second use dictionary in addition to previos version;
 *
 * Author: Antonov Alexandr (Bismuth208)
 * Date:   11 November, 2017
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

#include "rleUnpack.h"
#include "common.h"
#include "pics.h"

#ifndef __AVR__
#define getPicByte(a)       (*(const uint8_t *)(a))
#define getPicWord(a, b)    (*(const uint16_t*)(&(a[b])))
#endif /* __AVR__ */

// unsafe but it works on avr, and stm32
#define setPicWord(a, b)  (*(uint16_t*)(&a[b]))

//---------------------------------------------------------------------------//
#define MAX_UNFOLD_SIZE 32

uint8_t buf_packed[MAX_UNFOLD_SIZE<<1];
uint8_t buf_unpacked[MAX_UNFOLD_SIZE<<1];
//---------------------------------------------------------------------------//

uint8_t findPackedMark(uint8_t *ptr, uint8_t sizeData)
{
  do {
    if(*ptr++ > 0xd0) {
      return 1;
    }
  } while(--sizeData);
  
  return 0;
}

void printBuf_RLE(uint8_t dataSize)
{
  uint16_t repeatColor;
  uint8_t repeatTimes, tmpByte;
  uint8_t *pData = &buf_packed[0];
  
  do { // get color index or repeat times
    tmpByte = *pData;
    
    if(tmpByte & 0x80) { // is it color index?
      tmpByte &= 0x7f; // get color index to repeat
      repeatTimes = *(++pData)+2;
      --dataSize;
    } else {
      repeatTimes = 1;
    }
    
    // get color from colorTable by tmpInd color index
    repeatColor = palette_RAM[(tmpByte == replaceColorId) ? currentBackGroundColorId : tmpByte];
    
    do {
      pushColorFast(repeatColor);
    } while(--repeatTimes);
    
    ++pData;
  } while(--dataSize);
}

uint8_t unpackBuf_RLE(const uint8_t *pDict, uint8_t dataSize)
{
  wordData_t tmpData;
  uint8_t bufPackedPos,bufUnpackedPos;
  uint8_t noDictMarker = findPackedMark(&buf_packed[0], dataSize);
  
  if(noDictMarker) {
    bufPackedPos =0;
    bufUnpackedPos =0;
    
    while(noDictMarker) {
      if(buf_packed[bufPackedPos] > 0xd0) {
        setPicWord(buf_unpacked, bufUnpackedPos) = getPicWord(pDict, (buf_packed[bufPackedPos] - 0xd1)<<1);
        ++bufUnpackedPos;
      } else {
        buf_unpacked[bufUnpackedPos] = buf_packed[bufPackedPos];
      }
      ++bufUnpackedPos;
      ++bufPackedPos;
      
      if(bufPackedPos >= dataSize) {
        noDictMarker = findPackedMark(&buf_unpacked[0], bufUnpackedPos);
        dataSize = bufUnpackedPos;
        if(noDictMarker) {
          memcpy_F(&buf_packed[0], &buf_unpacked[0], bufUnpackedPos+1);
          bufUnpackedPos = 0;
          bufPackedPos =0;
        }
      }
    }
  } else {
    memcpy_F(&buf_unpacked[0], &buf_packed[0], dataSize+1);
  }
  
  return dataSize;
}

// extended RLE, a bit slower but better compression
void drawBMP_ERLE_P(uint8_t x, uint8_t y, const uint8_t *pPic)
{
  wordData_t tmpData = {.wData = getPicWord(pPic, 0)};
  tftSetAddrWindow(x, y, x+tmpData.u8Data1, y+tmpData.u8Data2);
  
  uint8_t tmpByte, unfoldPos;
  const uint8_t *pDict = &pPic[3];
  pPic += getPicByte(&pPic[2]); // make offset to picture data
  
  for(;;) {
    unfoldPos =0;
    
    do {
      if((tmpByte = getPicByte(++pPic)) < 0xff) {
        if(tmpByte < 0xd0) {
          buf_packed[unfoldPos] = tmpByte;
        } else {
          setPicWord(buf_packed, unfoldPos) = getPicWord(pDict, (tmpByte - 0xd1)<<1);
          ++unfoldPos;
        }
        ++unfoldPos;
      } else {
        break;
      }
    } while((unfoldPos < MAX_UNFOLD_SIZE) && (tmpByte > 0x7f));
    
    if(unfoldPos) { // or maybe unfoldPos?
      //printBuf_RLE(unpackBuf_RLE(pDict, unfoldPos));
      printBuf_RLE(unfoldPos);
    } else {
      return;
    }
  }
}

// =============================================================== //
// old version, faster, but less comression
void drawBMP_RLE_P(uint8_t x, uint8_t y, const uint8_t *pPic)
{
  uint16_t repeatColor;
  uint8_t tmpInd, repeatTimes;
  
  wordData_t tmpData = {.wData = getPicWord(pPic, 0)};
  tftSetAddrWindow(x, y, x+tmpData.u8Data1, y+tmpData.u8Data2);
  
  ++pPic; // make offset to picture data
  
  while((tmpInd = getPicByte(++pPic)) != 0xff) { // get color index or repeat times
    
    if(tmpInd & 0x80) { // is it color index?
      tmpInd &= 0x7f; // get color index to repeat
      repeatTimes = getPicByte(++pPic)+2;
    } else {
      repeatTimes = 1;
    }
    
    // get color from colorTable by tmpInd color index
    repeatColor = palette_RAM[(tmpInd == replaceColorId) ? currentBackGroundColorId : tmpInd];
    
    do {
      pushColorFast(repeatColor);
    } while(--repeatTimes);
  }
}
