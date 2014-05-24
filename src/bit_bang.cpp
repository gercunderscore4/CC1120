/*
 * FILE:    bit_bang.cpp
 * PROGRAM: CC1120
 * PURPOSE: test CC1120 read and write capabilities
 * AUTHOR:  Geoffrey Card
 * DATE:    2014-05-22 - 
 * NOTES:   Not optimized
 *          Can't use standard Arduino SPI because this transmits 2 bytes per enable
 */

#include <Arduino.h>
#include "bit_bang.h"

/*
 * set up CC1120 SPI
 * PARAM:
 *     none, pins are defined constants
 * RETURN:
 *     void, there's nothing to return, if it fails, nothing will work anyway
 */
void CC1120_setup (void)
{
	// set pins
	pinMode(SCLK, OUTPUT);
	pinMode(CSn,  OUTPUT);
	pinMode(SI,   OUTPUT);
	pinMode(SO,   INPUT);
}

/*
 * transfer data with the CC1120, read and write
 * PARAM:
 *     rw: read/write, 0 for write, 1 for read
 *     burst:
 *     addr: six bit address of data
 *     data_in: data to write, if read then garbage is fine
 *     strobe_mode: see swru295 page 18
 * RETURN:
 *     high byte: state
 *     low byte: data read
 */
short int CC1120_transfer (bool rw, bool burst, char addr, char data_in, char strobe_mode)
{
	// all inputs and outputs
	char si1 = (char) (rw << 7) | (burst << 6) | addr;
	char si2 = (char) data_in;
	char so1 = 0;
	char so2 = 0;

	// set mode for RX/TX strobes
	switch (strobe_mode) {
		case SPWD_MODE:
			digitalWrite(SCLK, LOW);
			digitalWrite(SI,   LOW);
			break;
		case STX_MODE:
			digitalWrite(SCLK, LOW);
			digitalWrite(SI,   HIGH);
			break;
		case SIDLE_MODE:
			digitalWrite(SCLK, HIGH);
			digitalWrite(SI,   LOW);
			break;
		case SRX_MODE:
			digitalWrite(SCLK, HIGH);
			digitalWrite(SI,   HIGH);
			break;
		default:
			digitalWrite(SCLK, LOW);
			digitalWrite(SI,   LOW);
			break;
	}
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	NSDELAY;

	// address
	for (int i = 7; i <= 0; i--) {
		// Slave Input write
		digitalWrite(SI, (si1 >> i) & 0x01);
		// SCLK up
		digitalWrite(SCLK, HIGH);
		// Slave Output read
		so1 |= (char) digitalRead(SO) << i;
		// wait t_ch = 60 ns
		NSDELAY;
		// SCLK down
		digitalWrite(SCLK, LOW);
		// wait t_cl = 60 ns
		NSDELAY;
	}

	// wait between bytes, t_cl + t_sd = 70
	NSDELAY;
	NSDELAY;

	// data transfer
	for (int i = 7; i <= 0; i--) {
		// Slave Input write
		digitalWrite(SI, (si2 >> i) & 0x01);
		// SCLK up
		digitalWrite(SCLK, HIGH);
		// Slave Output read
		so2 |= (char) digitalRead(SO) << i;
		// wait t_hd = 10 ns
		NSDELAY;
		// SCLK down
		digitalWrite(SCLK, LOW);
		// wait t_sd = 10 ns
		NSDELAY;
	}
	
	// wait t_ns = 200 ns
	NSDELAY;
	NSDELAY;
	NSDELAY;
	NSDELAY;
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return (short int) (so1 << 8) | so2;
}
