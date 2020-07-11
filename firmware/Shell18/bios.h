#ifndef BIOS_H
#define	BIOS_H

#ifdef	__cplusplus
extern "C" {
#endif

// this is the ROM address where the table with pointers to important variables start
#define DATA_LINKS_ADDR		0x100

// this is the ROM address where the table with GOTO links to functions start
#define JUMP_LINKS_ADDR		0x180	// must be divisible by WRITE_FLASH_BLOCKSIZE

// this is the maximum size of the GOTO table in bytes (each statement is 4 bytes long)
#define JUMP_LINKS_SIZE		0x100	// must be divisible by WRITE_FLASH_BLOCKSIZE

// create BIOS jump links
void linksC(void);

#ifdef	__cplusplus
}
#endif

#endif	// BIOS_H
