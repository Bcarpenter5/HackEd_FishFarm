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
#include "switch.h"
#include "Arduino.h"
#include "wiring_private.h"
#include "pins_arduino.h"






//*************************************************************************
//  @Method: void digitalToG(uint8_t pin)
//  @Purpose: toggles the pin on or off
//  @Args: uint8_t pin -- the pin to be turned on or off
//  @returns: void
//*************************************************************************
void digitalTOG(uint8_t pin)
{
	uint8_t timer = digitalPinToTimer(pin);
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *out;
	if (port == NOT_A_PIN) return;
	out = portOutputRegister(port);
	uint8_t oldSREG = SREG;
	cli();
		*out ^= bit;
	SREG = oldSREG;
}


SwState Sw_Process(SwState* state, uint8_t pin){
    if(digitalRead(pin) == HIGH){
        if (*state == Idle) {
            *state = Pressed;
        }
        else {
            *state = Held;
        }
    }
    else{
        if (*state == Held) {
            *state = Released;
        }
        else {
            *state = Idle;
        }
    }
    return *state;
}



