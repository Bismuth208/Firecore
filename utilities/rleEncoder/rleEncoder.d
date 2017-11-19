/+
 + Author: Antonov Alexander (Bismuth208)
 + Date: 12 november 2017
 + Lang: D
 + Compiler: DMD v2.075.1
 +
 + This program convert and compress by simple RLE
 + raw "*.data" picture files from "Gimp" program to "*.h" files
 +
 + Works ONLY whith data value < 0x50!
 + As unsed 0x2f values ARE USED by this compression!
 +
 + Format:
 +   # (0x80 | data) - marker and data (0x80 - marker specify RLE start)
 +   # repeatTimes - how much to repeat previous data
 +
 + Example:
 +  1# 0x81, 0x00 - mean: 0x01, 0x01, 0x01;
 +  2# 0x01, 0x01, 0x00 - no RLE, only raw data;
 +  3# 0x8f,0x02,0x0e,0x0e - mean: 0x0f,0x0f,0x0f,0x0f,0x0e,0x0e
 +/

immutable auto fmtSize = "// orig size %d * 2 = %d\n// RLE pic size = %d\n// RLE compress ratio %f\n";
immutable auto fmt = "const uint8_t %s[] PROGMEM = {\n  _PIC_W-1,_PIC_H-1,";

immutable auto pictureEndMarker = 0xff;
immutable auto maxDictSize = 45; // maximum dictionary size
immutable auto minMatchCount = 3;
immutable auto maxDataLength = 0xcf; // actual size is: (maxDataLength -1)

void fillHeader(string headerName, string fileName) {
  import std.file, std.outbuffer;

  auto buf = cast(ubyte[])read(fileName);
  auto bufSize = buf.length;
  auto array = compressRLE(buf);
  auto arrayEnd = new OutBuffer();
  auto dictionaryArr = cast(ubyte[])encodeMatches(array);
  auto pictureSize = dictionaryArr.length + array.length;
  
  auto writeEndArr = (ref ubyte[] refArr, int sep) { // make new line each sep bytes
    foreach(i, ref pArr; refArr) arrayEnd.writef("%s0x%.2x,", (!(i % sep) ? "\n  " : ""), pArr);
  };
  
  arrayEnd.writef(fmtSize, bufSize, bufSize*2, pictureSize, cast(float)(bufSize)/cast(float)pictureSize);
  arrayEnd.writef(fmt, headerName);
 
  writeEndArr(dictionaryArr, 16);
  writeEndArr(array, 16);
  arrayEnd.write("\n};");

  write(headerName ~ ".h", arrayEnd.data); // write end buffer to file
}

auto findMatch(ref ubyte[] buf)
{
  import std.algorithm : count;

  auto offset = 0;
  auto offsetMax = 0;
  auto matchCount = 0;
  auto matchCountMax = 0;
  auto allMatchFound = false;
  auto dictDataBuf = buf[offset..offset+2];

  matchCountMax = cast(int)count(buf[offset..$], dictDataBuf);

  while(!allMatchFound) {
    //++offset;
    
    do {
      ++offset;
    } while(((buf[offset] > cast(ubyte)0x7f) || (buf[offset+1] > cast(ubyte)0x7f)) && (offset < buf.length-1));
  
    if(offset < buf.length-1) {
      dictDataBuf = buf[offset..offset+2];
      matchCount = cast(int)count(buf[offset..$], dictDataBuf);

      if(matchCount > matchCountMax) {
        matchCountMax = matchCount;
        offsetMax = offset;
      }
    } else {
      allMatchFound = true;
    }
  }
  
  
  dictDataBuf.length = 0;
  
  if(matchCountMax >= minMatchCount) {
    if((buf[offsetMax] < cast(ubyte)0x7f) && (buf[offsetMax+1] < cast(ubyte)0x7f)) {
      dictDataBuf = buf[offsetMax..offsetMax+2];
    }
  }
  return dictDataBuf; // 2 bytes dictionary
  
  //return (matchCountMax >= minMatchCount) ? buf[offsetMax..offsetMax+2] : buf[0..0];
}

auto encodeMatches(ref ubyte[] buf)
{
  import std.array : replace;

  auto dictPosMarker = cast(ubyte)0xd0; // start value
  auto dictionaryArr = new ubyte[1];
  auto markerBuf = new ubyte[1];
  auto tmpDict = new ubyte[2];
  auto dictPos = 1;
  
  do {
    tmpDict = findMatch(buf);
    
    if(tmpDict.length > 0) { // found something?
      markerBuf[0] = cast(ubyte)(dictPosMarker + dictPos); // replaceVal
      buf = buf.replace(tmpDict, markerBuf);
      dictionaryArr ~= tmpDict;
      ++dictPos;
    }
  } while((tmpDict.length) && (dictPos <= maxDictSize));
  
  // size of dictionary offset
  dictionaryArr[0] = cast(ubyte)((dictionaryArr.length > 0) ? dictionaryArr.length+1 : 0x00);
  return dictionaryArr;
}

auto compressRLE(ref ubyte[] buf) {
  import std.stdio, std.outbuffer, std.algorithm : group;
  import std.typecons : tuple, Tuple;

  auto encodedRLE = new OutBuffer();
  auto dataBuf = new ubyte[2];
  auto tmpByte = cast(ubyte)0;
  auto fastGrop = group(buf);  // actually this one make all job...
  auto rleCount = 0;
  
  auto repeatWrite = (int cnt, int sz, ubyte dat) {
    dataBuf[0] = cast(ubyte)(dat | 0x80);
    dataBuf[1] = cast(ubyte)sz;
    while(cnt--){ encodedRLE.write(dataBuf[0..2]);}}; // repeatByte and how much to repeat

  foreach(rleData; fastGrop) {
    rleCount = rleData[1];
    tmpByte = rleData[0];

    if(rleCount > 2) {
      if(rleCount > maxDataLength) {
        repeatWrite((rleCount / maxDataLength), maxDataLength-1, tmpByte);
        rleCount -= maxDataLength * (rleCount / maxDataLength);
      }
      if(rleCount > 1) { // left something?
        repeatWrite(1, rleCount-2, tmpByte);
      }
    } else {
      do {
        encodedRLE.write(tmpByte); // from 1 to 2 bytes to write
      } while (--rleCount);
    }
  }

  encodedRLE.write(cast(ubyte)pictureEndMarker);
  return encodedRLE.toBytes();
}

void parallelWorker(string iName)
{
  import std.stdio, std.string;

  auto name = chomp(iName, ".data");
  writef("\t%s...\n", name);
  fillHeader(name, iName);
}

void main() {
  import std.file, std.stdio, std.conv, std.parallelism;

  // search all *.data in execution folder and lower folders
  auto dataFiles = dirEntries("", "*.{data}", SpanMode.breadth);
  
  writeln("Start RLE encode...");
  
  // each file parsed in thread,
  // number of threads depend on your CPU and it's number of real cores
  foreach(i; dataFiles.parallel(to!int(totalCPUs))) {
    parallelWorker(i.name);
  }

  writeln("All Done!");
}
