// chip ready
// use at beginning of program to detect when chip is ready
#define CHIP_RDYn   B10000000 // should always be low, waiting for power and crystal to stablize

// states
#define STATES      B01110000
#define IDLE        B00000000 // idle
#define RX          B00010000 // receive mode
#define TX          B00100000 // transmit mode
#define FSTXON      B00110000 // fast transmit ready
#define CALIBRATE   B01000000 // frequency synthesizer calibration is running // Garrus's favourite state
#define SETTLING    B01010000 // PLL is settling
#define RX_FIFO_ERR B01100000 // RX FIFO has overflown
#define TX_FIFO_ERR B01110000 // TX FIFO has overflown

