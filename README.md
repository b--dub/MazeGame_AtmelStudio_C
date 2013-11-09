MazeGame_AtmelStudio_C
======================

Maze Game for Arduino using shift registers, accelerometer and RED 8x8 LED Matrix

Arduino powered Maze Game with 2x 595 8 bit Shift Registers chained together to control Rows and Columns of a single color 8x8 LED Matrix.  A GY-61 5 pin 3 axis accelerometer is used to control the direction and speed of the player on the display.  I may end up adding some sound later with a piezo transducer if I don't get sidetracked onto another project first.  :)

As it sits now, the "Maze Game" doesn't actually have an end point, and the board that I've created is more a demo to show speed, maneuverability, and board size than to provide any sort of real maze to work through.  This could easily be changed by altering the board.h 2D array to create a more appropriate board and by adding a check for 'E' or other appropriate character during the drawScreen() process to satisfy ending the game.

The Fritzing based wiring schematics are located in the Fritzing_Schematics folder
There is a short 'Quick and Dirty' video demo in the Video folder as well
