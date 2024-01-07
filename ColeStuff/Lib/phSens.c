#include "phSens.h"
#include "string.h"

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////
void GetPH(int voltage, char *pHBuff, char *pHPacket);
void GetTemp(int voltage, char *tempBuff, char *tempPacket);
void GetLevel(int level, char *levelBuff, char *levelPacket);

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
    (void)sprintf(pHPacket, "1 %d\n", iPH);
}

// takes in a range of 0 to 1023 from an analog pin on the arduino then 
// converts to a float b/w 15 - 35 and creates a string. returns an int
// of the temp multiplied by 100
void GetTemp(int voltage, char *tempBuff, char *tempPacket)
{
    float fTemp;
    int iTemp;
    float gain;

    // gain to figure out the temperature
    gain = (35.0 - 15.0) / 1023.0;

    fTemp = ((float)voltage / 1023.0) * gain + 15.0;
    iTemp = (int)(fTemp * 100.0);
    (void)sprintf(tempBuff, "Temp: %d.%dC", iTemp / 100, iTemp % 100);
    (void)sprintf(tempPacket, "2 %d\n", iTemp);
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

    (void)sprintf(levelPacket, "3 %d\n", level);
}