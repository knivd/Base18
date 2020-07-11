#include "console.h"
#include "hardware.h"

#ifdef SANDBOX
	#include <time.h>	// needed for organising delays
#endif

// the console buffer is where lines from the console come (do not use directly)
uint8_t console_buffer[CONSOLE_LOG_ENTRIES][CONSOLE_LINE_LEN + 3];

// console log index
uint8_t clog_index = 0;

// convenient pointer to the actual text in the console buffer
uint8_t *cLine = &console_buffer[0][2];


// main console function
// reads line from the console and returns it as ASCIIZ in the console[] buffer
void getLine(char *plstr) {
	memset(console_buffer[clog_index], 0, (CONSOLE_LINE_LEN + 3));
	if(plstr && *plstr) {
		uint32_t cnt = 0;
		uint8_t *s = (uint8_t *) plstr;
		uint8_t *d = &console_buffer[clog_index][2];
		while( *s && (*s != '\n') && (cnt < CONSOLE_LINE_LEN) ) {
			if(*s >= ' ') {
				*(d++) = *(s++);
				cnt++;
			}
			else if(*s == '\t') {
				s++;
				uint8_t tl = strlen(TAB);
				while( tl-- && (cnt < CONSOLE_LINE_LEN) ) {
					*(d++) = ' ';
					cnt++;
				}
			}
			else s++;
		}
	}
	console_buffer[clog_index][0] = CONSOLE_LINE_LEN;
	cLine = (uint8_t *) cgets((char *) console_buffer[clog_index]);
}


#ifndef SANDBOX

void putch(char ch) {
	while(TXSTA2bits.TRMT == 0) continue;	// wait until the transmit register is empty
	TXREG2 = (uint8_t)ch;
}


char getch(void) {
	while(console_rxi_index == console_rxo_index) continue;	// wait for character in the console buffer
	uint8_t ch = console_rx_buffer[console_rxo_index];
	if (++console_rxo_index >= CON_RX_BUFFER_SIZE) console_rxo_index = 0;
	return (char)ch;
}


char getche(void) {
	char ch = getch();
	putch(ch);
	return ch;
}


void ungetch(char ch) {
	if(ch) {
		console_rx_buffer[console_rxi_index] = ch;
		if (++console_rxi_index >= CON_RX_BUFFER_SIZE) console_rxi_index = 0;
		if(console_rxi_index == console_rxo_index) {	// discard the oldest character if the buffer is full
			if (++console_rxo_index >= CON_RX_BUFFER_SIZE) console_rxo_index = 0;
		}
	}
}


__bit kbhit(void) {
	return ((console_rxi_index == console_rxo_index)? 0 : 1);
}

#endif


char peekch(void) {
	if(console_rxi_index == console_rxo_index) return 0;
	else return console_rx_buffer[console_rxo_index];
}


void cputs(const char *ch) {
	while(*ch) putch( *(ch++) );
}


char *cgets(char *buf) {

	uint8_t maxp = (uint8_t)*buf;	// buf[0] contains the maximum number of characters to be read
    if(maxp == 0) {		// if the maximum number of characters to read is set to be zero
		*(buf+1)=0;		// immediately return the number of read characters (zero)
        *(buf+2)=0;		// this is the first character of the actual string
        return (buf+2);
    }

	char *s = (buf+2);	// the actual string starts from buf[2]
	s[maxp] = 0;		// make sure the output string is always terminated in the last character of the buffer
	cputs(s);			// print the initial state of the buffer

	uint8_t cpos = strlen(s);
	uint32_t ch;
	while(cpos--) putch('\b');
	cpos = 0;

	for(;;) {

		#ifdef CONSOLE_ECHO
			ch = (uint32_t) getche();
		#else
			ch = (uint32_t) getch();
		#endif

		// check for escape sequences
		if(ch == 0) ch = KEY_ESC;
		if(ch == KEY_ESC) {
			#ifdef SANDBOX
				clock_t goal = 5 + clock();	// 5 ms delay
				while (goal > clock());
			#else
				uint32_t stmp = systick;
				while(systick == stmp) continue;
			#endif
			if(!kbhit()) {	// just Esc
				if(s[0]) {
					putch('\\');
					s[0] = 0;
					ch = KEY_ENTER;
				}
				else continue;
			}
			else {	// an Escape sequence
				uint8_t cnt = 1;
				while(kbhit() && (cnt < 4)) {
					ch = (ch << 8) + (uint32_t) getch();
					cnt++;
				}
			}
		}

		// [Enter]
		if( (ch == KEY_ENTER) || (ch == KEY_NLINE) ) {
			if(cpos) {	// line cutting will be performed only if the cursor if not at first position
				while( (cpos < maxp) && s[cpos] ) {	// clear all characters to the end of the line
					s[cpos++] = 0;
					putch(' ');
				}
			}
			putch('\r');
			if(s[0]) putch('\n');
            break;
        }

		// [BckSpc] or [Del]
		else if( (ch == KEY_BACKSPC) || (ch == ALT_BACKSPC) || (ch == KEY_DEL) || (ch == ALT_DEL) ) {

			if( (cpos == 0) && ( (ch == KEY_BACKSPC) || (ch == ALT_BACKSPC) ) ) continue;
			uint8_t z, tabl = 1;

			// [BackSpc] key
			if( (ch == KEY_BACKSPC) || (ch == ALT_BACKSPC) ) {
				if( (cpos > 2) && !strncmp( &s[cpos-strlen(TAB)], TAB, strlen(TAB) ) ) tabl = strlen(TAB);
				z = tabl;
				while(z--) {
					putch('\b');
					cpos--;
				}
			}

			// [Del] key
			else {
				if( !strncmp( &s[cpos], TAB, strlen(TAB) ) ) tabl = strlen(TAB);
			}

			if( (strlen(s) + tabl) >= maxp ) tabl = maxp - strlen(s);
			memmove( &s[cpos], &s[cpos+tabl], (strlen(s)-cpos-tabl+1) );
			cputs(&s[cpos]);
			z = tabl;
			while(z--) putch(' ');
			tabl += strlen(s);
			while((tabl--) > cpos) putch('\b');

		}

		// left arrow
		else if( (ch == KEY_LEFT) || (ch == ALT_LEFT) ) {
			if(cpos) {
				putch('\b');
				cpos--;
			}
		}

		// right arrow
		else if( (ch == KEY_RIGHT) || (ch == ALT_RIGHT) ) {
			if(cpos < strlen(s)) putch(s[cpos++]);
		}

		// [Home] key will move the cursor to the beginning of the line
		else if( (ch == KEY_HOME) || (ch == ALT_HOME) ) {
			while(cpos--) putch('\b');
			cpos = 0;
		}

		// [End] key will move the cursor to the end of the line
		else if( (ch == KEY_END) || (ch == ALT_END) ) {
			while(cpos--) putch('\b');
			cputs(s);
			cpos = strlen(s);
		}

		// console log
		#if CONSOLE_LOG_ENTRIES > 1

		else if( (ch == KEY_F3) || (ch == ALT_F3) || (ch == KEY_LOG) ) {

			uint8_t counter = CONSOLE_LOG_ENTRIES;
			uint8_t allow_empty = 1;	// this flag will ensure only one empty line is allowed

			while(counter--) {

				// execute HOME key, clear the characters from the console, and then HOME again
				while(cpos--) putch('\b');
				cpos = 0;
				while(cpos++ < strlen(s))  putch(' ');
				cpos--;
				while(cpos--) putch('\b');
				cpos = 0;

				if(*s == 0) allow_empty = 0;
				if(--clog_index >= CONSOLE_LOG_ENTRIES) clog_index = (CONSOLE_LOG_ENTRIES - 1);

				if(console_buffer[clog_index][2] || allow_empty) {
					s = (char *) &console_buffer[clog_index][2];
					cputs(s);
					cpos = strlen(s);
					break;
				}

			}

		}

		#endif

		// ASCII characters and [Tab] key
		else if( ( (ch >= ' ') && (ch < 0x100) ) || (ch == KEY_TAB) ) {

            uint8_t chl = 1;

			// [Tab] key will produce strlen(TAB) number of space characters inserted into the buffer
            if(ch == KEY_TAB) {
                ch = ' ';
                chl = strlen(TAB);
            }

			// alarm when the buffer limit has been reached
			if( (strlen(s)+chl) >= maxp ) {
				chl = maxp-strlen(s);
				putch('\a');
			}

			// insert into the buffer
			if(chl) {
				memmove( &s[cpos+chl], &s[cpos], (strlen(s)-cpos+1) );
				memset( &s[cpos], (char)ch, chl );
				cputs(&s[cpos]);
				cpos += chl;
				chl = strlen(s);
				while((chl--) > cpos) putch('\b');
			}

        }

	}

	s[maxp] = 0;			// make sure the output string is always terminated in the last character of the buffer
	*(buf+1) = strlen(s);	// the actual length of the read string is returned in buf[1]

	// store into the console log
	if(*s) {
		if(++clog_index >= CONSOLE_LOG_ENTRIES) clog_index = 0;
	}

	return s;

}
