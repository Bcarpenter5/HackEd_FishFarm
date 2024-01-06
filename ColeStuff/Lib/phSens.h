/////////////////////////////////////////////////////////////////////////////
// Libaray for pH sensor to work with Arduino Nano
// Author: Cole Yaremko (CYaremko)
// Date: January 06, 2024
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Function Prototypes
/////////////////////////////////////////////////////////////////////////////
// Converts analog voltage to a pH value to be output later also creates a 
// string for the LCD screen
void GetPH(int voltage, char *pHBuff, char *pHPacket);

// converts analog voltage to a temp value from 15 - 35 C 
void GetTemp(int voltage, char *tempBuff, char *tempPacket);

// Gets the level then creates a packet to send over
void GetLevel(int level, char *levelBuff, char *levelPacket);