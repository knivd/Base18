#include "console.h"
#include "hardware.h"
#include "xmem.h"
#include "ride.h"

#ifdef INCLUDE_RIDE

uint32_t text = 0;
uint32_t text_max = 0;
uint32_t line = 0;		// line number for the cursor (starts from 0)
char *buffer = NULL;	// copy/move buffer (dynamically allocated)
char *strF = NULL;		// "find" string (dynamically allocated)
char *strR = NULL;		// "replace" string (dynamically allocated)
uint16_t Hsize = 80;	// horizontal size of the terminal in characters

#if RIDE_UNDO_MAX > 0
	uint8_t undop = 0;				// undo stack index
	char *undos[RIDE_UNDO_MAX];		// undo stack (dynamically allocated)
	uint32_t undol[RIDE_UNDO_MAX];	// undo stack lines
#endif

#if defined(RIDE_USE_FILES) && defined(INCLUDE_FILES)
	#include "FatFs/source/ff.h"
	#include "FatFs/source/diskio.h"
	#include "fileops.h"
	FIL *ff = NULL;
#endif


// go to line with specified number (counting starts from line 1; line 0 moves to the last line)
// modifies the global (line) variable and returns pointer to the line
uint32_t gotoLine(uint32_t ln) {
	ln--;	// internally the counting of lines starts from 0
	line = 0;
	uint32_t s = text;
    while(s && memR(s) && ln) {
        if(memR(s++) == '\n') {
            line++;
            ln--;
        }
    }
    return s;
}


// print line from the text (counting starts from 0)
// return pointer to the next line
uint32_t printLine(uint32_t lnum, uint32_t lptr) {
	line = (lnum-1);
	char ch;
    printf( "%3lu: ", (line+1) );
	do {
		ch = (char) memR(lptr++);
		if( (ch >= ' ') && (ch != 0x7f) ) printf("%c", ch);
        else if(ch == '\t') printf("%s", TAB);
	} while(ch && ch != '\n');
	return lptr;
}


// get number from source string and modify the string pointer to be pointing after the number
// NOTE: Only works with strings in data RAM
uint32_t getNumber(char **str) {
	char *s = *str;
	uint32_t n = 0;
	while( (*s >= '0') && (*s <= '9') ) {
		n = (10 * n) + (*s - '0');
		s++;
	}
	*str = s;
	return n;
}


// return length of string until '\n'
// the parameter (maxlen) specifies the maximum allowed length of the returned string
uint32_t strNLen(uint32_t s, uint32_t maxlen) {
	uint32_t n = 0;
	while( s && (n < maxlen) && memR(s+n) && (memR(s+n) != '\n') ) n++;
	return n;
}


// return the number of '\n' characters in a string
uint32_t countLines(uint32_t str) {
	uint32_t n = 0;
	while(str && memR(str)) {
		if(memR(str) == '\n') n++;
		str++;
	}
	return n;
}


#if RIDE_UNDO_MAX > 0

// push string into the undo stack
// The string is copied until NULL or a '\n' character
void pushUndo(uint32_t str) {
	if(!str) return;
	uint32_t slen = strNLen(str, CONSOLE_LINE_LEN);
	char *s = NULL;
	xalloc( (uint8_t **) &s, (slen + 1) );	// try to allocate memory for the string
	if(!s) return;	// just abandon if allocation has failed
	if(undop >= RIDE_UNDO_MAX) {	// if the stack is full - remove the oldest element
		uint8_t t;
		for(t = 1; t < RIDE_UNDO_MAX; t++) {
			undos[t-1] = undos[t];
			undol[t-1] = undol[t];
		}
		undop = RIDE_UNDO_MAX - 1;
	}
	memMove( (DATA_RAM + (uint32_t) s), str, slen );
	s[slen] = 0;
	undos[undop] = s;
	undol[undop] = (line+1);
	undop++;
}


// pop string from the undo stack
// The undo buffer must be released externally by executing
// xfree((uint8_t **) &undos[undop]);
char *popUndo(void) {
	if(!undop) return NULL;	// the undo stack is empty
	return undos[--undop];
}

#endif


// add/remove lines
// (pos) entry position in the main text
// (*content) NULL when removing, or a string when adding - must be in data RAM
void arLine(uint32_t pos, char *content) {

	// add
	if(content) {
		uint32_t cl = strlen(content) + 1;
		if( (strLen(text) + cl) >= text_max ) cl = text_max - strLen(text) - 1;
		uint32_t m = strLen(pos) + 1;
		memMove( (pos + cl), pos, m );
		memMove( pos, (DATA_RAM + (uint32_t) content), cl );
		memW( (pos + cl - 1), '\n' );
	}

	// remove
	else {
		uint32_t s = pos;
		while( memR(s) && (memR(s) != '\n') ) s++;
		if(memR(s) == '\n') s++;
		uint32_t cl = strLen(s) + 1;
		memMove( pos, s, cl );
		memW( (pos + cl - 1), 0 );
	}

}


void RIDE(void) {

	if(SYSRAM_SIZE == 0) {
		printf("\r\n>>> ERROR: No memory\r\n\n");
		return;
	}
	printf("\r\nRationally Integrated Development Editor, (C) KnivD\r\n");

	#if defined(RIDE_USE_FILES) && defined(INCLUDE_FILES)

	xalloc((uint8_t **) &ff, sizeof(FIL));
	if(!ff) printf(">>> WARNING: No buffer memory for file operations\r\n");

	#endif

	xalloc((uint8_t **) &buffer, (CONSOLE_LINE_LEN+1));
	if(!buffer) printf(">>> WARNING: No buffer memory for Copy/Move/Replace and file operations\r\n");
	xalloc((uint8_t **) &strF, (CONSOLE_LINE_LEN+1));
	if(!strF) printf(">>> WARNING: No buffer memory for Find text operations\r\n");
	xalloc((uint8_t **) &strR, (CONSOLE_LINE_LEN+1));
	if(!strR) printf(">>> WARNING: No buffer memory for Replace text operations\r\n");

	printf("\n");

	#ifndef SANDBOX
		text = SYSRAM_ADDR;
		text_max = SYSRAM_SIZE - 1;
	#else
		text = (uint32_t) sandbox;
		text_max = sizeof(sandbox) - 1;
	#endif

	// memW(text, 0);
	line = (uint32_t) -1;
	#if RIDE_UNDO_MAX > 0
		undop = 0;	// reset the undo index
		memset(undos, 0, sizeof(undos));
		memset(undol, 0, sizeof(undol));
	#endif
	uint32_t repc, srcl = 0;
	char *rpcmd;
	uint8_t qflag = 0;
	while( !qflag ) {

		uint32_t st = gotoLine(line+1);	// this is pointer to the current line in the text
		printf( "%3lu: ", (line+1) );
		if(buffer) {
			memset(buffer, 0, (CONSOLE_LINE_LEN+1));
			uint32_t sl = strNLen(st, CONSOLE_LINE_LEN);
			memMove( (DATA_RAM + (uint32_t) buffer), st, sl );
		}
		getLine(buffer);	// pre-load the console buffer with the current line and then edit
		char *pcmd = (char *) cLine;	// this is pointer to the newly received line from the console
		rpcmd = NULL;
		repc = 1;	// set default value for the repeat counter

		// if the first character in a line is not '.' - write text to the end of the line
		if(*pcmd != '.') {
			if(*pcmd) {
				uint32_t sl = strNLen( st, CONSOLE_LINE_LEN );
				uint32_t pl = strNLen( (DATA_RAM + (uint32_t) pcmd), CONSOLE_LINE_LEN );
				if( (sl != pl) || memCmp( (DATA_RAM + (uint32_t) pcmd), st, sl ) ) {
					#if RIDE_UNDO_MAX > 0
						if(memR(st)) pushUndo(st);	// store the string that is about to be removed
					#endif
					arLine(st, NULL);	// remove the old content of the current line
					arLine(st, pcmd);	// add new content in its place
				}
				line++;
			}
			else if(memR(st)) {
				printf("\r\n");
				line++;
			}
			continue;
		}

		else skip(&pcmd, 1);	// skip the leading '.' character in a command string

		// single dot only - jump to the last line
		if(*pcmd == 0) {
			gotoLine(0);
			continue;
		}

		// main command loop including the repeat counter
		while(repc--) {

			// execute dot-commands
			while(*pcmd) {
				skip(&pcmd, 0);

				// help and info
				if(*pcmd == '?') {
					skip(&pcmd, 1);
					printf("\r\n");
					printf(">>> '.' in the beginning of a line marks command line with one or more commands\r\n");
					printf(".[J] line Jump to line number (0 = last line). The 'J' character can be omitted\r\n");
					printf(".[J] < or >    Jump to the previous/next line. The 'J' character can be omitted\r\n");
					printf(".         Jump to the last line in the text\r\n");
					printf(".* [num]  Repeat the commands following this one number of times\r\n");
					printf(".L [num]  If [num] if given, list N lines back from current, or list from start\r\n");
					printf(".= [num]  Set terminal width in characters for the ruler. Value 0 disables it\r\n");
					if(buffer) {
						printf(".S [line] Set source line for copy and move operations\r\n");
						printf(".C [num]  Copy number of lines from source to current. Assuming default num=1\r\n");
						printf(".M [num]  Move number of lines from source to current. Assuming default num=1\r\n");
					}
					printf(".I [num]  Insert number of blank lines. Assuming one line if not specified\r\n");
					printf(".D [num]  Delete number of lines. Assuming one line if not specified\r\n");
					if(strF) printf(".F [`text`]  Find text. Following operations do not require parameter\r\n");
					if(strF && strR && buffer) {
						printf(".R [`text`]  Replace text found by .F operation. Next .R can skip parameter\r\n");
					}

					#if defined(RIDE_USE_FILES) && defined(INCLUDE_FILES)
						printf("./ cmd   Execute command of the File Operating System\r\n");
						if(ff && buffer) printf(".W `file` Write to text file           .O `file` Open text file\r\n");
					#endif

					#if RIDE_UNDO_MAX > 0
						printf(".U  Undo overwritten or deleted line   ");
					#endif
					printf(".Z  Clear the terminal (128 blank lines)\r\n");

					printf(".X  Exit RIDE                          .?  Help and status (this screen)\r\n");
					printf("._  Clear all text (Be careful!! There is no warning or required confirmation!)\r\n");
					printf( "\r\nText: %6lu bytes (%lu lines)\r\nFree: %6lu bytes\r\n",
								strLen(text), countLines(text), (SYSRAM_SIZE - strLen(text) - 1) );

					#if RIDE_UNDO_MAX > 0
						uint8_t t;
						for(t = undop; t; t--) printf("%2i: `%s`\r\n", (t - undop), undos[t-1]);
					#endif
				}

				// exit
				else if( (*pcmd == 'X') || (*pcmd == 'x') ) {
					qflag = 1;
					repc = 0;
					break;
				}

				// clear the terminal
				else if( (*pcmd == 'Z') || (*pcmd == 'z') ) {
					skip(&pcmd, 1);
					uint8_t t = 128;
					while(t--) printf("\n");
				}

				// clear the text
				else if(*pcmd == '_') {
					skip(&pcmd, 1);
					uint8_t t = 128;
					while(t--) printf("\n");
					memW(text, 0);
					*strF = 0;
					*strR = 0;
					line = 0;
				}

				// set terminal width
				else if(*pcmd == '=') {
					skip(&pcmd, 1);
					if( (*pcmd >= '0') && (*pcmd <= '9') ) Hsize = getNumber(&pcmd);
					else {
						if(Hsize) printf("\r\n>>> Terminal width is set to %u characters\r\n", Hsize);
						else printf("\r\n>>> Screen ruler is disabled\r\n");
					}
				}

				// set repeat counter
				else if(*pcmd == '*') {
					skip(&pcmd, 1);
					repc = getNumber(&pcmd);
					if(repc) repc--;	// the first iteration will be executed anyway
					rpcmd = pcmd;
				}

				// jump to line
				else if( (*pcmd == 'J') || (*pcmd == 'j') || (*pcmd == '<') || (*pcmd == '>') ||
						( (*pcmd >= '0') && (*pcmd <= '9') ) ) {
					if( (*pcmd == 'J') || (*pcmd == 'j') ) skip(&pcmd, 1);
					uint32_t n = (line+1);
					if(*pcmd == '>') {		// jump to next line
						skip(&pcmd, 1);
						n++;
					}
					else if(*pcmd == '<') {	// jump to previous line
						skip(&pcmd, 1);
						if(n > 1) n--;
					}
					else n = getNumber(&pcmd);	// jump to specified line
					st = gotoLine(n);
				}

				#if RIDE_UNDO_MAX > 0
					// undo
					else if( (*pcmd == 'U') || (*pcmd == 'u') ) {
						skip(&pcmd, 1);
						char *sundo = popUndo();
						if(sundo) {
							st = gotoLine(undol[undop]);		// go to the original undo line
							arLine(st, NULL);	// remove the line
							arLine(st, sundo);	// replace it with the one from the undo stack
							xfree((uint8_t **) &undos[undop]);	// free the undo stack element
						}
					}
				#endif

				// list
				else if( (*pcmd == 'L') || (*pcmd == 'l') ) {
					skip(&pcmd, 1);
					uint32_t n, sl = (line+1);
					if( (*pcmd >= '0') && (*pcmd <= '9') ) {
						n = getNumber(&pcmd);
						if(sl >= n) sl -= n;
						else {
							n = sl+1;
							sl = 1;
						}
					}
					else {
						n = sl+1;
						sl = 1;
					}
					printf("\r\n");
					st = gotoLine(sl);
					while(memR(st) && n--) {
						char k = 0;
						while(kbhit() || (k == ' ')) {
							k = getch();
							while(kbhit()) getch();
							if(k == KEY_BREAK) break;
						}
						if(k == KEY_BREAK) {
							printf("\r\n^cancel\r\n");
							break;
						}
						st = printLine(sl++ , st);
						printf("\r\n");
					}

					// print the ruler
					if(Hsize) {
						printf("\r\n     ");	// this is covering the "%3lu: " format in printLine()
						uint16_t r = 5;			// 5 characters have been printed on the screen already
						uint32_t t = (line+1) / 1000;
						while(t) {	// compensate for line numbers longer than 3 characters
							printf(" ");
							r++;
							t /= 10;
						}
						t = Hsize - r;
						for(r = 1; r < t; r++) {
							if(r % 10) {
								if(r % 5) printf(".");
								else printf(":");
							}
							else printf( "%u", ((r / 10) % 10) );
						}
						printf("\r\n");
					}
				}

				// insert lines
				else if( (*pcmd == 'I') || (*pcmd == 'i') ) {
					skip(&pcmd, 1);
					uint32_t n = 1;
					if( (*pcmd >= '0') && (*pcmd <= '9') ) n = getNumber(&pcmd);
					while(n--) arLine(st, "");	// insert blank lines
				}

				// delete lines
				else if( (*pcmd == 'D') || (*pcmd == 'd') ) {
					skip(&pcmd, 1);
					uint32_t n = 1;
					if( (*pcmd >= '0') && (*pcmd <= '9') ) n = getNumber(&pcmd);
					while(n--) {	// delete lines
						#if RIDE_UNDO_MAX > 0
							if(memR(st)) pushUndo(st);	// store the string that is about to be removed
						#endif
						arLine(st, NULL);
					}
				}

				// set source line
				else if( buffer && ( (*pcmd == 'S') || (*pcmd == 's') ) ) {
					skip(&pcmd, 1);
					srcl = line + 1;
					if( (*pcmd >= '0') && (*pcmd <= '9') ) srcl = getNumber(&pcmd);
				}

				// copy or move lines
				else if( buffer && ( (*pcmd == 'C') || (*pcmd == 'c') || (*pcmd == 'M') || (*pcmd == 'm') ) ) {
					char opr = *pcmd | 0x20;	// store the operation ('C' or 'M') as lowercase character
					skip(&pcmd, 1);
					uint32_t n = 1;
					if( (*pcmd >= '0') && (*pcmd <= '9') ) n = getNumber(&pcmd);
					if(srcl >= 1) {
						uint32_t cline = line;
						uint32_t pline = line;
						uint32_t pn = 0;
						if( (opr == 'm') && (srcl <= pline) ) pline += n;	// correction when moving forward
						while( (pn < n) && (srcl <= countLines(text)) ) {
							uint32_t ss = gotoLine(srcl);
							uint32_t sl = strNLen(ss, CONSOLE_LINE_LEN);
							memMove( (DATA_RAM + (uint32_t) buffer), ss, sl );
							*(buffer+sl) = 0;
							if(opr == 'm') {
								arLine(ss, NULL);
								if(srcl <= pline) st = gotoLine((--pline) + 1);
								else srcl++;
							}
							arLine(st, buffer);
							if(opr == 'c') {
								if(srcl >= pline) srcl++;
								if(srcl == cline) srcl += (pn+1);
								srcl++;
							}
							st = gotoLine((++pline) + 1);
							pn++;
						}
						st = gotoLine(cline+1);
					}
					else {
						printf("\r\n>>> missing source line in copy or move operation\r\n");
						repc = 0;
						break;
					}
				}

				// find or replace
				else if( ( strF && ( (*pcmd == 'F') || (*pcmd == 'f') ) ) ||
						 ( strF && strR && buffer && ( (*pcmd == 'R') || (*pcmd == 'r') ) ) ) {
					char opr = *pcmd | 0x20;	// store the operation ('F' or 'R') as lowercase character
					char *tt = ((opr == 'r') ? strR : strF);
					skip(&pcmd, 1);
					if( (*tt == 0) && (*pcmd != '`') ) {	// if the function parameter is empty, require a parameter
						printf("\r\n>>> Expecting opening ` character\r\n");
						repc = 0;
						break;
					}
					if(*pcmd == '`') {	// get the parameter into (*strF) or (*strR)
						pcmd++;	// skip the opening '`' character (only!)
						char *s = tt;
						*tt = 0;
						while( *pcmd && (*pcmd != '`') ) {
							*(s++) = *(pcmd++);
							*s = 0;
						}
						if(*pcmd != '`') {
							printf("\r\n>>> Expecting closing ` character\r\n");
							repc = 0;
							break;
						}
						skip(&pcmd, 1);	// skip the closing '`' character and other useless stuff after it
					}
					else if(opr == 'f') {	// without a parameter just move to the next line (for Find only)
						st = gotoLine(line+2);
					}

					// a find operation is executed always
					while(memR(st)) {
						char *cb = strF;
						uint32_t ct = st;
						while(memR(ct) && memR(ct) != '\n') {
							if(memR(ct) != *cb) cb = strF;
							else if(*(++cb) == 0) break;	// matching text
							ct++;
						}
						if(*cb == 0) break;
						else st = gotoLine(line+2);	// go to the next line
					}

					// additional stuff for Replace function
					if( (opr == 'r') && (memR(st) != 0) && (*strF != 0) ) {
						uint32_t sl = strNLen(st, CONSOLE_LINE_LEN);
						memMove( (DATA_RAM + (uint32_t) buffer), st, sl );	// get the text line into buffer[]
						*(buffer+sl) = 0;
						char *ch = strstr(buffer, strF);
						if(ch) {
							arLine(st, NULL);	// remove the line from the text (will add it again later)
							memmove( ch, (ch + strlen(strF)), (strlen(buffer) - strlen(strF) + 1) );
							sl = strlen(strR);
							if((strlen(buffer) + sl) >= CONSOLE_LINE_LEN) sl = CONSOLE_LINE_LEN - strlen(buffer);
							memmove( (ch + sl), ch, (sl + 1) );
							memcpy(ch, strR, sl);
							arLine(st, buffer);	// insert the modified new line back in its original place
						}
					}
				}

				#if defined(RIDE_USE_FILES) && defined(INCLUDE_FILES)

				// write to file or open file
				else if( ff && buffer &&
						( (*pcmd == 'W') || (*pcmd == 'w') || (*pcmd == 'O') || (*pcmd == 'o') ) ) {
					char opr = *pcmd | 0x20;	// store the operation ('W' or 'O') as lowercase character
					skip(&pcmd, 1);
					if(*pcmd != '`') {
						printf("\r\n>>> Expecting opening ` character\r\n");
						repc = 0;
						break;
					}
					pcmd++;	// skip the opening '`' character (only!)
					char *s = buffer;
					while( *pcmd && (*pcmd != '`') ) {
						*(s++) = *(pcmd++);
						*s = 0;
					}
					if(*pcmd != '`') {
						printf("\r\n>>> Expecting closing ` character\r\n");
						repc = 0;
						break;
					}
					skip(&pcmd, 1);	// skip the closing '`' character and other useless stuff after it
					FRESULT fr = FR_OK;
					uint32_t rw = 0;
					if( buffer[0] && buffer[1] && buffer[2] && (buffer[3] == ':') ) {
						if(fr == FR_OK) fr = f_chdrive(buffer);
						if(fr == FR_OK) fr = f_mount(&FF, "", 1);
					}

					// write file
					if(opr == 'w') {
						if(fr == FR_OK) fr = f_open(ff, buffer, (FA_WRITE | FA_CREATE_ALWAYS));
						if(fr == FR_OK) {
							uint32_t a = text;
							uint32_t wlen = (strLen(text) + 1);
							uint32_t rw1 = CONSOLE_LINE_LEN;
							while( wlen && rw1 && (fr == FR_OK) ) {
								if(wlen >= CONSOLE_LINE_LEN) rw1 = CONSOLE_LINE_LEN;
								else rw1 = wlen;
								memMove( (DATA_RAM + (uint32_t) buffer), a, rw1 );
								fr = f_write(ff, buffer, rw1, (UINT *) &rw1);
								a += rw1; rw += rw1;
								wlen -= rw1;
							}
						}
						printf("\r\n>>> %lu bytes written\r\n", (unsigned long) rw);
					}

					// open file
					else {
						if(fr == FR_OK) fr = f_open(ff, buffer, (FA_READ | FA_OPEN_EXISTING));
						if(fr == FR_OK) {
							line = (uint32_t) - 1;
							uint32_t a = text;
							while(!f_eof(ff) && (f_error(ff) == FR_OK)) {
								memset(buffer, 0, (CONSOLE_LINE_LEN+1));
								f_gets( (TCHAR *) buffer, CONSOLE_LINE_LEN, ff );
								if(*buffer) {
									uint32_t rw1 = (uint32_t) strlen(buffer);
									memMove( a, (DATA_RAM + (uint32_t) buffer), (rw1+1) );
									printf("\r\n"); printLine(line+2, a);	// print the currently loaded line
									a += rw1; rw += rw1;
								}
							}
							memW(a, 0);	// ensure that the text is terminated
							printf("\r\n\n>>> %lu bytes read\r\n", (unsigned long) rw);
						}
					}

					f_close(ff);
					st = gotoLine(0);	// jump to the last line
					if(fr != FR_OK) fopsError(fr);
				}

				// execute FOS command (only one is allowed per command line)
				else if(*pcmd == '/') {
					skip(&pcmd, 1);
					fopsMain((uint8_t *) pcmd, 0);
					repc = 0;
					break;
				}

				#endif

				// unknown command
				else {
					printf("\r\n>>> `%c` is unknown command\r\n", *pcmd);
					repc = 0;
					break;
				}

			}

			// if the repeat counter is set - repeat the command line
			if(repc && rpcmd) pcmd = rpcmd;
			else repc = 0;

		}
		printf("\r\n");

	}

	printf("\r\n");

	#if RIDE_UNDO_MAX > 0
		for(qflag = 0; qflag < RIDE_UNDO_MAX; qflag++) xfree((uint8_t **) &undos[qflag]);
	#endif

	#if defined(RIDE_USE_FILES) && defined(INCLUDE_FILES)
		xfree((uint8_t **) &ff);
	#endif

	xfree((uint8_t **) &strR);
	xfree((uint8_t **) &strF);
	xfree((uint8_t **) &buffer);

}

#endif
