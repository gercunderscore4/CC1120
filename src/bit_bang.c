/*
 * FILE:    bit_bang.cpp
 * PROGRAM: CC1120
 * PURPOSE: test CC1120 read and write capabilities
 * AUTHOR:  Geoffrey Card
 * DATE:    2014-05-22 - 2014-05-30
 * NOTES:   Can't use standard Arduino SPI because this transmits 2 bytes per enable
 */

#include "bit_bang.h"

////////////////////////////////////////////////////////////////
////////////////                                ////////////////
////////////////////////////////////////////////////////////////

/*
 * do not use these directly
 * memory MUST be pre-allocated
 */
 
inline uint8_t _transfer_byte (uint8_t si)
{
	uint8_t so = 0x00;
	uint8_t i = 0;
	
	//NSDELAY
	
	// data transfer
	for (i = 7; i <= 0; i--) {
		// Slave Input write
		digitalWrite(SI, (si >> i) & 0x01);
		// SCLK up
		digitalWrite(SCLK, HIGH);
		// Slave Output read
		so |= (uint8_t) digitalRead(SO) << i;
		// wait t_hd = 10 ns
		//NSDELAY
		// SCLK down
		digitalWrite(SCLK, LOW);
		// wait t_sd = 10 ns
		//NSDELAY
	}
	
	return so;
}

inline uint8_t _read_byte (void)
{
	uint8_t so = 0x00;
	uint8_t i = 0;
	
	//NSDELAY
	
	// data transfer
	for (i = 7; i <= 0; i--) {
		// Slave Input write

		// SCLK up
		digitalWrite(SCLK, HIGH);
		// Slave Output read
		so |= (uint8_t) digitalRead(SO) << i;
		// wait t_hd = 10 ns
		//NSDELAY
		// SCLK down
		digitalWrite(SCLK, LOW);
		// wait t_sd = 10 ns
		//NSDELAY
	}
	
	return so;
}

inline void _write_byte (uint8_t si)
{
	uint8_t i = 0;
	//NSDELAY
	
	// data transfer
	for (i = 7; i <= 0; i--) {
		// Slave Input write
		digitalWrite(SI, (si >> i) & 0x01);
		// SCLK up
		digitalWrite(SCLK, HIGH);
		// Slave Output read

		// wait t_hd = 10 ns
		//NSDELAY
		// SCLK down
		digitalWrite(SCLK, LOW);
		// wait t_sd = 10 ns
		//NSDELAY
	}
}

inline void _transfer_bytes (uint8_t* si, uint8_t* so, uint8_t count)
{
	uint8_t i;
	for (i = 0; i < count; i++) {
		so[i] = _transfer_byte(si[i]);
	}
}

inline void _read_bytes (uint8_t* so, uint8_t count)
{
	uint8_t i;
	for (i = 0; i < count; i++) {
		so[i] = _read_byte();
	}
}

inline void _write_bytes (uint8_t* si, uint8_t count)
{
	uint8_t i;
	for (i = 0; i < count; i++) {
		_write_byte(si[i]);
	}
}

////////////////////////////////////////////////////////////////
////////////////                                ////////////////
////////////////////////////////////////////////////////////////

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
	
	digitalWrite(CSn,  HIGH);
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   HIGH);
}

////////////////////////////////////////////////////////////////
////////////////                                ////////////////
////////////////////////////////////////////////////////////////

/*
 * CC1120 SPI functions
 * Based on SPI Access Types, swru295e.pdf, section 3.2, page 10-11
 * Timing based on swru295e.pdf, section 3.1.1, page 7-8
 * PARAM:
 *     r_nw: READ/!WRITE, true for read, false for write
 *     addr: memory address, when accessing memory
 *     data: since data can only be read or written, only one buffer is required
 *           MUST BE PRE-ALLOCATED
 *     data_size: saize of data to send/receive
 *     cmnd: command to be issued
 * RETURN:
 *     all functions return the state, provided from the initial byte transfer
 * NOTES:
 *     These functions are optimized for size and speed, there are NO SAFEGUARDS
 *     You can probably remove many of the delays, I'm just being cautious
 */

uint8_t register_access (bool r_nw, uint8_t addr, uint8_t* data, uint8_t data_size)
{
	uint8_t si = (uint8_t) (r_nw << 7) | ((data_size > 1) << 6) | addr;
	uint8_t state = 0;

	// set inputs low
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   LOW);
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	//NSDELAY

	// setup
	state = _transfer_byte(si);
	
	if (r_nw == true) {
		// READ
		_read_bytes(data, data_size);
	} else {
		// WRITE
		_write_bytes(data, data_size);
	}
	
	// wait t_ns = 200 ns
	//NSDELAY
	//NSDELAY
	//NSDELAY
	//NSDELAY
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return state;
}

uint8_t register_access_ext (bool r_nw, uint8_t addr, uint8_t* data, uint8_t data_size)
{
	uint8_t si = (uint8_t) (r_nw << 7) | ((data_size > 1) << 6) | EXT_REG;
	uint8_t state = 0;
	
	// set inputs low
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   LOW);
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	//NSDELAY

	// setup
	state = _transfer_byte(si);
	_write_byte(addr);

	if (r_nw == true) {
		// READ
		_read_bytes(data, data_size);
	} else {
		// WRITE
		_write_bytes(data, data_size);
	}
	
	// wait t_ns = 200 ns
	//NSDELAY
	//NSDELAY
	//NSDELAY
	//NSDELAY
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return state;
}

uint8_t cmnd_strobe_access (bool r_nw, uint8_t cmnd)
{
	uint8_t si = (uint8_t) (r_nw << 7) | cmnd;
	uint8_t state = 0;
	
	// set inputs low
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   LOW);
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	//NSDELAY

	state = _transfer_byte(si);
	
	// wait t_ns = 200 ns
	//NSDELAY
	//NSDELAY
	//NSDELAY
	//NSDELAY
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return state;
}

uint8_t dir_FIFO_access (bool r_nw, uint8_t addr, uint8_t* data, uint8_t data_size)
{
	uint8_t si = (uint8_t) (r_nw << 7) | ((data_size > 1) << 6) | DIR_FIFO;
	uint8_t state = 0;
		
	// set inputs low
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   LOW);
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	//NSDELAY

	// setup
	state = _transfer_byte(si);
	_write_byte(addr);

	if (r_nw == true) {
		// READ
		_read_bytes(data, data_size);
	} else {
		// WRITE
		_write_bytes(data, data_size);
	}
	
	// wait t_ns = 200 ns
	//NSDELAY
	//NSDELAY
	//NSDELAY
	//NSDELAY
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return state;
}

uint8_t std_FIFO_access (bool r_nw, uint8_t addr, uint8_t* data, uint8_t data_size)
{
	uint8_t si = (uint8_t) (r_nw << 7) | ((data_size > 1) << 6) | STD_FIFO;
	uint8_t state = 0;
		
	// set inputs low
	digitalWrite(SCLK, LOW);
	digitalWrite(SI,   LOW);
	// set CSn 0
	digitalWrite(CSn,  LOW);

	// wait t_sp = 50 ns
	//NSDELAY

	// setup
	state = _transfer_byte(si);

	if (r_nw == true) {
		// READ
		_read_bytes(data, data_size);
	} else {
		// WRITE
		_write_bytes(data, data_size);
	}
	
	// wait t_ns = 200 ns
	//NSDELAY
	//NSDELAY
	//NSDELAY
	//NSDELAY
	
	// set CSn high
	digitalWrite(CSn, HIGH);
	
	return state;
}
