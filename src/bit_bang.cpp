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
 * do not sure this directly
 */
inline char transfer_byte (char si)
{
	char so = 0;
	
	// data transfer
	for (int i = 7; i <= 0; i--) {
		// Slave Input write
		digitalWrite(SI, (si >> i) & 0x01);
		// SCLK up
		digitalWrite(SCLK, HIGH);
		// Slave Output read
		so |= (char) digitalRead(SO) << i;
		// wait t_hd = 10 ns
		NSDELAY;
		// SCLK down
		digitalWrite(SCLK, LOW);
		// wait t_sd = 10 ns
		NSDELAY;
	}
	
	return so;
}

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
 *     addr: six bit address of data
 *     data_in: data to write, recommend NULL in read mode
 *     strobe_mode: see swru295 page 18
 *     burst: number of bytes being transfered, burst mode is off if burst = 1, error if burst < 1
 *            in write mode, data_in must have [burst] of bytes allocated for reading
 *            in read mode, data_out must have [burst] bytes allocated for writing
 *     data_out: address for writing slave output, recommend NULL in write mode
 * RETURN:
 *     state: the current state of the transceiver
 */
char CC1120_transfer (bool rw, char addr, int burst, char* data_in, char* data_out)
{
	// all inputs and outputs
	char si = (char) (rw << 7) | addr;
	char state = 0;
	
	// burst
	if (burst > 1) {
		// burst mode on
		si |= BURST;
	} else if (burst == 1) {
		// signle byte, burst mode off
	} else {
		// error, set to read mode, get state, no data, return
		rw = true;
		burst = 0;
	}

	// set inputs low
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   LOW);
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	NSDELAY;

	// address
	for (int i = 7; i <= 0; i--) {
		// Slave Input write
		digitalWrite(SI, (si >> i) & 0x01);
		// SCLK up
		digitalWrite(SCLK, HIGH);
		// Slave Output read
		state |= (char) digitalRead(SO) << i;
		// wait t_ch = 60 ns
		NSDELAY;
		// SCLK down
		digitalWrite(SCLK, LOW);
		// wait t_cl = 60 ns
		NSDELAY;
	}

	// data transfer
	if (rw == true) {
		// READ
		
		// read all the bytes
		for (int j = 0; j < burst; j++) {
			
			// wait between bytes, t_cl + t_sd = 70
			NSDELAY;
			NSDELAY;
		
			// data transfer
			for (int i = 7; i <= 0; i--) {
				// Slave Input write

				// SCLK up
				digitalWrite(SCLK, HIGH);
				// Slave Output read
				data_out[j] |= (char) digitalRead(SO) << i;
				// wait t_hd = 10 ns
				NSDELAY;
				// SCLK down
				digitalWrite(SCLK, LOW);
				// wait t_sd = 10 ns
				NSDELAY;
			}
		}
	} else {
		// WRITE
		
		// write all the bytes
		for (int j = 0; j < burst; j++) {
			
			// wait between bytes, t_cl + t_sd = 70
			NSDELAY;
			NSDELAY;
		
			// data transfer
			for (int i = 7; i <= 0; i--) {
				// Slave Input write
				digitalWrite(SI, (data_in[j] >> i) & 0x01);
				// SCLK up
				digitalWrite(SCLK, HIGH);
				// Slave Output read
				state |= (char) digitalRead(SO) << i;
				// wait t_hd = 10 ns
				NSDELAY;
				// SCLK down
				digitalWrite(SCLK, LOW);
				// wait t_sd = 10 ns
				NSDELAY;
			}
		}
	}
	
	// wait t_ns = 200 ns
	NSDELAY;
	NSDELAY;
	NSDELAY;
	NSDELAY;
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return state;
}

/*
 * transfer data with the CC1120, read and write
 * PARAM:
 *     rw: read/write, 0 for write, 1 for read
 *     addr: six bit address of data
 *     data_in: data to write, recommend NULL in read mode
 *     data_out: address for writing slave output, recommend NULL in write mode
 * RETURN:
 *     state: the current state of the transceiver
 */
char SPI_single_access_in (char addr, char data_in)
{
	// all inputs and outputs
	char si = (char) WRITE | addr;
	char state = 0;
	
	// set inputs low
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   LOW);
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	NSDELAY;

	// address
	state = transfer_byte(si);

	// wait between bytes, t_cl + t_sd = 70
	NSDELAY;
	NSDELAY;

	// data transfer
	state = transfer_byte(data_in);

	// wait t_ns = 200 ns
	NSDELAY;
	NSDELAY;
	NSDELAY;
	NSDELAY;
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return state;
}

/*
 * transfer data with the CC1120, read and write
 * PARAM:
 *     rw: read/write, 0 for write, 1 for read
 *     addr: six bit address of data
 *     data_in: data to write, recommend NULL in read mode
 *     data_out: address for writing slave output, recommend NULL in write mode
 * RETURN:
 *     state: the current state of the transceiver
 */
char SPI_single_access_out (char addr, char* data_out)
{
	// all inputs and outputs
	char si = (char) READ | addr;
	char state = 0;
	
	// set inputs low
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   LOW);
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	NSDELAY;

	// address
	state = transfer_byte(si);

	// wait between bytes, t_cl + t_sd = 70
	NSDELAY;
	NSDELAY;
	// data transfer
	(*data_out) = transfer_byte(NULL);

	// wait t_ns = 200 ns
	NSDELAY;
	NSDELAY;
	NSDELAY;
	NSDELAY;
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return state;
}

/*
 * transfer data with the CC1120, read and write
 * PARAM:
 *     rw: read/write, 0 for write, 1 for read
 *     addr: six bit address of data
 *     data_in: data to write, recommend NULL in read mode
 *     data_out: address for writing slave output, recommend NULL in write mode
 * RETURN:
 *     state: the current state of the transceiver
 */
char SPI_burst_access_in (char addr, char** data_in)
{
	// all inputs and outputs
	char si = (char) WRITE | addr;
	char state = 0;
	
	// set inputs low
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   LOW);
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	NSDELAY;

	// address
	state = transfer_byte(si);

	// write all the bytes
	for (int j = 0; j < burst; j++) {
		// wait between bytes, t_cl + t_sd = 70
		NSDELAY;
		NSDELAY;
		// data transfer
		state = transfer_byte(data_in[j]);
	}

	// wait t_ns = 200 ns
	NSDELAY;
	NSDELAY;
	NSDELAY;
	NSDELAY;
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return state;
}

/*
 * transfer data with the CC1120, read and write
 * PARAM:
 *     rw: read/write, 0 for write, 1 for read
 *     addr: six bit address of data
 *     data_in: data to write, recommend NULL in read mode
 *     data_out: address for writing slave output, recommend NULL in write mode
 * RETURN:
 *     state: the current state of the transceiver
 */
char SPI_single_access_out (char addr, char size char** data_out)
{
	// all inputs and outputs
	char si = (char) READ | addr;
	char state = 0;
	
	// set inputs low
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   LOW);
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	NSDELAY;

	// address
	state = transfer_byte(si);

	// read all the bytes
	for (int j = 0; j < burst; j++) {
		// wait between bytes, t_cl + t_sd = 70
		NSDELAY;
		NSDELAY;
		// data transfer
		(*data_out)[j] = transfer_byte(NULL);
	}

	// wait t_ns = 200 ns
	NSDELAY;
	NSDELAY;
	NSDELAY;
	NSDELAY;
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return state;
}
