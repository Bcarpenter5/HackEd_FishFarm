///////////////////////////////////////////////////////////////////////////////////////////////////
///  Project: MainNano
///  Created: January 06, 2024
///  Authors: Brandon Carpenter
///           Andreas Delfin
///           Cyler LaRocque
///           Liam Moore
///           Cole Yaremko
/// 
///  Main File that will run on the Arduino Nano. Reads and interprets data from the sensors. 
///  Creates Strings to be displayed on the LCD screen. Creates packets to send to the ESP 32.
///  
///  Modification History: See Git
///  Program status: In Progress
///////////////////////////////////////////////////////////////////////////////////////////////////
// Pin Defines
#define phPin A0
#define tempPin A1

// Main Includes
#include <Arduino.h>
#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"
#include "string.h"
#include <SoftwareSerial.h>

// C libraries
extern "C"{
#include "switch.h"
#include "phSens.h"
}

// states for the LCD
typedef enum {
  Stats,
  Internet,
  Pump,
  Num
} State;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Local Prototypes
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////////////////////////
State state;         // Will store the state of the LCD screen
SwState D2, D3, D4;  // Switch states for the buttons controlling the LCD

// char arrays for LCD Strings
unsigned char pHBuff[40];
unsigned char tempBuff[40] = "a"; // these two needed to be initialized???
unsigned char levelBuff[40] = "a";

// char arrays for the packets to be sent to the ESP
unsigned char pHPacket[7];
unsigned char tempPacket[7];
unsigned char levelPacket[4];

///////////////////////////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// one-time initializations
///////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Setup for the LCD
  Config_Init();
  LCD_Init();
  LCD_Clear(0xffff);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, WHITE);
  Paint_Clear(WHITE);

  state = Stats; // start the program on the stats screen
  SwState D2, D3, D4 = Idle;  // start the buttons in Idle

  // Set the pins for the buttons to input
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  // Serial set up
  Serial.begin(115200);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// main program loop
///////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Read from the Sensors
  GetPH(511, pHBuff, pHPacket);
  GetTemp(511, tempBuff, tempPacket);
  GetLevel(1, levelBuff, levelPacket);

  Paint_DrawRectangle(2 , 2, LCD_WIDTH -2, LCD_HEIGHT - 2, BLACK, 1, 0);
  Paint_DrawString_EN(30, 10, pHBuff, &Font24, WHITE, 0x8813);
  Paint_DrawString_EN(30, 40, tempBuff, &Font24, WHITE, 0x8813);
  Paint_DrawString_EN(30, 70, levelBuff, &Font24, WHITE, 0x8813);
  Paint_DrawString_EN(30, 100, "Pump: Runnning", &Font24, WHITE, 0x8813);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////////////////////////////////