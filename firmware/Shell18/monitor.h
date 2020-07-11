#ifndef MONITOR_H
#define	MONITOR_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "main.h"

// number of hexadecimal values displayed per single line
#define MON_HEX_PER_LINE	16

// number of hex characters in address
#define MON_ADDR_LEN		6

// if defined, multi-byte values will be stored in memory with the most significant byte first
// otherwise, multi-byte values will be stored with the least significant byte first (default)
//#define MON_MSB_FIRST

// if defined, the two commands for save to and load from file will become available
// requires INCLUDE_FILES defined too
#define MON_USE_FILES

// buffer size in file operation
#define MON_FILEBUF_SIZE	128

// error message codes
#define MON_OK			0
#define MON_NOMEM		97
#define MON_ERROR		98
#define MON_UNKN_CMD	99

// print error
// (msgcode) error code
void monError(int8_t msgcode);

// monitor entry point
uint8_t monMain(uint8_t *cmd);

#ifdef	__cplusplus
}
#endif

#endif	// MONITOR_H
