/*
 * MazeGame.h
 *
 * Created: 10/25/2013 7:47:50 PM
 * Using Atmel Studio 6.1 with Arduino Template Express
 * Author: Brad Walsh
 */


#ifndef MAZEGAME_H_
#define MAZEGAME_H_

#ifndef Arduino_h
#include <Arduino.h>
#endif

#include <string.h>
#include "board.h"

const int BOARD_HEIGHT = 125;
const int BOARD_WIDTH = 125;

// 74HC595N Shift Register pins
const int dataPin = 3;
const int latchPin = 5;
const int clockPin = 7;
// ADXL335 Accelerometer pins (analog)
const int xPin = 1;
const int yPin = 3;
const int zPin = 5;
// TODO might want to consider setting these to +- 70 from initial reading for use at an angle...??
const int minVal = 260;    // min and max values for -90 to 90 degrees on x and y axis from accelerometer at 3.3v
const int maxVal = 400;    // +-70 with 330 being approx level reading last time I checked it

//////////////////////////////////////
boolean debugModeOn = false;        // opens Serial for lprintf() calls on status, slows program dramatically
//////////////////////////////////////

int logCounter = 0, xOrientation = 0, yOrientation = 0;
int xpos = -1, ypos = -2;
float x = 4.0f, y = 4.0f, xinc=0.0f, yinc=0.0f;
long time = 0;    // for fps()
char display[10][10];								// used to keep track of current display
int rows[9] = {0,128,512,8,4,16384,16,8192,64};         // these values are for this specific pinout between two chained 595s and 1 8x8
int columns[9] = {0,256,4096,2048,1,32,2,1024,32768};   // first 595 - Q0-16 Q1-1 Q2-2 Q3-3 Q4-4 Q5-15 Q6-14 Q7-13
														// second 595 - Q0-9 Q1-5 Q2-6 Q3-7 Q4-8 Q5-12 Q6-11 Q7-10
void setup();
void loop();
void mazeGame();

void findStart();
void drawPlayer();
void drawScreen();
void writeScreen();
void writeToRegisters(int, int);
void adjustDisplayForMovement();

void setOrientation();
int orient(int);

boolean goingOffTheDisplay();
boolean goingToHitSomething();
boolean inMiddleOfDisplay();
boolean movingToANewSpace();

void movePlayerForward();
void movePlayerBack();
void movePlayerXBack();
void movePlayerYBack();
void moveDisplayForward();
void moveDisplayXForward();
void moveDisplayYForward();

void preventSticking();
void preventXSticking();
void preventYSticking();

char getCharFromPROGMEM(int, int);
void fps(long);
int cmap(int, int, int, int, int);
void logScreen();
void lprintf(char*, ... );   

#endif /* MAZEGAME_H_ */

