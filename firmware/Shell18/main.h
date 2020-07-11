/*

SHELL-18 operating environment

Copyright (c) 2019, Konstantin Dimitrov
All rights reserved, knivd@me.com

Redistribution and use in source and binary forms, with or without modification, are permitted provided
that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice, this list of conditions and
		the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
		and the following disclaimer in the documentation and/or other materials provided with the distribution.
    3. Neither the name of Konstantin Dimitrov nor the names of other contributors may be used to endorse
		or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.

IN NO EVENT SHALL KONSTANTIN DIMITROV BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

// ============= VERSION =============
// [year] [month] [[sw major]:[sw minor]] [hardware]

#define VERSION	0x130A1000

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <time.h>

// main system configuration options
// ===========================================================================

// enabling this will be compile the system abstract from any particular hardware
// The XMEM_SIZE definition must be additionally provided for abstract systems
//#define SANDBOX

// internal file storage size in KB (minimum FATFS_SIZE_MIN_KB, if not 0)
#define IFS_SIZE_KB			33

#ifndef SANDBOX
	// size in bytes of the internal dynamically managed memory
	#define XMEM_SIZE		1022
#endif

// console baud rate (protocol 8N1)
#define CONSOLE_BAUDRATE	115200

// maximum length of console line (the absolute allowed maximum is 255 characters)
#define CONSOLE_LINE_LEN	144

// enable file operations
#define INCLUDE_FILES

// enable the monitor
#define INCLUDE_MONITOR

// enable RIDE
#define INCLUDE_RIDE

// ===========================================================================

#define printBanner() printf("\r\nSHELL-18 (C) Konstantin Dimitrov\r\nSoftware Build Date: %s\r\n", __DATE__);

#define FATFS_SIZE_MIN_KB 	33	// absolute minimum size of drive supported by FatFs (do not change this!)

// check whether a string is matches a given pattern and ends with space or \r or \n
#define iscmd(x,pcmd) (!strncmp(x,pcmd,strlen(x)) && (*(pcmd+strlen(x))==0 || strchr(" \r\n",*(pcmd+strlen(x)))))

// stringification macros
#define STR_(X) #X		// this converts to string (do not use this one but STR(X) !)
#define STR(X) STR_(X)	// this makes sure the argument is expanded before converting to string

#define BIT(x) (1ULL << (x))

// prompt string in the main console for file operations
// Must be no longer than 15 characters
extern uint8_t PROMPT[16];

// version information constant
extern const uint32_t VER_INFO;

// system date/time structure
struct tm date_time;

// skip whitespace characters in string
// (preinc) specifies the minimum number of characters that definitely need to be skipped
void skip(char **source, uint32_t preinc);

#ifdef	__cplusplus
}
#endif

#endif	// MAIN_H
