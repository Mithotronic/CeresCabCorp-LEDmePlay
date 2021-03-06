// Ceres Cab Corp
// for the LEDmePlay
//
// www.mithotronic.de
//
// Version: 1.0.0
// Authors: Michael Rosskopf (2021)
// 
// Release Notes:
// V1.0.0: First release

// Include libraries for adafruit matrix
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <avr/pgmspace.h> // Necessary in order to maintain the maze data in program memory

// General settings
const byte initialNumberOfLives = 3;             // Number of lives at game start
const int fuelMax = 1024;                        // Maximal fuel (this is also the initial value)
const byte numberOfPassengersToFinishLevel = 5;  // Passengers per level
const byte maximalNumberOfWaitingPassengers = 3; // Maximal number of waiting passengers
const boolean hiddenLevelJumps = false;          // Allows to select any level by first moving left/right followed by pressing fire button

// Setup adafruit matrix
#define CLK 50
#define OE  51
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

// Audio out
int audio = 2;

// Define notes
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

// Joystick 1
int buttonU1 = 30;
int buttonD1 = 32;
int buttonL1 = 34;
int buttonR1 = 36;
int buttonFire1 = 38;
int analogX1 = 8;
int analogY1 = 9;

// Joystick 2
int buttonU2 = 31;
int buttonD2 = 33;
int buttonL2 = 35;
int buttonR2 = 37;
int buttonFire2 = 39;
int analogX2 = 10;
int analogY2 = 11;

// Sensitivity of analog thumb joysticks (of the LEDmePlay Joypad) in case of "digital usage" (detects movement if deviates from center position value of 512 by sensitivity value)
const int sensitivity = 192;

// Other buttons
int buttonReset = 42;
int buttonPause = 43;

// Define maps
//
// 1st value: Number of tiles in x-direction
// 2nd value: Number of tiles in y-direction
// 3rd value: Level color: 0 == Invisible, 1 == Blue, 2 == Green, 3 == Turquoise, 4 == Red, 5 == Violet, 6 == Yellow, 7 == White
//
//   type 0     type 1     type 2     type 3     type 4     type 5     type 6     type 7     type 8
//   --------   --------   --------   O-------   -------O   O-------   -------O   O-------   -------O
//   --------   --------   --------   O-------   -------O   O-------   -------O   -O------   ------O-
//   --------   --------   --------   O-------   -------O   O-------   -------O   --O-----   -----O--
//   --------   --------   OOOOOOOO   O-------   -------O   O-------   -------O   ---O----   ----O---
//   --------   --------   --------   O-------   -------O   O-------   -------O   ----O---   ---O----
//   --------   --------   --------   O-------   -------O   O-------   -------O   -----O--   --O-----
//   --------   --------   --------   O-------   -------O   O-------   -------O   ------O-   -O------
//   --------   OOOOOOOO   --------   O-------   -------O   OOOOOOOO   OOOOOOOO   -------O   O-------
//
// The remaining numbers encode the tiles, extra lives, platforms, and dangers.
// Add tile type value (0 - 8)
// to values of
// Extra life:               10 //
// Platform:                 20 //
// Extra gas station:        30 //
// Other taxi flying left:   40 //
// Other taxi flying right:  50 //
// Container:                60 //
// Mech:                     70 //
// Fire:                     80 //
// Gripper:                  90 //
// Pirate:                  100 //
// Missile:                 110 //
// Drone:                   120 //
// Plasma floor:            130 //
// Sports glider left:      140 //
// Sports glider right:     150 //
// Plasma ufo:              160 //
// Railcar with steel:      170 //
// Meteor:                  180 //
//
// Play start position      240
//
// Example: Tile 1 with mech (value 70) is encoded by 71.
//
const byte numberOfLevels = 20;
byte maximalReachedLevel = 1;
const uint8_t levels[] PROGMEM  = {
                                         // Level 1: First challenge
                                         6, 6, 1,
                                         0,   0,   0,   0,   0,   0,
                                        21,   0,   0,   0,   0,  21,
                                        50,   0,   0,   0,   0,   0,
                                         0,   0,   1,   1,   0,   0,
                                         0,   0,   0,   0,   0,  40,
                                       241,   1,   1,   1,   1,  21,
                                         
                                         // Level 2: Container harbor
                                         7, 5, 2,
                                         3,   0,  60,   0,  60,   0,   4,
                                         3,   0,   0,  21,   0,   0,   4,
                                        50,   0,   0,   0,   0,   0,   0,
                                         3,   0,   0, 241,   0,   0,   4,
                                         5,  21,   1,   1,   1,  21,   6,

                                         // Level 3: Cave
                                         8, 8, 3,
                                         0,   0,   0,   0,   0,   0,   0,   0, 
                                         0,   0,   0,   0,   0,   0,   1,   1, 
                                       241,   0,   0,   0,  21,   8,  60,  31, 
                                         0,   0,   0,   0,   8,   0,   0,   0, 
                                         0,   0,  21,   8,  60,  21,   0,   1, 
                                         0,   0,   8,   0,   0,   0,   0,   0, 
                                         0,   8,   0,  21,   0,   1,   0,   1,                                           
                                         1,   1,   1,  71,   1,   1,   1,  11,

                                         // Level 4: Three Stations
                                         14, 6, 6,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0, 150,   0,   0,   0,   0,   0,   0, 150,
                                         0,   0,  21,   8,   7,   0,  21,   8,   7,   0,  21,   8,   7,   0,
                                       241,   0,   0,   7,   8,   0,   0,   7,   8,   0,   0,   7,   8,   0,
                                       140,   0,   0,   0,   0,   0,   0, 140,   0,   0,   0,   0,   0,   0,
                                         1,   1,   1,   1,   1,  81,   1,   1,   1,  81,   1,   1,   1,   1,
                                     
                                         // Level 5: Around and around
                                         10, 10, 4,                                         
                                         3,   0,   5,   1,   1,   1,   1,   6,   0,   4,
                                         3,  60,   0,   0,   0,   0,  90,   0,   0,   4,
                                         3,   0, 241,   5,  21,   1,   6,  21,   0,   4,
                                         5,   0,   3,   0,   0,   0,   0,   4,   0,   6,
                                         3,   0,   3,   0,   0,   0,   0,   4,   0,   4,
                                         3,   0,   3,   0,  10,   0,   0,   0,   0,   4,
                                         5,   0,   3,   0,   0,   0,   0,   4,   0,   6,
                                         3,   0,   5,   1,   1,   1,   1,   6,   0,   4,
                                         3,   0,   0,  90,   0,   0,   0,   0,  60,   4,
                                         5,   1,  21,   5,   1,  21,   6,  31,   1,   6,

                                         // Level 6: Electric Fire
                                         8, 8, 5,
                                         0,   0,   0,   0,   0,   0,   0,   0,
                                        21,   0,   0, 241,   1,   0,   0,  21,
                                         0,   0,   0,   3,   4,   0,   0,   0,
                                       160,   0,   0,   3,   4, 160,   0,   0,
                                         0, 160,   0,   3,   4,   0, 160,   0,
                                         0,   0,   0,   5,   6,   0,   0,   0,
                                         0,   0,   0,   1,  81,   0,   0,   0,
                                         1,  21,   1,   3,   4,   1,  21,   1,
                                                                              
                                         // Level 7: Mech Warrior
                                         9, 8, 1,
                                         0,   0,   0,   3, 241,   4,   0,   0,   0,
                                         0,  21,  71,   1,   1,   1,   1,  21,   0,
                                         1,   0,   0,   1,   0,   1,   0,  90,   1,
                                         1,   1,   1,   1, 130,   1,   1,   1,   1,
                                         3,   0,   1,   1,   1,   1,   1,   0,   4,
                                         3,   0,   0,   1,  31,   1,   0,   0,   4,
                                         3,   0,   0,   0,   0,   0,   0,   0,   4,
                                         5,  21,   1,  71,  11,   1,   1,  21,   6,

                                         // Level 8: Freeway
                                         15, 6, 7,
                                         3,  21,   4,   1,   1,   1,   3,  31,   4,   1,   1,   1,   3,  21,   4,
                                        40,   0,   0,   0,   0,   0,   0,  40,   0,   0,   0,   0,   0,   0,   0,
                                         0,   1,   1, 141,   1,   1,   1,   1,   1,   1, 141,   1,   1,   1,   0,
                                         0,   0,   0,   0, 150,   0,   0,   0,   0,   0,   0, 150,   0,   0,   0,
                                         0,   1,   0,  51,   1,   1,   0,   1,   0,   1,  51,   1,   0,   1,   0,
                                         5,  21,   6,   0,   0,   0,   5, 241,   6,   0,   0,   0,   5,  21,   6,

                                         // Level 9: Abyss
                                         6, 11, 3,
                                         0,   0,   0,   0,   0, 100,
                                       241,   0,   0,   0,   0,   1,
                                         4,   0,   0,   0,  21,   3,
                                         4,   0,   0,   0,   0,   3,
                                         4,   0,  21,   0,   0,   3,
                                         4,   0,   0,   0, 160,   3,
                                         4,   0,   0,  21,   0,   3,
                                         4,   0,   0,   0,   0,   3,
                                         4,  21,   0,   0,   0,   3,
                                         4,  10,   0,   1,   0,   3,
                                         4,  71,   1,   1,  31,   3,

                                         // Level 10: Drone Wars
                                         13, 13, 6,
                                         3, 120,   7,   0,   0,   8, 120,   7,   0,   0,   8, 120,   4,
                                         3,   0,   0,   7,   8,   0,  31,   0,   7,   8,   0,   0,   4,
                                        21,   0,   0,   0,   0,   8,   0,   7,   0,   0,   0,   0,  21,
                                         0,   7,   0,   0,   0,   7,   1,   8,   0,   0,   0,   8,   0,
                                         0,   8,   0,   0,   0,   0,   0,   0,   0,   0,   0,   7,   0,
                                         8,   0,   8,   7,   0,   0,   0,   0,   0,   8,   7,   0,   7,
                                         3,   4,   0,   0,   3,   0,   0,   0,   4,   0,   0,   3,   4,
                                         7,   0,   7,   8,   0,   0,   0,   0,   0,   7,   8,   0,   8,
                                         0,   7,   0,   0,   0,   0, 241,   0,   0,   0,   0,   8,   0,
                                         0,   8,   0,   0,   0,   8,   0,   7,   0,   0,   0,   7,   0,
                                         8,   0,   0,   0,   0,   7,   1,   8,   0,   0,   0,   0,   7,
                                         3,   0,   0,   8,   7,   0,   0,   0,   8,   7,   0,   0,   4,
                                         7,  21,   8,   0,   0,   7,   1,   8,   0,   0,   7,  21,   8,
                                         
                                         // Level 11: Metal Works
                                         9, 8, 4,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,  21,   1,   1,   1, 171, 130,   1,
                                       241, 130,   3,  60,   0,   0,   4,   1,  11,
                                         0,   0,   0,   0,   0,   1,   1,  90,   0,
                                         0,   0,   0,   0,   1,   5,   6,   1,  21, 
                                         1,   1,  60,   0,   0,  60,   0,   0,   0,
                                        21,   4,   1,   1,   0,   0,   0,   1,  31, 
                                       171,   1,   1,   1,   1,   1,   1,   1,  21,
                                        
                                         // Level 12: Frogger
                                         9, 16, 2,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0,  21,   0,   0,   0,   0,   0,  21,   0,
                                         0,   0,   0,   0,  31,   0,   0,   0,   0,
                                        50,   0,   0,   0,   0,   0,   0,   0,   0,
                                       150,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,  40,   0,   0,   0,  40,   0,   0,
                                         0,   0,   0,   0, 150,   0,   0,   0,   0,
                                        40,   0,   0,   0,  40,   0,   0,   0,   0,
                                        50,   0,   0,   0,  50,   0,   0,   0,   0,
                                         0,   0,   0,   0, 140,   0,   0,   0,   0,
                                         0,   0,  50,   0,   0,   0,  50,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0, 140,
                                         0,   0,   0,   0,   0,   0,   0,   0,  40,
                                         0,   0,   0,   0, 241,   0,   0,   0,   0,
                                         0,  21,   0,   0,   0,   0,   0,  21,   0,
                                         1,   1,   1,   1,   1,   1,   1,   1,   1,

                                         // Level 13: Donkey Kong
                                         8, 12, 3,
                                         0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,   0,  71,  11,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,  71,   1,   1,  31,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,
                                         0,  71,   1,   1,   1,   1,  21,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,
                                         1,  21,   1,   1,   1,   1,   1,   0,
                                         1,   1,   0,   1, 161,   1,   1,   1,
                                         1,   1,   1,   1,   1,   0,  21,   1,
                                       161,   1,   0,   1,   1,   1,   1,   1,
                                         1,  21,   1,   1,   1,   1, 241,   1,
                                         
                                         // Level 14: Missile attack
                                         11, 7, 5, 
                                         0, 110,   0,   0,   0,   0,   0,   0,   0,   0,  21,
                                         0,   0, 110,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,   0,   0, 110,   0,   0,  21,   0,   0,  31,
                                         0,   0,   0,   0,   0, 110,   0,   0,   0,   0, 120,
                                       241,   0,   0,  21,   0,   0, 110,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0, 110,   0,   0,
                                        21,   0,   0,   1,   0,   0,   0,   1,   0, 110,   1,
                                                                           
                                         // Level 15: Pharao
                                         16, 12, 6,
                                         0,   0, 180,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   8,   7,   0,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,   8,  31,   0,   7,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   8,  21,   0,   1,   0,   7,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   8,   0,   0,   1,   0,   0,  21,   7,   0,   0,                                         
                                         0,   0,   0, 180,   0,   8,  21,   0,   0, 160,   1,   0,   0,   0,   7,   0,
                                         0,   0,   0,   0,   8,   0,   0,   0,   0, 161,   0,   0,   0,   0,  21,   7,
                                       241,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   1,   1,   1,   1,   6,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   0,   3,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   0,   3,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   1,   1,   1,   1,   6,   0,   5,   1,   1,   1,   1,
                                         0,   0,   0,   0,   0,   5,  11,   1,   1,   1, 171,   1,   1,   1,  31,   6,

                                         // Level 16: Holes
                                        13, 7, 1,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 140,
                                         0,   0, 241,   0,   0,   0,  21,   0,   0,   0,  31,   0,   0,
                                       151, 130,   1,   1, 130,   1,   1,   1, 130,   1,   1, 130,   1,
                                         4,   0,   3,   4,   0,   3,   0,   4,   0,   3,   4,   0,   3,
                                         4,   0,   3,   4,   0,   3,   0,   4,   0,   3,   4,   0,   3,
                                         4,   0,   3,   4,   0,   5,   1,   6,   0,   3,   4,   0,   3,
                                         4,  21,   3,   4,  21,   1,  81,   1,  21,   3,   4,  21,   3,

                                         // Level 17: Meteor Storm
                                         16, 8, 7,
                                         0,   0,   0,   0,   0, 180,   0,   0, 180,   0, 180,   0, 180,   0,   0,  10,
                                       150,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 140,
                                         0,   0,   0,   0,   0,   0,   0,  21,   0,   0,   0,   0,   0,  21,   0,   0,
                                       241,   0,   0,   0,   0,   0,   8,   0,   7,   0,   0,   0,   8,   0,   7,  21,
                                         0,   7,   0,   0,   0,   8,   0,   0,   0,   7,  31,   8,   0,   0,   0,   0,
                                         0,   0,   7,  21,   8,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,

                                         // Level 18: Outside
                                         15, 10, 2,
                                         0,   0, 180,   0,   0,   0, 100,   0,   0,   0, 180,   0,   0,   0,   0,  
                                        21,  81,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  21,  81,   1,   0,  
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  
                                         0,   0,   0,   1,  81,  21,   0,   0,   0,   0,   0,   0,   0,   0,   0,  
                                       241,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  21,  
                                         0,   0,   0,   7,   0,   0,   0,   0,   0,   0,   0,   0,   0,   8,   0,  
                                         0,   0,   0,   0,   7,   0,   0,   0,   0, 110,   0,   0,   8,   0,   0,  
                                         0,   0,   0,   0,   0,   7,   0,   0, 110,   0,   0,   8,   0,   0,   0,  
                                         0,   0,   0,   0,   0,   0,   7, 110,   0,   0,   8,   0,   0,   0,   0,  
 
                                         // Level 19: Factory
                                         12, 12, 4,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0, 241,   1,   1,   1,   0,   0,   1,   1,   1,  21,   0,
                                         0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   1,   1,   1,  21, 160,
                                         0,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0, 110,   0,   0,   0,   0,   0,
                                         0,  21,   1,   1,   1, 160,   0,   1,   1,   1,  31,   1,
                                         0,   0,   0,   0,   0,   0,   0,  90,   0,   0,   0,   0,
                                         0,   0,   0,   0,   0,   0,   0,   1,   1, 171,   1,  21,
                                         1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,  10,
                                        90,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
                                        21,   1, 171,   1,   1, 110,   0,   1,   1,   1,   1,  31,
                                         
                                         // Level 20: Space Pirates
                                         16, 16, 5,
                                         3, 100,   0, 120,   0,   0, 120,   0,   0, 120,   0,   0, 120,   0, 100,   4,
                                         3,   0,   0,   0,   0,  21,   0,   0,   0,   0,  31,   0,   0,   0,   0,   4,
                                         3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,
                                         3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,
                                         3,  21,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  21,   4,
                                         3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,
                                         3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,
                                         3,  10,   0,   0,   0,   0,   0,   1,   1,   0,   0,   0,   0,   0,  10,   4,
                                         3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,
                                         3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,
                                         3,  21,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  21,   4,
                                         3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,
                                         3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   4,
                                         3,   0,   0,   0,   0, 241,   0,   0,   0,   0,  21,   0,   0,   0,   0,   4,
                                         3, 100,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 100,   4,
                                         5,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   6
                                  };
                                  
byte levelMap[256]; // Contains the current level after loading from PROGMEM

byte tileNumberX; // Number of tiles in X-direction of the current map
byte tileNumberY; // Number of tiles in Y-direction of the current map
byte mapWidth;    // Number of pixels in X-direction of the current map (tiles * 8)
byte mapHeight;   // Number of pixels in Y-direction of the current map (tiles * 8)

// Level color
byte levelColor;  // Level platform color: 0 == Invisible, 1 == Blue, 2 == Green, 3 == Turquoise, 4 == Red, 5 == Violet, 6 == Yellow, 7 == White
byte red;
byte green;
byte blue;

// Playfield
// 0: Nothing
// 1: Obstacle
// 2: Danger
// 3: Gas station
// 4-9: Platform
// 10-: Extra lives
byte playfield[48][48]; // Visible buffer on screen

// Position of the screen (screenX and screenY define the upper left position of the complete map)
int screenX;
int screenY;
int screenXNew; // New X position after movement
int screenYNew; // New Y position after movement
boolean redraw; // Update of screen if true

// Positions of the upper left tile which is visible on the screen (all others tiles are drawn relative to it to realize the scrolling)
int upperLeftTileX;
int upperLeftTileXOffset; // Number of pixels of upper left tile in X-direction which are not visible (out of screen)
int upperLeftTileY;
int upperLeftTileYOffset; // Number of pixels of upper left tile in Y-direction which are not visible (out of screen)

// Taxi
byte lives;               // Remaining lives
byte level;               // Current level
int fuel;                 // Remaining fuel
boolean refuling;
int passengers;           // Counts the delivered passengers
int taxiXScreen;          // X position on screen
int taxiYScreen;          // Y position on screen
int taxiXScreenNew;       // New X position after movement
int taxiYScreenNew;       // New Y position after movement
float taxiXMap;           // X position on map
float taxiYMap;           // Y position on map
float taxiXMapNew;        // New X position after movement
float taxiYMapNew;        // New Y position after movement
boolean taxiParks;        // true if taxi waits on a platform
boolean footstepSound;    // Sound of embarking/disembarking passenger

#define LEFT  0
#define RIGHT 1
boolean taxiDirection;    // 0 == looking right, 1 == looking left
boolean taxiDirectionNew; // New direction after movement

float xSpeed;
float ySpeed;
float xStepCounter;
float yStepCounter;

boolean mainThrusters;
boolean breakThrusters;
boolean hoverThrusters;
boolean downThrusters;

boolean heavyCollisionWithObstacle;
boolean taxiExplosion;

// X and Y Positions of the body parts after death
float todesanimationX[5];
float todesanimationY[5];

// Dangers
int dangerXScreen[16];    // X position on screen
int dangerYScreen[16];    // Y position on screen
int dangerXScreenNew[16]; // New X position after movement
int dangerYScreenNew[16]; // New Y position after movement
float dangerXMap[16];     // X position on map
float dangerYMap[16];     // Y position on map
byte dangerType[16];      // Danger type (1 - 16)
// Some dangers move between two points (X1, Y1) and (X2, Y2). The dangerMovement defines the direction (depends on danger type).
byte dangerMovement[16];
int dangerX1[16];
int dangerY1[16];
int dangerX2[16];
int dangerY2[16];
byte dangerStatus[16]; // 0 == inactive, 1 == active
byte dangerCounter;    // Number of dangers in current level (16 is the maximum)

// Platforms
#define RED       1
#define GREEN     2
#define BLUE      3
#define YELLOW    4
#define VIOLET    5
#define TURQUOISE 6
int platformXScreen[6];                // X position on screen
int platformYScreen[6];                // Y position on screen
int platformXScreenNew[6];             // New X position after movement
int platformYScreenNew[6];             // New Y position after movement
float platformXMap[6];                 // X position on map
float platformYMap[6];                 // Y position on map
byte platformColor[6];                 // RED, GREEN, BLUE, YELLOW, VIOLOET, TURQUOISE
byte platformBordingStatus[6];         // 0 == inactive, 1 == empty, 2 == passenger is waiting, 3 == start bording, 4-6 == bording animation
byte platformDisembarkingStatus[6];    // 0 == inactive, 1 == empty, 2 == passenger is waiting
byte platformCounter;                  // Number of platforms in current level (6 is the maximum)
byte numberOfWaitingPassengers;
byte remainingPassengersToFinishLevel; // Jump to next level if 0
byte targetPlatform;                   // RED, GREEN, BLUE, YELLOW, VIOLOET, TURQUOISE

// Gas stations
int gasStationXScreen[4];              // X position on screen
int gasStationYScreen[4];              // Y position on screen
int gasStationXScreenNew[4];           // New X position after movement
int gasStationYScreenNew[4];           // New Y position after movement
float gasStationXMap[4];               // X position on map
float gasStationYMap[4];               // Y position on map
boolean gasStationStatus[4];           // false == inactive, true == active
byte gasStationCounter;                // Number of gas stations

// Extra lives
int extraLifeXScreen[16];              // X position on screen
int extraLifeYScreen[16];              // Y position on screen
int extraLifeXScreenNew[16];           // New X position after movement
int extraLifeYScreenNew[16];           // New Y position after movement
float extraLifeXMap[16];               // X position on map
float extraLifeYMap[16];               // Y position on map
byte extraLifeStatus[16];              // 0 == inactive, 1 == active, 2 == just collected
byte extraLifeCounter;                 // Number of extra lives in current level (16 is the maximum)

boolean initializeNewLevel;            // true, if a new level is initialized
boolean showInitialStatus;             // true, if the game is started (when the initial status is shown)

// Game synchronization
unsigned long engineLoopStartPoint; // Contains millisconds since start of LEDmePlay at begin of each engine loop (every 20ms)

// Some helpers
int i, j, k, x1, y1, x2, y2;
float x, y, z;
boolean changed;       // Helper used in the status screen

byte animationCounter; // Counts up till 255 and starts again with 0 (animations are synchronized to it)
byte animationCounterSyncValue;
byte audioOffUntil;    // If an extra life is collected, other sounds are muted in order to hear a longer "collected" sound.
boolean reset;         // true if reset button has been set


// Function declarations
void setup();
boolean joy1Up();
boolean joy1Down();
boolean joy1Left();
boolean joy1Right();
float joy1XValue();
float joy1YValue();
boolean joy1Fire();
boolean joy1FireA();
boolean joy1FireB();
boolean joy1IsLEDmePlayJoypad();
boolean joy2Up();
boolean joy2Down();
boolean joy2Left();
boolean joy2Right();
float joy2XValue();
float joy2YValue();
boolean joy2Fire();
boolean joy2FireA();
boolean joy2FireB();
boolean joy2IsLEDmePlayJoypad();
void drawM(int x, int y);
void drawT(int x, int y);
void mithotronic();
void setLEDMePlayColor(int i);
void ledMePlay();
void title();
void setupGame();
void setupLevel();
void drawMiniMap();
void updateLevelColors();
void drawScreen();
void drawTile(byte _type, int _x, int _y);
void clearTile(byte _type, int _x, int _y);
void drawDangers(byte i);
void moveDangers();
void initializeDangerMovement();
void drawExtraLives();
void drawPlatforms();
void drawGasStations();
void drawTaxi();
boolean directionLeftFree();
boolean directionRightFree();
boolean directionUpFree();
boolean directionDownFree();
void drawThrusters();
void moveTaxi();
byte collisionDetection();
boolean checkForLosingLive();
void checkForFuelPlatformsExtraLives();
void showStatus();
void taxiExplodes();
void endSequence();
void loop();


// Program setup  
void setup() 
{
  // Initialize serial connection
  Serial.begin(9600);

  // Initialize joysticks and  buttons
  pinMode(buttonL1, INPUT);     
  pinMode(buttonR1, INPUT);     
  pinMode(buttonU1, INPUT);     
  pinMode(buttonD1, INPUT);     
  pinMode(buttonFire1, INPUT);     
  pinMode(buttonL2, INPUT);     
  pinMode(buttonR2, INPUT);     
  pinMode(buttonU2, INPUT);     
  pinMode(buttonD2, INPUT);     
  pinMode(buttonFire2, INPUT);     
  pinMode(buttonReset, INPUT);     
  pinMode(buttonPause, INPUT);    

  // Activate internal pull-up resistors 
  digitalWrite(buttonL1, HIGH);
  digitalWrite(buttonR1, HIGH);
  digitalWrite(buttonU1, HIGH);
  digitalWrite(buttonD1, HIGH);
  digitalWrite(buttonFire1, HIGH);
  digitalWrite(buttonL2, HIGH);
  digitalWrite(buttonR2, HIGH);
  digitalWrite(buttonU2, HIGH);
  digitalWrite(buttonD2, HIGH);
  digitalWrite(buttonFire2, HIGH);
  digitalWrite(buttonReset, HIGH);
  digitalWrite(buttonPause, HIGH);

  // Initialize matrix and define text mode
  matrix.begin();
  matrix.setTextSize(1);
  matrix.setTextWrap(false);

  // Enter the game loop
  loop();
}

// Joystick inquiry (allows to use a classic joystick or a LEDmePlay Joypad without any configuration)
boolean joy1Up()
{
  if((digitalRead(buttonU1) == LOW && digitalRead(buttonD1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogY1) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy1Down()
{
  if((digitalRead(buttonD1) == LOW && digitalRead(buttonU1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogY1) < (512 - sensitivity))){ return true; }
  return false;
}
boolean joy1Left()
{
  if((digitalRead(buttonL1) == LOW && digitalRead(buttonR1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogX1) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy1Right()
{
  if((digitalRead(buttonR1) == LOW && digitalRead(buttonL1) != LOW) || (digitalRead(buttonL1) == LOW && digitalRead(buttonR1) == LOW && analogRead(analogX1) < (512 - sensitivity))){ return true; }
  return false;
}
boolean joy1Fire()
{
  if(digitalRead(buttonFire1) == LOW || (digitalRead(buttonU1) == LOW && digitalRead(buttonD1) == LOW)){ return true; }
  return false;
}
boolean joy1FireA()
{
  if(digitalRead(buttonFire1) == LOW){ return true; }
  return false;
}
boolean joy1FireB()
{
  if(digitalRead(buttonU1) == LOW && digitalRead(buttonD1) == LOW){ return true; }
  return false;
}
boolean joy2Up()
{
  if((digitalRead(buttonU2) == LOW && digitalRead(buttonD2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogY2) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy2Down()
{
  if((digitalRead(buttonD2) == LOW && digitalRead(buttonU2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogY2) < (512 - sensitivity))){ return true; }
  return false;
}
boolean joy2Left()
{
  if((digitalRead(buttonL2) == LOW && digitalRead(buttonR2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogX2) > (512 + sensitivity))){ return true; }
  return false;
}
boolean joy2Right()
{
  if((digitalRead(buttonR2) == LOW && digitalRead(buttonL2) != LOW) || (digitalRead(buttonL2) == LOW && digitalRead(buttonR2) == LOW && analogRead(analogX2) < (512 - sensitivity))){ return true; }
  return false;
}
boolean joy2Fire()
{
  if(digitalRead(buttonFire2) == LOW || (digitalRead(buttonU2) == LOW && digitalRead(buttonD2) == LOW)){ return true; }
  return false;
}
boolean joy2FireA()
{
  if(digitalRead(buttonFire2)){ return true; }
  return false;
}
boolean joy2FireB()
{
  if(digitalRead(buttonU2) == LOW && digitalRead(buttonD2) == LOW){ return true; }
  return false;
}

// Draw the M of the Mithotronic logo
void drawM(int x, int y)
{
  matrix.fillRect(x + 2, y + 2, 6, 1, matrix.Color333(0, 0, 0));
  matrix.fillRect(x, y + 3, 10, 1, matrix.Color333(0, 0, 0));

  matrix.fillRect(x, y + 4, 2, 6, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 2, y + 3, 2, 2, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 4, y + 4, 2, 6, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 6, y + 3, 2, 2, matrix.Color333(3, 3, 3));
  matrix.fillRect(x + 8, y + 4, 2, 6, matrix.Color333(3, 3, 3));
}

// Draw the T of the Mithotronic logo
void drawT(int x, int y)
{
  matrix.fillRect(x, y + 5, 6, 1, matrix.Color333(0, 0, 0));
  matrix.fillRect(x + 2, y + 10, 3, 1, matrix.Color333(0, 0, 0));

  matrix.fillRect(x, y + 3, 6, 2, matrix.Color333(0, 0, 7));
  matrix.fillRect(x + 2, y, 2, 10, matrix.Color333(0, 0, 7));
  matrix.fillRect(x + 4, y + 8, 1, 2, matrix.Color333(0, 0, 7));
}

// Draw the animated Mithotronic logo and play jingle
void mithotronic()
{
  i = -10;
  do
  {
    drawM(7, i);
    drawT(19, 22 - i);
    i++;
    delay(50);
  }
  while(i <= 11);

  // Jingle on start screen
  tone(audio,NOTE_C4,200);
  delay(400+20);
  tone(audio,NOTE_C4,90);
  delay(200-20);
  tone(audio,NOTE_G4,140);
  delay(400+20);
  tone(audio,NOTE_G4,140);
  delay(200-20);
  tone(audio,NOTE_C5,450);
  delay(600);
  tone(audio,NOTE_AS4,140);
  delay(200-20);
  tone(audio,NOTE_A4,130);
  delay(200-10);
  tone(audio,NOTE_F4,120);
  delay(200);
  tone(audio,NOTE_G4,1000);
  delay(3000);
}

// Sets color for the next character to show the LEDmePLay logo
void setLEDMePlayColor(int i)
{
  switch(i % 9)
  {
    case 0:
    matrix.setTextColor(matrix.Color333(5,0,0));
    break;
    case 1:
    matrix.setTextColor(matrix.Color333(5,2,0));
    break;
    case 2:
    matrix.setTextColor(matrix.Color333(2,5,0));
    break;
    case 3:
    matrix.setTextColor(matrix.Color333(0,5,0));
    break;
    case 4:
    matrix.setTextColor(matrix.Color333(0,5,2));
    break;
    case 5:
    matrix.setTextColor(matrix.Color333(0,2,5));
    break;
    case 6:
    matrix.setTextColor(matrix.Color333(0,0,5));
    break;
    case 7:
    matrix.setTextColor(matrix.Color333(2,0,5));
    break;
    case 8:
    matrix.setTextColor(matrix.Color333(5,0,2));
    break;
  }
}

// Draw the LEDmePlay logo
void ledMePlay()
{
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  i = 0;
  do
  {
    // Write 'LEDmePlay'
    setLEDMePlayColor(i++);
    matrix.setCursor(7, 5);
    matrix.println("L");
    setLEDMePlayColor(i++);
    matrix.setCursor(13, 5);
    matrix.println("E");
    setLEDMePlayColor(i++);
    matrix.setCursor(19, 5);
    matrix.println("D");

    setLEDMePlayColor(i++);
    matrix.setCursor(10, 11);
    matrix.println("m");
    setLEDMePlayColor(i++);
    matrix.setCursor(16, 11);
    matrix.println("e");

    setLEDMePlayColor(i++);
    matrix.setCursor(4, 19);
    matrix.println("P");
    setLEDMePlayColor(i++);
    matrix.setCursor(10, 19);
    matrix.println("l");
    setLEDMePlayColor(i++);
    matrix.setCursor(16, 19);
    matrix.println("a");
    setLEDMePlayColor(i++);
    matrix.setCursor(22, 19);
    matrix.println("y");
    
    i++;
    if(i > 81)
    {
      i = 0;
    }

    j = 0;
    do
    {
      j++;
      delay(1);
    }
    while(j < 250 && !joy1Fire() && !joy2Fire());
  }
  while(!joy1Fire() && !joy2Fire());
  tone(audio,1024,20);
  delay(200);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
}

// title: Ceres Cab Corp title screen
void title()
{
  // Clear screen
  matrix.fillScreen(matrix.Color333(0, 0, 0));

  // Write 'Ceres Cab Corp.'
  matrix.setTextColor(matrix.Color333(4,2,0));
  matrix.setCursor(1, 0);
  matrix.println("Ceres");
  matrix.setCursor(1, 8);
  matrix.println(" Cab ");
  matrix.setCursor(1, 16);
  matrix.println("Corp.");

  // Moon
  matrix.drawLine(0, 27, 1, 27, matrix.Color333(4, 3, 1));
  matrix.drawLine(2, 26, 5, 26, matrix.Color333(4, 3, 1));
  matrix.drawLine(6, 25, 11, 25, matrix.Color333(3, 3, 1));
  matrix.drawLine(12, 24, 19, 24, matrix.Color333(3, 3, 1));
  matrix.drawLine(20, 25, 25, 25, matrix.Color333(3, 2, 1));
  matrix.drawLine(26, 26, 29, 26, matrix.Color333(3, 2, 1));
  matrix.drawLine(30, 27, 31, 27, matrix.Color333(3, 1, 1));

  // Craters
  matrix.drawLine(0, 29, 2, 29, matrix.Color333(3, 3, 1));
  matrix.drawLine(3, 30, 5, 30, matrix.Color333(3, 2, 1));
  matrix.drawLine(6, 31, 7, 31, matrix.Color333(3, 1, 1));
  
  matrix.drawLine(8, 27, 12, 27, matrix.Color333(4, 3, 1));
  matrix.drawPixel(7, 28, matrix.Color333(3, 3, 1));
  matrix.drawPixel(13, 28, matrix.Color333(3, 1, 1));
  matrix.drawLine(8, 29, 12, 29, matrix.Color333(3, 2, 1));

  matrix.drawLine(12, 31, 13, 31, matrix.Color333(4, 3, 1));
  matrix.drawLine(14, 30, 19, 30, matrix.Color333(3, 3, 1));
  matrix.drawLine(20, 31, 21, 31, matrix.Color333(3, 3, 1));

  matrix.drawLine(16, 26, 20, 26, matrix.Color333(4, 3, 1));
  matrix.drawPixel(15, 27, matrix.Color333(3, 3, 1));
  matrix.drawPixel(21, 27, matrix.Color333(3, 1, 1));
  matrix.drawLine(16, 28, 20, 28, matrix.Color333(3, 2, 1));

  matrix.drawLine(23, 28, 28, 28, matrix.Color333(4, 3, 1));
  matrix.drawPixel(22, 29, matrix.Color333(3, 3, 1));
  matrix.drawPixel(29, 29, matrix.Color333(3, 1, 1));
  matrix.drawLine(23, 30, 28, 30, matrix.Color333(3, 2, 1));

  matrix.drawLine(30, 31, 31, 31, matrix.Color333(4, 3, 1));

  do
  {
    delay(1);
  }
  while(!joy1Fire()); 
  tone(audio,1024,20);
  delay(200);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
  
  // Initialize random number generator
  randomSeed(analogRead(40));
}

// setupGame: Setup a new game and initialize all variables
void setupGame()
{
  lives = initialNumberOfLives;
  level = 1;
  passengers = 0;
  initializeNewLevel = true;
  showInitialStatus = true;
}

// setupLevel: Setup a new level and initialize all variables
void setupLevel()
{
  // If a live has been lost, the level is not initialized with extra lives (only on level start).
  if(initializeNewLevel)
  {
    for(i = 0; i < 16; i++)
    {
      extraLifeXScreen[i] = 0;
      extraLifeYScreen[i] = 0;
      extraLifeXScreenNew[i] = 0;
      extraLifeYScreenNew[i] = 0;
      extraLifeXMap[i] = 0;
      extraLifeYMap[i] = 0;
      extraLifeStatus[i] = 0;
    }
    extraLifeCounter = 0;
    remainingPassengersToFinishLevel = numberOfPassengersToFinishLevel;
  
    for(i = 0; i < 6; i++)
    {
      platformXScreen[i] = 0;
      platformYScreen[i] = 0;
      platformXScreenNew[i] = 0;
      platformYScreenNew[i] = 0;
      platformXMap[i] = 0.0;
      platformYMap[i] = 0.0;
      platformColor[i] = 0;
      platformBordingStatus[i] = 0;
      platformDisembarkingStatus[i] = 0;    
    }
    platformCounter = 0;
  }
  targetPlatform = 0;
  
  for(i = 0; i < 4; i++)
  {
    gasStationXScreen[i] = 0;
    gasStationYScreen[i] = 0;
    gasStationXScreenNew[i] = 0;
    gasStationYScreenNew[i] = 0;
    gasStationXMap[i] = 0.0;
    gasStationYMap[i] = 0.0;
    gasStationStatus[i] = false;
  }
  gasStationCounter = 0;
    
  // Reset dangers
  for(i = 0; i < 16; i++)
  {
    dangerXScreen[i] = 0;
    dangerYScreen[i] = 0;
    dangerXScreenNew[i] = 0;
    dangerYScreenNew[i] = 0;
    dangerXMap[i] = 0;
    dangerYMap[i] = 0;
    dangerType[i] = 0;
    dangerMovement[i] = 0;
    dangerX1[i] = 0;
    dangerY1[i] = 0;
    dangerX2[i] = 0;
    dangerY2[i] = 0;
    dangerStatus[i] = 0;
  }
  dangerCounter = 0;
  
  // Copy level map and dangers from progmem
  k = 0;
  for(i = 0; i < level; i++)
  {
    tileNumberX = byte(pgm_read_byte_near(levels + k));
    tileNumberY = byte(pgm_read_byte_near(levels + 1 + k));
    levelColor = byte(pgm_read_byte_near(levels + 2 + k));
    updateLevelColors();
    
    if(i < level - 1)
    {
      k = k + ((tileNumberX * tileNumberY) + 3);
    }
  }

  mapWidth = tileNumberX * 8;
  mapHeight = tileNumberY * 8;

  for(i = 0; i < (tileNumberX * tileNumberY); i++)
  {
    j = byte(pgm_read_byte_near(levels + 3 + i + k));
    levelMap[i] = j % 10; // Returns tile type
    j = j / 10; // Returns start position, extra life, platform or danger
    
    // Set starting position of taxi
    if(j == 24)
    {
      taxiXMap = ((i % tileNumberX) * 8) + 2;
      taxiYMap = ((i / tileNumberX) * 8) + 2;
      taxiXScreen = 14;
      taxiYScreen = 14;

      if(taxiXMap > 13)
      {
        screenX = taxiXMap - 14;
        if(screenX > mapWidth - 32)
        {
          screenX = mapWidth - 32;
          taxiXScreen = 32 - (mapWidth - taxiXMap);
        }
      }
      else
      {
        screenX = 0;
        taxiXScreen = taxiXMap;
      }
      
      if(taxiYMap > 13)
      {
        screenY = taxiYMap - 14;
        if(screenY > mapHeight - 32)
        {
          screenY = mapHeight - 32;
          taxiYScreen = 32 - (mapHeight - taxiYMap);
        }
      }
      else
      {
        screenY = 0;
        taxiYScreen = taxiYMap;
      }
    }
        
    // Set extra life
    if(j == 1 && initializeNewLevel)
    {
      extraLifeXMap[extraLifeCounter] = ((i % tileNumberX) * 8) + 2;
      extraLifeYMap[extraLifeCounter] = ((i / tileNumberX) * 8) + 3;
      extraLifeStatus[extraLifeCounter] = 1;
      if(extraLifeCounter < 15)
      {
        extraLifeCounter++;
      }
    }
    
    // Set platform
    if(j == 2 && initializeNewLevel)
    {
      platformXMap[platformCounter] = ((i % tileNumberX) * 8);
      platformYMap[platformCounter] = ((i / tileNumberX) * 8) + 5;
      if(platformCounter < 6)
      {
        platformCounter++;
      }
    }

    // Set gas station
    if(j == 3 || j == 24) // Taxi always starts at a gas station platform
    {
      gasStationXMap[gasStationCounter] = ((i % tileNumberX) * 8) + 1;
      gasStationYMap[gasStationCounter] = ((i / tileNumberX) * 8) + 5;
      gasStationStatus[gasStationCounter] = true;
      if(gasStationCounter < 4)
      {
        gasStationCounter++;
      }
    }

    // Set danger: Other taxi flying left
    if(j == 4)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 2;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 2;
      dangerType[dangerCounter] = 2;
      dangerMovement[dangerCounter] = 1;
      dangerStatus[dangerCounter] = 1;
    }

    // Set danger: Other taxi flying right
    if(j == 5)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 2;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 2;
      dangerType[dangerCounter] = 3;
      dangerMovement[dangerCounter] = 1;
      dangerStatus[dangerCounter] = 1;
    }

    // Set danger: Container
    if(j == 6)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 2;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 2;
      dangerType[dangerCounter] = 4;
      dangerMovement[dangerCounter] = 1;
      dangerX1[dangerCounter] = i % tileNumberX;
      dangerY1[dangerCounter] = i / tileNumberX;
      dangerX2[dangerCounter] = i % tileNumberX;
      dangerY2[dangerCounter] = i / tileNumberX;
      dangerStatus[dangerCounter] = 1;
    }
    
    // Set danger: Mech
    if(j == 7)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 2;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 1;
      dangerType[dangerCounter] = 5;
      dangerMovement[dangerCounter] = 1;
      dangerX1[dangerCounter] = i % tileNumberX;
      dangerY1[dangerCounter] = i / tileNumberX;
      dangerX2[dangerCounter] = i % tileNumberX;
      dangerY2[dangerCounter] = i / tileNumberX;
      dangerStatus[dangerCounter] = 1;
    }

    // Set danger: Fire
    if(j == 8)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 2;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 1;
      dangerType[dangerCounter] = 6;
      dangerMovement[dangerCounter] = 1;
      dangerStatus[dangerCounter] = 1;
    }

    // Set danger: Gripper
    if(j == 9)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 3;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8);
      dangerType[dangerCounter] = 7;
      dangerMovement[dangerCounter] = 1;
      dangerX1[dangerCounter] = i % tileNumberX;
      dangerY1[dangerCounter] = i / tileNumberX;
      dangerX2[dangerCounter] = i % tileNumberX;
      dangerY2[dangerCounter] = i / tileNumberX;
      dangerStatus[dangerCounter] = 1;
    }

    // Set danger: Pirate
    if(j == 10)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 1;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 2;
      dangerType[dangerCounter] = 8;
      dangerMovement[dangerCounter] = 1;
      dangerStatus[dangerCounter] = 1;
    }

    // Set danger: Missile
    if(j == 11)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 3;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 3;
      dangerType[dangerCounter] = 9;
      dangerMovement[dangerCounter] = 1;
      dangerStatus[dangerCounter] = 1;
    }

    // Set danger: Drone
    if(j == 12)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 2;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 2;
      dangerType[dangerCounter] = 10;
      dangerMovement[dangerCounter] = 1;
      dangerX1[dangerCounter] = i % tileNumberX;
      dangerY1[dangerCounter] = i / tileNumberX;
      dangerX2[dangerCounter] = i % tileNumberX;
      dangerY2[dangerCounter] = i / tileNumberX;
      dangerStatus[dangerCounter] = 1;
    }
    
    // Set danger: Plasma floor
    if(j == 13)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8);
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 7;
      dangerType[dangerCounter] = 11;
      dangerMovement[dangerCounter] = 1;
      dangerStatus[dangerCounter] = 1;
    }
            
    // Set danger: Sports glider flying left
    if(j == 14)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 2;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 2;
      dangerType[dangerCounter] = 12;
      dangerMovement[dangerCounter] = 1;
      dangerStatus[dangerCounter] = 1;
    }

    // Set danger: Sports glider flying right
    if(j == 15)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 2;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 2;
      dangerType[dangerCounter] = 13;
      dangerMovement[dangerCounter] = 1;
      dangerStatus[dangerCounter] = 1;
    }

    // Set danger: Plasma ufo
    if(j == 16)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8);
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8);
      dangerType[dangerCounter] = 14;
      dangerMovement[dangerCounter] = 1;
      dangerX1[dangerCounter] = i % tileNumberX;
      dangerY1[dangerCounter] = i / tileNumberX;
      dangerX2[dangerCounter] = i % tileNumberX;
      dangerY2[dangerCounter] = i / tileNumberX;
      dangerStatus[dangerCounter] = 1;
    }
    
    // Set danger: Railcar with steel
    if(j == 17)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 2;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8) + 1;
      dangerType[dangerCounter] = 15;
      dangerMovement[dangerCounter] = 1;
      dangerX1[dangerCounter] = i % tileNumberX;
      dangerY1[dangerCounter] = i / tileNumberX;
      dangerX2[dangerCounter] = i % tileNumberX;
      dangerY2[dangerCounter] = i / tileNumberX;
      dangerStatus[dangerCounter] = 1;
    }

    // Set danger: Meteor
    if(j == 18)
    {
      dangerXMap[dangerCounter] = ((i % tileNumberX) * 8) + 3;
      dangerYMap[dangerCounter] = ((i / tileNumberX) * 8);
      dangerType[dangerCounter] = 16;
      dangerMovement[dangerCounter] = random(21);
      dangerX1[dangerCounter] = ((i % tileNumberX) * 8) + 3; // Special usage of dangerX1: Meteor always starts at the same x position
      dangerStatus[dangerCounter] = 1;
    }

    if(j > 2 && dangerCounter < 15)
    {
      dangerCounter++;
    }
  }

  if(initializeNewLevel)
  {
    for(i = 0; i < platformCounter; i++)
    {
      boolean colorDuplicate;
      do
      {   
        platformColor[i] = random(6) + 1;
        colorDuplicate = false;
        for(j = 0; j < i; j++)
        {
          if(platformColor[i] == platformColor[j]){ colorDuplicate = true; }
        }
      }
      while(colorDuplicate);
    }
  }

  // Initialize danger movement
  initializeDangerMovement();  

  // Clear screen
  matrix.fillScreen(matrix.Color333(0,0,0));
  
  // Initialize playfield
  for(i = 0; i < 48; i++)
  {
    for(j = 0; j < 48; j++)
    {
      playfield[i][j] = 0;
    }
  }

  fuel = fuelMax;
  refuling = false;
  taxiXMapNew = taxiXMap;
  taxiYMapNew = taxiYMap;
  taxiXScreenNew = taxiXScreen;
  taxiYScreenNew = taxiYScreen;
  taxiDirection = RIGHT;
  taxiDirectionNew = RIGHT;
  taxiParks = false;
  footstepSound = true;
  xSpeed = 0.0;
  ySpeed = 0.0;
  xStepCounter = 0.0;
  yStepCounter = 0.0;
  screenXNew = screenX;
  screenYNew = screenY;
  redraw = false;
  mainThrusters = false;
  breakThrusters = false;
  hoverThrusters = false;
  downThrusters = false;
  heavyCollisionWithObstacle = false;
  taxiExplosion = false;

  // Ensure that all platforms can be used by new passengers
  for(byte i = 0; i < platformCounter; i++)
  {
    platformBordingStatus[i] = 1;
  }
  numberOfWaitingPassengers = 0;

  animationCounter = 0;
  animationCounterSyncValue = 0;
  audioOffUntil = 0;
}

// drawMiniMap: Shows a small map of the current level for better orientation.
void drawMiniMap()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
  
  byte k = 0;
  byte xOffset = (32 - (tileNumberX * 2)) / 2;
  byte yOffset = (32 - (tileNumberY * 2)) / 2; 

  matrix.drawRect(xOffset - 1, yOffset - 1, (tileNumberX * 2) + 2, (tileNumberY * 2) + 2, matrix.Color333(3, 2, 1));

  for(byte j = 0; j < tileNumberY; j++)
  {
    for(byte i = 0; i < tileNumberX; i++)
    {
      switch(levelMap[k])
      {
        case 0:
          break; 
        case 1:
          matrix.drawPixel((i * 2) + 0 + xOffset, (j * 2) + 1 + yOffset, matrix.Color333(1, 1, 1));
          matrix.drawPixel((i * 2) + 1 + xOffset, (j * 2) + 1 + yOffset, matrix.Color333(1, 1, 1));
          break; 
        case 2:
          matrix.drawPixel((i * 2) + 0 + xOffset, (j * 2) + 0 + yOffset, matrix.Color333(1, 1, 1));
          matrix.drawPixel((i * 2) + 1 + xOffset, (j * 2) + 0 + yOffset, matrix.Color333(1, 1, 1));
          break; 
        case 3:
          matrix.drawPixel((i * 2) + 0 + xOffset, (j * 2) + 0 + yOffset, matrix.Color333(1, 1, 1));
          matrix.drawPixel((i * 2) + 0 + xOffset, (j * 2) + 1 + yOffset, matrix.Color333(1, 1, 1));
          break; 
        case 4:
          matrix.drawPixel((i * 2) + 1 + xOffset, (j * 2) + 0 + yOffset, matrix.Color333(1, 1, 1));
          matrix.drawPixel((i * 2) + 1 + xOffset, (j * 2) + 1 + yOffset, matrix.Color333(1, 1, 1));
          break; 
        case 5:
          matrix.drawPixel((i * 2) + 0 + xOffset, (j * 2) + 0 + yOffset, matrix.Color333(1, 1, 1));
          matrix.drawPixel((i * 2) + 0 + xOffset, (j * 2) + 1 + yOffset, matrix.Color333(1, 1, 1));
          matrix.drawPixel((i * 2) + 1 + xOffset, (j * 2) + 1 + yOffset, matrix.Color333(1, 1, 1));
          break; 
        case 6:
          matrix.drawPixel((i * 2) + 1 + xOffset, (j * 2) + 0 + yOffset, matrix.Color333(1, 1, 1));
          matrix.drawPixel((i * 2) + 0 + xOffset, (j * 2) + 1 + yOffset, matrix.Color333(1, 1, 1));
          matrix.drawPixel((i * 2) + 1 + xOffset, (j * 2) + 1 + yOffset, matrix.Color333(1, 1, 1));
          break; 
        case 7:
          matrix.drawPixel((i * 2) + 0 + xOffset, (j * 2) + 0 + yOffset, matrix.Color333(1, 1, 1));
          matrix.drawPixel((i * 2) + 1 + xOffset, (j * 2) + 1 + yOffset, matrix.Color333(1, 1, 1));
          break; 
        case 8:
          matrix.drawPixel((i * 2) + 1 + xOffset, (j * 2) + 0 + yOffset, matrix.Color333(1, 1, 1));
          matrix.drawPixel((i * 2) + 0 + xOffset, (j * 2) + 1 + yOffset, matrix.Color333(1, 1, 1));
          break; 
        default:
          break; 
      }
      k++;
    }
  }

  for(byte i = 0; i < platformCounter; i++)
  {
    switch(platformColor[i])
    {
      case RED:
        matrix.drawPixel((platformXMap[i] / 4) + 0 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(4, 0, 0));
        matrix.drawPixel((platformXMap[i] / 4) + 1 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(4, 0, 0));
        break;
      case GREEN:
        matrix.drawPixel((platformXMap[i] / 4) + 0 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(0, 4, 0));
        matrix.drawPixel((platformXMap[i] / 4) + 1 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(0, 4, 0));
        break;
      case BLUE:
        matrix.drawPixel((platformXMap[i] / 4) + 0 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(0, 0, 4));
        matrix.drawPixel((platformXMap[i] / 4) + 1 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(0, 0, 4));
        break;
      case YELLOW:
        matrix.drawPixel((platformXMap[i] / 4) + 0 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(4, 4, 0));
        matrix.drawPixel((platformXMap[i] / 4) + 1 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(4, 4, 0));
        break;
      case VIOLET:
        matrix.drawPixel((platformXMap[i] / 4) + 0 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(4, 0, 4));
        matrix.drawPixel((platformXMap[i] / 4) + 1 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(4, 0, 4));
        break;
      case TURQUOISE:
        matrix.drawPixel((platformXMap[i] / 4) + 0 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(0, 4, 4));
        matrix.drawPixel((platformXMap[i] / 4) + 1 + xOffset, (platformYMap[i] / 4) + yOffset, matrix.Color333(0, 4, 4));
        break;
      default:
        break;
    }
  }

  byte counter = 0;
  boolean buttonReleased = false;
  do
  {
    if(counter < 256)
    {
      matrix.drawPixel((taxiXMap / 4) + 0 + xOffset, (taxiYMap / 4) + 0 + yOffset, matrix.Color333(7, 7, 7));
      matrix.drawPixel((taxiXMap / 4) + 1 + xOffset, (taxiYMap / 4) + 0 + yOffset, matrix.Color333(7, 7, 7));       
    }
    else
    {
      matrix.drawPixel((taxiXMap / 4) + 0 + xOffset, (taxiYMap / 4) + 0 + yOffset, matrix.Color333(0, 0, 0));
      matrix.drawPixel((taxiXMap / 4) + 1 + xOffset, (taxiYMap / 4) + 0 + yOffset, matrix.Color333(0, 0, 0));       
    }
    for(byte i = 0; i < gasStationCounter; i++)
    {
      if(counter % 2 == 0)
      {
        matrix.drawPixel((gasStationXMap[i] / 4) + 0 + xOffset, (gasStationYMap[i] / 4) + yOffset, matrix.Color333(4, 4, 4));
        matrix.drawPixel((gasStationXMap[i] / 4) + 1 + xOffset, (gasStationYMap[i] / 4) + yOffset, matrix.Color333(4, 4, 4));
      }
      else
      {
        matrix.drawPixel((gasStationXMap[i] / 4) + 0 + xOffset, (gasStationYMap[i] / 4) + yOffset, matrix.Color333(1, 1, 1));
        matrix.drawPixel((gasStationXMap[i] / 4) + 1 + xOffset, (gasStationYMap[i] / 4) + yOffset, matrix.Color333(1, 1, 1));
      }      
    }
    delay(1);
    counter++;
    if(!joy1Fire()){ buttonReleased = true; }
  }
  while(!(joy1Fire() && buttonReleased)); 
  tone(audio,1024,20);
  delay(500);
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
}

// updateLevelColors: Defines values for R, G, and B to draw the map elements
// Set RGB-values depending on level color
// 0 == Invisible
// 1 == Blue
// 2 == Green
// 3 == Turquoise
// 4 == Red
// 5 == Violet
// 6 == Yellow
// 7 == White
void updateLevelColors()
{
  switch(levelColor)
  {
    case 0:
    red = 0;
    green = 0;
    blue = 0;
    break;

    case 1:
    red = 0;
    green = 0;
    blue = 1;
    break;

    case 2:
    red = 0;
    green = 1;
    blue = 0;
    break;

    case 3:
    red = 0;
    green = 1;
    blue = 1;
    break;

    case 4:
    red = 1;
    green = 0;
    blue = 0;
    break;

    case 5:
    red = 1;
    green = 0;
    blue = 1;
    break;

    case 6:
    red = 1;
    green = 1;
    blue = 0;
    break;

    case 7:
    red = 1;
    green = 1;
    blue = 1;
    break;
  }
}

// drawScreen: Draw the screen according to the current x and y position
void drawScreen()
{
  if(redraw)
  {
    // Clear all tiles at old positions
    upperLeftTileX = screenX / 8;
    upperLeftTileXOffset = screenX % 8;
    upperLeftTileY = screenY / 8;
    upperLeftTileYOffset = screenY % 8;

    for(j = 0; j < 5; j++)
    {
      for(i = 0; i < 5; i++)
      {
        clearTile(levelMap[((upperLeftTileY + j ) * tileNumberX) + upperLeftTileX + i], (i * 8) - upperLeftTileXOffset, (j * 8) - upperLeftTileYOffset);
      }
    }
    screenX = screenXNew;
    screenY = screenYNew;
  }
  // Draw all tiles at new positions
  upperLeftTileX = screenX / 8;
  upperLeftTileXOffset = screenX % 8;
  upperLeftTileY = screenY / 8;
  upperLeftTileYOffset = screenY % 8;
    
  for(j = 0; j < 5; j++)
  {
    for(i = 0; i < 5; i++)
    {
      drawTile(levelMap[((upperLeftTileY + j ) * tileNumberX) + upperLeftTileX + i], (i * 8) - upperLeftTileXOffset, (j * 8) - upperLeftTileYOffset);
    }
  }
  redraw = false;
}

// drawTile: Draws a tile (8*8 pixels) at position x, y
//
//   type 0     type 1     type 2     type 3     type 4     type 5     type 6     type 7     type 8
//   --------   --------   --------   O-------   -------O   O-------   -------O   O-------   -------O
//   --------   --------   --------   O-------   -------O   O-------   -------O   -O------   ------O-
//   --------   --------   --------   O-------   -------O   O-------   -------O   --O-----   -----O--
//   --------   --------   OOOOOOOO   O-------   -------O   O-------   -------O   ---O----   ----O---
//   --------   --------   --------   O-------   -------O   O-------   -------O   ----O---   ---O----
//   --------   --------   --------   O-------   -------O   O-------   -------O   -----O--   --O-----
//   --------   --------   --------   O-------   -------O   O-------   -------O   ------O-   -O------
//   --------   OOOOOOOO   --------   O-------   -------O   OOOOOOOO   OOOOOOOO   -------O   O-------
//
void drawTile(byte _type, int _x, int _y)
{
  if(_type == 0)
  {
  }
  else if(_type == 1)
  {
    matrix.drawPixel(_x, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 8][_y + 15] = 1;
    playfield[_x + 9][_y + 15] = 1;
    playfield[_x + 10][_y + 15] = 1;
    playfield[_x + 11][_y + 15] = 1;
    playfield[_x + 12][_y + 15] = 1;
    playfield[_x + 13][_y + 15] = 1;
    playfield[_x + 14][_y + 15] = 1;
    playfield[_x + 15][_y + 15] = 1;
  }
  else if(_type == 2)
  {
    matrix.drawPixel(_x, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 1, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 2, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 3, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 4, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 5, _y + 3, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 6, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(red, green, blue));
    playfield[_x + 8][_y + 11] = 1;
    playfield[_x + 9][_y + 11] = 1;
    playfield[_x + 10][_y + 11] = 1;
    playfield[_x + 11][_y + 11] = 1;
    playfield[_x + 12][_y + 11] = 1;
    playfield[_x + 13][_y + 11] = 1;
    playfield[_x + 14][_y + 11] = 1;
    playfield[_x + 15][_y + 11] = 1;
  }
  else if(_type == 3)
  {
    matrix.drawPixel(_x, _y, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 1, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 2, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 4, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 5, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 6, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 8][_y + 8] = 1;
    playfield[_x + 8][_y + 9] = 1;
    playfield[_x + 8][_y + 10] = 1;
    playfield[_x + 8][_y + 11] = 1;
    playfield[_x + 8][_y + 12] = 1;
    playfield[_x + 8][_y + 13] = 1;
    playfield[_x + 8][_y + 14] = 1;
    playfield[_x + 8][_y + 15] = 1;
  }
  else if(_type == 4)
  {
    matrix.drawPixel(_x + 7, _y, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 1, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 2, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 4, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 5, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 6, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 15][_y + 8] = 1;
    playfield[_x + 15][_y + 9] = 1;
    playfield[_x + 15][_y + 10] = 1;
    playfield[_x + 15][_y + 11] = 1;
    playfield[_x + 15][_y + 12] = 1;
    playfield[_x + 15][_y + 13] = 1;
    playfield[_x + 15][_y + 14] = 1;
    playfield[_x + 15][_y + 15] = 1;
  }
  else if(_type == 5)
  {
    matrix.drawPixel(_x, _y, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 1, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 2, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 4, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 5, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x, _y + 6, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 8][_y + 8] = 1;
    playfield[_x + 8][_y + 9] = 1;
    playfield[_x + 8][_y + 10] = 1;
    playfield[_x + 8][_y + 11] = 1;
    playfield[_x + 8][_y + 12] = 1;
    playfield[_x + 8][_y + 13] = 1;
    playfield[_x + 8][_y + 14] = 1;
    playfield[_x + 8][_y + 15] = 1;
    playfield[_x + 9][_y + 15] = 1;
    playfield[_x + 10][_y + 15] = 1;
    playfield[_x + 11][_y + 15] = 1;
    playfield[_x + 12][_y + 15] = 1;
    playfield[_x + 13][_y + 15] = 1;
    playfield[_x + 14][_y + 15] = 1;
    playfield[_x + 15][_y + 15] = 1;
  }
  else if(_type == 6)
  {
    matrix.drawPixel(_x + 7, _y, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 1, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 2, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 4, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 5, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 7, _y + 6, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 15][_y + 8] = 1;
    playfield[_x + 15][_y + 9] = 1;
    playfield[_x + 15][_y + 10] = 1;
    playfield[_x + 15][_y + 11] = 1;
    playfield[_x + 15][_y + 12] = 1;
    playfield[_x + 15][_y + 13] = 1;
    playfield[_x + 15][_y + 14] = 1;
    playfield[_x + 8][_y + 15] = 1;
    playfield[_x + 9][_y + 15] = 1;
    playfield[_x + 10][_y + 15] = 1;
    playfield[_x + 11][_y + 15] = 1;
    playfield[_x + 12][_y + 15] = 1;
    playfield[_x + 13][_y + 15] = 1;
    playfield[_x + 14][_y + 15] = 1;
    playfield[_x + 15][_y + 15] = 1;
  }
  else if(_type == 7)
  {
    matrix.drawPixel(_x, _y, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 1, _y + 1, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 2, _y + 2, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 3, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 4, _y + 4, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 5, _y + 5, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 6, _y + 6, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 8][_y + 8] = 1;
    playfield[_x + 9][_y + 9] = 1;
    playfield[_x + 10][_y + 10] = 1;
    playfield[_x + 11][_y + 11] = 1;
    playfield[_x + 12][_y + 12] = 1;
    playfield[_x + 13][_y + 13] = 1;
    playfield[_x + 14][_y + 14] = 1;
    playfield[_x + 15][_y + 15] = 1;
  }
  else if(_type == 8)
  {
    matrix.drawPixel(_x + 7, _y, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 6, _y + 1, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 5, _y + 2, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 4, _y + 3, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x + 3, _y + 4, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 2, _y + 5, matrix.Color333(red * 2, green * 2, blue * 2));
    matrix.drawPixel(_x + 1, _y + 6, matrix.Color333(red, green, blue));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(red, green, blue));
    playfield[_x + 15][_y + 8] = 1;
    playfield[_x + 14][_y + 9] = 1;
    playfield[_x + 13][_y + 10] = 1;
    playfield[_x + 12][_y + 11] = 1;
    playfield[_x + 11][_y + 12] = 1;
    playfield[_x + 10][_y + 13] = 1;
    playfield[_x + 9][_y + 14] = 1;
    playfield[_x + 8][_y + 15] = 1;
  }
}

// clearTile: Clear a tile (8*8 pixels) at position x, y
void clearTile(byte _type, int _x, int _y)
{
  if(_type == 0)
  {
  }
  else if(_type == 1)
  {
    matrix.drawPixel(_x, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 8][_y + 15] = 0;
    playfield[_x + 9][_y + 15] = 0;
    playfield[_x + 10][_y + 15] = 0;
    playfield[_x + 11][_y + 15] = 0;
    playfield[_x + 12][_y + 15] = 0;
    playfield[_x + 13][_y + 15] = 0;
    playfield[_x + 14][_y + 15] = 0;
    playfield[_x + 15][_y + 15] = 0;
  }
  else if(_type == 2)
  {
    matrix.drawPixel(_x, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 1, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 2, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 3, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 4, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 5, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 6, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(0, 0, 0));
    playfield[_x + 8][_y + 11] = 0;
    playfield[_x + 9][_y + 11] = 0;
    playfield[_x + 10][_y + 11] = 0;
    playfield[_x + 11][_y + 11] = 0;
    playfield[_x + 12][_y + 11] = 0;
    playfield[_x + 13][_y + 11] = 0;
    playfield[_x + 14][_y + 11] = 0;
    playfield[_x + 15][_y + 11] = 0;
  }
  else if(_type == 3)
  {
    matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 4, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 6, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 8][_y + 8] = 0;
    playfield[_x + 8][_y + 9] = 0;
    playfield[_x + 8][_y + 10] = 0;
    playfield[_x + 8][_y + 11] = 0;
    playfield[_x + 8][_y + 12] = 0;
    playfield[_x + 8][_y + 13] = 0;
    playfield[_x + 8][_y + 14] = 0;
    playfield[_x + 8][_y + 15] = 0;
  }
  else if(_type == 4)
  {
    matrix.drawPixel(_x + 7, _y, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 4, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 6, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 15][_y + 8] = 0;
    playfield[_x + 15][_y + 9] = 0;
    playfield[_x + 15][_y + 10] = 0;
    playfield[_x + 15][_y + 11] = 0;
    playfield[_x + 15][_y + 12] = 0;
    playfield[_x + 15][_y + 13] = 0;
    playfield[_x + 15][_y + 14] = 0;
    playfield[_x + 15][_y + 15] = 0;
  }
  else if(_type == 5)
  {
    matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 4, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 6, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 8][_y + 8] = 0;
    playfield[_x + 8][_y + 9] = 0;
    playfield[_x + 8][_y + 10] = 0;
    playfield[_x + 8][_y + 11] = 0;
    playfield[_x + 8][_y + 12] = 0;
    playfield[_x + 8][_y + 13] = 0;
    playfield[_x + 8][_y + 14] = 0;
    playfield[_x + 8][_y + 15] = 0;
    playfield[_x + 9][_y + 15] = 0;
    playfield[_x + 10][_y + 15] = 0;
    playfield[_x + 11][_y + 15] = 0;
    playfield[_x + 12][_y + 15] = 0;
    playfield[_x + 13][_y + 15] = 0;
    playfield[_x + 14][_y + 15] = 0;
    playfield[_x + 15][_y + 15] = 0;
  }
  else if(_type == 6)
  {
    matrix.drawPixel(_x + 7, _y, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 4, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 6, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 1, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 2, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 3, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 4, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 5, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 6, _y + 7, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 15][_y + 8] = 0;
    playfield[_x + 15][_y + 9] = 0;
    playfield[_x + 15][_y + 10] = 0;
    playfield[_x + 15][_y + 11] = 0;
    playfield[_x + 15][_y + 12] = 0;
    playfield[_x + 15][_y + 13] = 0;
    playfield[_x + 15][_y + 14] = 0;
    playfield[_x + 8][_y + 15] = 0;
    playfield[_x + 9][_y + 15] = 0;
    playfield[_x + 10][_y + 15] = 0;
    playfield[_x + 11][_y + 15] = 0;
    playfield[_x + 12][_y + 15] = 0;
    playfield[_x + 13][_y + 15] = 0;
    playfield[_x + 14][_y + 15] = 0;
    playfield[_x + 15][_y + 15] = 0;
  }
  else if(_type == 7)
  {
    matrix.drawPixel(_x, _y, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 1, _y + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 2, _y + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 3, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 4, _y + 4, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 5, _y + 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 6, _y + 6, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 7, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 8][_y + 8] = 0;
    playfield[_x + 9][_y + 9] = 0;
    playfield[_x + 10][_y + 10] = 0;
    playfield[_x + 11][_y + 11] = 0;
    playfield[_x + 12][_y + 12] = 0;
    playfield[_x + 13][_y + 13] = 0;
    playfield[_x + 14][_y + 14] = 0;
    playfield[_x + 15][_y + 15] = 0;
  }
  else if(_type == 8)
  {
    matrix.drawPixel(_x + 7, _y, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 6, _y + 1, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 5, _y + 2, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 4, _y + 3, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 3, _y + 4, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 2, _y + 5, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x + 1, _y + 6, matrix.Color333(0, 0, 0));
    matrix.drawPixel(_x, _y + 7, matrix.Color333(0, 0, 0));
    playfield[_x + 15][_y + 8] = 0;
    playfield[_x + 14][_y + 9] = 0;
    playfield[_x + 13][_y + 10] = 0;
    playfield[_x + 12][_y + 11] = 0;
    playfield[_x + 11][_y + 12] = 0;
    playfield[_x + 10][_y + 13] = 0;
    playfield[_x + 9][_y + 14] = 0;
    playfield[_x + 8][_y + 15] = 0;
  }
}

// drawDanger: Draw danger i (from the list of all dangers)
void drawDangers(byte i)
{
  x1 = dangerXScreen[i];
  y1 = dangerYScreen[i];
  x2 = dangerXScreenNew[i];
  y2 = dangerYScreenNew[i];

  // Other taxi flying left
  if(dangerType[i] == 2)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 4, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 3, matrix.Color333(0, 0, 0));
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
      playfield[x1 + 8][y1 + 10] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 10][y1 + 10] = 0;
      playfield[x1 + 11][y1 + 10] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2 + 2, y2, matrix.Color333(0, 1, 0));
      matrix.drawPixel(x2 + 3, y2, matrix.Color333(0, 3, 0));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(0, 0, 1));
      matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(0, 1, 0));
      if(animationCounter % 4 == 0){ matrix.drawPixel(x2 + 4, y2 + 1, matrix.Color333(random(3) + 2, random(3) + 2, 2)); }
      matrix.drawPixel(x2, y2 + 2, matrix.Color333(0, 3, 0));
      matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(0, 1, 0));
      matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(0, 1, 0));
      matrix.drawPixel(x2 + 3, y2 + 2, matrix.Color333(0, 3, 0));
      if(animationCounter % 8 == 0)
      {
        matrix.drawPixel(x2 + 1, y2 + 3, matrix.Color333(random(3) + 2, random(3) + 2, 2));
        matrix.drawPixel(x2 + 2, y2 + 3, matrix.Color333(random(3) + 2, random(3) + 2, 2));
      }
      playfield[x2 + 10][y2 + 8] = 2;
      playfield[x2 + 11][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 11][y2 + 9] = 2;
      playfield[x2 + 8][y2 + 10] = 2;
      playfield[x2 + 9][y2 + 10] = 2;
      playfield[x2 + 10][y2 + 10] = 2;
      playfield[x2 + 11][y2 + 10] = 2;
    }
  }

  // Other taxi flying right
  if(dangerType[i] == 3)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 4, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 3, matrix.Color333(0, 0, 0));
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 10][y1 + 10] = 0;
      playfield[x1 + 11][y1 + 10] = 0;
      playfield[x1 + 12][y1 + 10] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2 + 1, y2, matrix.Color333(0, 3, 0));
      matrix.drawPixel(x2 + 2, y2, matrix.Color333(0, 1, 0));
      if(animationCounter % 4 == 0){ matrix.drawPixel(x2, y2 + 1, matrix.Color333(random(3) + 2, random(3) + 2, 2)); }
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(0, 1, 0));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(0, 0, 1));
      matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(0, 3, 0));
      matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(0, 1, 0));
      matrix.drawPixel(x2 + 3, y2 + 2, matrix.Color333(0, 1, 0));
      matrix.drawPixel(x2 + 4, y2 + 2, matrix.Color333(0, 3, 0));
      if(animationCounter % 8 == 0)
      {
        matrix.drawPixel(x2 + 2, y2 + 3, matrix.Color333(random(3) + 2, random(3) + 2, 2));
        matrix.drawPixel(x2 + 3, y2 + 3, matrix.Color333(random(3) + 2, random(3) + 2, 2));
      }
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 10][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 11][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 10] = 2;
      playfield[x2 + 10][y2 + 10] = 2;
      playfield[x2 + 11][y2 + 10] = 2;
      playfield[x2 + 12][y2 + 10] = 2;
    }
  }

  // Container
  if(dangerType[i] == 4)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 10][y1 + 10] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2, y2, matrix.Color333(4, 4, 1));
      matrix.drawPixel(x2 + 1, y2, matrix.Color333(random(7), 0, 0));
      matrix.drawPixel(x2 + 2, y2, matrix.Color333(random(7), 0, 0));
      matrix.drawPixel(x2 + 3, y2, matrix.Color333(3, 3, 1));
      matrix.drawPixel(x2, y2 + 1, matrix.Color333(3, 3, 1));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(4, 4, 1));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(2, 2, 1));
      matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(2, 2, 1));
      if(dangerMovement[i] == 0 && animationCounter % 4 == 0)
      {
        matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(random(3) + 2, random(3) + 2, 2));
        matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(random(3) + 2, random(3) + 2, 2));
        playfield[x2 + 9][y2 + 10] = 2;
        playfield[x2 + 10][y2 + 10] = 2;
      }
      playfield[x2 + 8][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 10][y2 + 8] = 2;
      playfield[x2 + 11][y2 + 8] = 2;
      playfield[x2 + 8][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 11][y2 + 9] = 2;
    }
  }
  
  // Mech
  if(dangerType[i] == 5)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {  
      // Moving left
      if(dangerMovement[i] == 0)
      {
        matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 4, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 4, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 5, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 5, matrix.Color333(0, 0, 0));
        playfield[x1 + 10][y1 + 8] = 0;
        playfield[x1 + 8][y1 + 9] = 0;
        playfield[x1 + 9][y1 + 9] = 0;
        playfield[x1 + 10][y1 + 9] = 0;
        playfield[x1 + 9][y1 + 10] = 0;
        playfield[x1 + 10][y1 + 10] = 0;
        playfield[x1 + 10][y1 + 11] = 0;
        playfield[x1 + 9][y1 + 12] = 0;
        playfield[x1 + 10][y1 + 12] = 0;
        playfield[x1 + 9][y1 + 13] = 0;
        playfield[x1 + 10][y1 + 13] = 0;
      }
      else
      {
        matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 4, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 4, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 5, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 5, matrix.Color333(0, 0, 0));
        playfield[x1 + 8][y1 + 8] = 0;
        playfield[x1 + 8][y1 + 9] = 0;
        playfield[x1 + 9][y1 + 9] = 0;
        playfield[x1 + 10][y1 + 9] = 0;
        playfield[x1 + 8][y1 + 10] = 0;
        playfield[x1 + 9][y1 + 10] = 0;
        playfield[x1 + 8][y1 + 11] = 0;
        playfield[x1 + 8][y1 + 12] = 0;
        playfield[x1 + 9][y1 + 12] = 0;
        playfield[x1 + 8][y1 + 13] = 0;
        playfield[x1 + 9][y1 + 13] = 0;
      }
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      if(dangerMovement[i] == 0)
      {
        matrix.drawPixel(x2 + 2, y2, matrix.Color333(0, 0, 3));
        matrix.drawPixel(x2, y2 + 1, matrix.Color333(3, 0, 0));
        matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(3, 0, 0));
        matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(1, 0, 0));
        matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(1, 1, 1));
        matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(2, 2, 2));
        matrix.drawPixel(x2 + 2, y2 + 3, matrix.Color333(3, 0, 0));
        if(animationCounter % 32 < 16)
        {
          matrix.drawPixel(x2 + 2, y2 + 4, matrix.Color333(3, 0, 0));
          matrix.drawPixel(x2 + 1, y2 + 5, matrix.Color333(1, 0, 0));
          matrix.drawPixel(x2 + 2, y2 + 5, matrix.Color333(1, 0, 0));
          playfield[x2 + 10][y2 + 12] = 2;
          playfield[x2 + 9][y2 + 13] = 2;
          playfield[x2 + 10][y2 + 13] = 2;
        }
        else
        {
          matrix.drawPixel(x2 + 1, y2 + 4, matrix.Color333(3, 0, 0));
          matrix.drawPixel(x2 + 2, y2 + 5, matrix.Color333(3, 0, 0));
          playfield[x2 + 9][y2 + 12] = 2;
          playfield[x2 + 10][y2 + 13] = 2;            
        }
        playfield[x2 + 10][y2 + 8] = 2;
        playfield[x2 + 8][y2 + 9] = 2;
        playfield[x2 + 9][y2 + 9] = 2;
        playfield[x2 + 10][y2 + 9] = 2;
        playfield[x2 + 9][y2 + 10] = 2;
        playfield[x2 + 10][y2 + 10] = 2;
        playfield[x2 + 10][y2 + 11] = 2;
      }
      else
      {
        matrix.drawPixel(x2, y2, matrix.Color333(0, 0, 3));
        matrix.drawPixel(x2, y2 + 1, matrix.Color333(1, 0, 0));
        matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(3, 0, 0));
        matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(3, 0, 0));
        matrix.drawPixel(x2, y2 + 2, matrix.Color333(2, 2, 2));
        matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(1, 1, 1));
        matrix.drawPixel(x2, y2 + 3, matrix.Color333(3, 0, 0));
        if(animationCounter % 32 < 16)
        {
          matrix.drawPixel(x2, y2 + 4, matrix.Color333(3, 0, 0));
          matrix.drawPixel(x2, y2 + 5, matrix.Color333(1, 0, 0));
          matrix.drawPixel(x2 + 1, y2 + 5, matrix.Color333(1, 0, 0));        
          playfield[x2 + 8][y2 + 12] = 2;
          playfield[x2 + 8][y2 + 13] = 2;
          playfield[x2 + 9][y2 + 13] = 2;
        }
        else
        {
          matrix.drawPixel(x2 + 1, y2 + 4, matrix.Color333(3, 0, 0));
          matrix.drawPixel(x2, y2 + 5, matrix.Color333(3, 0, 0));
          playfield[x2 + 9][y2 + 12] = 2;
          playfield[x2 + 8][y2 + 13] = 2;
        }
        playfield[x2 + 8][y2 + 8] = 2;
        playfield[x2 + 8][y2 + 9] = 2;
        playfield[x2 + 9][y2 + 9] = 2;
        playfield[x2 + 10][y2 + 9] = 2;
        playfield[x2 + 8][y2 + 10] = 2;
        playfield[x2 + 9][y2 + 10] = 2;
        playfield[x2 + 8][y2 + 11] = 2;
      }
    }
  }

  // Fire
  if(dangerType[i] == 6)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      if(animationCounter > 128 || animationCounter == 0)
      {
        matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 3, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 4, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 4, matrix.Color333(0, 0, 0));
        playfield[x1 + 9][y1 + 8] = 0;
        playfield[x1 + 10][y1 + 8] = 0;
        playfield[x1 + 8][y1 + 9] = 0;
        playfield[x1 + 9][y1 + 9] = 0;
        playfield[x1 + 10][y1 + 9] = 0;
        playfield[x1 + 11][y1 + 9] = 0;
        playfield[x1 + 8][y1 + 10] = 0;
        playfield[x1 + 9][y1 + 10] = 0;
        playfield[x1 + 10][y1 + 10] = 0;
        playfield[x1 + 11][y1 + 10] = 0;
        playfield[x1 + 9][y1 + 11] = 0;
        playfield[x1 + 10][y1 + 11] = 0;
        playfield[x1 + 9][y1 + 12] = 0;
        playfield[x1 + 10][y1 + 12] = 0;

      }
      matrix.drawPixel(x1 + 1, y1 + 5, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 5, matrix.Color333(0, 0, 0));
      playfield[x1 + 9][y1 + 13] = 0;
      playfield[x1 + 10][y1 + 13] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    { 
      if(animationCounter > 127)
      {
        if(random(100) < 15){ matrix.drawPixel(x2 + 1, y2, matrix.Color333(1, 1, 0)); playfield[x2 + 9][y2 + 8] = 2; }
        if(random(100) < 15){ matrix.drawPixel(x2 + 2, y2, matrix.Color333(1, 1, 0)); playfield[x2 + 10][y2 + 8] = 2; }
        if(random(100) < 15){ matrix.drawPixel(x2, y2 + 1, matrix.Color333(1, 1, 0)); playfield[x2 + 8][y2 + 9] = 2; }
        if(random(100) < 20){ matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(2, 2, 0)); playfield[x2 + 9][y2 + 9] = 2; }
        if(random(100) < 20){ matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(2, 2, 0)); playfield[x2 + 10][y2 + 9] = 2; }
        if(random(100) < 15){ matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(1, 1, 0)); playfield[x2 + 11][y2 + 9] = 2; }
        if(random(100) < 20){ matrix.drawPixel(x2, y2 + 2, matrix.Color333(2, 2, 0)); playfield[x2 + 8][y2 + 10] = 2; }
        if(random(100) < 25){ matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(3, 3, 1)); playfield[x2 + 9][y2 + 10] = 2; }
        if(random(100) < 25){ matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(3, 3, 1)); playfield[x2 + 10][y2 + 10] = 2; }
        if(random(100) < 20){ matrix.drawPixel(x2 + 3, y2 + 2, matrix.Color333(2, 2, 0)); playfield[x2 + 11][y2 + 10] = 2; }
        if(random(100) < 30){ matrix.drawPixel(x2 + 1, y2 + 3, matrix.Color333(4, 4, 1)); playfield[x2 + 9][y2 + 11] = 2; }
        if(random(100) < 30){ matrix.drawPixel(x2 + 2, y2 + 3, matrix.Color333(4, 4, 1)); playfield[x2 + 10][y2 + 11] = 2; }
        if(random(100) < 35){ matrix.drawPixel(x2 + 1, y2 + 4, matrix.Color333(5, 5, 2)); playfield[x2 + 9][y2 + 12] = 2; }
        if(random(100) < 35){ matrix.drawPixel(x2 + 2, y2 + 4, matrix.Color333(5, 5, 2)); playfield[x2 + 10][y2 + 12] = 2; }
        // SOUND: Fire
        tone(audio, 50 + random(350), 5);        
      }
      matrix.drawPixel(x2 + 1, y2 + 5, matrix.Color333(1, 0, 3));
      matrix.drawPixel(x2 + 2, y2 + 5, matrix.Color333(1, 0, 3));
      playfield[x2 + 9][y2 + 13] = 2;
      playfield[x2 + 10][y2 + 13] = 2;
    }
  }

  // Gripper  
  if(dangerType[i] == 7)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      if(dangerMovement[i] > 2 && dangerMovement[i] < 14){ matrix.drawPixel(x1 + 1, y1 - 1, matrix.Color333(0, 0, 0)); playfield[x1 + 9][y1 + 7] = 0; }
      if(dangerMovement[i] > 4 && dangerMovement[i] < 12){ matrix.drawPixel(x1 + 1, y1 - 2, matrix.Color333(0, 0, 0)); playfield[x1 + 9][y1 + 6] = 0; }
      if(dangerMovement[i] > 6 && dangerMovement[i] < 10){ matrix.drawPixel(x1 + 1, y1 - 3, matrix.Color333(0, 0, 0)); playfield[x1 + 9][y1 + 5] = 0; }
      
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 3, matrix.Color333(0, 0, 0));
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 8][y1 + 10] = 0;
      playfield[x1 + 10][y1 + 10] = 0;
      playfield[x1 + 8][y1 + 11] = 0;
      playfield[x1 + 10][y1 + 11] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      if(dangerMovement[i] > 2 && dangerMovement[i] < 12){ matrix.drawPixel(x2 + 1, y2 - 1, matrix.Color333(1, 1, 1)); playfield[x2 + 9][y2 + 7] = 2; }
      if(dangerMovement[i] > 4 && dangerMovement[i] < 10){ matrix.drawPixel(x2 + 1, y2 - 2, matrix.Color333(1, 1, 1)); playfield[x2 + 9][y2 + 6] = 2; }
      if(dangerMovement[i] > 6 && dangerMovement[i] < 8){ matrix.drawPixel(x2 + 1, y2 - 3, matrix.Color333(1, 1, 1)); playfield[x2 + 9][y2 + 5] = 2; }
   
      matrix.drawPixel(x2 + 1, y2, matrix.Color333(1, 1, 1));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(1, 1, 1));
      matrix.drawPixel(x2, y2 + 2, matrix.Color333(1, 1, 1));
      matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(1, 1, 1));  
      matrix.drawPixel(x2, y2 + 3, matrix.Color333(3, 3, 3));
      matrix.drawPixel(x2 + 2, y2 + 3, matrix.Color333(3, 3, 3));    
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 8][y2 + 10] = 2;
      playfield[x2 + 10][y2 + 10] = 2;
      playfield[x2 + 8][y2 + 11] = 2;
      playfield[x2 + 10][y2 + 11] = 2;
    }
  }

  // Pirate
  if(dangerType[i] == 8)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 4, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 5, y1 + 2, matrix.Color333(0, 0, 0));
      playfield[x1 + 10][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
      playfield[x1 + 12][y1 + 9] = 0;
      playfield[x1 + 8][y1 + 10] = 0;
      playfield[x1 + 10][y1 + 10] = 0;
      playfield[x1 + 11][y1 + 10] = 0;
      playfield[x1 + 13][y1 + 10] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2 + 2, y2, matrix.Color333(0, 3, 3));
      matrix.drawPixel(x2 + 3, y2, matrix.Color333(0, 3, 3));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x2 + 4, y2 + 1, matrix.Color333(0, 1, 1));
      matrix.drawPixel(x2 + 1 + (animationCounter % 16) / 4, y2 + 1, matrix.Color333(3, 3, 0));
      matrix.drawPixel(x2, y2 + 2, matrix.Color333(0, 3, 3));
      if(animationCounter % 2 == 0)
      {
        matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(random(5) + 1, random(5) + 1, 1));
        matrix.drawPixel(x2 + 3, y2 + 2, matrix.Color333(random(5) + 1, random(5) + 1, 10));
        playfield[x2 + 10][y2 + 10] = 2;
        playfield[x2 + 11][y2 + 10] = 2;
      }
      matrix.drawPixel(x2 + 5, y2 + 2, matrix.Color333(0, 3, 3));
      playfield[x2 + 11][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 11][y2 + 9] = 2;
      playfield[x2 + 12][y2 + 9] = 2;
      playfield[x2 + 8][y2 + 10] = 2;
      playfield[x2 + 13][y2 + 10] = 2;
    }
  }

  // Missile
  if(dangerType[i] == 9)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 4, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 4, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 8][y1 + 10] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 8][y1 + 11] = 0;
      playfield[x1 + 9][y1 + 11] = 0;
      playfield[x1 + 8][y1 + 12] = 0;
      playfield[x1 + 9][y1 + 12] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2, y2, matrix.Color333(1, 0, 0));
      matrix.drawPixel(x2 + 1, y2, matrix.Color333(1, 0, 0));
      matrix.drawPixel(x2, y2 + 1, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2, y2 + 2, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(3, 0, 0));
      if(animationCounter % 2 == 0)
      {
        matrix.drawPixel(x2, y2 + 3, matrix.Color333(random(5) + 1, random(5) + 1, 1));
        matrix.drawPixel(x2 + 1, y2 + 3, matrix.Color333(random(5) + 1, random(5) + 1, 1));
        playfield[x2 + 8][y2 + 11] = 2;
        playfield[x2 + 9][y2 + 11] = 2;
        // Sound: Missile
        tone(audio, 100 + random(100), 5);
      }
      if(animationCounter % 4 == 0)
      {
        matrix.drawPixel(x2, y2 + 4, matrix.Color333(random(3) + 1, random(3) + 1, 1));
        matrix.drawPixel(x2 + 1, y2 + 4, matrix.Color333(random(3) + 1, random(3) + 1, 1));
        playfield[x2 + 8][y2 + 12] = 2;
        playfield[x2 + 9][y2 + 12] = 2;
      }
      playfield[x2 + 8][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 8][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 8][y2 + 10] = 2;
      playfield[x2 + 9][y2 + 10] = 2;
    }
  }

  // Drone
  if(dangerType[i] == 10)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 3, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 10][y1 + 10] = 0;
      playfield[x1 + 8][y1 + 11] = 0;
      playfield[x1 + 11][y1 + 11] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2, y2, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2 + 3, y2, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(int((animationCounter % 32) / 4), 0, 0));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(int(((animationCounter + 8)% 32) / 4), 0, 0));
      matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(int(((animationCounter + 16) % 32) / 4), 0, 0));
      matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(int(((animationCounter + 24) % 32) / 4), 0, 0));
      matrix.drawPixel(x2, y2 + 3, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2 + 3, y2 + 3, matrix.Color333(0, 0, 3));
      playfield[x2 + 8][y2 + 8] = 2;
      playfield[x2 + 11][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 10] = 2;
      playfield[x2 + 10][y2 + 10] = 2;
      playfield[x2 + 8][y2 + 11] = 2;
      playfield[x2 + 11][y2 + 11] = 2;
    }
  }

  // Plasma floor
  if(dangerType[i] == 11)
  {
    // Remove danger at old position
    if(x1 > -8 && x1 < 32 && y1 > -8 && y1 < 32)
    {   
      if(animationCounter < 65 || (animationCounter > 128 && animationCounter < 193))
      {
        if((animationCounter - 1) % 4 == 0)
        {
          matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 7, y1, matrix.Color333(0, 0, 0));
          playfield[x1 + 8][y1 + 8] = 0;
          playfield[x1 + 15][y1 + 8] = 0;
        }
        else if((animationCounter - 1) % 4 == 1)
        {
          matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 6, y1, matrix.Color333(0, 0, 0));
          playfield[x1 + 9][y1 + 8] = 0;
          playfield[x1 + 14][y1 + 8] = 0;
        }
        else if((animationCounter - 1) % 4 == 2)
        {
          matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 5, y1, matrix.Color333(0, 0, 0));
          playfield[x1 + 10][y1 + 8] = 0;
          playfield[x1 + 13][y1 + 8] = 0;
        }
        else if((animationCounter - 1) % 4 == 3)
        {
          matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 4, y1, matrix.Color333(0, 0, 0));
          playfield[x1 + 11][y1 + 8] = 0;
          playfield[x1 + 12][y1 + 8] = 0;
        }
      }
    }

    // Draw danger at new position
    if(x2 > -8 && x2 < 32 && y2 > -8 && y2 < 32)
    {
      if(animationCounter < 64 || (animationCounter > 127 && animationCounter < 192))
      {
        if(animationCounter % 4 == 0)
        {
          matrix.drawPixel(x2, y2, matrix.Color333(1, 1, 0));
          matrix.drawPixel(x2 + 7, y2, matrix.Color333(1, 1, 0));
          playfield[x2 + 8][y2 + 8] = 2;
          playfield[x2 + 15][y2 + 8] = 2;
          // Sound: Plasma on screen
          tone(audio, 100, 5);
        }
        else if(animationCounter % 4 == 1)
        {
          matrix.drawPixel(x2 + 1, y2, matrix.Color333(3, 3, 1));
          matrix.drawPixel(x2 + 6, y2, matrix.Color333(3, 3, 1));
          playfield[x2 + 9][y2 + 8] = 2;
          playfield[x2 + 14][y2 + 8] = 2;
          // Sound: Plasma on screen
          tone(audio, 200, 5);
        }
        else if(animationCounter % 4 == 2)
        {
          matrix.drawPixel(x2 + 2, y2, matrix.Color333(5, 5, 2));
          matrix.drawPixel(x2 + 5, y2, matrix.Color333(5, 5, 2));
          playfield[x2 + 10][y2 + 8] = 2;
          playfield[x2 + 13][y2 + 8] = 2;
          // Sound: Plasma on screen
          tone(audio, 400, 5);
        }
        else if(animationCounter % 4 == 3)
        {
          matrix.drawPixel(x2 + 3, y2, matrix.Color333(7, 7, 3));
          matrix.drawPixel(x2 + 4, y2, matrix.Color333(7, 7, 3));
          playfield[x2 + 11][y2 + 8] = 2;
          playfield[x2 + 12][y2 + 8] = 2;
          // Sound: Plasma on screen
          tone(audio, 800, 5);
        }
      }
    }
  }

  // Sports glider flying left
  if(dangerType[i] == 12)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 4, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
      playfield[x1 + 11][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
      playfield[x1 + 12][y1 + 9] = 0;
      playfield[x1 + 8][y1 + 10] = 0;
      playfield[x1 + 9][y1 + 10] = 0;
      playfield[x1 + 10][y1 + 10] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2 + 3, y2, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(1, 0, 0));
      if(animationCounter % 2 == 0){ matrix.drawPixel(x2 + 4, y2 + 1, matrix.Color333(random(5) + 1, random(5) + 1, 1)); }
      matrix.drawPixel(x2, y2 + 2, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(1, 0, 0));
      playfield[x2 + 11][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 11][y2 + 9] = 2;
      playfield[x2 + 12][y2 + 9] = 2;
      playfield[x2 + 8][y2 + 10] = 2;
      playfield[x2 + 9][y2 + 10] = 2;
      playfield[x2 + 10][y2 + 10] = 2;
    }
  }

  // Sports glider flying right
  if(dangerType[i] == 13)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 +2, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 2, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 4, y1 + 2, matrix.Color333(0, 0, 0));
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 9] = 0;
      playfield[x1 + 11][y1 + 9] = 0;
      playfield[x1 + 10][y1 + 10] = 0;
      playfield[x1 + 11][y1 + 10] = 0;
      playfield[x1 + 12][y1 + 10] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      matrix.drawPixel(x2 + 1, y2, matrix.Color333(3, 0, 0));
      if(animationCounter % 2 == 0){ matrix.drawPixel(x2, y2 + 1, matrix.Color333(random(5) + 1, random(5) + 1, 1)); }
      matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(1, 0, 0));
      matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 3, y2 + 1, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2 +2, y2 + 2, matrix.Color333(1, 0, 0));
      matrix.drawPixel(x2 + 3, y2 + 2, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 4, y2 + 2, matrix.Color333(3, 0, 0));
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 8][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 9] = 2;
      playfield[x2 + 11][y2 + 9] = 2;
      playfield[x2 + 10][y2 + 10] = 2;
      playfield[x2 + 11][y2 + 10] = 2;
      playfield[x2 + 12][y2 + 10] = 2;
    }
  }  
  
  // Plasma ufo
  if(dangerType[i] == 14)
  {
    // Remove danger at old position
    if(x1 > -8 && x1 < 32 && y1 > -8 && y1 < 32)
    {   
      if(animationCounter < 65 || (animationCounter > 128 && animationCounter < 193))
      {
        if((animationCounter - 1) % 4 == 0)
        {
          matrix.drawPixel(x1 + 1, y1 + 3, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 1, y1 + 4, matrix.Color333(0, 0, 0));
          playfield[x1 + 9][y1 + 11] = 0;
          playfield[x1 + 9][y1 + 12] = 0;
        }
        if((animationCounter - 1) % 4 == 1)
        {
          matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 1, y1 + 5, matrix.Color333(0, 0, 0));
          playfield[x1 + 9][y1 + 10] = 0;
          playfield[x1 + 9][y1 + 13] = 0;
        }
        if((animationCounter - 1) % 4 == 2)
        {
          matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 1, y1 + 6, matrix.Color333(0, 0, 0));
          playfield[x1 + 9][y1 + 9] = 0;
          playfield[x1 + 9][y1 + 14] = 0;
        }
        if((animationCounter - 1) % 4 == 3)
        {
          matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 1, y1 + 7, matrix.Color333(0, 0, 0));
          playfield[x1 + 9][y1 + 8] = 0;
          playfield[x1 + 9][y1 + 15] = 0;
        }
      }
      matrix.drawPixel(x1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 3, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 4, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 4, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 4, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 11] = 0;
      playfield[x1 + 9][y1 + 11] = 0;
      playfield[x1 + 10][y1 + 11] = 0;
      playfield[x1 + 8][y1 + 12] = 0;
      playfield[x1 + 9][y1 + 12] = 0;
      playfield[x1 + 10][y1 + 12] = 0;
    }

    // Draw danger at new position
    if(x2 > -8 && x2 < 32 && y2 > -8 && y2 < 32)
    {
      if(animationCounter < 64 || (animationCounter > 127 && animationCounter < 192))
      {
        if(animationCounter % 4 == 0)
        {
          matrix.drawPixel(x2 + 1, y2 + 3, matrix.Color333(7, 7, 3));
          matrix.drawPixel(x2 + 1, y2 + 4, matrix.Color333(7, 7, 3));
          playfield[x2 + 9][y2 + 11] = 2;
          playfield[x2 + 9][y2 + 12] = 2;
          // Sound: Plasma on screen
          tone(audio, 800, 5);
        }
        if(animationCounter % 4 == 1)
        {
          matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(5, 5, 2));
          matrix.drawPixel(x2 + 1, y2 + 5, matrix.Color333(5, 5, 2));
          playfield[x2 + 9][y2 + 10] = 2;
          playfield[x2 + 9][y2 + 13] = 2;
          // Sound: Plasma on screen
          tone(audio, 400, 5);
        }
        if(animationCounter % 4 == 2)
        {
          matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(3, 3, 1));
          matrix.drawPixel(x2 + 1, y2 + 6, matrix.Color333(3, 3, 1));
          playfield[x2 + 9][y2 + 9] = 2;
          playfield[x2 + 9][y2 + 14] = 2;
          // Sound: Plasma on screen
          tone(audio, 200, 5);
        }
        if(animationCounter % 4 == 3)
        {
          matrix.drawPixel(x2 + 1, y2, matrix.Color333(1, 1, 0));
          matrix.drawPixel(x2 + 1, y2 + 7, matrix.Color333(1, 1, 0));
          playfield[x2 + 9][y2 + 8] = 2;
          playfield[x2 + 9][y2 + 15] = 2;
          // Sound: Plasma on screen
          tone(audio, 100, 5);
        }
      }
      else
      {
        if(animationCounter / 32 < 4)
        {
          matrix.drawPixel(x2 + 1, y2 + 3, matrix.Color333(3, 3, 3));
          playfield[x2 + 9][y2 + 11] = 2;
        }
        else
        {
          matrix.drawPixel(x2 + 1, y2 + 4, matrix.Color333(3, 3, 3));
          playfield[x2 + 9][y2 + 12] = 2;          
        }    
      }
      matrix.drawPixel(x2, y2 + 3, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 2, y2 + 3, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2, y2 + 4, matrix.Color333(3, 0, 0));
      matrix.drawPixel(x2 + 2, y2 + 4, matrix.Color333(3, 0, 0));
      playfield[x2 + 8][y2 + 11] = 2;
      playfield[x2 + 10][y2 + 11] = 2;
      playfield[x2 + 8][y2 + 12] = 2;
      playfield[x2 + 10][y2 + 12] = 2;
    }
  }
  
  // Railcar with steel
  if(dangerType[i] == 15)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      if(animationCounter < 129)
      {
        matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 3, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 3, matrix.Color333(0, 0, 0));
        playfield[x1 + 9][y1 + 8] = 0;
        playfield[x1 + 10][y1 + 8] = 0;
        playfield[x1 + 9][y1 + 9] = 0;
        playfield[x1 + 10][y1 + 9] = 0;
        playfield[x1 + 9][y1 + 10] = 0;
        playfield[x1 + 10][y1 + 10] = 0;
        playfield[x1 + 9][y1 + 11] = 0;
        playfield[x1 + 10][y1 + 11] = 0;
      }
      matrix.drawPixel(x1, y1 + 4, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 4, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 4, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 4, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 5, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 5, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 2, y1 + 5, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 3, y1 + 5, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 12] = 0;
      playfield[x1 + 9][y1 + 12] = 0;
      playfield[x1 + 10][y1 + 12] = 0;
      playfield[x1 + 11][y1 + 12] = 0;
      playfield[x1 + 8][y1 + 13] = 0;
      playfield[x1 + 9][y1 + 13] = 0;
      playfield[x1 + 10][y1 + 13] = 0;
      playfield[x1 + 11][y1 + 13] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    { 
      if(animationCounter < 128)
      {
        if(random(100) < 15){ matrix.drawPixel(x2 + 1, y2, matrix.Color333(1, 1, 0)); }
        if(random(100) < 15){ matrix.drawPixel(x2 + 2, y2, matrix.Color333(1, 1, 0)); }
        if(random(100) < 20){ matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(2, 2, 0)); }
        if(random(100) < 20){ matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(2, 2, 0)); }
        if(random(100) < 25){ matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(3, 3, 1)); }
        if(random(100) < 25){ matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(3, 3, 1)); }
        if(random(100) < 30){ matrix.drawPixel(x2 + 1, y2 + 3, matrix.Color333(4, 4, 1)); }
        if(random(100) < 30){ matrix.drawPixel(x2 + 2, y2 + 3, matrix.Color333(4, 4, 1)); }
        playfield[x2 + 9][y2 + 8] = 2;
        playfield[x2 + 10][y2 + 8] = 2;
        playfield[x2 + 9][y2 + 9] = 2;
        playfield[x2 + 10][y2 + 9] = 2;
        playfield[x2 + 9][y2 + 10] = 2;
        playfield[x2 + 10][y2 + 10] = 2;
        playfield[x2 + 9][y2 + 11] = 2;
        playfield[x2 + 10][y2 + 11] = 2;
        // SOUND: Fire
        tone(audio, 50 + random(350), 5);        
      }
      matrix.drawPixel(x2, y2 + 4, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2 + 1, y2 + 4, matrix.Color333(0, 0, 1));
      matrix.drawPixel(x2 + 2, y2 + 4, matrix.Color333(0, 0, 1));
      matrix.drawPixel(x2 + 3, y2 + 4, matrix.Color333(0, 0, 3));
      matrix.drawPixel(x2, y2 + 5, matrix.Color333(2, 2, 2));
      matrix.drawPixel(x2 + 1, y2 + 5, matrix.Color333(0, 0, 2));
      matrix.drawPixel(x2 + 2, y2 + 5, matrix.Color333(0, 0, 2));
      matrix.drawPixel(x2 + 3, y2 + 5, matrix.Color333(2, 2, 2));
      playfield[x2 + 8][y2 + 12] = 2;
      playfield[x2 + 9][y2 + 12] = 2;
      playfield[x2 + 10][y2 + 12] = 2;
      playfield[x2 + 11][y2 + 12] = 2;
      playfield[x2 + 8][y2 + 13] = 2;
      playfield[x2 + 9][y2 + 13] = 2;
      playfield[x2 + 10][y2 + 13] = 2;
      playfield[x2 + 11][y2 + 13] = 2;
    }
  }
  
  // Meteor
  if(dangerType[i] == 16)
  {
    // Remove danger at old position
    if(x1 > -5 && x1 < 32 && y1 > -5 && y1 < 32)
    {   
      matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
      playfield[x1 + 8][y1 + 8] = 0;
      playfield[x1 + 9][y1 + 8] = 0;
      playfield[x1 + 8][y1 + 9] = 0;
      playfield[x1 + 9][y1 + 9] = 0;
    }

    // Draw danger at new position
    if(x2 > -5 && x2 < 32 && y2 > -5 && y2 < 32)
    {
      if(animationCounter % 4 == 3)
      {
        matrix.drawPixel(x2, y2, matrix.Color333(4, 4, 1));
        matrix.drawPixel(x2 + 1, y2, matrix.Color333(3, 3, 1));
      }
      if(animationCounter % 2 == 1)
      {
        matrix.drawPixel(x2, y2 + 1, matrix.Color333(3, 3, 1));
        matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(2, 2, 1));
      }
      playfield[x2 + 8][y2 + 8] = 2;
      playfield[x2 + 9][y2 + 8] = 2;
      playfield[x2 + 8][y2 + 9] = 2;
      playfield[x2 + 9][y2 + 9] = 2;
    }
  }
  
  dangerXScreen[i] = x2;
  dangerYScreen[i] = y2;
}

// moveDangers: Move all active dangers
void moveDangers()
{
  for(i = 0; i < 16; i++)
  {
    if(dangerStatus[i] > 0)
    {
      // Other taxi flying left
      if(dangerType[i] == 2)
      { 
        if(dangerXMap[i] > -4)
        {
          dangerXMap[i] = dangerXMap[i] - 0.25;
        }
        else
        {
          dangerXMap[i] = mapWidth - 1;
        }      
      }

      // Other taxi flying right
      if(dangerType[i] == 3)
      { 
        if(dangerXMap[i] < mapWidth)
        {
          dangerXMap[i] = dangerXMap[i] + 0.25;
        }
        else
        {
          dangerXMap[i] = -3;
        }      
      }
      
      // Container
      if(dangerType[i] == 4)
      { 
        // Moving up
        if(dangerMovement[i] == 0)
        {
          if(dangerYMap[i] > dangerY1[i])
          {
            dangerYMap[i] = dangerYMap[i] - 0.15;
          }
          else
          {
            dangerMovement[i] = 1;
          }
        }
        // Moving down
        else
        {
          if(dangerYMap[i] < dangerY2[i])
          {
            dangerYMap[i] = dangerYMap[i] + 0.45;
          }
          else
          {
            dangerMovement[i] = 0;
          }
        }  
      }    

      // Mech
      if(dangerType[i] == 5)
      { 
        // Moving left
        if(dangerMovement[i] == 0)
        {
          if(dangerXMap[i] > dangerX1[i])
          {
            dangerXMap[i] = dangerXMap[i] - 0.2;
          }
          else
          {
            dangerMovement[i] = 1;
          }
        }
        else
        {
          if(dangerXMap[i] < dangerX2[i])
          {
            dangerXMap[i] = dangerXMap[i] + 0.2;
          }
          else
          {
            dangerMovement[i] = 0;
          }
        }      
      }

      // Fire
      if(dangerType[i] == 6)
      {
        // Do nothing
      } 

      // Gripper
      if(dangerType[i] == 7 && animationCounter % 2 == 0) // Reduce speed by factor 2
      {
        // Gripper is raised
        if(dangerMovement[i] > 7)
        {
          dangerYMap[i]--;
          dangerMovement[i] = dangerMovement[i] + 2;
        }
        // Gripper lowers down
        if(dangerMovement[i] > 1 && dangerMovement[i] < 8)
        {
          dangerYMap[i]++;
          dangerMovement[i] = dangerMovement[i] + 2;
        }
        // Gripper back in standard position
        if(dangerMovement[i] > 13)
        {
          dangerMovement[i] = dangerMovement[i] - 14;
        }
        // Moving left
        if(dangerMovement[i] % 2 == 0)
        {
          if(dangerXMap[i] > dangerX1[i])
          {
            dangerXMap[i] = dangerXMap[i] - 0.5;
          }
          else
          {
            dangerMovement[i] = dangerMovement[i] + 1;
          }
        }
        else
        {
          if(dangerXMap[i] < dangerX2[i])
          {
            dangerXMap[i] = dangerXMap[i] + 0.5;
          }
          else
          {
            dangerMovement[i] = dangerMovement[i] - 1;
          }
        }
        // Start to lower gripper
        if(dangerMovement[i] < 2 && random(100) < 5)
        {
          dangerMovement[i] = dangerMovement[i] + 2;
        }
      } 

      // Pirate
      if(dangerType[i] == 8)
      {
        // Move (turns towards taxi´s ship)
        // Calculate directional vector of the shot
        x = (double(taxiXMap + 1.0) - (dangerXMap[i] + 2.0));
        y = (double(taxiYMap + 2.0) - (dangerYMap[i] + 1.5));
        // Calculate length of directional vector to normalize the shot speed
        z = sqrt(sq(x) + sq(y));
        dangerXMap[i] = dangerXMap[i] + ((x / z) / 20.0);
        dangerYMap[i] = dangerYMap[i] + ((y / z) / 20.0);
        if(dangerXMap[i] < 0)
        {
          dangerXMap[i] = 0;
        }
        if(dangerXMap[i] > (mapWidth - 4))
        {
          dangerXMap[i] = (mapWidth - 4);
        }
        if(dangerYMap[i] < 0)
        {
          dangerYMap[i] = 0;
        }
        if(dangerYMap[i] > (mapHeight - 4))
        {
          dangerYMap[i] = (mapHeight - 4);
        }
      } 

      // Missile
      if(dangerType[i] == 9)
      {
        if(dangerYMap[i] > -5)
        {
          dangerYMap[i] = dangerYMap[i] - (float(dangerMovement[i]) / 200.0);
          if(dangerMovement[i] < 255){ dangerMovement[i]++; }
        }        
        else
        {
          dangerYMap[i] = mapHeight;
          dangerMovement[i] = 1;
        }      
      } 

      // Drone
      if(dangerType[i] == 10)
      {
        if(dangerMovement[i] == 1)
        {
          if(dangerXMap[i] < (mapWidth - 5) && dangerYMap[i] > 2)
          {
            dangerXMap[i] = dangerXMap[i] + 0.15;
            dangerYMap[i] = dangerYMap[i] - 0.15;
          }
          else
          {
            dangerMovement[i] = byte(random(4)) + 1;
          }
        }
        else if(dangerMovement[i] == 2)
        {
          if(dangerXMap[i] < (mapWidth - 5) && dangerYMap[i] < (mapHeight - 5))
          {
            dangerXMap[i] = dangerXMap[i] + 0.15;
            dangerYMap[i] = dangerYMap[i] + 0.15;
          }
          else
          {
            dangerMovement[i] = byte(random(4)) + 1;
          }
        }
        else if(dangerMovement[i] == 3)
        {
          if(dangerXMap[i] > 2 && dangerYMap[i] < (mapHeight - 5))
          {
            dangerXMap[i] = dangerXMap[i] - 0.15;
            dangerYMap[i] = dangerYMap[i] + 0.15;
          }
          else
          {
            dangerMovement[i] = byte(random(4)) + 1;
          }
        }
        else if(dangerMovement[i] == 4)
        {
          if(dangerXMap[i] > 2 && dangerYMap[i] > 2)
          {
            dangerXMap[i] = dangerXMap[i] - 0.15;
            dangerYMap[i] = dangerYMap[i] - 0.15;
          }
          else
          {
            dangerMovement[i] = byte(random(4)) + 1;
          }
        }      
      } 

      // Plasma floor
      if(dangerType[i] == 11)
      {
        // Do nothing
      } 

      // Sports glider flying left
      if(dangerType[i] == 12)
      { 
        if(dangerXMap[i] > -4)
        {
          dangerXMap[i] = dangerXMap[i] - 0.4;
        }
        else
        {
          dangerXMap[i] = mapWidth - 1;
        }      
      }

      // Sports glider flying right
      if(dangerType[i] == 13)
      { 
        if(dangerXMap[i] < mapWidth)
        {
          dangerXMap[i] = dangerXMap[i] + 0.4;
        }
        else
        {
          dangerXMap[i] = -3;
        }      
      }

      // Plasma ufo
      if(dangerType[i] == 14)
      {
        // Moving left
        if(dangerMovement[i] == 0)
        {
          if(dangerXMap[i] > dangerX1[i])
          {
            dangerXMap[i] = dangerXMap[i] - 0.25;
          }
          else
          {
            dangerMovement[i] = 1;
          }
        }
        else
        {
          if(dangerXMap[i] < dangerX2[i])
          {
            dangerXMap[i] = dangerXMap[i] + 0.25;
          }
          else
          {
            dangerMovement[i] = 0;
          }
        }        
      } 

      // Railcar with steel
      if(dangerType[i] == 15)
      {
        // Moving left
        if(dangerMovement[i] == 0)
        {
          if(dangerXMap[i] > dangerX1[i])
          {
            dangerXMap[i] = dangerXMap[i] - 0.15;
          }
          else
          {
            dangerMovement[i] = 1;
          }
        }
        else
        {
          if(dangerXMap[i] < dangerX2[i])
          {
            dangerXMap[i] = dangerXMap[i] + 0.15;
          }
          else
          {
            dangerMovement[i] = 0;
          }
        }      
      } 

      // Meteor
      if(dangerType[i] == 16)
      {
        dangerXMap[i] = dangerXMap[i] - (float(dangerMovement[i]) - 10.0) / 100.0;
        if(dangerYMap[i] < mapHeight)
        {
          dangerYMap[i] = dangerYMap[i] + 0.4;
        }        
        else
        {
          dangerYMap[i] = -1;
          dangerXMap[i] = dangerX1[i];
          dangerMovement[i] = random(21);
        }      
      }

      dangerXScreenNew[i] = dangerXMap[i] - screenXNew;
      dangerYScreenNew[i] = dangerYMap[i] - screenYNew;
      drawDangers(i);
    }
  }  
}

// initializeDangerMovement: Initialize danger movement (for those dangers moving between two points)
void initializeDangerMovement()
{
  for(i = 0; i < 16; i++)
  {
    if(dangerStatus[i] > 0)
    {      
      // Other taxi flying left
      if(dangerType[i] == 2)
      {  
        // Nothing to do
      }
      
      // Other taxi flying right
      if(dangerType[i] == 3)
      {  
        // Nothing to do
      }
      
      // Container
      if(dangerType[i] == 4)
      {
        j = dangerX1[i];
        k = dangerY1[i];
        while(k > 0 && ((levelMap[(tileNumberX * k) + j] == 0) || (levelMap[(tileNumberX * k) + j] == 3) || (levelMap[(tileNumberX * k) + j] == 4)))
        {
          k--;
        }
        if(k == 0)
        {
          dangerY1[i] = k * 8;
        }
        else
        {
          dangerY1[i] = ((k + 1) * 8) + 1;
        }
        j = dangerX2[i];
        k = dangerY2[i];
        while(k < tileNumberY && levelMap[(tileNumberX * k) + j] == 0)
        {
          k++;
        }
        if(levelMap[(tileNumberX * k) + j] == 1 || levelMap[(tileNumberX * k) + j] == 5 || levelMap[(tileNumberX * k) + j] == 6)
        {
          dangerY2[i] = (k * 8) + 5;
        }
        else if(levelMap[(tileNumberX * k) + j] == 2)
        {
          dangerY2[i] = (k * 8) + 1;
        }
        else
        {
          dangerY2[i] = ((k - 1) * 8) + 6;
        }
      }
      
      // Mech
      if(dangerType[i] == 5)
      {  
        j = dangerX1[i];
        k = dangerY1[i];
        while(j > 0 && levelMap[(tileNumberX * k) + j] == 1)
        {
          j--;
        }
        if(j == 0 && levelMap[(tileNumberX * k) + j] == 1)
        {
          dangerX1[i] = (j * 8) + 1;
        }
        else
        {
          dangerX1[i] = ((j + 1) * 8) + 1;
        }
        j = dangerX2[i];
        k = dangerY2[i];
        while(j < tileNumberX && levelMap[(tileNumberX * k) + j] == 1)
        {
          j++;
        }
        dangerX2[i] = ((j - 1) * 8) + 3;
      }
      
      // Fire
      if(dangerType[i] == 6)
      {  
        // Nothing to do
      }
      
      // Gripper
      if(dangerType[i] == 7)
      {  
        j = dangerX1[i];
        k = dangerY1[i];
        while(j > 0 && (levelMap[(tileNumberX * (k - 1)) + j] == 1 || levelMap[(tileNumberX * (k - 1)) + j] == 5 || levelMap[(tileNumberX * (k - 1)) + j] == 6))
        {
          j--;
        }
        if(j == 0 && (levelMap[(tileNumberX * (k - 1)) + j] == 1 || levelMap[(tileNumberX * (k - 1)) + j] == 5 || levelMap[(tileNumberX * (k - 1)) + j] == 6))
        {
          dangerX1[i] = j * 8;
        }
        else
        {
          dangerX1[i] = (j + 1) * 8;
        }
        j = dangerX2[i];
        k = dangerY2[i];
        while(j < tileNumberX && (levelMap[(tileNumberX * (k - 1)) + j] == 1 || levelMap[(tileNumberX * (k - 1)) + j] == 5 || levelMap[(tileNumberX * (k - 1)) + j] == 6))
        {
          j++;
        }
        if(j == 1)
        {
          dangerX2[i] = (j * 8) + 6;
        }
        else
        {
          dangerX2[i] = ((j - 1) * 8) + 6;
        }
      }
      
      // Pirate
      if(dangerType[i] == 8)
      {  
        // Nothing to do
      }
      
      // Missile
      if(dangerType[i] == 9)
      {  
        // Nothing to do
      }
      
      // Drone
      if(dangerType[i] == 10)
      {  
        // Nothing to do
      }
      
      // Plasma ufo
      if(dangerType[i] == 14)
      {  
        j = dangerX1[i];
        k = dangerY1[i];
        while(j > 0 && (levelMap[(tileNumberX * k) + j] == 0 || levelMap[(tileNumberX * k) + j] == 1))
        {
          j--;
        }
        if(j == 0 && (levelMap[(tileNumberX * k) + j] == 0 || levelMap[(tileNumberX * k) + j] == 1))
        {
          dangerX1[i] = j * 8;
        }
        else
        {
          dangerX1[i] = (j + 1) * 8;
        }
        j = dangerX2[i];
        k = dangerY2[i];
        while(j < tileNumberX && (levelMap[(tileNumberX * k) + j] == 0 || levelMap[(tileNumberX * k) + j] == 1))
        {
          j++;
        }
        if(levelMap[(tileNumberX * k) + j] == 4)
        {
          dangerX2[i] = (j * 8) + 3;
        }
        else
        {
          dangerX2[i] = ((j - 1) * 8) + 4;
        }
      }      
      
      // Railcar with steel
      if(dangerType[i] == 15)
      {  
        j = dangerX1[i];
        k = dangerY1[i];
        while(j > 0 && levelMap[(tileNumberX * k) + j] == 1)
        {
          j--;
        }
        if(j == 0 && levelMap[(tileNumberX * k) + j] == 1)
        {
          dangerX1[i] = j * 8;
        }
        else if(levelMap[(tileNumberX * k) + j] == 5)
        {
          dangerX1[i] = (j * 8) + 1;
        }
        else
        {
          dangerX1[i] = (j + 1) * 8;
        }
        j = dangerX2[i];
        k = dangerY2[i];
        while(j < tileNumberX && levelMap[(tileNumberX * k) + j] == 1)
        {
          j++;
        }
        if(levelMap[(tileNumberX * k) + j] == 6)
        {
          dangerX2[i] = (j * 8) + 5;
        }
        else
        {
          dangerX2[i] = ((j - 1) * 8) + 6;
        }
      }

      // Meteor
      if(dangerType[i] == 16)
      {
        // Nothing to do
      }      
    }
  }
}

// drawExtraLives: Draw all not collected extra lives
void drawExtraLives()
{
  for(i = 0; i < 16; i++)
  {
    if(extraLifeStatus[i] == 1 || extraLifeStatus[i] == 2)
    {
      extraLifeXScreenNew[i] = extraLifeXMap[i] - screenXNew;
      extraLifeYScreenNew[i] = extraLifeYMap[i] - screenYNew;

      if(extraLifeXScreen[i] > -8 && extraLifeXScreen[i] < 32 && extraLifeYScreen[i] > -8 && extraLifeYScreen[i] < 32)
      {     
        x1 = extraLifeXScreen[i];
        y1 = extraLifeYScreen[i];
        x2 = extraLifeXScreenNew[i];
        y2 = extraLifeYScreenNew[i];
                
        // Remove extra lives at old position
        matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1 + 2, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 3, y1 + 2, matrix.Color333(0, 0, 0));

        playfield[x1 + 8][y1 + 8] = 0;
        playfield[x1 + 9][y1 + 8] = 0;
        playfield[x1 + 8][y1 + 9] = 0;
        playfield[x1 + 10][y1 + 9] = 0;
        playfield[x1 + 8][y1 + 10] = 0;
        playfield[x1 + 9][y1 + 10] = 0;
        playfield[x1 + 10][y1 + 10] = 0;
        playfield[x1 + 11][y1 + 10] = 0;
        
        if(extraLifeStatus[i] == 2)
        {
          extraLifeStatus[i] = 0;
          extraLifeCounter--;
        }

        // Draw extra Lives at new position
        if(extraLifeStatus[i] == 1)
        {
          if(extraLifeXScreenNew[i] > -8 && extraLifeXScreenNew[i] < 32 && extraLifeYScreenNew[i] > -8 && extraLifeYScreenNew[i] < 32)
          {
            matrix.drawPixel(x2, y2, matrix.Color333(random(2), random(2), random(2)));
            matrix.drawPixel(x2 + 1, y2, matrix.Color333(random(2), random(2), random(2)));
            matrix.drawPixel(x2, y2 + 1, matrix.Color333(random(2), random(2), random(2)));
            matrix.drawPixel(x2 + 2, y2 + 1, matrix.Color333(random(2), random(2), random(2)));
            matrix.drawPixel(x2, y2 + 2, matrix.Color333(random(2), random(2), random(2)));
            matrix.drawPixel(x2 + 1, y2 + 2, matrix.Color333(random(2), random(2), random(2)));
            matrix.drawPixel(x2 + 2, y2 + 2, matrix.Color333(random(2), random(2), random(2)));
            matrix.drawPixel(x2 + 3, y2 + 2, matrix.Color333(random(2), random(2), random(2)));
    
            playfield[x2 + 8][y2 + 8] = i + 10;
            playfield[x2 + 9][y2 + 8] = i + 10;
            playfield[x2 + 8][y2 + 9] = i + 10;
            playfield[x2 + 10][y2 + 9] = i + 10;
            playfield[x2 + 8][y2 + 10] = i + 10;
            playfield[x2 + 9][y2 + 10] = i + 10;
            playfield[x2 + 10][y2 + 10] = i + 10;
            playfield[x2 + 11][y2 + 10] = i + 10;
          }
        }
      }  
      extraLifeXScreen[i] = extraLifeXScreenNew[i];
      extraLifeYScreen[i] = extraLifeYScreenNew[i];
    }
  }  
}

// drawPlatforms: Draw platforms where passengers are picked up or delivered to
void drawPlatforms()
{
  byte r, g, b;
  for(i = 0; i < 6; i++)
  {
    if(platformBordingStatus[i] > 0)
    {
      // Add another waiting passenger
      if(platformBordingStatus[i] == 1 && numberOfWaitingPassengers < maximalNumberOfWaitingPassengers && random(1000) < 10)
      {
        numberOfWaitingPassengers++;
        platformBordingStatus[i] = 2;
      }
      
      platformXScreenNew[i] = platformXMap[i] - screenXNew;
      platformYScreenNew[i] = platformYMap[i] - screenYNew;

      if(platformXScreen[i] > -8 && platformXScreen[i] < 32 && platformYScreen[i] > -8 && platformYScreen[i] < 32)
      {     
        x1 = platformXScreen[i];
        y1 = platformYScreen[i];
        x2 = platformXScreenNew[i];
        y2 = platformYScreenNew[i];
                
        // Remove platforms at old position
        matrix.drawPixel(x1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 4, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 5, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 6, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 7, y1 + 1, matrix.Color333(0, 0, 0));
        playfield[x1 + 8][y1 + 9] = 0;
        playfield[x1 + 9][y1 + 8] = 0;
        playfield[x1 + 10][y1 + 8] = 0;
        playfield[x1 + 11][y1 + 8] = 0;
        playfield[x1 + 12][y1 + 8] = 0;
        playfield[x1 + 13][y1 + 8] = 0;
        playfield[x1 + 14][y1 + 8] = 0;
        playfield[x1 + 15][y1 + 9] = 0;
        playfield[x1 + 11][y1 + 7] = 0; // "Detection zone" above platform
        playfield[x1 + 12][y1 + 7] = 0; // "Detection zone" above platform

        // Passenger is waiting
        if(platformBordingStatus[i] == 2)
        {
          if((animationCounter / 4) > 55 || (animationCounter / 4) == 0){ matrix.drawPixel(x1 + 7, y1 - 2, matrix.Color333(0, 0, 0)); }
          matrix.drawPixel(x1 + 7, y1 - 1, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 7, y1, matrix.Color333(0, 0, 0));              
        }
        // Intermediate status (passenger stops jumping and starts to bord taxi)
        else if(platformBordingStatus[i] == 3)
        {
          if((animationCounter / 4) > 55 || (animationCounter / 4) == 0){ matrix.drawPixel(x1 + 7, y1 - 2, matrix.Color333(0, 0, 0)); }
          matrix.drawPixel(x1 + 7, y1 - 1, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 7, y1, matrix.Color333(0, 0, 0));
          platformBordingStatus[i] = 4;
        }
        else if(platformBordingStatus[i] == 4)
        {
          matrix.drawPixel(x1 + 6, y1 - 2, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 6, y1 - 1, matrix.Color333(0, 0, 0));
          if(animationCounter % 32 == animationCounterSyncValue){ platformBordingStatus[i] = 5; footstepSound = true; }
        }          
        else if(platformBordingStatus[i] == 5)
        {
          matrix.drawPixel(x1 + 5, y1 - 2, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 5, y1 - 1, matrix.Color333(0, 0, 0));              
          if(animationCounter % 32 == animationCounterSyncValue){ platformBordingStatus[i] = 6; footstepSound = true; }
        }          
        else if(platformBordingStatus[i] == 6)
        {
          matrix.drawPixel(x1 + 4, y1 - 2, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 4, y1 - 1, matrix.Color333(0, 0, 0));              
          if(animationCounter % 32 == animationCounterSyncValue)
          { 
            // New target platform
            do
            {
              targetPlatform = platformColor[random(platformCounter)];
            }
            while(targetPlatform == platformColor[i]); // Avoids that the target platform is the current platform
            platformBordingStatus[i] = 1;
            numberOfWaitingPassengers--;            
            taxiParks = false;
          }
        }
        
        // Passenger leaves taxi
        if(platformDisembarkingStatus[i] == 1)
        {
          matrix.drawPixel(x1 + 3, y1 - 2, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 3, y1 - 1, matrix.Color333(0, 0, 0));                          
          if(animationCounter % 32 == animationCounterSyncValue){ platformDisembarkingStatus[i] = 2; footstepSound = true; }
        }
        else if(platformDisembarkingStatus[i] == 2)
        {
          matrix.drawPixel(x1 + 2, y1 - 2, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 2, y1 - 1, matrix.Color333(0, 0, 0));                          
          if(animationCounter % 32 == animationCounterSyncValue){ platformDisembarkingStatus[i] = 3; footstepSound = true; }
        }
        else if(platformDisembarkingStatus[i] == 3)
        {
          matrix.drawPixel(x1 + 1, y1 - 2, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1 + 1, y1 - 1, matrix.Color333(0, 0, 0));                          
          if(animationCounter % 32 == animationCounterSyncValue){ platformDisembarkingStatus[i] = 4; footstepSound = true; }
        }
        else if(platformDisembarkingStatus[i] == 4)
        {
          matrix.drawPixel(x1, y1 - 1, matrix.Color333(0, 0, 0));
          matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));                          
          if(animationCounter % 32 == animationCounterSyncValue)
          { 
            targetPlatform = 0;
            platformDisembarkingStatus[i] = 0;
            remainingPassengersToFinishLevel--;
            passengers++;
            taxiParks = false;
          }
        }
                  
        // Draw platforms at new position
        if(platformXScreenNew[i] > -8 && platformXScreenNew[i] < 32 && platformYScreenNew[i] > -8 && platformYScreenNew[i] < 32)
        {
          switch(platformColor[i])
          {
            case RED:
              r = 2; g = 0; b = 0;
              break;
            case GREEN:
              r = 0; g = 2; b = 0;
              break;
            case BLUE:
              r = 0; g = 0; b = 2;
              break;
            case YELLOW:
              r = 2; g = 2; b = 0;
              break;
            case VIOLET:
              r = 2; g = 0; b = 2;
              break;
            case TURQUOISE:
              r = 0; g = 2; b = 2;
              break;
            default:
              break;
          }
                   
          matrix.drawPixel(x2, y2 + 1, matrix.Color333(1, 1, 1));
          matrix.drawPixel(x2 + 1, y2, matrix.Color333(r, g, b));
          matrix.drawPixel(x2 + 2, y2, matrix.Color333(r, g, b));
          matrix.drawPixel(x2 + 3, y2, matrix.Color333(r, g, b));
          matrix.drawPixel(x2 + 4, y2, matrix.Color333(r, g, b));
          matrix.drawPixel(x2 + 5, y2, matrix.Color333(r, g, b));
          matrix.drawPixel(x2 + 6, y2, matrix.Color333(r, g, b));
          matrix.drawPixel(x2 + 7, y2 + 1, matrix.Color333(1, 1, 1));
          
          playfield[x2 + 8][y2 + 9] = 1;
          playfield[x2 + 9][y2 + 8] = 1;
          playfield[x2 + 10][y2 + 8] = 1;
          playfield[x2 + 11][y2 + 8] = 1;
          playfield[x2 + 12][y2 + 8] = 1;
          playfield[x2 + 13][y2 + 8] = 1;
          playfield[x2 + 14][y2 + 8] = 1;
          playfield[x2 + 15][y2 + 9] = 1;
          playfield[x2 + 11][y2 + 7] = platformColor[i] + 3; // "Detection zone" above platform
          playfield[x2 + 12][y2 + 7] = platformColor[i] + 3; // "Detection zone" above platform
          // Passenger is waiting and jumps
          if(platformBordingStatus[i] == 2)
          {
            if((animationCounter / 4) < 56)
            {
              matrix.drawPixel(x2 + 7, y2 - 1, matrix.Color333(5, 2, 0));
              matrix.drawPixel(x2 + 7, y2, matrix.Color333(3, 0, 0));              
            }
            else
            {
              matrix.drawPixel(x2 + 7, y2 - 2, matrix.Color333(5, 2, 0));
              matrix.drawPixel(x2 + 7, y2 - 1, matrix.Color333(3, 0, 0));                            
            }
          }
          // Passenger enters taxi
          else if(platformBordingStatus[i] == 4)
          {
            matrix.drawPixel(x2 + 6, y2 - 2, matrix.Color333(5, 2, 0));
            matrix.drawPixel(x2 + 6, y2 - 1, matrix.Color333(3, 0, 0));
            if(footstepSound){ tone(audio, 512, 10); footstepSound = false; }
          }          
          else if(platformBordingStatus[i] == 5)
          {
            matrix.drawPixel(x2 + 5, y2 - 2, matrix.Color333(5, 2, 0));
            matrix.drawPixel(x2 + 5, y2 - 1, matrix.Color333(3, 0, 0));              
            if(footstepSound){ tone(audio, 256, 10); footstepSound = false; }
          }          
          else if(platformBordingStatus[i] == 6)
          {
            matrix.drawPixel(x2 + 4, y2 - 2, matrix.Color333(5, 2, 0));
            matrix.drawPixel(x2 + 4, y2 - 1, matrix.Color333(3, 0, 0));              
            if(footstepSound){ tone(audio, 512, 10); footstepSound = false; }
          }

          // Passenger leaves taxi
          if(platformDisembarkingStatus[i] == 1)
          {
            matrix.drawPixel(x2 + 3, y2 - 2, matrix.Color333(5, 2, 0));
            matrix.drawPixel(x2 + 3, y2 - 1, matrix.Color333(3, 0, 0));                          
            if(footstepSound){ tone(audio, 512, 10); footstepSound = false; }
          }
          else if(platformDisembarkingStatus[i] == 2)
          {
            matrix.drawPixel(x2 + 2, y2 - 2, matrix.Color333(5, 2, 0));
            matrix.drawPixel(x2 + 2, y2 - 1, matrix.Color333(3, 0, 0));                          
            if(footstepSound){ tone(audio, 256, 10); footstepSound = false; }
          }
          else if(platformDisembarkingStatus[i] == 3)
          {
            matrix.drawPixel(x2 + 1, y2 - 2, matrix.Color333(5, 2, 0));
            matrix.drawPixel(x2 + 1, y2 - 1, matrix.Color333(3, 0, 0));                          
            if(footstepSound){ tone(audio, 512, 10); footstepSound = false; }
          }
          else if(platformDisembarkingStatus[i] == 4)
          {
            matrix.drawPixel(x2, y2 - 1, matrix.Color333(5, 2, 0));
            matrix.drawPixel(x2, y2, matrix.Color333(3, 0, 0));                          
            if(footstepSound){ tone(audio, 256, 10); footstepSound = false; }
          }
        }
      }  
      platformXScreen[i] = platformXScreenNew[i];
      platformYScreen[i] = platformYScreenNew[i];
    }
  }  
}

// drawGasStations: Draw gas stations where taxi can refuel (and start each game)
void drawGasStations()
{
  for(i = 0; i < 4; i++)
  {
    if(gasStationStatus[i])
    {
      gasStationXScreenNew[i] = gasStationXMap[i] - screenXNew;
      gasStationYScreenNew[i] = gasStationYMap[i] - screenYNew;

      if(gasStationXScreen[i] > -8 && gasStationXScreen[i] < 32 && gasStationYScreen[i] > -8 && gasStationYScreen[i] < 32)
      {     
        x1 = gasStationXScreen[i];
        y1 = gasStationYScreen[i];
        x2 = gasStationXScreenNew[i];
        y2 = gasStationYScreenNew[i];
                
        // Remove platforms at old position
        matrix.drawPixel(x1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 2, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 3, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 4, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 5, y1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 1, y1 + 1, matrix.Color333(0, 0, 0));
        matrix.drawPixel(x1 + 4, y1 + 1, matrix.Color333(0, 0, 0));
        playfield[x1 + 8][y1 + 8] = 0;
        playfield[x1 + 9][y1 + 8] = 0;
        playfield[x1 + 10][y1 + 8] = 0;
        playfield[x1 + 11][y1 + 8] = 0;
        playfield[x1 + 12][y1 + 8] = 0;
        playfield[x1 + 13][y1 + 8] = 0;
        playfield[x1 + 9][y1 + 9] = 0;
        playfield[x1 + 12][y1 + 9] = 0;
        playfield[x1 + 10][y1 + 7] = 0; // "Detection zone" above gas station
        playfield[x1 + 11][y1 + 7] = 0; // "Detection zone" above gas station

        // Draw platforms at new position
        if(gasStationXScreenNew[i] > -8 && gasStationXScreenNew[i] < 32 && gasStationYScreenNew[i] > -8 && gasStationYScreenNew[i] < 32)
        { 
          if(refuling)
          {
            if((animationCounter / 8) % 2 == 0)
            {
              matrix.drawPixel(x2, y2, matrix.Color333(3, 3, 0));
              matrix.drawPixel(x2 + 5, y2, matrix.Color333(3, 3, 0));         
            }
            else
            {
              matrix.drawPixel(x2, y2, matrix.Color333(1, 1, 1));
              matrix.drawPixel(x2 + 5, y2, matrix.Color333(1, 1, 1));         
            }            
          }
          else
          {
            if((animationCounter / 64) % 2 == 0)
            {
              matrix.drawPixel(x2, y2, matrix.Color333(0, 4, 0));
              matrix.drawPixel(x2 + 5, y2, matrix.Color333(4, 0, 0));         
            }
            else
            {
              matrix.drawPixel(x2, y2, matrix.Color333(1, 1, 1));
              matrix.drawPixel(x2 + 5, y2, matrix.Color333(1, 1, 1));         
            }
          }          
          matrix.drawPixel(x2 + 1, y2, matrix.Color333(2, 2, 2));
          matrix.drawPixel(x2 + 2, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 3, y2, matrix.Color333(3, 3, 3));
          matrix.drawPixel(x2 + 4, y2, matrix.Color333(2, 2, 2));
          matrix.drawPixel(x2 + 1, y2 + 1, matrix.Color333(1, 1, 1));
          matrix.drawPixel(x2 + 4, y2 + 1, matrix.Color333(1, 1, 1));
          playfield[x2 + 8][y2 + 8] = 1;
          playfield[x2 + 9][y2 + 8] = 1;
          playfield[x2 + 10][y2 + 8] = 1;
          playfield[x2 + 11][y2 + 8] = 1;
          playfield[x2 + 12][y2 + 8] = 1;
          playfield[x2 + 13][y2 + 8] = 1;
          playfield[x2 + 9][y2 + 9] = 1;
          playfield[x2 + 12][y2 + 9] = 1;
          playfield[x2 + 10][y2 + 7] = 3; // "Detection zone" above gas station
          playfield[x2 + 11][y2 + 7] = 3; // "Detection zone" above gas station
        }
      }  
      gasStationXScreen[i] = gasStationXScreenNew[i];
      gasStationYScreen[i] = gasStationYScreenNew[i];
    }
  }  
}

// drawTaxi: Draw taxi
void drawTaxi(boolean blinking, byte blinkInterval)
{
  // Remove taxi at old position
  matrix.drawPixel(taxiXScreen, taxiYScreen, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen + 1, taxiYScreen, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen + 2, taxiYScreen, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen + 3, taxiYScreen, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen, taxiYScreen + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen + 1, taxiYScreen + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen + 2, taxiYScreen + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen + 3, taxiYScreen + 1, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen, taxiYScreen + 2, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen + 1, taxiYScreen + 2, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen + 2, taxiYScreen + 2, matrix.Color333(0, 0, 0));
  matrix.drawPixel(taxiXScreen + 3, taxiYScreen + 2, matrix.Color333(0, 0, 0));

  if(!blinking || animationCounter % blinkInterval != 0)
  {
    // Draw taxi at new position
    if(taxiDirectionNew == RIGHT)
    {
      matrix.drawPixel(taxiXScreenNew, taxiYScreenNew, matrix.Color333(0, 5, 2));
      matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew, matrix.Color333(0, 3, 1));
      matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew, matrix.Color333(0, 0, 0));
      matrix.drawPixel(taxiXScreenNew + 3, taxiYScreenNew, matrix.Color333(0, 0, 0));
      matrix.drawPixel(taxiXScreenNew, taxiYScreenNew + 1, matrix.Color333(0, 3, 1));
      switch(targetPlatform)
      {
        case RED:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(2, 0, 0));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(3, 0, 0));
          break;
        case GREEN:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(0, 2, 0));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(0, 3, 0));
          break;
        case BLUE:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(0, 0, 2));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(0, 0, 3));
          break;
        case YELLOW:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(2, 2, 0));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(3, 3, 0));
          break;
        case VIOLET:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(2, 0, 2));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(3, 0, 3));
          break;
        case TURQUOISE:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(0, 2, 2));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(0, 3, 3));
          break;
        default:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(1, 1, 1));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(2, 2, 2));
          break;
      }
      matrix.drawPixel(taxiXScreenNew + 3, taxiYScreenNew + 1, matrix.Color333(0, 0, 0));
      matrix.drawPixel(taxiXScreenNew, taxiYScreenNew + 2, matrix.Color333(0, 5, 2));
      matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 2, matrix.Color333(0, 3, 1));
      matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 2, matrix.Color333(0, 3, 1));
      matrix.drawPixel(taxiXScreenNew + 3, taxiYScreenNew + 2, matrix.Color333(0, 5, 2));
    }
    else
    {
      matrix.drawPixel(taxiXScreenNew, taxiYScreenNew, matrix.Color333(0, 0, 0));
      matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew, matrix.Color333(0, 0, 0));
      matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew, matrix.Color333(0, 3, 1));
      matrix.drawPixel(taxiXScreenNew + 3, taxiYScreenNew, matrix.Color333(0, 5, 2));
      matrix.drawPixel(taxiXScreenNew, taxiYScreenNew + 1, matrix.Color333(0, 0, 0));
      switch(targetPlatform)
      {
        case RED:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(3, 0, 0));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(2, 0, 0));
          break;
        case GREEN:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(0, 3, 0));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(0, 2, 0));
          break;
        case BLUE:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(0, 0, 3));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(0, 0, 2));
          break;
        case YELLOW:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(3, 3, 0));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(2, 2, 0));
          break;
        case VIOLET:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(3, 0, 3));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(2, 0, 2));
          break;
        case TURQUOISE:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(0, 3, 3));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(0, 2, 2));
          break;
        default:
          matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 1, matrix.Color333(2, 2, 2));
          matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 1, matrix.Color333(1, 1, 1));
          break;
      }
      matrix.drawPixel(taxiXScreenNew + 3, taxiYScreenNew + 1, matrix.Color333(0, 3, 1));
      matrix.drawPixel(taxiXScreenNew, taxiYScreenNew + 2, matrix.Color333(0, 5, 2));
      matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 2, matrix.Color333(0, 3, 1));
      matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 2, matrix.Color333(0, 3, 1));
      matrix.drawPixel(taxiXScreenNew + 3, taxiYScreenNew + 2, matrix.Color333(0, 5, 2));
    }
  }
}

// directionLeftFree: Check whether movement to the left is possible
boolean directionLeftFree()
{
  if(playfield[taxiXScreenNew + 9][taxiYScreenNew + 8] == 1){ return false; }
  if(playfield[taxiXScreenNew + 8][taxiYScreenNew + 9] == 1){ return false; }
  if(playfield[taxiXScreenNew + 7][taxiYScreenNew + 10] == 1){ return false; }
  return true;
}

// directionRightFree:Check whether movement to the right is possible
boolean directionRightFree()
{
  if(playfield[taxiXScreenNew + 10][taxiYScreenNew + 8] == 1){ return false; }
  if(playfield[taxiXScreenNew + 11][taxiYScreenNew + 9] == 1){ return false; }
  if(playfield[taxiXScreenNew + 12][taxiYScreenNew + 10] == 1){ return false; }
  return true;
}

// directionUpFree: Check whether movement upwards is possible
boolean directionUpFree()
{
  if(taxiDirection == RIGHT)
  {
    if(playfield[taxiXScreenNew + 8][taxiYScreenNew + 7] == 1){ return false; }
    if(playfield[taxiXScreenNew + 9][taxiYScreenNew + 8] == 1){ return false; }
    if(playfield[taxiXScreenNew + 10][taxiYScreenNew + 9] == 1){ return false; }
    if(playfield[taxiXScreenNew + 11][taxiYScreenNew + 9] == 1){ return false; }
  }
  else
  {
    if(playfield[taxiXScreenNew + 8][taxiYScreenNew + 9] == 1){ return false; }
    if(playfield[taxiXScreenNew + 9][taxiYScreenNew + 9] == 1){ return false; }
    if(playfield[taxiXScreenNew + 10][taxiYScreenNew + 8] == 1){ return false; }
    if(playfield[taxiXScreenNew + 11][taxiYScreenNew + 7] == 1){ return false; }
  }
  return true;
}

// directionDownFree: Check whether movement downwards is possible
boolean directionDownFree()
{
  if(playfield[taxiXScreenNew + 8][taxiYScreenNew + 11] == 1){ return false; }
  if(playfield[taxiXScreenNew + 9][taxiYScreenNew + 11] == 1){ return false; }
  if(playfield[taxiXScreenNew + 10][taxiYScreenNew + 11] == 1){ return false; }
  if(playfield[taxiXScreenNew + 11][taxiYScreenNew + 11] == 1){ return false; }
  return true;
}

// drawThrusters: Draw thruster effects
void drawThrusters()
{
  // Remove old thruster effect
  if(taxiDirection == LEFT)
  {
    if(playfield[taxiXScreen + 12][taxiYScreen + 9] == 0)
    {
      matrix.drawPixel(taxiXScreen + 4, taxiYScreen + 1, matrix.Color333(0, 0, 0));                
    }    
    if(playfield[taxiXScreen + 7][taxiYScreen + 10] == 0)
    {
      matrix.drawPixel(taxiXScreen - 1, taxiYScreen + 2, matrix.Color333(0, 0, 0));                
    }
    if(playfield[taxiXScreen + 9][taxiYScreen + 11] == 0 && playfield[taxiXScreen + 10][taxiYScreen + 11] == 0)
    {
      matrix.drawPixel(taxiXScreen + 1, taxiYScreen + 3, matrix.Color333(0, 0, 0));                
      matrix.drawPixel(taxiXScreen + 2, taxiYScreen + 3, matrix.Color333(0, 0, 0));                
    }
    if(playfield[taxiXScreen + 10][taxiYScreen + 7] == 0)
    {
      matrix.drawPixel(taxiXScreen + 2, taxiYScreen - 1, matrix.Color333(0, 0, 0));                
    }
  }
  else
  {
    if(playfield[taxiXScreen + 7][taxiYScreen + 9] == 0)
    {
      matrix.drawPixel(taxiXScreen - 1, taxiYScreen + 1, matrix.Color333(0, 0, 0));                
    }
    if(playfield[taxiXScreen + 12][taxiYScreen + 10] == 0)
    {
      matrix.drawPixel(taxiXScreen + 4, taxiYScreen + 2, matrix.Color333(0, 0, 0)); 
    }                   
    if(playfield[taxiXScreen + 9][taxiYScreen + 11] == 0 && playfield[taxiXScreen + 10][taxiYScreen + 11] == 0)
    {
      matrix.drawPixel(taxiXScreen + 1, taxiYScreen + 3, matrix.Color333(0, 0, 0));                
      matrix.drawPixel(taxiXScreen + 2, taxiYScreen + 3, matrix.Color333(0, 0, 0));                
    }      
    if(playfield[taxiXScreen + 9][taxiYScreen + 7] == 0)
    {
      matrix.drawPixel(taxiXScreen + 1, taxiYScreen - 1, matrix.Color333(0, 0, 0));                
    }
  }

  if(taxiDirectionNew == LEFT)
  {
    if(mainThrusters && playfield[taxiXScreenNew + 12][taxiYScreenNew + 9] == 0 && animationCounter % 4 == 0)
    {
      matrix.drawPixel(taxiXScreenNew + 4, taxiYScreenNew + 1, matrix.Color333(random(3) + 2, random(3) + 2, 2));
    }
    if(breakThrusters && playfield[taxiXScreenNew + 7][taxiYScreenNew + 10] == 0 && animationCounter % 4 == 0)
    {
      matrix.drawPixel(taxiXScreenNew - 1, taxiYScreenNew + 2, matrix.Color333(random(3) + 2, random(3) + 2, 2));
    }
    if(hoverThrusters && playfield[taxiXScreenNew + 9][taxiYScreenNew + 11] == 0 && playfield[taxiXScreenNew + 10][taxiYScreenNew + 11] == 0 && animationCounter % 4 == 0)
    {
      matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 3, matrix.Color333(random(3) + 2, random(3) + 2, 2));        
      matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 3, matrix.Color333(random(3) + 2, random(3) + 2, 2));
    }
    if(downThrusters && playfield[taxiXScreenNew + 10][taxiYScreenNew + 7] == 0 && animationCounter % 4 == 0)
    {
      matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew - 1, matrix.Color333(random(3) + 2, random(3) + 2, 2));
    }
  }
  else
  {
    if(mainThrusters && playfield[taxiXScreenNew + 7][taxiYScreenNew + 9] == 0 && animationCounter % 4 == 0)
    {
      matrix.drawPixel(taxiXScreenNew - 1, taxiYScreenNew + 1, matrix.Color333(random(3) + 2, random(3) + 2, 2));
    }
    if(breakThrusters && playfield[taxiXScreenNew + 12][taxiYScreenNew + 10] == 0 && animationCounter % 4 == 0)
    {
      matrix.drawPixel(taxiXScreenNew + 4, taxiYScreenNew + 2, matrix.Color333(random(3) + 2, random(3) + 2, 2));
    }
    if(hoverThrusters && playfield[taxiXScreenNew + 9][taxiYScreenNew + 11] == 0 && playfield[taxiXScreenNew + 10][taxiYScreenNew + 11] == 0 && animationCounter % 4 == 0)
    {
      matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew + 3, matrix.Color333(random(3) + 2, random(3) + 2, 2));        
      matrix.drawPixel(taxiXScreenNew + 2, taxiYScreenNew + 3, matrix.Color333(random(3) + 2, random(3) + 2, 2));
    }
    if(downThrusters && playfield[taxiXScreenNew + 9][taxiYScreenNew + 7] == 0 && animationCounter % 4 == 0)
    {
      matrix.drawPixel(taxiXScreenNew + 1, taxiYScreenNew - 1, matrix.Color333(random(3) + 2, random(3) + 2, 2));
    }
  } 
}

// moveTaxi: Checks joystick and moves taxi
void moveTaxi()
{
  // Simulate friction
  if(xSpeed < 0.0)
  { 
    xSpeed = xSpeed + 0.01;
    if(xSpeed > 0.0){ xSpeed = 0.0; }
  }
  else if(xSpeed > 0.0)
  { 
    xSpeed = xSpeed - 0.01;
    if(xSpeed < 0.0){ xSpeed = 0.0; }
  }

  // Simulate gravitiy
  if(ySpeed < 1.0){ ySpeed = ySpeed + 0.01; }
  
  // Left
  if(joy1Left() && fuel > 0 && !taxiParks)
  {
    if(taxiDirection == LEFT){ mainThrusters = true; }
    else{ breakThrusters = true; }
    fuel--;
    // SOUND: Thrusters
    tone(audio, 80 + random(20), 10);
    xSpeed = xSpeed - 0.05;
    if(xSpeed < -1.0){ xSpeed = -1.0; }
  }
  // Right
  else if(joy1Right() && fuel > 0 && !taxiParks)
  {
    if(taxiDirection == RIGHT){ mainThrusters = true; }
    else{ breakThrusters = true; }
    fuel--;
    // SOUND: Thrusters
    tone(audio, 80 + random(20), 10);
    xSpeed = xSpeed + 0.05;
    if(xSpeed > 1.0){ xSpeed = 1.0; }    
  }

  // Up
  if((joy1Up() || joy1FireB()) && fuel > 0 && !taxiParks)
  {
    hoverThrusters = true;
    fuel = fuel - 2;
    if(fuel < 0){ fuel = 0; }
    // SOUND: Thrusters
    tone(audio, 50 + random(10), 10);
    ySpeed = ySpeed - 0.025;
    if(ySpeed < -1.0){ ySpeed = -1.0; }
  }
  // Down
  else if(joy1Down() && fuel > 0 && !taxiParks)
  {
    downThrusters = true;
    fuel--;
    // SOUND: Thrusters
    tone(audio, 100 + random(40), 10);
    ySpeed = ySpeed + 0.05;
    if(ySpeed > 1.0){ ySpeed = 1.0; }    
  }

  // Mini Map
  if(joy1FireA())
  {
    drawMiniMap();
  }

  // SOUND: Gas low
  if(fuel < 64 && (animationCounter % 8 == 0))
  {
    tone(audio, 800, 20);
  }
  else if(fuel < 128 && (animationCounter % 16 == 0))
  {
    tone(audio, 800, 20);
  }
  else if(fuel < 256 && (animationCounter % 32 == 0))
  {
    tone(audio, 800, 20);
  }
  
  if(xSpeed < 0.0 && int(taxiXMap) > 0)
  {
    taxiDirectionNew = LEFT;
    xStepCounter = xStepCounter + xSpeed;      
    if(directionLeftFree())
    {
      if(xStepCounter <= -1.0)
      {
        xStepCounter = xStepCounter + 1.0;
        taxiXMapNew = taxiXMap - 1;
        if(screenX > 0 && taxiXScreen < 16)
        {
          screenXNew = screenX - 1;
          redraw = true;
        }
        else
        {
          taxiXScreenNew = taxiXScreen - 1;
        }
      }
    }
    else
    {
      if(xSpeed < -0.6){ heavyCollisionWithObstacle = true; }
      xSpeed = 0.0;
      xStepCounter = 0.0;
    }
  }
  // Moving to the right side
  else if(xSpeed > 0.0 && int(taxiXMap) < (mapWidth - 4))
  {
    taxiDirectionNew = RIGHT;
    xStepCounter = xStepCounter + xSpeed;      
    if(directionRightFree())
    {
      if(xStepCounter >= 1.0)
      {
        xStepCounter = xStepCounter - 1.0;
        taxiXMapNew = taxiXMap + 1;
        if(screenX < (mapWidth - 32) && taxiXScreen > 13)
        {
          screenXNew = screenX + 1;
          redraw = true;
        }
        else
        {
          taxiXScreenNew = taxiXScreen + 1;
        }
      }
    }
    else
    {
      if(xSpeed > 0.6){ heavyCollisionWithObstacle = true; }
      xSpeed = 0.0;
      xStepCounter = 0.0;
    }
  }

  // Moving up
  if(ySpeed < 0.0 && int(taxiYMap) > 0)
  {
    yStepCounter = yStepCounter + ySpeed;          
    if(directionUpFree())
    {
      if(yStepCounter <= -1.0)
      {
        yStepCounter = yStepCounter + 1.0;
        taxiYMapNew = taxiYMap - 1;
        if(screenY > 0 && taxiYScreen < 16)
        {
          screenYNew = screenY - 1;
          redraw = true;
        }
        else
        {
          taxiYScreenNew = taxiYScreen - 1;
        }
      }
    }
    else
    {
      if(ySpeed < -0.6){ heavyCollisionWithObstacle = true; }
      ySpeed = 0.0;
      yStepCounter = 0.0;
    }
  }
  // Moving down
  else if(ySpeed > 0.0 && int(taxiYMap) < (mapHeight - 3))
  {
    yStepCounter = yStepCounter + ySpeed;              
    if(directionDownFree())
    {
      if(yStepCounter >= 1.0)
      {
        yStepCounter = yStepCounter - 1.0;
        taxiYMapNew = taxiYMap + 1;
        if(screenY < (mapHeight - 32) && taxiYScreen > 13)
        {
          screenYNew = screenY + 1;
          redraw = true;
        }
        else
        {
          taxiYScreenNew = taxiYScreen + 1;
        }
      }
    }
    else
    {
      if(ySpeed > 0.6){ heavyCollisionWithObstacle = true; }
      ySpeed = 0.0;
      yStepCounter = 0.0;
    }
  }

  // Draw Thrusters
  drawThrusters();
  mainThrusters = false;        
  breakThrusters = false;        
  hoverThrusters = false;        
  downThrusters = false;        

  // Redraw taxi if position has changed or animation is incomplete   
  if(fuel < 64){ drawTaxi(true, 8); }
  else if(fuel < 128){ drawTaxi(true, 16); }
  else if(fuel < 256){ drawTaxi(true, 32); }
  else{ drawTaxi(false, 0); }
  taxiXScreen = taxiXScreenNew;
  taxiYScreen = taxiYScreenNew;
  taxiXMap = taxiXMapNew;
  taxiYMap = taxiYMapNew;
  taxiDirection = taxiDirectionNew;
}

// collisionDetection: Checks whether new taxi position collides with something (except walls)
byte collisionDetection()
{
  if(taxiDirection == RIGHT)
  {
    if(playfield[taxiXScreenNew + 8][taxiYScreenNew + 8] > 1){ return playfield[taxiXScreenNew + 8][taxiYScreenNew + 8]; }
    if(playfield[taxiXScreenNew + 8][taxiYScreenNew + 9] > 1){ return playfield[taxiXScreenNew + 8][taxiYScreenNew + 9]; }
    if(playfield[taxiXScreenNew + 9][taxiYScreenNew + 9] > 1){ return playfield[taxiXScreenNew + 9][taxiYScreenNew + 9]; }
    if(playfield[taxiXScreenNew + 10][taxiYScreenNew + 9] > 1){ return playfield[taxiXScreenNew + 10][taxiYScreenNew + 9]; }
    if(playfield[taxiXScreenNew + 8][taxiYScreenNew + 10] > 1){ return playfield[taxiXScreenNew + 8][taxiYScreenNew + 10]; }
    if(playfield[taxiXScreenNew + 9][taxiYScreenNew + 10] > 1){ return playfield[taxiXScreenNew + 9][taxiYScreenNew + 10]; }
    if(playfield[taxiXScreenNew + 10][taxiYScreenNew + 10] > 1){ return playfield[taxiXScreenNew + 10][taxiYScreenNew + 10]; }
    if(playfield[taxiXScreenNew + 11][taxiYScreenNew + 10] > 1){ return playfield[taxiXScreenNew + 11][taxiYScreenNew + 10]; }  
  }
  else
  {
    if(playfield[taxiXScreenNew + 11][taxiYScreenNew + 8] > 1){ return playfield[taxiXScreenNew + 11][taxiYScreenNew + 8]; }
    if(playfield[taxiXScreenNew + 9][taxiYScreenNew + 9] > 1){ return playfield[taxiXScreenNew + 9][taxiYScreenNew + 9]; }
    if(playfield[taxiXScreenNew + 10][taxiYScreenNew + 9] > 1){ return playfield[taxiXScreenNew + 10][taxiYScreenNew + 9]; }
    if(playfield[taxiXScreenNew + 11][taxiYScreenNew + 9] > 1){ return playfield[taxiXScreenNew + 11][taxiYScreenNew + 9]; }
    if(playfield[taxiXScreenNew + 8][taxiYScreenNew + 10] > 1){ return playfield[taxiXScreenNew + 8][taxiYScreenNew + 10]; }
    if(playfield[taxiXScreenNew + 9][taxiYScreenNew + 10] > 1){ return playfield[taxiXScreenNew + 9][taxiYScreenNew + 10]; }
    if(playfield[taxiXScreenNew + 10][taxiYScreenNew + 10] > 1){ return playfield[taxiXScreenNew + 10][taxiYScreenNew + 10]; }
    if(playfield[taxiXScreenNew + 11][taxiYScreenNew + 10] > 1){ return playfield[taxiXScreenNew + 11][taxiYScreenNew + 10]; }  
  }
  return 0;  
}

// checkForLosingLive: Check if collision with danger is true
boolean checkForLosingLive()
{
  // Heavy collision with obstacle
  if(heavyCollisionWithObstacle)
  {
    lives--;
    taxiExplosion = true;
    return true;
  }
  
  // Collision with danger
  if(collisionDetection() == 2)
  {
    lives--;
    taxiExplosion = true;
    return true;
  }

  // Running out of gas
  if(fuel == 0 && xSpeed == 0.0 && ySpeed == 0.0)
  {
    lives--;
    return true;
  }
  return false;  
}

// checkForFuelPlatformsExtraLives: Check if collision is true
void checkForFuelPlatformsExtraLives()
{
  i = collisionDetection();
  refuling = false;
  if(xSpeed == 0.0 && ySpeed == 0.0)
  {
    if(i == 3)
    {
      if(fuel < fuelMax)
      {
        refuling = true;
        fuel = fuel + 5;
        // SOUND: Refuel
        if(animationCounter % 32 == 0){ tone(audio, 400, 100); }
        if(fuel > fuelMax - 5){ tone(audio, 400, 200); }
        if(fuel > fuelMax){ fuel = fuelMax; }
      }
    }
    if(i >= 4 && i <= 9)
    {
      // Disembarking / Deliver passenger
      if(targetPlatform == (i - 3) && !taxiParks)
      {
        for(byte j = 0; j < platformCounter; j++)
        {
          if(platformColor[j] == i - 3)
          {
            taxiParks = true;
            footstepSound = true;              
            animationCounterSyncValue = animationCounter % 32;
            platformDisembarkingStatus[j] = 1;
          }
        }
      }
      // Bording / Pickup passenger      
      else if(targetPlatform == 0)
      {
        for(byte j = 0; j < platformCounter; j++)
        {
          if((platformColor[j] == i - 3) && platformBordingStatus[j] == 2)
          {
            taxiParks = true;
            footstepSound = true;              
            animationCounterSyncValue = animationCounter % 32;
            platformBordingStatus[j] = 3;
          }
        }
      }
    }
  }
  if(i > 9)
  {
    // Extralife collected
    if(extraLifeStatus[i - 10] == 1)
    {
      if(lives < 9)
      {
        lives++;
      }
      // Sound: Extra life collected
      tone(audio, 220, 400);
    }
    extraLifeStatus[i - 10] = 2;
    // Blocks all movement sounds
    audioOffUntil = animationCounter + 4;
    if(audioOffUntil == 0)
    {
      audioOffUntil = 1;
    }
  }
}

// showStatus: Show status screen
void showStatus()
{

  // If a new game is started, it is possible to jump to a higher level which has already been reached in a previous game.
  boolean levelSelection = false;
  if(level == 1 && maximalReachedLevel > 1 && showInitialStatus)
  {
    levelSelection = true;
  }
  showInitialStatus = false;

  // Frame
  matrix.setTextColor(matrix.Color333(4,2,0));
  // Game ends because of no more lives or last level completed (in this case a full screen frame is drawn)
  if(lives < 1 || level == numberOfLevels + 1)
  {
    matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
    matrix.drawRect(0, 0, 32, 32, matrix.Color333(1,1,1));
  }
  // Status
  else
  {
    if(!levelSelection)
    {
      matrix.fillRect(1, 6, 29, 19, matrix.Color333(0,0,0));
      matrix.drawRect(1, 6, 29, 19, matrix.Color333(3,2,1));
    }
    // Start new game and level selection enabled
    else
    {
      matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
      matrix.drawLine(0, 0, 0, 32, matrix.Color333(1,1,1));
      matrix.drawLine(31, 0, 31, 32, matrix.Color333(1,1,1));
      matrix.setCursor(4, 0);
      matrix.print("Jump");                    
      matrix.setCursor(4, 24);
      matrix.print("<  >");                    
    }
  }
  // Lives
  matrix.drawPixel(3, 10, matrix.Color333(0, 5, 2));
  matrix.drawPixel(4, 10, matrix.Color333(0, 3, 1));
  matrix.drawPixel(5, 10, matrix.Color333(0, 0, 0));
  matrix.drawPixel(6, 10, matrix.Color333(0, 0, 0));
  matrix.drawPixel(3, 11, matrix.Color333(0, 3, 1));
  matrix.drawPixel(4,11, matrix.Color333(0, 0, 2));
  matrix.drawPixel(5, 11, matrix.Color333(0, 0, 3));
  matrix.drawPixel(6, 111, matrix.Color333(0, 0, 0));
  matrix.drawPixel(3, 12, matrix.Color333(0, 5, 2));
  matrix.drawPixel(4, 12, matrix.Color333(0, 3, 1));
  matrix.drawPixel(5, 12, matrix.Color333(0, 3, 1));
  matrix.drawPixel(6, 12, matrix.Color333(0, 5, 2));
  matrix.setCursor(8, 8);
  matrix.print(lives);
  // Passengers
  matrix.drawLine(4, 17, 6, 17, matrix.Color333(3, 3, 3));
  matrix.drawLine(4, 18, 4, 21, matrix.Color333(3, 3, 3));
  matrix.drawLine(5, 19, 6, 19, matrix.Color333(3, 3, 3));
  matrix.drawPixel(6, 18, matrix.Color333(3, 3, 3));
  matrix.setCursor(8, 16);
  matrix.print(passengers);
  
  changed = true;
  do
  {
    if(changed)
    {      
      // Level
      matrix.drawLine(14, 9, 14, 13, matrix.Color333(3, 3, 3));
      matrix.drawLine(14, 13, 16, 13, matrix.Color333(3, 3, 3));
      matrix.fillRect(17, 8, 12, 7, matrix.Color333(0,0,0));
      if(level < 10){ matrix.setCursor(18, 8); }
      else{ matrix.setCursor(17, 8); }
      if(level <= numberOfLevels)
      {
        matrix.print(level);
      }
      else
      {
        matrix.print("-");
      }
      changed = false;
    }
    
    if((joy1Left() && levelSelection && level > 1 && lives > 0) || (hiddenLevelJumps && joy1Fire() && joy1Left() && level > 1 && lives > 0))
    {
      level--;
      initializeNewLevel = true;
      changed = true;
      tone(audio,1024,20);
      delay(200);  
    }
    else if((joy1Right() && levelSelection && level < maximalReachedLevel && lives > 0) || (hiddenLevelJumps && joy1Fire() && joy1Right() && level < numberOfLevels && lives > 0))
    {
      level++;
      initializeNewLevel = true;
      changed = true;
      tone(audio,1024,20);
      delay(200);  
    }
  }
  while(!(joy1Fire() && !joy1Left() && !joy1Right())); 
  tone(audio,1024,20);
  delay(200);  
}

// taxiExplodes: Taxi explodes
void taxiExplodes()
{
  for(byte i = 0; i < 8; i++)
  {
    for(byte j = 0; j < i; j++)
    {
      matrix.drawCircle(taxiXScreen + 1, taxiYScreen + 1, j, matrix.Color333(random(3) + 3, random(3) + 3, 7 - i));
      delay(8);
      // SOUND: Taxi explodes
      tone(audio, 120 - i * j + random(40), 7);
      matrix.drawCircle(taxiXScreen + 2, taxiYScreen + 1, j, matrix.Color333(random(3) + 3, random(3) + 3, 7 - i));
      delay(8);
      // SOUND: Taxi explodes
      tone(audio, 120 - i * j + random(20), 7);
    }
  }
  for(byte i = 0; i < 8; i++)
  {
     matrix.drawCircle(taxiXScreen + 1, taxiYScreen + 1, i, matrix.Color333(0, 0, 0));
     delay(8);
     matrix.drawCircle(taxiXScreen + 2, taxiYScreen + 1, i, matrix.Color333(0, 0, 0));
     delay(8);
  }
}

// endSequence: End sequence after completion of all levels
void endSequence()
{
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
  matrix.setTextColor(matrix.Color333(3,0,0));
  matrix.setCursor(2, 0);
  matrix.println("Done!");
  matrix.setTextColor(matrix.Color333(0,3,0));
  matrix.setCursor(2, 8);
  matrix.println("Done!");
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.setCursor(2, 16);
  matrix.println("Done!");
  matrix.setTextColor(matrix.Color333(3,3,3));
  matrix.setCursor(2, 24);
  matrix.println("Done!");
  for(i = 0; i < 6; i++)
  {
    tone(audio, NOTE_C5, 50);
    delay(50);
    tone(audio, NOTE_E5, 50);
    delay(50);
    tone(audio, NOTE_G5, 50);
    delay(50);        
  }
  tone(audio,1047,400);
  delay(4000);
  
  // End screen
  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
  matrix.setTextColor(matrix.Color333(3,0,0));
  matrix.setCursor(4, 0);
  matrix.println("Best");
  matrix.setTextColor(matrix.Color333(0,3,0));
  matrix.setCursor(1, 8);
  matrix.println("Ceres");
  matrix.setTextColor(matrix.Color333(0,0,3));
  matrix.setCursor(1, 16);
  matrix.println(" Cab ");
  matrix.setTextColor(matrix.Color333(3,3,3));
  matrix.setCursor(1, 24);
  matrix.println("Pilot");
    
  delay(500);
  
  do
  {
  }
  while(!joy1Fire());  
  delay(200);

  showStatus();
  initializeNewLevel = true; // Important for a new start of the game (to have a correct setup of extra lives and passengers)
}

// loop: Main loop
void loop()
{
  mithotronic();
  ledMePlay();
  
  do
  { 
    title();
    setupGame();
    do
    {
      showStatus();
      setupLevel();
      drawMiniMap();
      
      reset = false; // Set reset indicator to false
      matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));

      drawTaxi(false, 0);
      
      do
      {
        // Get time of engine loop start point (important for a constant game speed)
        engineLoopStartPoint = millis();
        
        // Release audio after collection sound (this blocks all movement sounds)
        if(audioOffUntil == animationCounter)
        {
          audioOffUntil = 0;
        }

        // Draw screen
        drawScreen();
        
        // Move taxi (draws also the taxi)
        if(animationCounter % 2 == 0)
        {
          moveTaxi();
        }

        // Draw platforms
        drawPlatforms();
        
        // Draw gas stations
        drawGasStations();

        // Draw extra lives
        drawExtraLives();
        
        // Move dangers
        moveDangers();
        
        // Check for goods
        checkForFuelPlatformsExtraLives();
        
        animationCounter++;
    
        // Pause
        if(digitalRead(buttonPause) == LOW)
        {          
          tone(audio,1024,20);
          delay(500);
          do
          {
          }
          while((!digitalRead(buttonPause) == LOW) && (!joy1Fire()));
          tone(audio,1024,20);
          delay(500);
        }

        // Reset
        if(digitalRead(buttonReset) == LOW)
        {
          reset = true;
          lives = 0;
          break;
        }    

        // Loop until 20 milliseconds has passed since begin of engine loop
        do
          {
          }
        while(abs(millis() - engineLoopStartPoint) < 20);

      }
      while (!checkForLosingLive() && remainingPassengersToFinishLevel > 0);

      // Live lost
      if(remainingPassengersToFinishLevel > 0)
      {
        if(taxiExplosion){ taxiExplodes(); }
        delay(1000);
        initializeNewLevel = false;
      }
      // Level complete
      else
      {
        matrix.setTextColor(matrix.Color333(0,3,0));
        matrix.setCursor(2, 8);
        matrix.println("Level");
        matrix.setCursor(2, 16);
        matrix.println("done!");
        for(i = 0; i < 3; i++)
        {
          tone(audio, NOTE_C5, 100);
          delay(100);
          tone(audio, NOTE_E5, 100);
          delay(100);
          tone(audio, NOTE_G5, 100);
          delay(100);        
        }
        tone(audio,1047,400);
        delay(1000);
        level++;
        if(level > maximalReachedLevel){ maximalReachedLevel = level; } // Update maximalReachedLevel
        initializeNewLevel = true;
        matrix.fillRect(0, 0, 32, 32, matrix.Color333(0,0,0));
      }
    }
    while(!reset && level < (numberOfLevels + 1) && lives > 0);

    // All lives lost (Game Over Sequence)
    if(lives < 1)
    {
      matrix.setTextColor(matrix.Color333(4,2,0));
      matrix.setCursor(4, 8);
      matrix.println("Game");
      matrix.setCursor(4, 16);
      matrix.println("Over");
      tone(audio, NOTE_C5, 200);
      delay(200);
      tone(audio, NOTE_G4, 200);
      delay(200);
      tone(audio, NOTE_E4, 200);
      delay(200);
      tone(audio, NOTE_C4, 400);
      delay(4000);
      showStatus();
    }
    
    // Game completed (End sequence)
    if(level == (numberOfLevels + 1))
    {
      endSequence();
    }
  }
  while(true);
    
} // loop
