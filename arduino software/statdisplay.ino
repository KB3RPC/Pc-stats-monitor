//make sure nothing is commuicating with the arduino when you try to flash!
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 8);

// CPU Bitmap, 15x15px
static const unsigned char CPU[] = { 
0x54, 0x15, 0xFE, 0x3F, 0xFF, 0x7F, 0xFE, 0x33, 0xFF, 0x73, 0xFE, 0x3F, 
0xFF, 0x7F, 0xFE, 0x3F, 0xFF, 0x7F, 0xFE, 0x3F, 0xFF, 0x7F, 0xFE, 0x3F, 
0xFF, 0x7F, 0xFE, 0x3F, 0x54, 0x15, 
};

// GPU Bitmap, 15x15px
static const unsigned char GPU[] = { 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0xFE, 0x7F, 0xFE, 0x7F, 
0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xFE, 0x7F, 0xF2, 0x07, 0x02, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

// RAM Bitmap, 15x15px
static const unsigned char RAM[] = { 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0x22, 0x22, 
0x23, 0x62, 0x23, 0x62, 0xFF, 0x7F, 0x77, 0x7F, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

// DVD Bitmap, 15x15px
static const unsigned char DVD[] = { 
0x00, 0x00, 0x3F, 0x3E, 0x2C, 0x62, 0x69, 0x4B, 0x49, 0x49, 0xCD, 0x69, 
0x87, 0x38, 0x00, 0x00, 0xE0, 0x03, 0xF0, 0x07, 0x78, 0x0F, 0xF0, 0x07, 
0xE0, 0x03, 0x00, 0x00, 0x00, 0x00,
};

String data = "0|0|0|0|0|0";
bool loopyLock = false;
int lastData = 60;

//dvd stuff
int xspeed = 2;
int yspeed = 2;
int x = 0;
int y = 0;

void setup(void) {
  u8g2.begin();
  Serial.begin(9600);
}

void loop(void) {
  //setup screen stuff
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);

  //this is used to keep track of the number of loops seince the last data set
  lastData++;
  
  //only run if the statistic sendy thingy is connected
  if (Serial.available() > 0){
    data = Serial.readString();
    loopyLock = true;
    lastData = 0;
  }

  //this if statement is just so the screen saver doesnt break everything
  if(loopyLock){
  //Some picasso level abstraction 
  drawStatic();
  cpuUpdate();
  gpuUpdate();
  rampdate();

  //draw little title at top
  u8g2.drawStr(28, 1, "Perseverance");
  }

  //this is the screen saver
  if(lastData > 60){
    loopyLock = false;
    drawDVD();
  }
  
  //send screen data to screen
  u8g2.sendBuffer();
  delay(500);
  
}

void drawStatic(){  
  //draw borders
  u8g2.drawRFrame(0, 12, 42, 52, 1);
  u8g2.drawRFrame(43, 12, 42, 52, 1);
  u8g2.drawRFrame(86, 12, 42, 52, 1);
   u8g2.drawRFrame(0, 0, 128, 11, 1);

  //draw labels
  u8g2.drawXBM(3, 15, 15, 15, CPU);
  u8g2.drawXBM(46, 15, 15, 15, GPU);
  u8g2.drawXBM(89, 15, 15, 15, RAM);

  //draw images
  u8g2.drawStr(21, 18, "CPU");
  u8g2.drawStr(64, 18, "GPU");
  u8g2.drawStr(107, 18, "RAM");
}

void cpuUpdate(){
  drawVar(3, 32, getData(data, 1) + "%");
  drawVar(3, 42, getData(data, 3) + "C");
}

void gpuUpdate(){
  drawVar(46, 32, getData(data, 0) + "%");
  drawVar(46, 42, getData(data, 2) + "C");
}

void rampdate(){
  drawVar(89, 32, getData(data, 5) + "%");
  drawVar(89, 42, getData(data, 4) + "GB");
} 

void drawDVD() {
  u8g2.drawXBM(x, y, 15, 15, DVD);
  
  x = x + xspeed;
  y = y + yspeed;

  if (x + 15 >= 128) {
    xspeed = -xspeed;
    x = 128 - 15;
  } else if (x <= 0) {
    xspeed = -xspeed;
    x = 0;
  }

  if (y + 15 >= 64) {
    yspeed = -yspeed;
    y = 64 - 15;
  } else if (y <= 0) {
    yspeed = -yspeed;
    y = 0;
  }
}

void drawVar(int x, int y, String msg){
  u8g2.setCursor(x, y); 
  u8g2.print(msg);
}

//parses data using delimiters 
String getData(String strang, int chipotle){
    char *strings[8];
    char *ptr = NULL;
    byte index = 0;

    char buf[20];
    strang.toCharArray(buf,20); 
    
    ptr = strtok(buf, "|");  // takes a list of delimiters
    while(ptr != NULL)
    {
        strings[index] = ptr;
        index++;
        ptr = strtok(NULL, "|");  // takes a list of delimiters
    }
    return strings[chipotle];
}
