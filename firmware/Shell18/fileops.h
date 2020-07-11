#ifndef FILEOPS_H
#define	FILEOPS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "main.h"

#ifdef INCLUDE_FILES

#include "FatFs/source/ff.h"
#include "FatFs/source/diskio.h"

// main file ops entry point
// (*cmd) is a single command that can be executed automatically and the execution result returned
// If (*cmd == NULL) the function will enter manual execution mode and will be waiting for console input until exit
// (execf) is a flag enabling (non-0) or disabling (0) execution of external files
uint8_t fopsMain(uint8_t *cmd, uint8_t execf);

// print errors from FatFs
// (fe) error code (see FRESULT in FatFs, plus additional FR_xxxx definitions here)
void fopsError(int8_t fe);

// initialise storage drive and return error code (negative) or size in bytes (positive)
// (*dev) case-sensitive device identifier (eg. "ifs:", "ram:", etc.)
int64_t fopsInit(const char *dev);

// process (*str) with the provided wildcard (*wild) and return 1, if they match, or 0 otherwise
char wcMatch(char *str, char *wild);

#endif

#ifdef	__cplusplus
}
#endif

#endif	// FILEOPS_H
