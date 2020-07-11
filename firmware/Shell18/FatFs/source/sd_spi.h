#ifndef SD_SPI_H
#define SD_SPI_H

// modified by (c) KnivD, 2019

#ifdef __cplusplus
extern "C" {
#endif

#include "diskio.h"

// currently selected drive
// 0: drive 0, etc...
extern BYTE currentSD;

// Get Disk Status
DSTATUS sd_status (
	BYTE pdrv		// Physical drive number (0)
);

// Initialise Disk Drive
DSTATUS sd_init (
	BYTE pdrv		// Physical drive number (0)
);

// Read Sector(s)
DRESULT sd_read (
	BYTE pdrv,		// Physical drive number (0)
	BYTE *buff,		// Pointer to the data buffer to store read data
	DWORD sector,	// Start sector number (LBA)
	UINT count		// Sector count (1..128)
);

// Write Sector(s)
#if FF_FS_READONLY == 0
DRESULT sd_write (
	BYTE pdrv,		// Physical drive number (0)
	const BYTE *buff,	// Pointer to the data to be written
	DWORD sector,	// Start sector number (LBA)
	UINT count		// Sector count (1..128)
);
#endif

// Miscellaneous Functions
DRESULT sd_ioctl (
	BYTE pdrv,		// Physical drive number (0)
	BYTE cmd,		// Control code
	void *buff		// Buffer to send/receive data block
);

#ifdef __cplusplus
}
#endif

#endif // SD_SPI_H
