#include "phSens.h"
#include "string.h"

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////
void GetPH(int voltage, char *pHBuff, char *pHPacket);
void GetTemp(int voltage, char *tempBuff, char *tempPacket);
void GetLevel(int level, char *levelBuff, char *levelPacket);
void GetPump(int pump, char *pumpBuff, char *pumpPacket);

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////
// takes in a range of 0 to 1023 from an analog pin on the Arduino then 
// converts to a float b/w 0 - 14 and creates a string. returns an integer
// of the pH multiplied by 100
void GetPH(int voltage, char *pHBuff, char *pHPacket)
{
    float fPH;
    int iPH;

    fPH = ((float)voltage / 1023.0) * 14.0;
    iPH = (int)(fPH * 100.0);
    (void)sprintf(pHBuff, "pH: %d.%d", iPH / 100, iPH % 100);
    (void)sprintf(pHPacket, "1 %d", iPH);
}

// takes in a range of 0 to 1023 from an analog pin on the arduino then 
// converts to a float b/w 15 - 35 and creates a string. returns an int
// of the temp multiplied by 100
void GetTemp(int voltage, char *tempBuff, char *tempPacket)
{
    float fTemp;
    int iTemp;
    float vDiff;

    vDiff = (((float)voltage / 1023.0 * 4.99 - 1.73962242) / 6171.52) * 1000.0;

    fTemp = 22 +
            25.08355 * vDiff +
            0.07860106 * (vDiff * vDiff) -
            0.2503131 * (vDiff * vDiff * vDiff)+
            0.08315270 * (vDiff * vDiff * vDiff * vDiff) -
            0.01228034 * (vDiff * vDiff * vDiff * vDiff * vDiff) +
            0.0009804036 * (vDiff * vDiff * vDiff * vDiff * vDiff * vDiff) -
            0.0000441303 * (vDiff * vDiff * vDiff * vDiff * vDiff * vDiff * vDiff) +
            0.000001057734 * (vDiff * vDiff * vDiff * vDiff * vDiff * vDiff * vDiff * vDiff) -
            0.00000001052755 * (vDiff * vDiff * vDiff * vDiff * vDiff * vDiff * vDiff * vDiff * vDiff);

    iTemp = (int)(fTemp * 100.0);
    (void)sprintf(tempBuff, "Temp: %d.%dC", iTemp / 100, iTemp % 100);
    (void)sprintf(tempPacket, "2 %d", iTemp);
}

// takes in a digital read 0 or 1, then creates a packet to send over
void GetLevel(int level, char *levelBuff, char *levelPacket)
{
    if (level)
    {
        (void)sprintf(levelBuff, "Level: Good");
    }
    else
    {
        (void)sprintf(levelBuff, "Level: Low");
    }

    (void)sprintf(levelPacket, "3 %d", level);
}

// read the pin running the pump and create a string for the LCD
void GetPump(int pump, char *pumpBuff, char *pumpPacket)
{
    if (pump)
    {
        (void)sprintf(pumpBuff, "Pump: Running");
    }
    else
    {
        (void)sprintf(pumpBuff, "Pump: Off");
    }

    (void)sprintf(pumpPacket, "4 %d", pump);
}