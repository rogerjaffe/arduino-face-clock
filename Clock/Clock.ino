// Binary clock code with 2.8" TFT capacitive touch display 
// Ardino shield from Adafruit.
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
#define START_X 220
#define START_Y 300
#define HEIGHT 30
#define WIDTH 30
#define HEIGHT_SPACING 80
#define WIDTH_SPACING 40
#define RADIUS 5
#define Y_SET 25
#define X_SET_HOUR 205
#define X_SET_MINUTE 125
#define SET_RADIUS 15

// Light status constants
#define ON 1
#define OFF 0
#define NA -1

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

// current light status (ON, OFF, or NA-not initialized)
int lights[3][6] = {
  {NA, NA, NA, NA, NA, NA},
  {NA, NA, NA, NA, NA, NA},
  {NA, NA, NA, NA, NA, NA},  
};

// From the library documentation
// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

void setup() {
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
  paintHand(HOUR, hour);
  paintHand(MINUTE, minute);
  paintHand(SECOND, second);
}

// Paint the hours, minutes, or seconds
void paintHand(int unit, int val) {
  for (int i=0; i<=5; i++) {
    paintPlaceValue(unit, i, val);
  }  
}

// Paint one of the lights
// unit:   MINUTE, HOUR, or SECOND
// place:  0..5 (standing for 1s, 2s, 4s, ... 32s
// val:    The number to display - use bitmask to see if on or off
void paintPlaceValue(int unit, int place, int val) {
  // Skip 32s place for hour only
  if (!(unit == HOUR && place == 5)) {    
    // Get position of light
    int y = START_Y - (place * WIDTH_SPACING) - WIDTH;
    int x = START_X - (unit * HEIGHT_SPACING) - HEIGHT;
    // Compute the mask. The .5 is added to get correct integer
    // when it's cast to an integer
    int mask = pow(2, place)+.5;
    if (val & mask) {
      // Light should be on. Repaint only if it's off right now
      if (lights[unit][place] != ON) {
        tft.fillRoundRect(x, y, WIDTH, HEIGHT, RADIUS, colors[unit]);
        lights[unit][place] = ON;
      }
    } else {
      // Light should be off. Repaint only if it's on right now
      if (lights[unit][place] != OFF) {      
        tft.fillRoundRect(x, y, WIDTH, HEIGHT, RADIUS, BLACK);
        tft.drawRoundRect(x, y, WIDTH, HEIGHT, RADIUS, colors[unit]);
        lights[unit][place] = OFF;
      }
    }
  }
}
