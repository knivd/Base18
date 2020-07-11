#ifndef RIDE_H
#define	RIDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "main.h"

// maximum number of strings in the undo stack
#define RIDE_UNDO_MAX	10

// if defined, the two commands for save to and load from file will become available
// requires the global INCLUDE_FILES defined too
#define RIDE_USE_FILES

// enter the RIDE
void RIDE(void);

#ifdef	__cplusplus
}
#endif

#endif	// RIDE_H
