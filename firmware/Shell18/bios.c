#include "main.h"
#include "bios.h"
#include "xmem.h"
#include "console.h"
#include "hardware.h"
#include "FatFs/source/diskio.h"
#include "fileops.h"
#include "monitor.h"
#include "ride.h"

// data links
__at(DATA_LINKS_ADDR) static const void * const linksD[] = {

	// ============= VERSION =============
	// [year] [month] [[sw major]:[sw minor]]

	& VER_INFO,		// const uint32_t VER_INFO = VERSION

	// ============= VARIABLES AND CONSTANTS =============

	// memory
	& SYSRAM_ADDR,	// uint32_t SYSRAM_ADDR	// start address of the system RAM (installed on a separate board)
	& SYSRAM_SIZE,	// uint32_t SYSRAM_SIZE	// detected size of system RAM in bytes

	// console interface
	& cLine,		// uint8_t *cLine = &console_buffer[0][2];	// pointer to the actual text in the console buffer
	& PROMPT,		// uint8_t PROMPT[16]	// prompt string in the main console

	// file system
	& FF,			// FATFS FF				// variable used for the file and storage operations
	& RAM,			// uint32_t RAM			// entry point of the internal RAM disk
	& RAM_SIZE_KB,	// uint32_t RAM_SIZE_KB	// RAM disk size in KB (minimum FATFSSIZEMINKB, if not 0)

	// top level modules and system
	& XTAL_FREQ,	// uint32_t XTAL_FREQ	// calculated system clock frequency in Hz
	& INTchain,		// uint8_t INTchain		// address for interrupt chaining function
	& systick,		// uint32_t systick		// system tick counter increasing at 100 Hz rate
	& date_time,	// struct tm date_time	// system date/time structure

	// marks the end of the table; must be always the last element here
	(void *) 0

};

// code links
// must be aligned to (WRITE_FLASH_BLOCKSIZE)
__at(JUMP_LINKS_ADDR) static const volatile uint8_t jt[JUMP_LINKS_SIZE] = { [0 ... (JUMP_LINKS_SIZE-1)] = 0xff };


// create GOTO addr link at a specified location in buffer
void jLink(uint8_t *buf, uint16_t index, uint32_t addr) {
	addr >>= 1;	// the PIC18 GOTO instruction encodes addresses this way
	buf[index] = (uint8_t) addr;
	buf[index+1] = 0xEF;
	buf[index+2] = (uint8_t) (addr >> 8);
	buf[index+3] = (uint8_t) (0xF0 + ((addr >> 16) & 0x0F));
}


void linksC(void) {
	if((jt[1] != 0xEF) || ((jt[3] & 0xF0) != 0xF0)) {

		printf("Initialising BIOS function table at address 0x%04x\r\n\n", JUMP_LINKS_ADDR);

		uint8_t *b = NULL;
		xalloc((uint8_t **) &b, -JUMP_LINKS_SIZE);	// negative size = allocation MUST succeed

		#define V(x) ((uint32_t)(const void (*)(void))(x))

		// memory
		jLink(b, 0x00, V(memW));		// void memW(uint32t addr, uint8t val)
		jLink(b, 0x04, V(memR));		// uint8t memR(uint32t addr)
		jLink(b, 0x08, V(memSet));		// void memSet(uint32t addr, uint8t val, uint32t n)
		jLink(b, 0x0C, V(memMove));		// void memMove(uint32t dstaddr, uint32t srcaddr, uint32t n)
		jLink(b, 0x10, V(memCmp));		// int16t memCmp(uint32t addr1, uint32t addr2, uint32t n)
		jLink(b, 0x14, V(strLen));		// uint32t strLen(uint32t addr)
		jLink(b, 0x18, V(LCALL));		// void LCALL(uint32t addr)

		// console interface
		jLink(b, 0x1C, V(skip));		// void skip(char **source, uint32t preinc)
		jLink(b, 0x20, V(putch));		// void putch(char ch)
		jLink(b, 0x24, V(getch));		// char getch(void)
		jLink(b, 0x28, V(getche));		// char getche(void)
		jLink(b, 0x2C, V(ungetch));		// void ungetch(char ch)
		jLink(b, 0x30, V(kbhit));		// bit kbhit(void)
		jLink(b, 0x34, V(peekch));		// char peekch(void)
		jLink(b, 0x38, V(cputs));		// void cputs(const char *ch)
		jLink(b, 0x3C, V(cgets));		// char *cgets(char *buf)
		jLink(b, 0x40, V(getLine));		// void getLine(char *plstr)	// read from the console and return in console[]

		// FatFs file system
		jLink(b, 0x44, V(wcMatch));		// char wcMatch(char *str, char *wild)	// true, if (*s) has wildcard pattern (*wild)
		jLink(b, 0x48, V(f_open));		// FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode)
		jLink(b, 0x4C, V(f_close));		// FRESULT f_close (FIL* fp)
		jLink(b, 0x50, V(f_read));		// FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br)
		jLink(b, 0x54, V(f_write));		// FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw)
		jLink(b, 0x58, V(f_lseek));		// FRESULT f_lseek (FIL* fp, FSIZEt ofs)
		jLink(b, 0x5C, V(f_truncate));	// FRESULT f_truncate (FIL* fp)
		jLink(b, 0x60, V(f_sync));		// FRESULT f_sync (FIL* fp)
		jLink(b, 0x64, V(f_opendir));	// FRESULT f_opendir (DIR* dp, const TCHAR* path)
		jLink(b, 0x68, V(f_closedir));	// FRESULT f_closedir (DIR* dp)
		jLink(b, 0x6C, V(f_readdir));	// FRESULT f_readdir (DIR* dp, FILINFO* fno)
		jLink(b, 0x70, V(f_findfirst));	// FRESULT f_findfirst (DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern)
		jLink(b, 0x74, V(f_findnext));	// FRESULT f_findnext (DIR* dp, FILINFO* fno)
		jLink(b, 0x78, V(f_mkdir));		// FRESULT f_mkdir (const TCHAR* path)
		jLink(b, 0x7C, V(f_unlink));	// FRESULT f_unlink (const TCHAR* path)
		jLink(b, 0x80, V(f_rename));	// FRESULT f_rename (const TCHAR* pathold, const TCHAR* pathnew)
		jLink(b, 0x84, V(f_stat));		// FRESULT f_stat (const TCHAR* path, FILINFO* fno)
		jLink(b, 0x88, V(f_chmod));		// FRESULT f_chmod (const TCHAR* path, BYTE attr, BYTE mask)
		jLink(b, 0x8C, V(f_utime));		// FRESULT f_utime (const TCHAR* path, const FILINFO* fno)
		jLink(b, 0x90, V(f_chdir));		// FRESULT f_chdir (const TCHAR* path)
		jLink(b, 0x94, V(f_chdrive));	// FRESULT f_chdrive (const TCHAR* path)
		jLink(b, 0x98, V(f_getcwd));	// FRESULT f_getcwd (TCHAR* buff, UINT len)
		jLink(b, 0x9C, V(f_getfree));	// FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs)
		jLink(b, 0xA0, V(f_getlabel));	// FRESULT f_getlabel (const TCHAR* path, TCHAR* label, DWORD* vsn)
		jLink(b, 0xA4, V(f_setlabel));	// FRESULT f_setlabel (const TCHAR* label)
		jLink(b, 0xA8, V(f_forward));	// FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf)
		jLink(b, 0xAC, V(f_expand));	// FRESULT f_expand (FIL* fp, FSIZEt szf, BYTE opt)
		jLink(b, 0xB0, V(f_mount));		// FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt)
		jLink(b, 0xB4, V(f_mkfs));		// FRESULT f_mkfs (const TCHAR* path, BYTE opt, DWORD au, void* work, UINT len)
		jLink(b, 0xB8, V(f_setcp));		// FRESULT f_setcp (WORD cp)
		jLink(b, 0xBC, V(f_putc));		// int f_putc (TCHAR c, FIL* fp)
		jLink(b, 0xC0, V(f_puts));		// int f_puts (const TCHAR* str, FIL* cp)
		jLink(b, 0xC4, V(f_printf));	// int f_printf (FIL* fp, const TCHAR* str, ...)
		jLink(b, 0xC8, V(f_gets));		// TCHAR* f_gets (TCHAR* buff, int len, FIL* fp)

		// top level modules and system
		jLink(b, 0xCC, V(delay_us));	// void delay_us(uint32t us)	// 10 microseconds granularity
		jLink(b, 0xD0, V(delay_ms));	// void delay_ms(uint32t ms)	// 1 millisecond granularity
		jLink(b, 0xD4, V(coldInit));	// void coldInit(void)			// system initialisation
		jLink(b, 0xD8, V(fopsMain));	// uint8t fopsMain(uint8t *cmd, uint8t execf)
		jLink(b, 0xDC, V(fopsError));	// void fopsError(int8t fe)		// error codes as per FatFs plus extended
		jLink(b, 0xE0, V(fopsInit));	// int64t fopsInit(const char *dev)
		jLink(b, 0xE4, V(monMain));		// uint8t monMain(uint8t *cmd)
		jLink(b, 0xE8, V(RIDE));		// void RIDE(void)

		#undef V

		uint8_t *buf = b;
		uint32_t addr = JUMP_LINKS_ADDR;
		int32_t len = JUMP_LINKS_SIZE;
		while(len > 0) {
			FLASH_WriteBlock(addr, buf);
			addr += WRITE_FLASH_BLOCKSIZE;
			buf += WRITE_FLASH_BLOCKSIZE;
			len -= WRITE_FLASH_BLOCKSIZE;
		}

		xfree((uint8_t **) &b);

	}
}
