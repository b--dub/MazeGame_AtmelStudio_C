/*
 * Main.cpp
 *
 * Created: 10/25/2013 7:47:50 PM 
 * Using Atmel Studio 6.1 with Arduino Template Express
 * Author: Brad Walsh
 */



#ifndef SKETCH_H_
#include "Sketch.h"
#endif

//#pragma message "Sketch Running At: " XSTR(F_CPU) " HZ"

 
void setup() {
	pinMode(dataPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	pinMode(clockPin, OUTPUT);
			
	if (debugModeOn) Serial.begin(115200);

	randomSeed(analogRead(0));
  
#if defined(ENABLE_TEXT_MESSAGE)
	ShowBuildInfo();
#endif	

}

void loop() {
	//randomMovement();
	//randomMovementWithTilt();
	//tiltControl();
	mazeGame();
}


// randomly adds or subtracts one from X and Y values to move around the display
void randomMovement() {
	x += (random(3) - 1); y += (random(3) - 1);
	if (x<1) x=1; if (x>8) x=8;
	if (y<1) y=1; if (y>8) y=8;
	writeToRegisters(x, y);
	delay(100);
}

// random movement with added "tilt" from orientation
void randomMovementWithTilt() {
	setOrientation();

	x += (random(3) - 1); y += (random(3) - 1);

	if (abs(xOrientation) > 5) x -= xOrientation / 25.0f;
	if (abs(yOrientation) > 5) y += yOrientation / 25.0f;

	if (x<1) x=1; if (x>8) x=8;
	if (y<1) y=1; if (y>8) y=8;

	writeToRegisters(x, y);
	delay(10);
}

// Just moves the dot by tilt for testing purposes
void tiltControl() {
	setOrientation();

	if (abs(xOrientation) > 5) x -= xOrientation / 50.0f;
	if (abs(yOrientation) > 5) y += yOrientation / 50.0f;

	if (x<1) x=1; if (x>8) x=8;
	if (y<1) y=1; if (y>8) y=8;

	writeToRegisters(x, y);
	fps(1500);
}

void mazeGame() {
	findStart();
	Serial.end();
	while(true) {
		drawScreen();
		writeScreen();
		setOrientation();
		
		// Five degrees worth of play before accelerometer kicks in
		if (abs(xOrientation) > 5) xinc = constrain(-xOrientation / 100.0f, -1, 1); 
		if (abs(yOrientation) > 5) yinc = constrain(yOrientation / 100.0f, -1, 1);
		
		adjustDisplayForMovement();
  }
}

// Get the starting point for player from board[][] and establish frame location for display
void findStart() {
  for (unsigned int i=0; i<40; ++i) {
    for (unsigned int j=0; j<40; ++j) {
      if (board[i][j]=='S') {
        xpos=i-x; ypos=j-y;
      }
    }
  }
}

// look at board and relative positions to map our 8x8 display area
void drawScreen() {
	memset(display, 0, sizeof(display));		// Clear out the display storage array
	
	//logScreen();
	
	for (int i=0; i<10; ++i) {
		for (int j=0; j<10; ++j) {
			if (xpos+i<0 || xpos+i>39 || ypos+j<0 || ypos+j>39)
				display[i][j]='X';
			else if (board[xpos+i][ypos+j] == 'X')
				display[i][j]='X';
		}
	}
	drawPlayer();
	logScreen();
}

//  Mark where the player is currently located on the display so we can light his led
void drawPlayer() {
	display[(int)x][(int)y] = 'P';		// Using different chars in case I want to use multi colors later
}

// For each point in display not 0 write HIGH to corresponding LED
void writeScreen() {
	for (int i=1; i<9; ++i) {
		for (int j=1; j<9; ++j) {
			if (display[i][j]) {
				writeToRegisters(i,j);
				delayMicroseconds(1);		// The longer the delay here, the longer each LED is held HIGH
			}
		}
		writeToRegisters((int)x,(int)y);	// Make sure the player LED is brightest
		delayMicroseconds(100);
	}
	fps(3000);
}

void adjustDisplayForMovement() {
	Serial.println(xpos);			// TODO switch these to lprintf or something
	Serial.println(ypos);			// Don't want them to slow program down unless in DEBUG session
	Serial.println(x);
	Serial.println(y);
	Serial.println(xinc);
	Serial.println(yinc);
	
	//  First, is this move going to mean that our player is going to be going to a new space?
	//  Because we're using floats for the player's position it is possible we haven't accrued
	//  enough forward moves yet to call for moving onto a new space
	if (!movingToANewSpace) {
		movePlayerForward();
	}
	else if (goingOffTheDisplay() && goingToHitSomething()) {
		//  Moving the display forward (relative to the board) while moving the player back
		//  relative to the display gives the effect of looking forward into the unseen 
		//  area of the board, while not progressing the player forward onto the next space
		if ((display[(int)(x)][(int)(y+yinc)] == 'X') &&
		((y<4 && ((y+yinc)<y)) || (y>=6 && ((y+yinc)>y)))) {
			movePlayerYBack();
			moveDisplayYForward();
		}
		else if ((int)y==1 || (int)y==8) moveDisplayYForward();
		
		if ((display[(int)(x+xinc)][(int)(y)] == 'X') &&
		((x<4 && ((x+xinc)<x)) || (x>=6 && ((x+xinc)>x)))) {
			movePlayerXBack();
			moveDisplayXForward();
		}
		else if ((int)x==1 || (int)x==8) moveDisplayXForward();
	}
	else if (goingOffTheDisplay() && !goingToHitSomething()) {
		moveDisplayForward();
	}
	else if (!goingOffTheDisplay() && goingToHitSomething() && inMiddleOfDisplay()) {
		preventSticking();
	}
	else if (!goingOffTheDisplay() && goingToHitSomething() && !inMiddleOfDisplay()) 
	{		
		if ((display[(int)(x)][(int)(y+yinc)] == 'X') &&
		((y<4 && ((y+yinc)<y)) || (y>=6 && ((y+yinc)>y)))) {
			movePlayerYBack();
			moveDisplayYForward();
		}
		else preventYSticking();
		if ((display[(int)(x+xinc)][(int)(y)] == 'X') &&
		((x<4 && ((x+xinc)<x)) || (x>=6 && ((x+xinc)>x)))) {
			movePlayerXBack();
			moveDisplayXForward();
		}
		else preventXSticking();
	}
	else movePlayerForward();
	
}

// If adding one of the increments to our current position results in a new whole number 
// portion of either of our float position values, then we are attempting to move to a new space
boolean movingToANewSpace() {
	return  (((int)(x+xinc)!=(int)x) || ((int)(y+yinc)!=(int)x));
}

boolean goingOffTheDisplay() {
	return (x+xinc >= 9 || x+xinc < 1 || y+yinc < 1 || y+yinc >= 9);
}

boolean goingToHitSomething() {
	return (display[(int)(x+xinc)][(int)(y+yinc)] == 'X');
}

boolean inMiddleOfDisplay() {
	return (x>=4 && x<6 && y>=4 && y<6);
}

boolean movingFurtherFromMiddle() {
	return ((x<4  && (int)(x+xinc)<(int)x) || 
			(x>=6 && (int)(x+xinc)>(int)x) ||
			(y<4  && (int)(y+yinc)<(int)y) ||
			(y>=6 && (int)(y+yinc)>(int)y) ); 
}

void movePlayerForward() {
	x += xinc;
	y += yinc;
}

// Moving the player backwards requires a full 1 or -1 space move, otherwise the move could 
// register as not changing spaces, and the player could be overwritten by a wall!
void movePlayerBack() {
	movePlayerXBack();
	movePlayerYBack();
}

void movePlayerXBack() {
	if ((int)(x+xinc)!=(int)x) x -= 2*(xinc==fabs(xinc))-1;
	else x += xinc;
}

void movePlayerYBack() {
	if ((int)(y+yinc)!=(int)y) y -= 2*(yinc==fabs(yinc))-1;
	else y += yinc;
}

// Moving the display also requires using a full 1 or -1, as there are no partial spaces
void moveDisplayForward() {
	moveDisplayXForward();
	moveDisplayYForward();
	
	// TODO fix here for too fast scrolling from edge 
}

void moveDisplayXForward() {
	if ((int)(x+xinc)!=(int)x) xpos += 2*(xinc==fabs(xinc))-1;
	if ((int)(x+xinc)==(int)x) x+=xinc;		// add increment if not going to cause movement
	
	// TODO fix here for too fast scrolling from edge
}

void moveDisplayYForward() {
	if ((int)(y+yinc)!=(int)y) ypos += 2*(yinc==fabs(yinc))-1;
	if ((int)(y+yinc)==(int)y) y+=yinc;
	
	// TODO fix here for too fast scrolling from edge
}

// For the case that we are trying to move diagonally to a taken spot, but could still
// move up/down or left/right without hitting anything, let's do that instead
void preventSticking() {
	preventXSticking();
	preventYSticking();
}

void preventXSticking() {
	if (display[(int)(x+xinc)][(int)(y)] != 'X' && x+xinc<9 && x+xinc >=1)
	x += xinc;
}

void preventYSticking() {
	if (display[(int)(x)][(int)(y+yinc)] != 'X' && y+yinc<9 && y+yinc >=1)
	y += yinc;
}

// Gets bit weights from rows and columns arrays to match 595 outs and 8x8s ins
void writeToRegisters(int row, int column) {
  int tmp = (25308 - rows[row]) + columns[column];
  for (int i=0; i<16; ++i) {
    digitalWrite(dataPin, (tmp & (1 << i)) ? 0 : 1);    // change "? 0 : 1" to "? 1 : 0" to switch HIGHs and LOWs
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);

    lprintf("Bit %d: %d  clockPin cycled", i, (tmp & (1 << i)) ? 0 : 1);   // repeat switch if made above to depict in debugging console
  }

  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);

  lprintf("latchPin Cycled");
}


// Uses accelerometer to get current orientation of matrix WRT level (laying flat)
void setOrientation() {
	  int xRead = analogRead(xPin);
	  int yRead = analogRead(yPin);
	  int zRead = analogRead(zPin);

	  int xAng = cmap(xRead, minVal, maxVal, -90, 90);
	  int yAng = cmap(yRead, minVal, maxVal, -90, 90);
	  int zAng = cmap(zRead, minVal, maxVal, -90, 90);

	  // source: http://bildr.org/2011/04/sensing-orientation-with-the-adxl335-arduino/
	  //Calculate 360deg values like so: atan2(-yAng, -zAng)
	  //atan2 outputs the value of -pi to pi (radians)
	  //We are then converting the radians to degrees
	  int xTilt = (int)(RAD_TO_DEG * (atan2(-yAng, -zAng) + PI));
	  int yTilt = (int)(RAD_TO_DEG * (atan2(-xAng, -zAng) + PI));
	  int zTilt = (int)(RAD_TO_DEG * (atan2(-yAng, -xAng) + PI));

	  xOrientation = orient(xTilt);			// these represent number of degrees off from being level
	  yOrientation = orient(yTilt);

	  lprintf("x:%3d|xAng:%3d|xRead:%4d|y:%3d|yAng:%3d|yRead:%4d|z:%3d|zAng:%3d|zRead%4d  \n",
		  xOrientation, xAng, xRead, yOrientation, yAng, yRead, zTilt, zAng, zRead);
}

// translates orientation values to a usable range of -90 to 90 about level (lying flat) that avoids immediate
// "jump-across" if player goes past either
int orient(int n) {
	  if (n <= 180) n *= -1;
	  else n = cmap(n,181,360,179,0);
	  return constrain(n,-90,90);
}

// Logging print formatted = printf() to Serial
void lprintf(char *fmt, ... ) {        //  Source:  http://playground.arduino.cc/Main/Printf
	if (debugModeOn) {
		char tmp[128];                       //  resulting string limited to 128 chars
		va_list args;
		va_start (args, fmt );
		vsnprintf(tmp, 128, fmt, args);
		va_end (args);

		logCounter++;
		Serial.print("Arduino serial log entry ");  
		Serial.print(logCounter);
		Serial.print(":  ");
		Serial.println(tmp);
	}
}

// TODO need to revisit this function
void fps(long usecs) {      // factors in frame rate for refreshing screen by subtracting difference in current and last
                            // time recorded from desired refresh delay; usecs = desired delay, micro() gives time since
                            // uC was started last in useconds; for delays longer than 16ms use delay();
	  long tmp = usecs - ((long)micros() - time);
	  if ( tmp < 0) tmp = 0;
	  delayMicroseconds((unsigned int)tmp);
	  time = (long)micros();
}

// Constrained map() - adds 1 to in_high and out_high to balance distribution and constrains output range
// see: http://www.jetmore.org/john/blog/2011/09/arduinos-map-function-and-numeric-distribution/
int cmap(int value, int from_low, int from_high, int to_low, int to_high) {
	  return constrain(map(value,
						  (from_low > from_high ? from_low + 1 : from_low),
						  (from_high > from_low ? from_high + 1 : from_high),
						  (to_low > to_high ? to_low + 1 : to_low),
						  (to_high > to_low ? to_high + 1 : to_high)),
					  (to_low < to_high ? to_low : to_high),
					  (to_low < to_high ? to_high : to_low));
}

// For debugging purposes - shows display in 8x8 text with space before and after and size of display[][]
void logScreen() {
	Serial.println();
	Serial.println(sizeof(display));
	for (int i=0; i<10; i++) {
		for (int j=0; j<10; ++j) {
			if (display[i][j]==0)
			Serial.print(".");
			else Serial.print((char)display[i][j]);
		}
		Serial.println();
	}
	Serial.println();
}

