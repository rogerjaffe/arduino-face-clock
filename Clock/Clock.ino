#include "Time.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
#define BLACK 0x0000
#define RED 0xF000

#define HEIGHT 320
#define WIDTH 240
#define PLACE_WIDTH 40
#define LIGHT_SIZE 10
#define MINUTE 1
#define HOUR 0
#define SECOND 2
#define Y_MARGIN 25
#define X_MARGIN 25

#define START_SECOND 0
#define START_HOUR 6
#define START_MINUTE 0

int currentHour = 0;
int currentMinute = 0;
int currentSecond = 0;

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setTime(START_HOUR, START_MINUTE, START_SECOND, 3, 11, 2014);
  tft.begin();
  tft.fillScreen(BLACK);
//  tft.drawLine(0, 0, WIDTH/2, HEIGHT/2, RED);
}

void loop() {
  // If the time has changed, then we want to repaint the clock
  if (checkTime()) {
    paintClock(currentHour, currentMinute, currentSecond);
  }
}

bool checkTime() {
  int thisSecond = second();
  if (thisSecond == currentSecond) {
    return false;
  } else {
    updateClock();
    return true;
  }
}

void updateClock() {
  currentSecond = second();
  currentMinute = minute();
  currentHour = hour(); 
}

void paintClock(int hour, int minute, int second) {
  printClockTime();
  paintHand(HOUR, hour);
  paintHand(MINUTE, minute);
  paintHand(SECOND, second);
}

void paintHand(int unit, int val) {
  for (int i=0; i<=5; i++) {
    paintPlaceValue(unit, i, val);
  }  
}

// unit:   MINUTE, HOUR, or SECOND
// place:  0..5
// val:    The number to display
void paintPlaceValue(int unit, int place, int val) {
  int x = PLACE_WIDTH * unit + X_MARGIN;
  int y = PLACE_WIDTH * place + Y_MARGIN;
  int mask = pow(2, place)+.5;
  if (val & mask) {
    tft.fillCircle(x, y, LIGHT_SIZE, RED);
  } else {
    tft.fillCircle(x, y, LIGHT_SIZE, BLACK);
    tft.drawCircle(x, y, LIGHT_SIZE, RED);
  }
}

void printClockTime() {
  Serial.print(currentHour);
  Serial.print(":");
  Serial.print(currentMinute);
  Serial.print(":");
  Serial.println(currentSecond);
}



