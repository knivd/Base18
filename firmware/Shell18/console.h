#ifndef CONSOLE_H
#define	CONSOLE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "main.h"

// console interface
// ===========================================================================

// maximum number of stored log entries for the console line
// The log entries are walked through by pressing the F3 key or Ctrl-]
// This number also includes the console line working buffer so the
// minimum value must be 1 and the maximum possible value is 255
#define CONSOLE_LOG_ENTRIES		4

// enabling this definition will produce automatic output echo in the console
// #define CONSOLE_ECHO

// convenient pointer to the actual text in the console buffer
extern uint8_t *cLine;

// main console function
// reads line from the console and returns it as ASCIIZ in the cLine[] buffer
// (*plstr) can be either NULL or a string that gets pre-loaded into the console buffer
void getLine(char *plstr);

// ===========================================================================

// size of the console reception buffer
#define CON_RX_BUFFER_SIZE	152

volatile uint8_t console_rx_buffer[CON_RX_BUFFER_SIZE];

// These two variables must be cleared during system initialisation
// When the incoming index is equal to the outgoing index, that means the buffer is empty
volatile uint8_t console_rxi_index;	// incoming character index, modified by the reception interrupt function
volatile uint8_t console_rxo_index;	// outgoing character index, modified by the buffer reading function

// CONIO.H standard console support functions

#ifndef SANDBOX

void putch(char ch);	// put char to the console
char getch(void);		// get char from the console and remove it from the buffer
char getche(void);		// same as getch() but with output echo
void ungetch(char ch);	// insert character into the console buffer
__bit kbhit(void);		// return 1 if there is char in the console buffer, or 0 otherwise

#endif

// return the first available character in the console buffer without removing it from it
// will return 0 if the buffer is empty
char peekch(void);

// implementation of cputs() as per the standardised definition:
// The cputs() function writes the character string pointed to by buf[] directly to the console
// using the putch() function. Unlike the puts() function, the carriage-return and line-feed characters
// are not appended to the string. The terminating null character is not written.
void cputs(const char *ch);

// implementation of cgets() as per the standardised definition:
// The cgets() function gets a string of characters directly from the console, and stores the string and its length
// in the array pointed to by buf[]. The first element of the array, buf[0], must contain the maximum length in
// characters of the string to be read. The array must be big enough to hold the string, a terminating null character,
// and two additional bytes.
// The cgets() function reads characters until a newline character is read, or until the specified number of characters
// is read. The string is stored in the array starting at buf[2]. The actual length of the string read is placed in buf[1].
// A pointer to the output string is returned.
// The functional reference for cgets() is taken from this link:
// https://docs.microsoft.com/en-us/cpp/c-runtime-library/cgets-cgetws?view=vs-2019
char *cgets(char *buf);

#define TAB			"   "	// space representation of a tabulation character

// basic keyboard codes
#define KEY_BREAK	0x03	// Ctrl-C used for terminating execution
#define KEY_ENTER   '\r'
#define KEY_TAB     '\t'
#define KEY_BACKSPC	'\b'
#define KEY_NLINE	'\n'
#define KEY_BEEP	'\a'
#define KEY_ESC     0x1b
#define KEY_FFEED	0x0b
#define KEY_CLREOL	0x1a
#define KEY_LOG		0x1d	// Ctrl-] code

// VT100-compatible key codes
#define KEY_LEFT	(((uint32_t)KEY_ESC << 16) + ((uint16_t)'[' << 8) + (uint8_t)'D')
#define KEY_RIGHT	(((uint32_t)KEY_ESC << 16) + ((uint16_t)'[' << 8) + (uint8_t)'C')
#define KEY_UP		(((uint32_t)KEY_ESC << 16) + ((uint16_t)'[' << 8) + (uint8_t)'A')
#define KEY_DOWN	(((uint32_t)KEY_ESC << 16) + ((uint16_t)'[' << 8) + (uint8_t)'B')
#define KEY_HOME	(((uint32_t)KEY_ESC << 24) + ((uint32_t)'[' << 16) + ((uint16_t)'1' << 8) + (uint8_t)'~')
#define KEY_END		(((uint32_t)KEY_ESC << 24) + ((uint32_t)'[' << 16) + ((uint16_t)'4' << 8) + (uint8_t)'~')
#define KEY_DEL		(((uint32_t)KEY_ESC << 24) + ((uint32_t)'[' << 16) + ((uint16_t)'3' << 8) + (uint8_t)'~')
#define KEY_INS		(((uint32_t)KEY_ESC << 24) + ((uint32_t)'[' << 16) + ((uint16_t)'2' << 8) + (uint8_t)'~')
#define KEY_PGUP	(((uint32_t)KEY_ESC << 24) + ((uint32_t)'[' << 16) + ((uint16_t)'5' << 8) + (uint8_t)'~')
#define KEY_PGDN	(((uint32_t)KEY_ESC << 24) + ((uint32_t)'[' << 16) + ((uint16_t)'6' << 8) + (uint8_t)'~')
#define KEY_F1		(((uint32_t)KEY_ESC << 16) + ((uint16_t)'O' << 8) + (uint8_t)'P')
#define KEY_F2		(((uint32_t)KEY_ESC << 16) + ((uint16_t)'O' << 8) + (uint8_t)'Q')
#define KEY_F3		(((uint32_t)KEY_ESC << 16) + ((uint16_t)'O' << 8) + (uint8_t)'R')
#define KEY_F4		(((uint32_t)KEY_ESC << 16) + ((uint16_t)'O' << 8) + (uint8_t)'S')

// alternative key codes (for other popular consoles; confirmed in Windows 10)
#define ALT_BACKSPC	0x7f
#define ALT_LEFT	(((uint16_t)KEY_ESC << 8) + (uint8_t)'K')
#define ALT_RIGHT	(((uint16_t)KEY_ESC << 8) + (uint8_t)'M')
#define ALT_UP		(((uint16_t)KEY_ESC << 8) + (uint8_t)'H')
#define ALT_DOWN	(((uint16_t)KEY_ESC << 8) + (uint8_t)'P')
#define ALT_HOME	(((uint16_t)KEY_ESC << 8) + (uint8_t)'G')
#define ALT_END		(((uint16_t)KEY_ESC << 8) + (uint8_t)'O')
#define ALT_DEL		(((uint16_t)KEY_ESC << 8) + (uint8_t)'S')
#define ALT_INS		(((uint16_t)KEY_ESC << 8) + (uint8_t)'R')
#define ALT_PGUP	(((uint16_t)KEY_ESC << 8) + (uint8_t)'I')
#define ALT_PGDN	(((uint16_t)KEY_ESC << 8) + (uint8_t)'Q')
#define ALT_F1		(((uint16_t)KEY_ESC << 8) + (uint8_t)';')
#define ALT_F2		(((uint16_t)KEY_ESC << 8) + (uint8_t)'<')
#define ALT_F3		(((uint16_t)KEY_ESC << 8) + (uint8_t)'=')
#define ALT_F4		(((uint16_t)KEY_ESC << 8) + (uint8_t)'>')

#ifdef	__cplusplus
}
#endif

#endif	// CONSOLE_H
