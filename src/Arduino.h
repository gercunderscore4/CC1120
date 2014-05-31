/*
 * FILE:    Arduino.h // not really
 * PROGRAM: CC1120
 * PURPOSE: pretend arduino functions so that my code will compile
 * AUTHOR:  Geoffrey Card
 * DATE:    2014-06-01 - 
 * NOTES:   I should find the real Arduino.h
 */

#ifndef ARDUINO_H_
#define ARDUINO_H_

#include <stdint.h>

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP  0x2

#define LOW 0x0
#define HIGH 0x1

void pinMode (uint8_t pin, uint8_t mode);
void digitalWrite (uint8_t pin, uint8_t val);
int digitalRead (uint8_t pin);

#endif // ARDUINO_H_
