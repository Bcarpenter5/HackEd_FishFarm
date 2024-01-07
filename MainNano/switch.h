//////////////////////////////////////////////////////////////////////////////////////
// Project: switch library
//       
//       
// Author: Brandon Carpenter 
//       
//       
//       
// updates: 
//       created: 11/26/2023
//       
//       
//////////////////////////////////////////////////////////////////////////////////////

#ifndef switch_h
#define switch_h


#include "Arduino.h"
#include "wiring_private.h"
#include "pins_arduino.h"




typedef enum SwStateTypedef__
{
  Idle,
  Pressed,
  Held,
  Released,
}SwState;



//*************************************************************************
//  @Method: void digitalToG(uint8_t pin)
//  @Purpose: toggles the pin on or off
//  @Args: uint8_t pin -- the pin to be turned on or off
//  @returns: void
//*************************************************************************
void digitalTOG(uint8_t pin);


//*************************************************************************
//  @Method: SwState Sw_Process(SwState*, uint8_t pin); 
//  @Purpose: Processes the state of a gpio pin
//  @Args: SwState* -- pointer to the state of the switch
//       @ uint8_t pin -- the pin to be processed
//  @returns: SwState -- the state of the switch
//*************************************************************************
SwState Sw_Process(SwState* state, uint8_t pin);



#endif
