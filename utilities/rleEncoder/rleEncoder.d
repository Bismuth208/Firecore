/+
 + Author: Antonov Alexander (Bismuth208)
 + Date: 12 november 2017
 + Lang: D
 + Compiler: DMD v2.078.1
 +
 + This program convert and compress by simple RLE and dictionary (like LZ family)
 + raw ".data" picture files from "GIMP" program to ".h" files.
 +
 + For more info look readme.md for rleEncoder!
 +/


immutable auto fmtSize = "// orig size %d * 2 = %d\n// RLE pic size = %d\n// RLE compress ratio %f\n";
immutable auto fmt = "const uint8_t %s[] PROGMEM = {\n  _PIC_W-1,_PIC_H-1,";

immutable auto maxDictSize = 44; // maximum dictionary size
immutable auto minMatchCount = 3;
immutable auto dictPosMarker = 0xd0; // base value of pair bytes in dictionary; (0x80 + 0x50) or (0xff - maxDictSize - 1)
immutable auto maxDataLength = 0xcf; // actual size is: (maxDataLength -1); (0xff - dictPosMarker -1)
immutable auto pictureEndMarker = 0xff;

immutable auto compressRatioV2V3Flag = true;  // V3 needs more RAM on unpack

auto findMatch(ref ubyte[] buf)
{
  import std.algorithm : count;

  auto offset = 0;
  auto offsetMax = 0;
  auto matchCount = 0UL;
  auto allMatchFound = false;
  auto matchCountMax = buf.count(buf[0..2]);

  while(!allMatchFound) {
    if(compressRatioV2V3Flag) {  // v3
      ++offset;
    } else { // v2
      do {
        ++offset;
      } while(((buf[offset] >= dictPosMarker) || (buf[offset+1] >= dictPosMarker)) && (offset < buf.length-1));
    }
  
    if(offset < buf.length-1) {
      matchCount = buf[offset..$].count(buf[offset..offset+2]);

      if(matchCount > matchCountMax) {
        matchCountMax = matchCount;
        offsetMax = offset;
      }
    } else {
      allMatchFound = true;
    }
  }
  
  return (matchCountMax >= minMatchCount) ? buf[offsetMax..offsetMax+2] : buf[0..0];
}

auto encodeMatches(ref ubyte[] buf)
{
  import std.array : replace;

  auto dictionaryArr = new ubyte[1];
  auto markerBuf = new ubyte[1];
  auto tmpDict = new ubyte[2];
  auto dictPos = 0;
  
  do {
    tmpDict = buf.findMatch;
    
    if(tmpDict.length) { // found something?
      markerBuf[0] = cast(ubyte)(dictPosMarker + dictPos); // replaceVal
      buf = buf.replace(tmpDict, markerBuf);
      dictionaryArr ~= tmpDict;
      ++dictPos;
    }
  } while((tmpDict.length) && (dictPos <= maxDictSize));
  
  // size of dictionary offset
  dictionaryArr[0] = cast(ubyte)(dictionaryArr.length ? dictionaryArr.length+1 : 0x00);
  return dictionaryArr;
}

auto compressRLE(ref ubyte[] buf) {
  import std.stdio, std.outbuffer, std.algorithm : group;
  import std.typecons : tuple, Tuple;

  auto encodedRLE = new OutBuffer;
  auto dataBuf = new ubyte[2];
  auto tmpByte = cast(ubyte)0;
  auto fastGrop = buf.group;  // actually this one make all job...
  auto rleCount = 0;
  
  auto repeatWrite = (int i, int size, ubyte data) {
    dataBuf[0] = cast(ubyte)(data | 0x80);
    dataBuf[1] = cast(ubyte)size;
    while(i--){ encodedRLE.write(dataBuf);}}; // repeatByte and how much to repeat

  foreach(rleData; fastGrop) {
    rleCount = rleData[1];
    tmpByte = rleData[0];

    if(rleCount > 2) {
      if(rleCount > maxDataLength) {
        repeatWrite((rleCount / maxDataLength), maxDataLength-1, tmpByte);
        rleCount -= maxDataLength * (rleCount / maxDataLength);
      }
      if(rleCount) { // left something?
        repeatWrite(1, rleCount-2, tmpByte);
      }
    } else {
      do {
        encodedRLE.write(tmpByte); // from 1 to 2 bytes to write
      } while (--rleCount);
    }
  }

  encodedRLE.write(cast(ubyte)pictureEndMarker);
  return encodedRLE.toBytes;
}

void fillHeader(string fileName) {
  import std.file, std.outbuffer;//, std.string;

  auto arrayEnd = new OutBuffer;
  auto dictionaryArr = new ubyte[0]; 
  auto array = cast(ubyte[])fileName.read;
  auto bufSize = array.length;
  auto pictureSize = 0UL;
  auto headerName = fileName[0..$-5]; // fileName.chomp(".data");
  
  auto writeEndArr = (ref ubyte[] refArr, int sep) { // make new line each sep bytes
    foreach(i, ref pArr; refArr) arrayEnd.writef("%s0x%.2x,", (!(i % sep) ? "\n  " : ""), pArr);
  };

  array = array.compressRLE;
  dictionaryArr = array.encodeMatches;
  pictureSize = dictionaryArr.length + array.length;
  
  arrayEnd.writef(fmtSize, bufSize, bufSize*2, pictureSize, cast(float)(bufSize)/cast(float)pictureSize);
  arrayEnd.writef(fmt, headerName);
 
  writeEndArr(dictionaryArr, 16);
  writeEndArr(array, 16); // comressed and encoded array
  arrayEnd.write("\n};");

  (headerName ~ ".h").write(arrayEnd.data); // write result to file
}

void main() {
  import std.file, std.stdio, std.conv, std.parallelism;

  // search all *.data in execution folder and lower folders
  auto dataFiles = dirEntries("", "*.{data}", SpanMode.breadth, false);
  
  "Start RLE encode...".writeln;
  
  // each file parsed in thread,
  // number of threads depend on your CPU and it's number of real cores
  foreach(i; dataFiles.parallel(to!int(totalCPUs))) {
    "\t%s...\n".writef(i.name);
    i.name.fillHeader;
  }

  "All Done!".writeln;
}
