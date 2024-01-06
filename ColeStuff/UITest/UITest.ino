// Libraries
#include <Arduino.h>
#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"
#include "string.h"
#include <SoftwareSerial.h>

extern "C"{
#include "switch.h"
}

// Pins
#define phPin A0
#define tempPin A1

// states for the LCD
typedef enum {
  Stats,
  Internet,
  Pump,
  Num
} State;

State state;
SwState D3, D2;

// Set Up
void setup() {
  // Setup for the LCD
  Config_Init();
  LCD_Init();
  LCD_Clear(0xffff);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, WHITE);
  Paint_Clear(WHITE);

  // Serial set up
  Serial.begin(115200);

  // initialize the state to stats by default
  state = Stats;
  SwState D3, D2 = Idle;
  pinMode(3, INPUT);
  pinMode(2, INPUT);
}

// Main Loop
void loop() {
  // clear the screen
  switch(state)
  {
    case Stats:
      Paint_DrawRectangle(2 , 2, LCD_WIDTH -2, LCD_HEIGHT - 2, BLACK, 1, 0);
      Paint_DrawString_EN(30, 10, "pH: 7", &Font24, WHITE, 0x8813);
      Paint_DrawString_EN(30, 40, "Temp: 24.21C", &Font24, WHITE, 0x8813);
      Paint_DrawString_EN(30, 70, "Level: Good", &Font24, WHITE, 0x8813);
      Paint_DrawString_EN(30, 100, "Pump: Runnning", &Font24, WHITE, 0x8813);
      break;
    case Internet:
      Paint_DrawRectangle(2 , 2, LCD_WIDTH -2, LCD_HEIGHT - 2, BLACK, 1, 0);
      Paint_DrawString_EN(10, 10, "Network:", &Font24, WHITE, 0x8813);
      Paint_DrawString_EN(10, 40, "FishFarmNetwork", &Font24, WHITE, 0x8813);
      Paint_DrawString_EN(10, 100, "Password:", &Font24, WHITE, 0x8813);
      Paint_DrawString_EN(10, 130, "123456789", &Font24, WHITE, 0x8813);
      Paint_DrawString_EN(10, 190, "Go to 192.168.1.1", &Font24, WHITE, 0x8813);
      break;
    case Pump:
      Paint_DrawRectangle(2 , 2, LCD_WIDTH -2, LCD_HEIGHT - 2, BLACK, 1, 0);
      Paint_DrawString_EN(10, 10, "Pump State:", &Font24, WHITE, 0x8813);
      Paint_DrawString_EN(10, 40, "Running", &Font24, WHITE, 0x8813);
      Paint_DrawString_EN(10, 100, "Press Green button", &Font24, WHITE, 0x8813);
      Paint_DrawString_EN(10, 130, "to toggle", &Font24, WHITE, 0x8813);
      break;
  }

  Sw_Process(&D3, 3);
  Sw_Process(&D2, 2);

  if(D3 == Pressed){
      state = (state + 1) % Num;
      Paint_Clear(WHITE);
  }

  if(D2 == Pressed){
     state = (state - 1 + Num) % Num;
     Paint_Clear(WHITE);
  }
}
