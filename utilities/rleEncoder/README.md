# RLE Encoder

This program convert and compress by simple RLE and dictionary (like LZ family)
raw ".data" picture files from "GIMP" program to ".h" files.

#### Build:
- YOU NEED to instal dmd compiler for D lang, grab it at https://dlang.org;
- compile it according howto at https://dlang.org.

#### Be sure what you: 
- remove Alpha channel before export to raw ".data" files;
- make image indexed whith "gimp_firecore_80c.gpl" palette.

#### Usage: 
- place program in folder whith ".data" files;
- run terminal\command promt;
- exec rleEncoder (execution is OS speciffic look in Internet howto);
- wait untill program print "All done!". 
  - *p.s program also scan and process lower folders if they exists.*
  - *p.p.s Works ONLY whith data value < 0x50 as unsed 0x2f values *ARE USED* by this compressor!*
  - *p.p.p.s If program seems to stuck, then something is going wrong...*

Time of converting depends on size of raw data files
and power of your CPU, SSD and\or HDD (aka magic).
Also multicore CPU have advantage (converts faster), formula is
((Number of files) devided by (Number of cores)) multiplexed by magic index.


#### RLE format: 
- (0x80 | data), marker and data (0x80 - marker specify RLE start);
- repeatTimes, how much to repeat previous data.

#### Dictionary rules: 
- may be empty, in this case size of *offset* will be 0x02;
- consists of byte pairs (duplets - two bytes as one index);
- size of dictionary may be from 0 to 44 duplets;
- minimal size of repeats to be replaced by dict index is 3 (minMatchCount);
- duplets have index values from 0xd0 to 0xfc.

#### Examples: 
Packed data | Unpacked data
------------|--------------
0x81, 0x01 | 0x01, 0x01, 0x01
0x01, 0x01, 0x00 | no RLE, only raw data
0x8f,0x02,0x0e,0x0e | 0x0f,0x0f,0x0f,0x0f,0x0e,0x0e
0x81,0x02,0xd0,0x22 | 0x01,0x01,0x01,0x01,(get duplet at index 0),0x22
- *p.s duplets may represent everything: RLE pair, raw data or another duplet (if V3 enabled).*


Structure of every generated header (picture):
```C
const uint8_t somePicName[] PROGMEM = { 
  width-1, height-1,  // one byte each 
  offset,             // one byte offset to picture data and dictionary size 
  dictionary,         // from zero to 92 bytes (values from 0xd0 to 0xfc) 
  picture data,       // a lot of bytes... or at least one byte ;) 
  end marker          // 0xff mean picture data end 
};
```
- *p.s -1 at begin of every pic is conversations to correspond to tft display addr size.* 
- *p.p.s width, height you need to write manually as raw data not contain this params.*

***
- Author: Antonov Alexander (Bismuth208) 
- Date: 12 november 2017 
- Lang: D 
- Compiler: DMD v2.075.1 


> ### :exclamation: ATTENTION! :exclamation:
>  * Sometimes it's generate garbage! :beetle:
>  * If it's happen then just remove dictionary encode and use only RLE part.
