#include "hardware.h"
#include "monitor.h"
#include "console.h"
#include "hardware.h"	// for the memX() functions

#ifdef INCLUDE_MONITOR

uint32_t addr = 0
				#ifdef SANDBOX
					+ (uint32_t) sandbox;	// address pointer
				#endif
				;
uint32_t aend = 0;	// end of area address
uint32_t anew = 0;	// new location address

// named PIC18 registers in the area 0xF00 - 0xFFF
const char * const PIC18reg[0X100] = {
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "",
	"", "", "", "", "", "","PMD3", "PMD2",
	"PMD1", "PMD0", "PSTR3CON", "PSTR2CON", "TXREG2", "RCREG2", "SPBRG2", "SPBRGH2",
	"BAUDCON2", "TXSTA2", "RCSTA2", "ANCON2", "ANCON1", "ANCON0", "MEMCON", "ODCON3",
	"ODCON2", "ODCON1", "REFOCON", "CCPTMRS2", "CCPTMRS1", "CCPTMRS0", "CM3CON", "CM2CON",
	"T12CON", "PR12", "TMR12", "T10CON", "PR10", "TMR10", "T8CON", "PR8",
	"TMR8", "T6CON", "PR6", "TMR6", "T7GCON", "T7CON", "TMR7L", "TMR7H",
	"CCP10CON", "CCPR10L", "CCPR10H", "CCP9CON", "CCPR9L", "CCPR9H", "CCP8CON", "CCPR8L",
	"CCPR8H", "CCP3CON", "CCPR3L", "CCPR3H", "ECCP3DEL", "ECCP3AS", "CCP2CON", "CCPR2L",
	"CCPR2H", "ECCP2DEL", "ECCP2AS", "PADCFG1", "CM1CON", "CTMUICONH", "CTMUCONL", "CTMUCONH",
	"ALRMVALL", "ALRMVALH", "ALRMRPT", "ALRMCFG", "RTCVALL", "RTCVALH", "RTCCAL", "RTCCFG",
	"PIE6", "EEDATA", "EEADR", "EEADRH", "OSCCON2", "BAUDCON1", "SSP2CON2", "SSP2CON1",
	"SSP2STAT", "SSP2ADD", "SSP2BUF", "T4CON", "PR4", "TMR4", "CCP7CON", "CCPR7L",
	"CCPR7H", "CCP6CON", "CCPR6L", "CCPR6H", "CCP5CON", "CCPR5L", "CCPR5H", "CCP4CON",
	"CCPR4L", "CCPR4H", "T5GCON", "T5CON", "TMR5L", "TMR5H", "EECON2", "EECON1",
	"PORTA", "PORTB", "PORTC", "PORTD", "PORTE", "PORTF", "PORTG", "PORTH",
	"PORTJ", "LATA", "LATB", "LATC", "LATD", "LATE", "LATF", "LATG",
	"LATH", "LATJ", "TRISA", "TRISB", "TRISC", "TRISD", "TRISE", "TRISF",
	"TRISG", "TRISH", "TRISJ", "OSCTUNE", "PSTR1CON", "PIE1", "PIR1", "IPR1",
	"PIE2", "PIR2", "IPR2", "PIE3", "PIR3", "IPR3", "PIR6", "PSPCON",
	"HLVDCON", "IPR6", "T1GCON", "RCSTA1", "TXSTA1", "TXREG1", "RCREG1", "SPBRG1",
	"T3GCON", "T3CON", "TMR3L", "TMR3H", "CMSTAT", "CVRCON", "PIE4", "PIR4",
	"IPR4", "PIE5", "PIR5", "CCP1CON", "CCPR1L", "CCPR1H", "ECCP1DEL", "ECCP1AS",
	"ADCON2", "ADCON1", "ADCON0", "ADRESL", "ADRESH", "SSP1CON2", "SSP1CON1", "SSP1STAT",
	"SSP1ADD", "SSP1BUF", "T2CON", "PR2", "TMR2", "T1CON", "TMR1L", "TMR1H",
	"RCON", "WDTCON", "IPR5", "OSCCON", "SPBRGH1", "T0CON", "TMR0L", "TMR0H",
	"STATUS", "FSR2L", "FSR2H", "PLUSW2", "PREINC2", "POSTDEC2", "POSTINC2", "INDF2",
	"BSR", "FSR1L", "FSR1H", "PLUSW1", "PREINC1", "POSTDEC1", "POSTINC1", "INDF1",
	"WREG", "FSR0L", "FSR0H", "PLUSW0", "PREINC0", "POSTDEC0", "POSTINC0", "INDF0",
	"INTCON3", "INTCON2", "INTCON", "PRODL", "PRODH", "TABLAT", "TBLPTRL", "TBLPTRH",
	"TBLPTRU", "PCL", "PCLATH", "PCLATU", "STKPTR", "TOSL", "TOSH", "TOSU"
};


const char *reg18(uint16_t addr) {
	if(addr >= 0xF00) return PIC18reg[addr-0xF00];
	else return (const char *) 0;
}


void fReg18a(const char *s, uint16_t r) {
	printf("%-7s   $%02X", s, (int) (r & 0xFF));
	if((r & 0x0100) == 0) {
		if((r & 0xFF) >= 0x80) printf(" (%s)", reg18(0xF00+(r & 0xFF)));
		printf(", a");
	}
	else printf(", BSR");
}


void fReg18da(const char *s, uint16_t r) {
	printf("%-7s   $%02X", s, (int) (r & 0xFF));
	if(((r & 0x0100) == 0) && ((r & 0xFF) >= 0x80)) printf(" (%s)", reg18(0xF00+(r & 0xFF)));
	if(r & 0x0200) printf(", f, "); else printf(", W, ");
	if(r & 0x0100) printf("BSR"); else printf("a");
}


void fReg18ba(const char *s, uint16_t r) {
	printf("%-7s   $%02X", s, (int) (r & 0xFF));
	uint8_t f = (r & 0xFF);
	uint8_t b = ((r >> 9) & 7);
	if(((r & 0x0100) == 0) && (f >= 0x80)) printf(" (%s)", reg18(0xF00+f));
	printf(", %1d", (int) b);
	if(((r & 0x0100) == 0) && (f == 0xD8)) {	// handling individual bits in the STATUS register
		if(b == 0) printf(" (.C)");
		else if(b == 1) printf(" (.DC)");
		else if(b == 2) printf(" (.Z)");
		else if(b == 3) printf(" (.OV)");
		else if(b == 4) printf(" (.N)");
	}
	if(r & 0x0100) printf(", BSR"); else printf(", a");
}


uint32_t relAddr18(uint32_t addr, uint16_t rel) {
	if(rel >= 0x80) addr -= ((0x100-rel) << 1);
	else addr += (rel << 1);
	return (addr & 0x1FFFFF);
}


uint16_t disW2(uint32_t *addr) {
	uint16_t w2 = memR(*addr) + ((uint16_t) memR((*addr)+1) << 8);
	*addr += 2;
	printf("\b\b\b\b\b\b\b\b%02X%02X    ", (int) (w2 >> 8), (int) (w2 & 0xFF));
	return w2;
}


// disassemble and print a single PIC18 instruction and modify the address to point after it
void disAsm18(uint32_t *addr) {
	*addr &= ~1;	// instructions can only start at 16-bit address boundaries
	uint16_t w1 = memR(*addr) + ((uint16_t) memR((*addr)+1) << 8);
	printf("%06lX:  %02X%02X          ", (*addr), (int) (w1 >> 8), (int) (w1 & 0xFF));
	*addr += 2;
	uint8_t b8 = w1 >> 8;	// instruction code pattern

	// ADDWF
	if((b8 & 0xFC) == 0x24) fReg18da("ADDWF", w1);

	// ADDWFC
	else if((b8 & 0xFC) == 0x20) fReg18da("ADDWFC", w1);

	// ANDWF
	else if((b8 & 0xFC) == 0x14) fReg18da("ANDWF", w1);

	// CLRF
	else if((b8 & 0xFE) == 0x6A) fReg18a("CLRF", w1);

	// COMF
	else if((b8 & 0xFC) == 0x1C) fReg18da("COMF", w1);

	// CPFSEQ
	else if((b8 & 0xFE) == 0x62) fReg18a("CPFSEQ", w1);

	// CPFSGT
	else if((b8 & 0xFE) == 0x64) fReg18a("CPFSGT", w1);

	// CPFSLT
	else if((b8 & 0xFE) == 0x60) fReg18a("CPFSLT", w1);

	// DECF
	else if((b8 & 0xFC) == 0x04) fReg18da("DECF", w1);

	// DECFSZ
	else if((b8 & 0xFC) == 0x2C) fReg18da("DECFSZ", w1);

	// DCFSNZ
	else if((b8 & 0xFC) == 0x4C) fReg18da("DCFSNZ", w1);

	// INCF
	else if((b8 & 0xFC) == 0x28) fReg18da("INCF", w1);

	// INCFSZ
	else if((b8 & 0xFC) == 0x3C) fReg18da("INCFSZ", w1);

	// INFSNZ
	else if((b8 & 0xFC) == 0x48) fReg18da("INFSNZ", w1);

	// IORWF
	else if((b8 & 0xFC) == 0x10) fReg18da("IORWF", w1);

	// MOVF
	else if((b8 & 0xFC) == 0x50) fReg18da("MOVF", w1);

	// MOVFF
	else if((b8 & 0xF0) == 0xC0) {
		uint16_t w2 = disW2(addr);
		printf("%-7s   $%04X", "MOVFF", (int) (w1 & 0xFFF));
		if(reg18(w1 & 0xFFF)) printf(" (%s)", (reg18(w1 & 0xFFF)));
		printf(", $%04X", (int) (w2 & 0xFFF));
		if(reg18(w2 & 0xFFF)) printf(" (%s)", (reg18(w2 & 0xFFF)));
	}

	// MOVWF
	else if((b8 & 0xFE) == 0x6E) fReg18a("MOVWF", w1);

	// MULWF
	else if((b8 & 0xFE) == 0x02) fReg18a("MULWF", w1);

	// NEGF
	else if((b8 & 0xFE) == 0x6C) fReg18a("NEGF", w1);

	// RLCF
	else if((b8 & 0xFC) == 0x34) fReg18da("RLCF", w1);

	// RLNCF
	else if((b8 & 0xFC) == 0x44) fReg18da("RLNCF", w1);

	// RRCF
	else if((b8 & 0xFC) == 0x30) fReg18da("RRCF", w1);

	// RRNCF
	else if((b8 & 0xFC) == 0x40) fReg18da("RRNCF", w1);

	// SETF
	else if((b8 & 0xFE) == 0x68) fReg18a("SETF", w1);

	// SUBFWB
	else if((b8 & 0xFC) == 0x54) fReg18da("SUBFWB", w1);

	// SUBWF
	else if((b8 & 0xFC) == 0x5C) fReg18da("SUBWF", w1);

	// SUBWFB
	else if((b8 & 0xFC) == 0x58) fReg18da("SUBWFB", w1);

	// SWAPF
	else if((b8 & 0xFC) == 0x38) fReg18da("SWAPF", w1);

	// TSTFSZ
	else if((b8 & 0xFC) == 0x66) fReg18a("TSTFSZ", w1);

	// XORWF
	else if((b8 & 0xFC) == 0x18) fReg18da("XORWF", w1);

	// BCF
	else if((b8 & 0xF0) == 0x90) fReg18ba("BCF", w1);

	// BSF
	else if((b8 & 0xF0) == 0x80) fReg18ba("BSF", w1);

	// BTFSC
	else if((b8 & 0xF0) == 0xB0) fReg18ba("BTFSC", w1);

	// BTFSS
	else if((b8 & 0xF0) == 0xA0) fReg18ba("BTFSS", w1);

	// BTG
	else if((b8 & 0xF0) == 0x70) fReg18ba("BTG", w1);

	// BC
	else if(b8 == 0xE2) printf("%-7s   %06lX", "BC", relAddr18((*addr), (w1 & 0xFF)));

	// BN
	else if(b8 == 0xE6) printf("%-7s   %06lX", "BN", relAddr18((*addr), (w1 & 0xFF)));

	// BNC
	else if(b8 == 0xE3) printf("%-7s   %06lX", "BNC", relAddr18((*addr), (w1 & 0xFF)));

	// BNN
	else if(b8 == 0xE7) printf("%-7s   %06lX", "BNN", relAddr18((*addr), (w1 & 0xFF)));

	// BNOV
	else if(b8 == 0xE5) printf("%-7s   %06lX", "BNOV", relAddr18((*addr), (w1 & 0xFF)));

	// BNZ
	else if(b8 == 0xE1) printf("%-7s   %06lX", "BNZ", relAddr18((*addr), (w1 & 0xFF)));

	// BOV
	else if(b8 == 0xE4) printf("%-7s   %06lX", "BOV", relAddr18((*addr), (w1 & 0xFF)));

	// BRA
	else if((b8 & 0xF8) == 0xD0) {
		uint16_t rel = (w1 & 0x7FF);
		uint32_t aa = *addr;
		if(rel >= 0x400) aa -= ((0x800-rel) << 1);
		else aa += (rel << 1);
		printf("%-7s   %06lX", "BRA", aa);
	}

	// BZ
	else if(b8 == 0xE0) printf("%-7s   %06lX", "BZ", relAddr18((*addr), (w1 & 0xFF)));

	// CALL
	else if((b8 & 0xFE) == 0xEC) {
		uint32_t w2 = (uint32_t) disW2(addr);
		w2 = (((w2 & 0xFFF) << 8) + (w1 & 0xFF)) << 1;
		printf("%-7s   %06lX", "CALL", w2);
		if(w1 & 0x0100) printf(", s");
	}

	// CLRWDT
	else if(w1 == 0x0004) printf("%-7s   ", "CLRWDT");

	// DAW
	else if(w1 == 0x0007) printf("%-7s   ", "DAW");

	// GOTO
	else if(b8 == 0xEF) {
		uint32_t w2 = (uint32_t) disW2(addr);
		w2 = (((w2 & 0xFFF) << 8) + (w1 & 0xFF)) << 1;
		printf("%-7s   %06lX", "GOTO", w2);
	}

	// NOP
	else if(((b8 & 0xF0) == 0xF0) || (w1 == 0x0000)) printf("%-7s   ", "NOP");

	// POP
	else if(w1 == 0x0006) printf("%-7s   ", "POP");

	// PUSH
	else if(w1 == 0x0005) printf("%-7s   ", "PUSH");

	// RCALL
	else if((b8 & 0xF8) == 0xD8) {
		uint16_t rel = (w1 & 0x7FF);
		uint32_t aa = *addr;
		if(rel >= 0x400) aa -= ((0x800-rel) << 1);
		else aa += (rel << 1);
		printf("%-7s   %06lX", "RCALL", aa);
	}

	// RESET
	else if(w1 == 0x00FF) printf("%-7s   ", "RESET");

	// RETFIE
	else if((w1 & 0xFFFE) == 0x0010) {
		printf("%-7s   ", "RETFIE");
		if(w1 & 0x0001) printf("s");
	}

	// RETURN
	else if((w1 & 0xFFFE) == 0x0012) {
		printf("%-7s   ", "RETURN");
		if(w1 & 0x0001) printf("s");
	}

	// SLEEP
	else if(w1 == 0x0003) printf("%-7s   ", "SLEEP");

	// ADDLW
	else if(b8 == 0x0F) printf("%-7s   $%02X", "ADDLW", (int) (w1 & 0xFF));

	// ANDLW
	else if(b8 == 0x0B) printf("%-7s   $%02X", "ANDLW", (int) (w1 & 0xFF));

	// IORLW
	else if(b8 == 0x09) printf("%-7s   $%02X", "IORLW", (int) (w1 & 0xFF));

	// LFSR
	else if(b8 == 0xEE) {
		uint16_t w2 = disW2(addr);
		printf("%-7s   %1d, $%04X", "LFSR", (int) ((w1 >> 4) & 3), (int) (((w2 & 0xFF) << 4) + (w1 & 0x0F)));
		if(reg18((((w2 & 0xFF) << 4) + (w1 & 0x0F)))) printf(" (%s)", reg18((((w2 & 0xFF) << 4) + (w1 & 0x0F))));
	}

	// MOVLB
	else if((w1 & 0xFFF0) == 0x0100) printf("%-7s   $%02X", "MOVLB", (int) (w1 & 0x0F));

	// MOVLW
	else if(b8 == 0x0E) printf("%-7s   $%02X", "MOVLW", (int) (w1 & 0xFF));

	// MULLW
	else if(b8 == 0x0D) printf("%-7s   $%02X", "MULLW", (int) (w1 & 0xFF));

	// RETLW
	else if(b8 == 0x0C) printf("%-7s   $%02X", "RETLW", (int) (w1 & 0xFF));

	// SUBLW
	else if(b8 == 0x08) printf("%-7s   $%02X", "SUBLW", (int) (w1 & 0xFF));

	// XORLW
	else if(b8 == 0x0A) printf("%-7s   $%02X", "XORLW", (int) (w1 & 0xFF));

	// TBLRD*
	else if(w1 == 0x0008) printf("%-7s   ", "TBLRD*");

	// TBLRD*+
	else if(w1 == 0x0009) printf("%-7s   ", "TBLRD*+");

	// TBLRD*-
	else if(w1 == 0x000A) printf("%-7s   ", "TBLRD*-");

	// TBLRD+*
	else if(w1 == 0x000B) printf("%-7s   ", "TBLRD+*");

	// TBLWT*
	else if(w1 == 0x000C) printf("%-7s   ", "TBLWT*");

	// TBLWT*+
	else if(w1 == 0x000D) printf("%-7s   ", "TBLWT*+");

	// TBLWT*-
	else if(w1 == 0x000E) printf("%-7s   ", "TBLWT*-");

	// TBLWT+*
	else if(w1 == 0x000F) printf("%-7s   ", "TBLWT+*");

	// unknown
	else printf("%-7s   ", "???");

	printf("\r\n");
}


// print error
// (msgcode) error code
void monError(int8_t msgcode) {
    if(msgcode == MON_OK) return;
    switch(msgcode) {
        default: break;
		case MON_NOMEM: 	printf(">>> MONITOR ERROR %i: insufficient memory\r\n\n", msgcode); break;
		case MON_ERROR: 	printf(">>> MONITOR ERROR %i: invalid parameter\r\n\n", msgcode); break;
		case MON_UNKN_CMD:	printf(">>> MONITOR ERROR %i: unknown command\r\n\n", msgcode); break;
    }
}


// get hex number from the command string and modify the command string pointer
// A valid HEX number is considered one with 1...8 hex characters '0'..'9' and ('a'..'f' or 'A'..'F')
// If the function is unable to get a valid hex number, it will return the command string pointer unchanged
// and will return value 0
// If the command string contains a valid hex number that is longer than 8 characters, only its last 8
// characters will be returned
// The number of bytes needed for the representation of the number is returned in (*bcnt)
//		if a pointer to it has been provided
uint32_t getHex(char **cmd, uint8_t *bcnt) {
	skip(cmd, 0);
	char *s = *cmd;
	if(bcnt) *bcnt = 1;
	uint8_t digc = 0;
	uint8_t quf = 0;
	uint32_t v = 0;
	while(s && *s) {
		if(*s == '\'') {
			quf = 1-quf;
			digc = 0;
		}
		else if(quf) {
			v = *s;
			digc = 0;
			if(bcnt) *bcnt = 1;
		}
		else if( (*s >= '0') && (*s <='9') ) {
			v = (v << 4) + (*s - '0');
			if(++digc > 2) {
				digc = 0;
				if(bcnt) (*bcnt)++;
			}
		}
		else if( (*s >= 'a') && (*s <='f') ) {
			v = (v << 4) + 10 + (*s - 'a');
			if(++digc > 2) {
				digc = 0;
				if(bcnt) (*bcnt)++;
			}
		}
		else if( (*s >= 'A') && (*s <='F') ) {
			v = (v << 4) + 10 + (*s - 'A');
			if(++digc > 2) {
				digc = 0;
				if(bcnt) (*bcnt)++;
			}
		}
		else break;
		s++;
	}
	*cmd = s;
	skip(cmd, 0);
	return v;
}


// print an entire line of hex numbers
// (a) start address - will be aligned to the value of (align)
// (align) total number of bytes in a line
// (count) number of bytes in this line - must be smaller or equal to (align)
void printHexLine(uint32_t a, uint8_t align, uint8_t count) {
	uint32_t aorg = a;
	align &= ~3;	// the value of (align) can be in multiples of 4 only
	addr = (a & ~(align - 1));
	printf("%0" STR(MON_ADDR_LEN) "lX:", addr
										#ifdef SANDBOX
											- (uint32_t) sandbox
										#endif
			);
	uint8_t t;
	uint8_t cnt = 0;
	for(t = 0; t < align; t++) {
		if((addr+t) >= aorg) {
			if(cnt < count) {
				// printf( " %02hhX", (uint8_t) *((uint8_t *) (addr+t)) );
				printf( " %02X", (int) memR(addr+t) );
			}
			else printf("   ");
			cnt++;
		}
		else printf("   ");
	}
	printf("    |  ");
	cnt = 0;
	for(t = 0; t < align; t++) {
		uint8_t v = (uint8_t) memR(addr+t);	// *((uint8_t *) (addr+t));
		if((addr+t) >= aorg) {
			if(cnt < count) {
				if( (v >= ' ') && (v != 0x7f) ) printf("%c", v);
				else printf(".");
				cnt++;
			}
			else printf(" ");
		}
		else printf(" ");
	}
	printf("\r\n");
}


// process command string and fill (addr), (aend), and (anew)
// The number of bytes needed for the representation of (anew) is returned in (*bcnt)
//		if a pointer to it has been provided
int8_t processHex(char **cmd, uint8_t *bcnt) {
	char *pcmd = *cmd;
	char *pp = pcmd;
	anew = aend = addr;

	if( (*pcmd != '.') && (*pcmd != '-') ) {
		pp = pcmd;
		uint32_t v = getHex(&pcmd, bcnt);
		if(pp == pcmd) return MON_OK;	// the command doesn't start with a number
		anew = v;
		#ifdef SANDBOX
			anew %= sizeof(sandbox);	// limit the address within the sandbox only
			anew += (uint32_t) sandbox;
		#endif
	}

	// new location address
	if(*pcmd == '<') {
		skip(&pcmd, 1);
		pp = pcmd;
		addr = getHex(&pcmd, NULL);	// get a new address value
		#ifdef SANDBOX
			addr %= sizeof(sandbox);	// limit the address within the sandbox only
			addr += (uint32_t) sandbox;
		#endif
		if(pp == pcmd) {		// the command doesn't start with a number
			*cmd = pcmd;
			return MON_ERROR;
		}
	}
	else addr = anew;	// the command doesn't have a (new) parameter

	aend = addr;
	pp = pcmd;

	// absolute end address
	if(*pcmd == '-') {
		skip(&pcmd, 1);
		aend = getHex(&pcmd, NULL);
		#ifdef SANDBOX
			aend %= sizeof(sandbox);	// limit the address within the sandbox only
			aend += (uint32_t) sandbox;
		#endif
		if(pp == pcmd) {	// expecting a parameter but didn't get one
			*cmd = pcmd;
			return MON_ERROR;
		}
	}

	// relative end address
	else if(*pcmd == '.') {
		skip(&pcmd, 1);
		aend = getHex(&pcmd, NULL);
		#ifdef SANDBOX
			aend = aend + (addr - (uint32_t) sandbox) - 1;
			aend %= sizeof(sandbox);	// limit the address within the sandbox only
			aend += (uint32_t)sandbox;
		#else
			aend = aend + addr - 1;
		#endif
		if(pp == pcmd) {	// expecting a parameter but didn't get one
			*cmd = pcmd;
			return MON_ERROR;
		}
	}

	*cmd = pcmd;
	return MON_OK;
}


#ifdef MON_USE_FILES

#include "xmem.h"		// for temporary memory buffers

#ifdef INCLUDE_FILES
#include "FatFs/source/ff.h"
#include "FatFs/source/diskio.h"
#include "fileops.h"	// for the FRESULT error messages
#endif

#endif


// monitor entry point
uint8_t monMain(uint8_t *cmd) {
	uint8_t interactive = (cmd ? 0 : 1);	// this flag will be 1 when there is no external command
	int8_t result = MON_OK;
	char *pcmd = (char *) cmd;

	if(interactive) {
		printf("\r\nSystem Monitor, (C) KnivD\r\n");
		#ifdef SANDBOX
			printf( "SANDBOX mode. Physical address: 0x%06lX, Length 0x%lX bytes\r\n", sandbox, sizeof(sandbox) );
		#endif
		printf("\r\n");
	}

	while( interactive || (pcmd && *pcmd) ) {
		result = MON_OK;

		// print prompt and get command line
		if( interactive && ((pcmd == NULL) || (*pcmd == 0)) ) {
			printf("%s ", "%_");	// monitor prompt
			getLine(NULL);
			cmd = cLine;
			pcmd = (char *) cmd;
		}

		// empty command string
		if(*pcmd == 0) {
			if(interactive) {	// in interactive mode show a full line of memory
				printHexLine(addr, MON_HEX_PER_LINE, MON_HEX_PER_LINE);
				addr += MON_HEX_PER_LINE;
				continue;
			}
			else break;
		}

		// exit
		if(interactive && ( (*pcmd == 'x') || (*pcmd == 'X') ) ) break;

		// command format:
		// [ newloc < ] [begin] [ .- end ] [command]

		// process the command parameters
		uint8_t newl = 0;	// (anew) length in bytes
		result = processHex(&pcmd, &newl);
		if(result != MON_OK) {
			if(interactive) {
				monError(result);
				*pcmd = 0;
				continue;
			}
			else break;
		}

		// help
		if( (*pcmd == '?') || (*pcmd == 'h') || (*pcmd == 'H') ) {
			skip(&pcmd, 1);
			printf("\r\n");
			printf("[addr] View byte at address     <Enter key>  View full line of memory\r\n");
			printf("[addr . len] View memory area   [addrB - addrE]  View memory area (abs address)\r\n");
			printf("[addr . len] L  or [addrB - addrE] L    List disassembled CPU code\r\n");
			printf("[addr] : [hex] [hex] ... [;]  Write into memory (support multi-byte and 'char')\r\n");
			printf("[addr] : \"text\"   Write ASCII string into memory until a closing double quote\r\n");
			printf("[val]  < [addr . len] I   or   [val]  < [addrB - addrE] I    Initialise area\r\n");
			printf("[addr] < [addr . len] M   or   [addr] < [addrB - addrE] M    Move area\r\n");
			printf("[addr] < [addr . len] V   or   [addr] < [addrB - addrE] V    Compare two areas\r\n");

			#if defined(MON_USE_FILES) && defined(INCLUDE_FILES)

			printf("[addr . len]   W \"fname\"  or   [addrB - addrE]   W \"fname\"   Write area as file\r\n");
			printf("[addr] [. len] R \"fname\"  or   [addrB] [- addrE] R \"fname\"   Read file to area\r\n");

			#endif

			#ifndef SANDBOX

			printf("[addr] J  Jump to address and start execution\r\n");

			#endif

			printf("? or H    This help\r\nX  Exit monitor\r\n");
			printf("\n");
		}

		// start checking the actual command letters
		else if( ( (*pcmd == 0) || (*pcmd == ';') ) && (anew == addr) ) {

			// single hex number - display memory location
			if(addr == aend) {
				printf("%0" STR(MON_ADDR_LEN) "lX:", addr
													#ifdef SANDBOX
														- (uint32_t) sandbox
													#endif
						);
				uint8_t v = (uint8_t) memR(addr); // *((uint8_t *) addr);
				printf(" %02X", (int) v);
				if( (v >= ' ') && (v != 0x7f) ) printf(" '%c'", v);
				printf("\r\n");
				addr++;
			}

			// [begin address] . <end address>
			// [begin address] - <end address>
			// memory dump
			else {
				while(addr <= aend) {
					char k = 0;
					while(kbhit() || (k == ' ')) {
						k = getch();
						while(kbhit()) getch();
						if(k == KEY_BREAK) break;
					}
					if(k == KEY_BREAK) {
						printf("^cancel\r\n");
						break;
					}
					if((addr+MON_HEX_PER_LINE) <= aend) {
						printHexLine(addr, MON_HEX_PER_LINE, MON_HEX_PER_LINE);
						addr += MON_HEX_PER_LINE;
					}
					else {
						printHexLine(addr, MON_HEX_PER_LINE, (aend - addr + 1));
						addr += (aend - addr + 1);
					}
				}
			}

		}

		// list disassembled CPU code
		else if( ( (*pcmd == 'l') || (*pcmd == 'L') ) && (anew == addr) ) {
			skip(&pcmd, 1);
			while(addr <= aend) {
				char k = 0;
				while(kbhit() || (k == ' ')) {
					k = getch();
					while(kbhit()) getch();
					if(k == KEY_BREAK) break;
				}
				if(k == KEY_BREAK) {
					printf("^cancel\r\n");
					break;
				}
				disAsm18(&addr);
			}
		}

		// enter values into memory
		else if(*pcmd == ':') {
			skip(&pcmd, 1);
			if( (anew == addr) && (aend == addr) ) {
				uint32_t written = 0;
				uint8_t quf = 0;
				while(*pcmd && ( quf || (*pcmd != ';') ) ) {
					uint8_t bcnt = 1;
					uint32_t v = 0;
					if(*pcmd == '"') {
						quf = 1-quf;
						pcmd++;
						continue;
					}
					else if(quf) {
						v = *(pcmd++);
					}
					else {
						char *pp = pcmd;
						v = getHex(&pcmd, &bcnt);
						if(pp == pcmd) break;
					}
					while(bcnt--) {
						#ifdef MON_MSB_FIRST
							// *((uint8_t *) (addr++ )) = (uint8_t) (v >> (8 * bcnt));
							memW( addr++, (uint8_t) (v >> (8 * bcnt)) );
						#else
							// *((uint8_t *) (addr++ )) = (uint8_t) v;
							memW( addr++, (uint8_t) v );
							v >>= 8;
						#endif
						written++;
					}
				}
				printf(">>> %lu bytes written\r\n\n", written);
			}
			else result = MON_ERROR;
		}

		// compare two areas
		else if( (*pcmd == 'v') || (*pcmd == 'V') ) {
			skip(&pcmd, 1);
			uint32_t a1 = anew;
			uint32_t a2 = addr;
			uint8_t v1, v2;
			uint32_t c = 0;
			while(a2 <= aend) {
				char k = 0;
				while(kbhit() || (k == ' ')) {
					k = getch();
					while(kbhit()) getch();
					if(k == KEY_BREAK) break;
				}
				if(k == KEY_BREAK) {
					printf("^cancel\r\n");
					break;
				}
				v1 = (uint8_t) memR(a1); // *((uint8_t *) a1);
				v2 = (uint8_t) memR(a2); // *((uint8_t *) a2);
				if(v1 != v2) {
					printf("%0" STR(MON_ADDR_LEN) "lX: %02X  <>  %0" STR(MON_ADDR_LEN) "lX: %02X\r\n",
							a1
							#ifdef SANDBOX
								- (uint32_t) sandbox
							#endif

							, (int) v1,
							a2
							#ifdef SANDBOX
								- (uint32_t) sandbox
							#endif

							, (int) v2
						);
					c++;
				}
				a1++; a2++;
			}
			printf(">>> %lu differences\r\n\n", c);
		}

		// move
		else if( (*pcmd == 'm') || (*pcmd == 'M') ) {
			skip(&pcmd, 1);
			int32_t len = (aend - addr + 1);
			if(len < 0) len = 0;
			uint32_t c = 0;
			if( (anew != addr) && (len > 0) ) {
				c = (aend - addr + 1);
				// memmove( (uint8_t *) anew, (uint8_t *) addr, c );
				memMove(anew, addr, c);
			}
			printf(">>> %lu bytes moved\r\n\n", c);
		}

		// initialise area with value
		else if( (*pcmd == 'i') || (*pcmd == 'I') ) {
			skip(&pcmd, 1);
			int32_t len = (aend - addr + 1);
			if(len < 0) len = 0;
			uint32_t written = 0;
			uint32_t v, vnew = anew;
			#ifdef SANDBOX
				vnew -= (uint32_t) sandbox;
			#endif
			uint32_t a = addr;
			uint8_t b;
			while(len > 0) {
				v = vnew;
				b = newl;
				while( (b--) && (len > 0) ) {
					#ifdef MON_MSB_FIRST
						// *((uint8_t *) (a++ )) = (uint8_t) (v >> (8 * bcnt));
						memW( a++, (uint8_t) (v >> (8 * bcnt)) );
					#else
						// *((uint8_t *) (a++ )) = (uint8_t) v;
						memW( a++, (uint8_t) v );
						v >>= 8;
					#endif
					written++;
					len--;
				}
			}
			printf(">>> %lu bytes initialised\r\n\n", written);
		}

		#if defined(MON_USE_FILES) && defined(INCLUDE_FILES)

		// write area as file
		else if( (*pcmd == 'w') || (*pcmd == 'W') ) {
			skip(&pcmd, 1);
			if( (anew == addr) && (*pcmd == '"') ) {
				skip(&pcmd, 1);	// skip the leading quote in the file name
				uint32_t wrt, written = 0;
				int32_t towrite = (aend - addr + 1);
				char *s = NULL;
				uint8_t *buf = NULL;
				FIL *ff = NULL;
				xalloc((uint8_t **) &s, strlen(pcmd));
				xalloc((uint8_t **) &buf, MON_FILEBUF_SIZE);
				xalloc((uint8_t **) &ff, sizeof(FIL));
				if(s && buf && ff) {
					s = pcmd;
					char *stmp = s;
					while(*stmp != '"') stmp++;
					*stmp = 0;	// (*s) now contains only the file name
					pcmd = ++stmp;
					FRESULT fr = FR_OK;
					if(towrite > 0) {
						printf(">>> writing ");
						if(towrite > 0) printf("%li bytes ", (signed long) towrite);
						printf("to file \"%s\" ...\r\n", s);
						if( s[0] && s[1] && s[2] && (s[3] == ':') ) {
							if(fr == FR_OK) fr = f_chdrive(s);
							if(fr == FR_OK) fr = f_mount(&FF, "", 1);
						}
						if(fr == FR_OK) fr = f_open(ff, s, (FA_WRITE | FA_CREATE_ALWAYS));
						if(fr == FR_OK) {
							uint32_t a = addr;
							UINT twb;
							wrt = MON_FILEBUF_SIZE;
							while( towrite && wrt && (fr == FR_OK) ) {
								if(towrite >= MON_FILEBUF_SIZE) twb = MON_FILEBUF_SIZE;
								else twb = towrite;
								memMove( (DATA_RAM + (uint32_t) buf), a, twb );
								fr = f_write(ff, (uint8_t *) buf, twb, (UINT *) &wrt);
								if(wrt) {
									written += wrt;
									towrite -= twb;
									a += twb;
								}
							}
						}
						f_close(ff);
					}
					printf(">>> %lu bytes written\r\n", (unsigned long) written);
					if(fr != FR_OK) {
						fopsError(fr);
						result = fr;
					}
				}
				else result = MON_NOMEM;
				xfree((uint8_t **) &s);
				xfree((uint8_t **) &buf);
				xfree((uint8_t **) &ff);
			}
			else result = MON_ERROR;
		}

		// read from file
		else if( (*pcmd == 'r') || (*pcmd == 'R') ) {
			skip(&pcmd, 1);
			if(SYSRAM_SIZE > 0) {

				if( (anew == addr) && (*pcmd == '"') ) {
					skip(&pcmd, 1);	// skip the leading quote in the file name
					uint32_t rdt, read = 0;
					int32_t toread = INT32_MAX;
					if(aend != addr) toread = (aend - addr + 1);
					char *s = NULL;
					uint8_t *buf = NULL;
					FIL *ff = NULL;
					xalloc((uint8_t **) &s, strlen(pcmd));
					xalloc((uint8_t **) &buf, MON_FILEBUF_SIZE);
					xalloc((uint8_t **) &ff, sizeof(FIL));
					if(s && buf && ff) {
						s = pcmd;
						char *stmp = s;
						while(*stmp != '"') stmp++;
						*stmp = 0;	// (*s) now contains only the file name
						pcmd = ++stmp;
						FRESULT fr = FR_OK;
						if(toread > 0) {
							printf(">>> reading ");
							if(toread < INT32_MAX) printf("up to %li bytes ", (signed long) toread);
							printf("from file \"%s\" ...\r\n", s);
							if( s[0] && s[1] && s[2] && (s[3] == ':') ) {
								if(fr == FR_OK) fr = f_chdrive(s);
								if(fr == FR_OK) fr = f_mount(&FF, "", 1);
							}
							if(fr == FR_OK) fr = f_open(ff, s, (FA_READ | FA_OPEN_EXISTING));
							if(fr == FR_OK) {
								uint32_t a = addr;
								UINT trb;
								rdt = MON_FILEBUF_SIZE;
								while( toread && rdt && (fr == FR_OK) ) {
									if(toread >= MON_FILEBUF_SIZE) trb = MON_FILEBUF_SIZE;
									else trb = toread;
									rdt = 0;
									fr = f_read(ff, (uint8_t *) buf, trb, (UINT *) &rdt);
									if(rdt) {
										memMove( a, (DATA_RAM + (uint32_t) buf), rdt );
										a += trb;
										read += rdt;
										toread -= trb;
									}
								}
							}
							f_close(ff);
						}
						printf(">>> %lu bytes read\r\n", (unsigned long) read);
						if(fr != FR_OK) {
							fopsError(fr);
							result = fr;
						}
					}
					else result = MON_NOMEM;
					xfree((uint8_t **) &s);
					xfree((uint8_t **) &buf);
					xfree((uint8_t **) &ff);
				}
				else result = MON_ERROR;

			}
			else result = MON_NOMEM;
		}

		#endif

		#ifndef SANDBOX

		// jump to address
		else if( (*pcmd == 'j') || (*pcmd == 'J') ) {
			skip(&pcmd, 1);
			if( (anew == addr) && (aend == addr) ) {
				printf("\r\n");
				LCALL(addr);
				printf("\r\n");
			}
			else result = MON_ERROR;
		}

		#endif

		// command separator or unknown command
		if(*pcmd == ';') skip(&pcmd, 1);
		else if(*pcmd) result = MON_UNKN_CMD;

		if( interactive && (result != MON_OK) ) {
			monError(result);
			*pcmd = 0;
		}

	}
	if(interactive) printf("\r\n");
	return (uint8_t) result;
}

#endif
