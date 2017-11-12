/+
 + Author: Antonov Alexander (Bismuth208)
 + Date: 26 august 2017
 + Lang: D
 + Compiler: DMD v2.075.1
 +
 + This program convert and compress by simple RLE
 + raw "*.data" picture files from "Gimp" program to "*.h" files
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

void fillHeader(string headerName, string fileName) {
  import std.file, std.outbuffer;

  auto buf = cast(ubyte[])read(fileName);
  auto bufSize = buf.length;
  auto array = compressRLE(buf);
  auto arrayEnd = new OutBuffer();
  
  arrayEnd.writef(fmtSize, bufSize, bufSize*2, array.length, cast(float)(bufSize*2)/cast(float)array.length);
  arrayEnd.writef(fmt, headerName,);

  foreach(i, ref pArr; array) {
    arrayEnd.writef("%s0x%.2x,", (!(i % 16) ? "\n  " : ""), pArr); // make new line each 16 bytes
  }
  arrayEnd.write("\n};");

  write(headerName ~ ".h", arrayEnd.data); // write end buffer to file
}

auto compressRLE(ref ubyte[] buf) {
  import std.outbuffer, std.algorithm : countUntil;
  
  auto encodedRLE = new OutBuffer();
  auto rleCount = 1;
  auto tmpNum = 0;
  auto repeatWrite = (int cnt, int sz) {
    buf[0] = cast(ubyte)(buf[0] | 0x80);
    buf[1] = cast(ubyte)sz;
    while(cnt--){ encodedRLE.write(buf[0..2]);}}; // repeatByte and how much to repeat
  
  while(rleCount > 0) { // wait for -1
    rleCount = cast(int)countUntil!"a != b"(buf, buf[0]); // if no RLE return -1
  
    if(rleCount > 0) { // find RLE ?
      if(rleCount > 2) {
        if((tmpNum = rleCount) > 0xfe) { // 255
          repeatWrite((tmpNum / 0xfe), 0xfd);
          tmpNum -= 0xfe * (tmpNum / 0xfe);
        }
        if(tmpNum > 0) { // left something?
          repeatWrite(1, tmpNum-2);
        }
      } else {
        encodedRLE.write(buf[0..rleCount]); // from 1 to 2 bytes to write
      }

      buf = buf[rleCount..$]; // chop left
    } else { // nope, last item
      if((cast(ubyte)buf.length-2) > 0) {
        repeatWrite(1, cast(ubyte)buf.length-2);
      } else {
        encodedRLE.write(buf[0]);
      }
    }
  }

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
