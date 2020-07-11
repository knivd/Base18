#include "main.h"
#include "console.h"
#include "hardware.h"
#include "xmem.h"

#ifdef INCLUDE_FILES
	#include "fileops.h"
#endif

#ifdef INCLUDE_MONITOR
	#include "monitor.h"
#endif

#ifdef INCLUDE_RIDE
	#include "ride.h"
#endif

#ifndef SANDBOX
	#include "bios.h"
#endif

// prompt string in the main console
uint8_t PROMPT[16] = "|_ ";

// version information constant
const uint32_t VER_INFO = VERSION;


// skip whitespace characters in string
// (preinc) specifies the minimum number of characters that definitely need to be skipped
void skip(char **source, uint32_t preinc) {
	char *src = *source;
	while(*src && preinc--) src++;
	while(*src) {
		if(strchr(" \t\v\r\n", *src)) src++;
		else break;
	}
	*source=src;
}


// main function
// ===========================================================================

#ifndef FF_MAX_LFN
#define FF_MAX_LFN 255
#endif

int main(void) {

	// system initialisation
	coldInit();
	#ifndef SANDBOX

		linksC();	// create the BIOS jump table on the first run

		// ====================================================

		// This is a completely bogus check that will never get executed
		// The only reason for it is to trick the compiler to not remove functions that
		// are needed for inclusion but have not been used elsewhere in the code yet
		if(PROMPT[strlen((char *) PROMPT)] == '\a') {
			getche();
			peekch();
			ungetch(0);
			#ifdef INCLUDE_FILES
				f_truncate(NULL);
				f_sync(NULL);
				f_findfirst(NULL, NULL, NULL, NULL);
				f_findnext(NULL, NULL);
				f_stat(NULL, NULL);
				f_chmod(NULL, 0, 0);
				f_utime(NULL, NULL);
				f_getlabel(NULL, NULL, NULL);
				f_setlabel(NULL);
				f_forward(NULL, NULL, 0, NULL);
				f_expand(NULL, 0, 0);
				f_putc(0, NULL);
				f_puts(NULL, NULL);
				f_printf(NULL, NULL);
				f_gets(NULL, 0, NULL);
			#endif
		}

	#endif

	// ====================================================

	printf("Included modules: (SHUTDOWN)  ");
	#ifdef INCLUDE_FILES
		printf("FOS  ");
	#endif
	#ifdef INCLUDE_MONITOR
		printf("MON  ");
	#endif
	#ifdef INCLUDE_RIDE
		printf("RIDE  ");
	#endif
	printf("\r\n\n");

	while(1) {

		// print prompt and get command line
		uint8_t *temp = NULL;
		xalloc( (uint8_t **) &temp, FF_MAX_LFN );
		#ifdef INCLUDE_FILES
			f_getcwd( (TCHAR *) temp, (FF_MAX_LFN - strlen((char *) PROMPT) - 20) );
		#endif
		if(*temp && (*temp != ':')) {
			strcat((char *) temp, "\r\n");
			strcat((char *) temp, (char *) PROMPT);
			cputs((char *) temp);
		}
		else cputs((char *) PROMPT);
		xfree((uint8_t **) &temp);
		getLine(NULL);

		// system shutdown
		if(iscmd("shutdown", (const char *) cLine) || iscmd("SHUTDOWN", (const char *) cLine)) {
			#ifndef SANDBOX
				OFF = 0;
				TRISBbits.TRISB5 = 0;	// set the OFF# line low and that shuts down the power supply module
			#endif
			break;
		}

		#ifdef INCLUDE_MONITOR

		// invoke monitor
		if(iscmd("mon", (const char *) cLine) || iscmd("MON", (const char *) cLine)) {
			monMain(NULL);	// use the monitor in interactive mode
			continue;
		}

		#endif

		#ifdef INCLUDE_RIDE

		// RIDE
		if(iscmd("ride", (const char *) cLine) || iscmd("RIDE", (const char *) cLine)) {
			RIDE();
			continue;
		}

		#endif

		#ifdef INCLUDE_FILES

		// FOS
		if(iscmd("fos", (const char *) cLine) || iscmd("FOS", (const char *) cLine)) {
			char cs[5];
			strcpy(cs, "help");
			fopsMain((uint8_t *) cs, 0);
			continue;
		}

		// try to execute FileOps command
		// NOTE: This must be AFTER all other checks for known commands
		// (not using the built-in interactive mode here because it will prevent from adding external commands)
		int8_t result = fopsMain(cLine, 1);
		if(result != FR_OK) {
			fopsError(result);
			continue;
		}

		#endif

	}

	// will never get here
	return 0;

}
