/*
 * FILE:    bit_bang.h
 * PROGRAM: CC1120
 * PURPOSE: test CC1120 read and write capabilities
 * AUTHOR:  Geoffrey Card
 * DATE:    2014-05-23 - 2014-05-30
 * NOTES:   Can't use standard Arduino SPI because this transmits 2 bytes per enable
 */

#include "Arduino.h"
#include <stdbool.h>
#include <stdint.h>
 
// pins
// to make this slighly faster, use direct I/O
// http://arduino.cc/en/Reference/PortManipulation
#define SCLK 6 // OUTPUT
#define CSn  5 // OUTPUT
#define SI   4 // OUTPUT
#define SO   3 // INPUT

// timing
// 16 MHz, 62.5 ns, re-write if you change MCP
// http://playground.arduino.cc/Main/AVR
//#define NSDELAY __asm__("nop\n\t")
// these are minima, real values should be greater than (say +10 ns)
/*
#define T_SP 50 // ns
#define T_CH 60 // ns
#define T_CL 60 // ns
#define T_SD 10 // ns
#define T_HD 10 // ns
*/

// extended register access
#define EXT_REG 0x2F
// direct FIFO access
#define DIR_FIFO 0x3E
// standard FIFO access
#define STD_FIFO 0x3F

////////////////////////////////////////////////////////////////
////////////////                                ////////////////
////////////////////////////////////////////////////////////////

/*
 * do not use these directly
 * memory MUST be pre-allocated
 */
/*
inline uint8_t _transfer_byte (uint8_t si);
inline uint8_t _read_byte (void);
inline void _write_byte (uint8_t si);
inline void _transfer_bytes (uint8_t* si, uint8_t* so, int count);
inline void _read_bytes (uint8_t* so, uint8_t count);
inline void _write_bytes (uint8_t* si, uint8_t count);
*/

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
void CC1120_setup (void);

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

uint8_t register_access (bool r_nw, uint8_t addr, uint8_t* data, uint8_t data_size);
uint8_t register_access_ext (bool r_nw, uint8_t addr, uint8_t* data, uint8_t data_size);
uint8_t cmnd_strobe_access (bool r_nw, uint8_t cmnd);
uint8_t dir_FIFO_access (bool r_nw, uint8_t addr, uint8_t* data, uint8_t data_size);
uint8_t std_FIFO_access (bool r_nw, uint8_t addr, uint8_t* data, uint8_t data_size);
