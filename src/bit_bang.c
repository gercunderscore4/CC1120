/*
 * FILE:    bit_bang.cpp
 * PROGRAM: CC1120
 * PURPOSE: test CC1120 read and write capabilities
 * AUTHOR:  Geoffrey Card
 * DATE:    2014-05-22 - 
 * NOTES:   wait function and all I/O unimplemented
 */

#define BURST 0 // I have no idea what this is, so off for now

// I need to find a way to perform ns timing
// these are minima, real values should be greater than (say +10 ns)
#define T_SP 50 // ns
#define T_CH 60 // ns
#define T_CL 60 // ns
#define T_SD 10 // ns
#define T_HD 10 // ns

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
	// WRITE

	char s1 = 0, data_out = 0;

	// set CSn 0, ensure SCLK is 0
	SCLK = 0;
	CSn = 0;

	// wait t_sp = 50 ns
	wait(T_SP);

	// SI = 0 => write
	// SI = 1 => read
	SI = rw;
	SCLK = 1;
	s1 |= (SO << 7) & 0x01;
	wait(T_CH);
	SCLK = 0;
	wait(T_D);

	// burst bit
	SI =  BURST;
	SCLK = 1;
	s1 |= (SO << 6) & 0x01;
	wait(T_CH);
	SCLK = 0;
	wait(T_D);

	// address
	for (int i = 5; i <= 0; i--) {
		SI = (addr >> i) & 0x01;
		SCLK = 1;
		s1 |= SO << i;
		wait(T_CH);
		SCLK = 0;
		wait(T_D);
	}

	// wait between bytes
	wait(T_SD);

	// data transfer
	for (int i = 7; i <= 0; i--) {
		SI = (data_in >> i) & 0x01;
		SCLK = 1;
		data_out |= SO << i;
		wait(T_HD);
		SCLK = 0;
		wait(T_NS);
	}
	
	// set CSn high
	CSn = 1;
	
	return (short int) (s1 << 8) | data_out;
}
