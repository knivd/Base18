#ifndef HARDWARE_H
#define	HARDWARE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "main.h"

// delay functions
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

// functions to work with the system memory
void memW(uint32_t addr, uint8_t val);					// write byte to address
uint8_t memR(uint32_t addr);							// read byte from address
void memSet(uint32_t addr, uint8_t val, uint32_t n);	// equivalent to memset()
void memMove(uint32_t dst, uint32_t src, uint32_t n);	// equivalent to memmove() and memcpy()
int16_t memCmp(uint32_t v1, uint32_t v2, uint32_t n);	// equivalent of memcmp()
uint32_t strLen(uint32_t s);							// equivalent to strlen()

#define memCpy memMove

#ifndef SANDBOX

	// Microchip PIC libraries
	#include <xc.h>
	#include "memory.h"

	// NOTE: Addresses above (DATA_RAM) are considered as access to the internal data RAM
	#define DATA_RAM	0xFF000000

	// macro to check whether an address is within the data RAM
	#define isDataRAM(addr) ((addr >= DATA_RAM)? 1 : 0)

	// internal file system
	#if IFS_SIZE_KB >= FATFS_SIZE_MIN_KB

		// start address of the IFS array
		#define IFS_ADDR (((_ROMSIZE - (1024UL * IFS_SIZE_KB)) / ERASE_FLASH_BLOCKSIZE) * ERASE_FLASH_BLOCKSIZE)

		// IFS data area
		// must be aligned to (ERASE_FLASH_BLOCKSIZE)
		__at(IFS_ADDR) const volatile uint8_t IFS[1024UL * IFS_SIZE_KB];

	#endif

	// long jump and long call
	#define LJUMP(a) { \
			PCLATU = (uint8_t) ((uint32_t) a >> 16); \
			PCLATH = (uint8_t) ((uint32_t) a >> 8); \
			PCLAT = (uint8_t) a; \
		}
	void LCALL(uint32_t addr);

	void regStore(void);	// store all fire registers into system memory
	void regRestore(void);	// restore all file registers from the system memory

	uint32_t XTAL_FREQ;	// calculated system clock frequency in Hz
	uint8_t INTchain;	// address for interrupt chaining function
	uint32_t systick;	// system tick counter increasing at 100 Hz rate

	uint32_t RAM;		// entry point of the internal RAM disk

	#include "hwconfig.h"

#else

	// internal file system data entry point for sandbox systems
	#if IFS_SIZE_KB >= FATFS_SIZE_MIN_KB
		FILE *IFS;
	#endif

	#define DATA_RAM	0		// not used in sandbox mode

	uint8_t sandbox[0x20000];	// emulated system RAM for testing and debug in sandbox systems
	uint8_t *RAM;				// entry point of the internal RAM disk

#endif

uint32_t RAM_SIZE_KB;	// RAM disk size in KB (minimum FATFS_SIZE_MIN_KB, if not 0)
uint32_t SYSRAM_ADDR;	// start address of the system RAM (installed on a separate board)
uint32_t SYSRAM_SIZE;	// detected size of system RAM in bytes
uint32_t STORE_ADDR;	// points to an area in memory where internal file registers are to be stored

// cold start initialisation of the platform hardware
void coldInit(void);

#ifdef	__cplusplus
}
#endif

#endif	// HARDWARE_H
