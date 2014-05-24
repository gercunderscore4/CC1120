/*
 * FILE:    bit_bang.cpp
 * PROGRAM: CC1120
 * PURPOSE: test CC1120 read and write capabilities
 * AUTHOR:  Geoffrey Card
 * DATE:    2014-05-22 - 
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

// burst
// I have no idea what this is, so off for now
#define BURST B00000000 // B01000000

// I need to find a way to perform ns timing
// these are minima, real values should be greater than (say +10 ns)
/*
#define T_SP 50 // ns
#define T_CH 60 // ns
#define T_CL 60 // ns
#define T_SD 10 // ns
#define T_HD 10 // ns
*/

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
	pinMode(SO,   OUTPUT);
}

/*
 * transfer data with the CC1120, read and write
 * PARAM:
 *     rw: read/write, 0 for write, 1 for read
 *     addr: six bit address of data
 *     data_in: data to write, if read then garbage is fine
 * RETURN:
 *     high byte: state
 *     low byte: data read
 */
short int CC1120_transfer (bool rw, char addr, char data_in)
{
	// all inputs and outputs
	char si1 = (char) (rw << 7) | BURST | addr;
	char si2 = (char) data_in;
	char so1 = 0;
	char so2 = 0;

	// set CSn 0, ensure SCLK is 0
	digitalWrite(SCLK, LOW);
	digitalWrite(CSn,  LOW);
	SCLK = 0;
	CSn = 0;

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
