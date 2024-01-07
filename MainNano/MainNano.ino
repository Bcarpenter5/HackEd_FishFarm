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
void PrintStats(void);
void PrintNet(void);
void PrintPump(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////////////////////////
State state;         // Will store the state of the LCD screen
SwState D2, D3, D4;  // Switch states for the buttons controlling the LCD
SoftwareSerial Serial1(0, 1);

// char arrays for LCD Strings
unsigned char pHBuff[40];
unsigned char tempBuff[40] = "a"; // these needed to be initialized???
unsigned char levelBuff[40] = "a";
unsigned char pumpBuff[40] = "a";

// char arrays for the packets to be sent to the ESP
unsigned char pHPacket[7];
unsigned char tempPacket[7];
unsigned char levelPacket[4];
unsigned char pumpPacket[4];

String ph = "";
String temp = "";
String level = "";
String pump = "";

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
  D2, D3, D4 = Idle;  // start the buttons in Idle

  // Set the pins for the buttons (and level sensor) to input
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);

  // set the pin for the pump to output
  pinMode(6, OUTPUT);

  Serial1.begin(9600);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// main program loop
///////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // Switch process
  Sw_Process(&D2, 2);
  Sw_Process(&D3, 3);
  Sw_Process(&D4, 4);

  // Read from the Sensors
  GetPH(analogRead(phPin), pHBuff, pHPacket);
  GetTemp(511, tempBuff, tempPacket);
  GetLevel(digitalRead(5), levelBuff, levelPacket);
  GetPump(digitalRead(6), pumpBuff, pumpPacket);

  ph = pHPacket;
  temp = tempPacket;
  level = levelPacket;
  pump = pumpPacket;

  Serial1.println(ph);
  Serial1.println(temp);
  Serial1.println(level);
  Serial1.println(pump);

  ph = "";
  temp = "";
  level = "";
  pump = "";

  switch(state)
  {
    case Stats:
      PrintStats();
      break;
    case Internet:
      PrintNet();
      break;
    case Pump:
      PrintPump();

      if(D4 == Pressed){
        digitalTOG(6);
      }

      break;
  }

  if(D3 == Pressed){
      state = (state + 1) % Num;
      LCD_Clear(0xffff);
  }

  if(D2 == Pressed){
     state = (state - 1 + Num) % Num;
     LCD_Clear(0xffff);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////////////////////////////////
void PrintStats(void)
{
  Paint_DrawRectangle(2 , 2, LCD_WIDTH -2, LCD_HEIGHT - 2, BLACK, 1, 0);
  Paint_DrawRectangle(100, 10, LCD_WIDTH, 40, WHITE, 1, 1);
  Paint_DrawString_EN(30, 10, pHBuff, &Font24, WHITE, 0x8813);
  Paint_DrawRectangle(125, 40, LCD_WIDTH, 70, WHITE, 1, 1);
  Paint_DrawString_EN(30, 40, tempBuff, &Font24, WHITE, 0x8813);
  Paint_DrawRectangle(150, 70, LCD_WIDTH, 100, WHITE, 1, 1);
  Paint_DrawString_EN(30, 70, levelBuff, &Font24, WHITE, 0x8813);
  Paint_DrawRectangle(135, 100, LCD_WIDTH, 130, WHITE, 1, 1);
  Paint_DrawString_EN(30, 100, pumpBuff, &Font24, WHITE, 0x8813);
}

void PrintNet(void)
{
  Paint_DrawRectangle(2 , 2, LCD_WIDTH -2, LCD_HEIGHT - 2, BLACK, 1, 0);
  Paint_DrawString_EN(10, 10, "Network:", &Font24, WHITE, 0x8813);
  Paint_DrawString_EN(10, 40, "FishFarmNetwork", &Font24, WHITE, 0x8813);
  Paint_DrawString_EN(10, 100, "Password:", &Font24, WHITE, 0x8813);
  Paint_DrawString_EN(10, 130, "123456789", &Font24, WHITE, 0x8813);
  Paint_DrawString_EN(10, 190, "Go to 192.168.1.1", &Font24, WHITE, 0x8813);
}

void PrintPump(void)
{
  Paint_DrawRectangle(2 , 2, LCD_WIDTH -2, LCD_HEIGHT - 2, BLACK, 1, 0);
  Paint_DrawString_EN(10, 10, "Pump State:", &Font24, WHITE, 0x8813);
  Paint_DrawRectangle(10, 40, LCD_WIDTH, 70, WHITE, 1, 1);

  if (digitalRead(6))
  {
    Paint_DrawString_EN(10, 40, "Running", &Font24, WHITE, 0x8813);
  }
  else
  {
    Paint_DrawString_EN(10, 40, "Off", &Font24, WHITE, 0x8813);
  }
  
  Paint_DrawString_EN(10, 100, "Press Green button", &Font24, WHITE, 0x8813);
  Paint_DrawString_EN(10, 130, "to toggle", &Font24, WHITE, 0x8813);
}