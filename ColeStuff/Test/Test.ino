// Libraries
#include <Arduino.h>
#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"
#include "string.h"
#include <SoftwareSerial.h>

/* // C libraries
extern "C" 
{
  #include "phSens.h"
} */

// Pins
#define phPin A0
#define tempPin A1

SoftwareSerial Serial1(0, 1);

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
  Serial1.begin(9600);
}

float pH;
int pHSens;
unsigned char pHBuff[40];
unsigned char analogBuff[40];
int morePH;

// Main Loop
void loop() {
  // clear the screen
  Paint_Clear(WHITE);

  // Get the sensor data and convert to a string
  pHSens = analogRead(phPin);
  pH = ((float)pHSens / 1023.0) * 14.0;

  morePH = (int)(pH * 100.0);

  (void)sprintf(pHBuff, "pH: %d.%d", morePH / 100, morePH % 100);
  (void)sprintf(analogBuff, "Steps: %d", pHSens);

  Serial.println(pH);

  //                  x    y   string   font  highlight  color
  Paint_DrawString_EN(30, 10, pHBuff, &Font24, YELLOW, RED);
  Paint_DrawString_EN(30, 70, analogBuff, &Font24, YELLOW, RED);

  delay(1000);
}
