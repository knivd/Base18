/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various existing       */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

// modified by (c) KnivD, 2019

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sd_spi.h"		/* SD card functions */

#include <stdio.h>		// printf() for debugging
#include <string.h>     // memcpy()
#include "../../main.h"	// hardware definitions
#include "../../xmem.h"	// memory manager functions
#include "../../hardware.h"	// hardware definitions

// internal for convenience to handle all situations
#if FF_MIN_SS != FF_MAX_SS
	#define sectorSize FF.ssize
#else
	#define sectorSize FF_MAX_SS
#endif


#ifndef SANDBOX

	#if IFS_SIZE_KB >= FATFS_SIZE_MIN_KB

	// write information from (*saddr) to (*daddr) using temporary data buffer (*buff)
	// (saddr) and (buff) must be in the data memory; (daddr) must be in the MCU flash
	// return 0 if there are no errors
	int8_t flashWrite(const uint8_t *daddr, uint8_t *saddr, uint32_t count, uint8_t *buff) {
		int8_t res = 0;
		uint32_t z = 0;	// total bytes
		uint32_t a = ((uint32_t) daddr / ERASE_FLASH_BLOCKSIZE) * ERASE_FLASH_BLOCKSIZE;	// page boundary
		uint32_t p = (uint32_t) daddr % ERASE_FLASH_BLOCKSIZE;	// internal page offset
		uint32_t t;
		uint8_t *d;
		while( (z < count) && (res == 0) ) {

			//get page into buffer and erase the page
			memcpy(buff, (const uint8_t *) a, ERASE_FLASH_BLOCKSIZE);
			FLASH_EraseBlock(a);

			// update the page in buffer
			while( (p < ERASE_FLASH_BLOCKSIZE) && (z < count) ) {
				*(buff + (p++)) = *(saddr++);
				z++;
			}
			p = 0;

			// write page from the buffer back to flash
			t = a + ERASE_FLASH_BLOCKSIZE;
			d = buff;
			while( (a < t) && (res == 0) ) {
				res = FLASH_WriteBlock((uint32_t) a, d);
				if(res == 0) res = memcmp((const uint8_t *) a, d, WRITE_FLASH_BLOCKSIZE);
				a += WRITE_FLASH_BLOCKSIZE;
				d += WRITE_FLASH_BLOCKSIZE;
			}

		}
		return res;
	}

	#endif

#else

// for sandbox systems
#include <time.h>

#endif


DWORD get_fattime (void) {
	BYTE year = 0;
	BYTE month = 0;
	BYTE day = 0;
	BYTE hour = 0;
	BYTE minute = 0;
	BYTE second = 0;
	struct tm *lt;

	#ifndef SANDBOX
		lt = &date_time;
	#else
		time_t tt;
		time(&tt);
		lt=localtime(&tt);
	#endif

	year = lt->tm_year;
	month = lt->tm_mon;
	day = lt->tm_mday;
	hour = lt->tm_hour;
	minute = lt->tm_min;
	second = lt->tm_sec;

	if(year > 99) year = 0;
	//if( (month < 1) || (month > 12) ) month = 1;
	//if( (day < 1) || (day > 31) ) day = 1;
	if(hour > 23) hour = 0;
	if(minute > 59) minute = 0;
	if(second > 59) second = 0;
    return ((DWORD)((2000+year) - 1980) << 25)	// Year
         | ((DWORD)month << 21)					// Month
         | ((DWORD)day << 16)					// Day
         | ((DWORD)hour << 11)					// Hour
         | ((DWORD)minute << 5)					// Minute
         | ((DWORD)second >> 1);				// Second
}


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive number to identify the drive */
) {
	DSTATUS stat = 0;

	switch (pdrv) {

        case DEV_NUL:
            return stat;

        case DEV_IFS:
			#if IFS_SIZE_KB >= FATFS_SIZE_MIN_KB

				#ifdef SANDBOX

				IFS = fopen("IFS.dat", "rb");
				if(!IFS) stat |= STA_NODISK;
				fclose(IFS);

				#endif	// nothing is needed when not in sandbox mode

			#else
				stat |= STA_NODISK;
			#endif
			return stat;

		case DEV_RAM:
			if(RAM_SIZE_KB >= FATFS_SIZE_MIN_KB) {
				if(!RAM) stat |= STA_NODISK;
			}
			else stat |= STA_NODISK;
			return stat;

		#ifndef SANDBOX

        case DEV_SD1:
			currentSD = 0;
			stat = sd_status(0);
            return stat;

		case DEV_SD2:
			currentSD = 1;
			stat = sd_status(0);
            return stat;

		case DEV_SD3:
			currentSD = 2;
			stat = sd_status(0);
            return stat;

		case DEV_SD4:
			currentSD = 3;
			stat = sd_status(0);
            return stat;

		#endif

		default:
			break;

	}
	return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Initialise a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive number to identify the drive */
) {
	DSTATUS stat = 0;

	switch (pdrv) {

        case DEV_NUL:
            return stat;

        case DEV_IFS:
			#if IFS_SIZE_KB >= FATFS_SIZE_MIN_KB

				#ifdef SANDBOX

				if(!IFS) {
					IFS = fopen("IFS.dat", "a+b");
					if(IFS) {
						fseek(IFS, 0, SEEK_END);
						uint32_t s = ftell(IFS);
						rewind(IFS);
						if( s < (1024UL * IFS_SIZE_KB) ) {	// create the IFS emulation file
							uint8_t *buff = NULL;
							xalloc( (uint8_t **) &buff, sectorSize );
							while( buff && (s < (1024UL * IFS_SIZE_KB)) ) {
								fwrite(buff, 1, sectorSize, IFS);
								s += sectorSize;
							}
							xfree((uint8_t **) &buff);
						}
						fclose(IFS);
					}
					else stat |= STA_NOINIT;
				}

				#endif	// nothing is needed when not in sandbox mode

			#else
				stat |= STA_NOINIT;
			#endif
			return stat;

		case DEV_RAM:
			if(RAM_SIZE_KB >= FATFS_SIZE_MIN_KB) {
				if( !RAM && ( SYSRAM_SIZE >= (1024UL + (1024UL * RAM_SIZE_KB)) ) ) {
					//xalloc( (uint8_t **) &RAM, (1024UL * RAM_SIZE_KB) );	// allocate RAM disk memory
					// instead of engaging the memory manager in it, the system RAM size
					// will simply be reduced by the size of the RAM disk
					#ifdef SANDBOX
						RAM = (uint8_t *) ( SYSRAM_ADDR + (SYSRAM_SIZE - (1024UL * RAM_SIZE_KB)) );
						if(RAM) memset(RAM, 0, (1024UL * RAM_SIZE_KB));
					#else
						RAM = ( SYSRAM_ADDR + (SYSRAM_SIZE - (1024UL * RAM_SIZE_KB)) );
						if(RAM) memSet(RAM, 0, (1024UL * RAM_SIZE_KB));
					#endif
					if(!RAM) stat |= STA_NOINIT;
					else SYSRAM_SIZE -= (1024UL * RAM_SIZE_KB);
				}
			}
			else stat |= STA_NOINIT;
			return stat;

		#ifndef SANDBOX

        case DEV_SD1:
			currentSD = 0;
			stat = sd_init(0);
            return stat;

		case DEV_SD2:
			currentSD = 1;
			stat = sd_init(0);
            return stat;

		case DEV_SD3:
			currentSD = 2;
			stat = sd_init(0);
            return stat;

		case DEV_SD4:
			currentSD = 3;
			stat = sd_init(0);
            return stat;

		#endif

		default:
			break;

	}
	return STA_NOINIT;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive number to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
) {
	DRESULT res = RES_OK;

	switch (pdrv) {

        case DEV_NUL:
			memset(buff, 0, (sectorSize * count));
            return res;

        case DEV_IFS: {
			#if IFS_SIZE_KB >= FATFS_SIZE_MIN_KB

				#ifdef SANDBOX

				if(IFS) {
					if(sector < ( (1024UL * IFS_SIZE_KB) / sectorSize ) ) {
						IFS = fopen("IFS.dat", "rb");
						if(IFS) {
							fseek(IFS, (sector * sectorSize), SEEK_SET);
							fread(buff, sectorSize, count, IFS);
							fclose(IFS);
						}
						else res = RES_NOTRDY;
					}
					else res = RES_PARERR;
				}
				else res = RES_NOTRDY;

				#else

				if( sector < ( (1024UL * IFS_SIZE_KB) / sectorSize ) ) {
					memMove( (uint32_t) (DATA_RAM + (uint32_t) buff),
							(uint32_t) &IFS[sectorSize * sector],
							(uint32_t) (sectorSize * count)
						);
				}
				else res = RES_PARERR;

				#endif

			#else
				res = RES_NOTRDY;
			#endif
			return res;
		}

		case DEV_RAM:
			if(RAM_SIZE_KB >= FATFS_SIZE_MIN_KB) {
				if(RAM) {
					if(sector < ( (1024UL * RAM_SIZE_KB) / sectorSize ) ) {
						#ifdef SANDBOX
							memcpy( (uint8_t *) buff,
									(uint8_t *) (RAM + (sectorSize * sector)),
									(sectorSize * count)
								);
						#else
							memMove( (DATA_RAM + (uint32_t) buff),
									(RAM + (sectorSize * sector)),
									(sectorSize * count)
								);
						#endif
					}
					else res = RES_PARERR;
				}
				else res = RES_NOTRDY;
			}
			else res = RES_NOTRDY;
			return res;

		#ifndef SANDBOX

        case DEV_SD1:
			currentSD = 0;
			res = sd_read(0, buff, sector, count);
            return res;

		case DEV_SD2:
			currentSD = 1;
			res = sd_read(0, buff, sector, count);
            return res;

		case DEV_SD3:
			currentSD = 2;
			res = sd_read(0, buff, sector, count);
            return res;

		case DEV_SD4:
			currentSD = 3;
			res = sd_read(0, buff, sector, count);
            return res;

		#endif

		default:
			break;

	}

	return RES_PARERR;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive number to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
) {
	DRESULT res = RES_OK;

	switch (pdrv) {

        case DEV_NUL:
            return res;

        case DEV_IFS: {
			#if IFS_SIZE_KB >= FATFS_SIZE_MIN_KB

				#ifdef SANDBOX

				if(IFS) {
					if(sector < ( (1024UL * IFS_SIZE_KB) / sectorSize ) ) {
						IFS = fopen("IFS.dat", "r+b");
						if(IFS) {
							fseek(IFS, (sector * sectorSize), SEEK_SET);
							fwrite(buff, sectorSize, count, IFS);
							fclose(IFS);
						}
						else res = RES_NOTRDY;
					}
					else res = RES_PARERR;
				}
				else res = RES_NOTRDY;

				#else

				if( sector < ( (1024UL * IFS_SIZE_KB) / sectorSize ) ) {
					unsigned char *pagebuff = NULL;
					xalloc( (uint8_t **) &pagebuff, ERASE_FLASH_BLOCKSIZE );
					if(pagebuff) {
						int8_t wrerr =
							flashWrite( (const uint8_t *) &IFS[sectorSize * sector],
										(uint8_t *) buff,
										(uint32_t) (sectorSize * count),
										(uint8_t *) pagebuff
									);
						xfree((uint8_t **) &pagebuff);
						if(wrerr) res = RES_ERROR;
					}
					else res = RES_ERROR;
				}
				else res = RES_PARERR;

				#endif

			#else
				res = RES_NOTRDY;
			#endif
			return res;
		}

		case DEV_RAM:
			if(RAM_SIZE_KB >= FATFS_SIZE_MIN_KB) {
				if(RAM) {
					if(sector < ( (1024UL * RAM_SIZE_KB) / sectorSize ) ) {
						#ifdef SANDBOX
							memcpy( (uint8_t *) (RAM + (sectorSize * sector) ),
									(uint8_t *) buff,
									(sectorSize * count)
								);
						#else
							memMove( (RAM + (sectorSize * sector)),
									(DATA_RAM + (uint32_t) buff),
									(sectorSize * count)
								);
						#endif
					}
					else res = RES_PARERR;
				}
				else res = RES_NOTRDY;
			}
			else res = RES_NOTRDY;
			return res;

		#ifndef SANDBOX

        case DEV_SD1:
			currentSD = 0;
			res = sd_write(0, buff, sector, count);
            return res;

		case DEV_SD2:
			currentSD = 1;
			res = sd_write(0, buff, sector, count);
            return res;

		case DEV_SD3:
			currentSD = 2;
			res = sd_write(0, buff, sector, count);
            return res;

		case DEV_SD4:
			currentSD = 3;
			res = sd_write(0, buff, sector, count);
            return res;

		#endif

		default:
			break;

	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive number (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
) {
	DRESULT res = RES_OK;
	WORD h;
    DWORD v;

	switch (pdrv) {

        case DEV_NUL:
			switch(cmd) {

				case GET_SECTOR_SIZE:
					h = FF_MIN_SS;
					memcpy( (uint8_t *) buff, (uint8_t *) &h, sizeof(WORD) );
					break;

				case GET_SECTOR_COUNT:
					v = 0;
					memcpy( (uint8_t *) buff, (uint8_t *) &v, sizeof(DWORD) );
					break;

				case GET_BLOCK_SIZE:
					v = 1;	// not flash memory
					memcpy( (uint8_t *) buff, (uint8_t *) &v, sizeof(DWORD) );
					break;

			}
			return res;

		case DEV_IFS:
			#if IFS_SIZE_KB >= FATFS_SIZE_MIN_KB

				#ifdef SANDBOX

				switch(cmd) {

					case GET_SECTOR_SIZE:
						h = FF_MIN_SS;
						memcpy( (uint8_t *) buff, (uint8_t *) &h, sizeof(WORD) );
						break;

					case GET_SECTOR_COUNT:
						v = (1024UL * IFS_SIZE_KB) / sectorSize;
						memcpy( (uint8_t *) buff, (uint8_t *) &v, sizeof(DWORD) );
						break;

					case GET_BLOCK_SIZE:
						v = 1;	// not flash memory
						memcpy( (uint8_t *) buff, (uint8_t *) &v, sizeof(DWORD) );
						break;

				}

				#else

				switch(cmd) {

					case GET_SECTOR_SIZE:
						h = FF_MIN_SS;
						memcpy( (unsigned char *) buff, (unsigned char *) &h, sizeof(WORD) );
						break;

					case GET_SECTOR_COUNT:
						v = (1024UL * IFS_SIZE_KB) / sectorSize;
						memcpy( (unsigned char *) buff, (unsigned char *) &v, sizeof(DWORD) );
						break;

					case GET_BLOCK_SIZE:
						v = ERASE_FLASH_BLOCKSIZE / sectorSize;
						if(v == 0) v = 1;
						memcpy( (unsigned char *) buff, (unsigned char *) &v, sizeof(DWORD) );
						break;

				}

				#endif

			#else
				res = RES_NOTRDY;
			#endif
			return res;

		case DEV_RAM:
			if(RAM_SIZE_KB >= FATFS_SIZE_MIN_KB) {
				switch(cmd) {

					case GET_SECTOR_SIZE:
						h = FF_MIN_SS;
						memcpy( (uint8_t *) buff, (uint8_t *) &h, sizeof(WORD) );
						break;

					case GET_SECTOR_COUNT:
						v = (1024UL * RAM_SIZE_KB) / sectorSize;
						memcpy( (uint8_t *) buff, (uint8_t *) &v, sizeof(DWORD) );
						break;

					case GET_BLOCK_SIZE:
						v = 1;	// not flash memory
						memcpy( (uint8_t *) buff, (uint8_t *) &v, sizeof(DWORD) );
						break;

				}
			}
			else res = RES_NOTRDY;
			return res;

		#ifndef SANDBOX

		case DEV_SD1:
			currentSD = 0;
			res = sd_ioctl(0, cmd, buff);
            return res;

		case DEV_SD2:

			currentSD = 1;
			res = sd_ioctl(0, cmd, buff);
            return res;

		case DEV_SD3:
			currentSD = 2;
			res = sd_ioctl(0, cmd, buff);
            return res;

		case DEV_SD4:
			currentSD = 3;
			res = sd_ioctl(0, cmd, buff);
            return res;

		#endif

		default:
			break;

    }

	return RES_PARERR;
}
