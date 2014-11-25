// Traditional face clock code with 2.8" TFT capacitive touch 
// display Ardino shield from Adafruit.
//
// Version history
// 2014-11-22 V1.0 Written
//
// Unrestricted license.  Do what you want with it!
//
#include "Time.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Wire.h"
#include "Adafruit_FT6206.h"
#include "position.h"

// Display constants
#define TFT_DC 9
#define TFT_CS 10
#define TOUCH_SENSITIVITY 40

// Color constants
#define BLACK 0x0000
#define RED 0xF000
#define GREEN 0x07E0
#define YELLOW 0xFFE0
#define GRAY 0x38E7

// Positioning constants
#define X_BL = 20
#define Y_BL = 100
#define X_TR = 220
#define Y_TR = 300

#define WIDTH 30
#define HEIGHT_SPACING 80
#define WIDTH_SPACING 40
#define HR_RADIUS 50
#define MIN_RADIUS 75
#define SEC_RADIUS 100
#define HR_THICKNESS 5
#define MIN_THICKNESS 3
#define SEC_THICKNESS 1

#define Y_SET 25
#define X_SET_HOUR 205
#define X_SET_MINUTE 125
#define SET_RADIUS 15
#define PI 3.14156

// Time constants
#define HOUR 0
#define MINUTE 1
#define SECOND 2

// Default start time should be 12:00:00
#define START_HOUR 12
#define START_MINUTE 0
#define START_SECOND 0

// Keeps current time
int currentHour = 0;
int currentMinute = 0;
int currentSecond = 0;

// Default colors hr, min, sec
int colors[] = {RED, GREEN, YELLOW};
int handRadius[] = {HR_RADIUS, MIN_RADIUS, SEC_RADIUS};
int handThick[] = {HR_THICKNESS, MIN_THICKNESS, SEC_THICKNESS};

// From the library documentation
// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
  Serial.begin(9600);
  ctp.begin(TOUCH_SENSITIVITY);
  tft.begin();
  // Clear display and draw the set time buttons
  tft.fillScreen(BLACK);
  tft.fillCircle(X_SET_MINUTE, Y_SET, SET_RADIUS, GRAY);
  tft.fillCircle(X_SET_HOUR, Y_SET, SET_RADIUS, GRAY);
  // Set Arduino time
  setTime(START_HOUR, START_MINUTE, START_SECOND, 3, 11, 2014);
}

void loop() {
  // If the time has changed, then we want to repaint the clock
  if (checkTime()) {
//    Serial.print(currentHour);
//    Serial.print(":");
//    Serial.print(currentMinute);
//    Serial.print(":");
//    Serial.println(currentSecond);
    paintClock(currentHour, currentMinute, currentSecond);
  }
  if (ctp.touched()) {
    // Get the coordinates and 
    // flip it around to match the screen
    TS_Point p = ctp.getPoint();
    p.x = map(p.x, 0, 240, 240, 0);
    p.y = map(p.y, 0, 320, 320, 0);
    // If we've tapped the hour or minute set then...
    if (abs(p.y-Y_SET) <= 15) {
      // Hour set
      if (abs(p.x-X_SET_HOUR) <= 15) {
        currentHour = (currentHour+1) % 24;
        setTime(currentHour, currentMinute, currentSecond, 3, 11, 2014);
      }
      // Minute set
      if (abs(p.x-X_SET_MINUTE) <= 15) {
        currentMinute = (currentMinute+1) % 60;
        setTime(currentHour, currentMinute, currentSecond, 3, 11, 2014);
      }
      // Repaint and delay for 1/2 second
      paintClock(currentHour, currentMinute, currentSecond);      
      delay(500);
    }
  }
}

// If the second has changed, then we need to update the clock
bool checkTime() {
  int thisSecond = second();
  if (thisSecond == currentSecond) {
    return false;
  } else {
    updateClock();
    return true;
  }
}

// Get the current hour, minute, second and save
void updateClock() {
  currentSecond = second();
  currentMinute = minute();
  currentHour = hour(); 
}

// Paint the clock time
void paintClock(int hour, int minute, int second) {
  tft.fillRect(20, 100, 200, 200, BLACK);
  paintHand(SECOND, second);
  paintHand(MINUTE, minute);
  paintHand(HOUR, hour);
}

// Paint the hours, minutes, or seconds
void paintHand(int unit, int val) {
  Position p = computeXY(unit, val, handRadius[unit], handThick[unit]);
  tft.drawLine(p.x1, p.y1, p.x2, p.y2, colors[unit]); 
}

// Compute x,y coordinates of hand
Position computeXY(int unit, int val, int radius, int width) {
  Position p;
  int conv;
  int cVal = val;
  if (unit == HOUR) {
    val = val % 12;
    conv = 6;
  } else {
    conv = 30;
  }
  Serial.print(val);
  double angle = PI / conv * val;
  int x = radius * cos(angle);
  int y = radius * sin(angle);
  p.x1 = 120;
  p.y1 = 200;
  p.x2 = map(x, -100, 100, 20, 220);
  p.y2 = map(y, -100, 100, 100, 300);
  return p;
}

