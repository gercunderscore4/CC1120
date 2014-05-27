/*
 * FILE:    bit_bang.h
 * PROGRAM: CC1120
 * PURPOSE: test CC1120 read and write capabilities
 * AUTHOR:  Geoffrey Card
 * DATE:    2014-05-23 - 
 * NOTES:   Not optimized
 *          Can't use standard Arduino SPI because this transmits 2 bytes per enable
 */

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
#define NSDELAY __asm__("nop\n\t")

// I need to find a way to perform ns timing
// these are minima, real values should be greater than (say +10 ns)
/*
#define T_SP 50 // ns
#define T_CH 60 // ns
#define T_CL 60 // ns
#define T_SD 10 // ns
#define T_HD 10 // ns
*/

// read / !write
#define READ  B10000000
#define WRITE B00000000
// burst mode
#define BURST B01000000

/*
 * set up CC1120 SPI
 * PARAM:
 *     none, pins are defined constants
 * RETURN:
 *     void, there's nothing to return, if it fails, nothing will work anyway
 */
void CC1120_setup (void);

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
char CC1120_transfer (char strobe_mode, bool rw, char addr, int burst, char* data_in, char* data_out);
