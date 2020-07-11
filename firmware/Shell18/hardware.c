#include "hardware.h"
#include "console.h"
#include "xmem.h"

#ifdef INCLUDE_FILES
#include "FatFs/source/diskio.h"
#include "FatFs/source/ff.h"
#include "fileops.h"
#endif

uint8_t xmem[XMEM_SIZE];	// default managed memory array in the internal RAM

#ifndef SANDBOX

__persistent uint8_t crystal_osc = 0;			// set in 1 if an external crystal oscillator is running
__persistent uint8_t stage = 0;					// used for oscillator switch and measuring the clock frequency
__persistent volatile uint32_t freq_cntr_int;	// used for measuring the clock frequency
__persistent volatile uint32_t freq_cntr_ext;	// used for measuring the clock frequency
__at(0x000) uint16_t varC;
__at(0x002) uint8_t *varD;

uint16_t T0val = 0;			// calculated value for the Timer 0 system tick counter
// uint8_t tblregs[4];		// array for saving/restoring the TBL regs

#define saveTBL()    { /* memcpy(tblregs, (uint8_t *) &TABLAT, 4); */ }
#define restoreTBL() { /* memcpy((uint8_t *) &TABLAT, tblregs, 4); */ }

#define TBLaddr(addr) { \
			TBLPTRU = (uint8_t) ((uint32_t) addr >> 16); \
			TBLPTRH = (uint8_t) ((uint32_t) addr >> 8); \
			TBLPTRL = (uint8_t) addr; \
		}

#endif


void memW(uint32_t addr, uint8_t val) {
	#ifndef SANDBOX
		if(isDataRAM(addr)) {	// addresses in this range are considered as access to the internal data RAM
			*( (volatile uint8_t *) (addr - DATA_RAM) ) = val;
		}
		else {
			saveTBL();
			TBLaddr(addr);
			TABLAT = val;
			asm("TBLWT*");
			restoreTBL();
		}
	#else
		sandbox[addr] = val;
	#endif
}


uint8_t memR(uint32_t addr) {
	#ifndef SANDBOX
		uint8_t dreg;
		if(isDataRAM(addr)) {	// addresses in this range are considered as access to the internal data RAM
			dreg = *( (volatile uint8_t *) (addr - DATA_RAM) );
		}
		else {
			saveTBL();
			TBLaddr(addr);
			asm("TBLRD*");
			dreg = TABLAT;
			restoreTBL();
		}
	#else
		uint8_t dreg = sandbox[addr];
	#endif
	return dreg;
}


void memSet(uint32_t addr, uint8_t val, uint32_t n) {
	#ifndef SANDBOX
		if(isDataRAM(addr)) {
			memset( (uint8_t *) (addr - DATA_RAM), val, n);
		}
		else {
			saveTBL();
			TBLaddr(addr);
			TABLAT = val;
			while(n--) asm("TBLWT*+");
			restoreTBL();
		}
	#else
		memset( (uint8_t *) &sandbox[addr], val, n);
	#endif
}


void memMove(uint32_t dst, uint32_t src, uint32_t n) {

	#ifndef SANDBOX

	uint32_t d = dst;
	uint32_t s = src;
	uint8_t da = isDataRAM(d);
	uint8_t sa = isDataRAM(s);

	if(!da && sa) {			// moving from data RAM to external memory
		uint8_t *sp = (uint8_t *) (s - DATA_RAM);
		saveTBL();
		TBLaddr(d);
		while(n--) {	// the blocks are in different memories and can't overlap, so no need for direction check
			TABLAT = *(sp++);
			asm("TBLWT*+");
		}
		restoreTBL();
	}

	else if(da && !sa) {	// moving from external memory to data RAM
		uint8_t *dp = (uint8_t *) (d - DATA_RAM);
		saveTBL();
		TBLaddr(s);
		while(n--) {	// the blocks are in different memories and can't overlap, so no need for direction check
			asm("TBLRD*+");
			*(dp++) = TABLAT;
		}
		restoreTBL();
	}

	else if(!da && !sa) {	// both blocks are in external memory
		if( (s < d) && ((s+n) > d) ) {
			s += n; d += n;
			while(n--) memW(--d, memR(--s));
		}
		else {
			while(n--) memW(d++, memR(s++));
		}
	}

	else {					// both blocks are in data RAM - use the standard memmove() function
		memmove( (uint8_t *) (dst - DATA_RAM), (uint8_t *) (src - DATA_RAM), n);
	}

	#else

	memmove( (uint8_t *) &sandbox[dst], (uint8_t *) &sandbox[src], n);

	#endif

}


uint32_t strLen(uint32_t s) {
	uint32_t len;
	#ifndef SANDBOX
		if(isDataRAM(s)) {
			len = strlen( (const char *) (s - DATA_RAM) );
		}
		else {
			saveTBL();
			TBLaddr(s);
			asm("TBLRD*+");
			for(len = 0; TABLAT; len++) asm("TBLRD*+");
			restoreTBL();
		}
	#else
		len = strlen((char *) &sandbox[s]);
	#endif
	return len;
}


int16_t memCmp(uint32_t v1, uint32_t v2, uint32_t n) {
	int16_t res;
	#ifndef SANDBOX
		if(isDataRAM(v1) && isDataRAM(v2)) {
			res = memcmp( (uint8_t *) (v1 - DATA_RAM), (uint8_t *) (v2 - DATA_RAM), n );
		}
		else {
			for(; n && memR(v1) == memR(v2); n--, v1++, v2++);
			res = (n ? (memR(v1) - memR(v2)) : 0);
		}
	#else
		res = memcmp( (uint8_t *) &sandbox[v1], (uint8_t *) &sandbox[v2], n);
	#endif
	return res;
}


#ifndef SANDBOX

void delay_us(uint32_t us) {
	us /= 10;	// 10 microseconds granularity
	OSCCONbits.SCS = 2;	// 0b10;	// switch to the internal oscillator
	while(us--) __delay_us(9);
	if(crystal_osc) OSCCONbits.SCS = 0;	// 0b00;	// return to the external crystal oscillator
}


void delay_ms(uint32_t ms) {
	OSCCONbits.SCS = 2;	// 0b10;	// switch to the internal oscillator
	while(ms--) __delay_ms(1);
	if(crystal_osc) OSCCONbits.SCS = 0;	// 0b00;	// return to the external crystal oscillator
}


void LCALL(uint32_t addr) {
	//asm("bra LABEL2");
	//asm("LABEL1:");
	LJUMP(addr);
	//asm("LABEL2:");
	//asm("rcall LABEL1");
}


void regStore(void) {
	if(STORE_ADDR) memMove(STORE_ADDR, DATA_RAM, 0x1000);
}


void regRestore(void) {
	if(STORE_ADDR) {
		varC = 0x1000 - (sizeof(varC) + sizeof(varD)) - 0x100;	// will not restore the SFR area
		varD = (uint8_t *) (sizeof(varC) + sizeof(varD));
		saveTBL();
		TBLaddr(STORE_ADDR);
		while(varC--) {
			asm("TBLRD*+");
			*(varD++) = TABLAT;
		}
		restoreTBL();
	}
}


// interrupt handler
// ===========================================================================

static void __interrupt() ISR(void) {

	// received character from the console
	if(PIR3bits.RC2IF) {

		uint8_t rf = RCSTA2;	// get the reception flags
		uint8_t ch = RCREG2;	// get the received byte

		if ((rf & (_RCSTA2_FERR_MASK | _RCSTA2_OERR_MASK)) == 0) {	// no errors
			if((console_rxo_index-console_rxi_index) != 1) {		// disallow buffer overrun
				console_rx_buffer[console_rxi_index] = ch;
				if (++console_rxi_index >= CON_RX_BUFFER_SIZE) console_rxi_index = 0;
				if(console_rxi_index == console_rxo_index) {	// discard the oldest character if the buffer is full
					if (++console_rxo_index >= CON_RX_BUFFER_SIZE) console_rxo_index = 0;
				}
			}
		}

		if (rf & _RCSTA2_OERR_MASK) {	// clear hardware overrun error
			RCSTA2bits.CREN = 0;
			Nop();
			RCSTA2bits.CREN = 1;
		}

		PIR3bits.RC2IF = 0;	// clear the interrupt flag
    }

	// jump to address set in INTchain, if one is set
	if(INTchain) LCALL(INTchain);

	// clear the system tick interrupt flag and reset Timer 0
	if(INTCONbits.TMR0IF) {
		systick++;
		TMR0H = (uint8_t) (T0val >> 8);
		TMR0L = (uint8_t) T0val;
		INTCONbits.TMR0IF = 0;
	}

}

#endif


// handler function for memory allocation errors
void errAlloc(uint32_t sz) {
	#ifndef SANDBOX
		WDTCONbits.SWDTEN = 0;	// disable the watchdog
	#endif
	printf("\r\n\n\n\n>>> FATAL ERROR: Unable to allocate %lu bytes\r\nSystem reset is required\r\n", (unsigned long)sz);
	uint8_t b;
	for(b = 0; ; b = 1-b) {	// loop forever
		#ifndef SANDBOX
			LED = b;
			delay_ms(250);
		#endif
	}
}


void coldInit(void) {

	#ifndef SANDBOX

		// switch to the crystal oscillator or use the internal one instead as a fall-back option
		// also, calculate the clock frequency and store it in (XTAL_FREQ)
		#if _XTAL_FREQ == 1000000
			OSCCONbits.IRCF = 3;	// 0b011;	// 1 MHz (UART may not work at 115200)
		#elif _XTAL_FREQ == 2000000
			OSCCONbits.IRCF = 4;	// 0b100;	// 2 MHz (UART may not work at 115200)
		#elif _XTAL_FREQ == 4000000
			OSCCONbits.IRCF = 5;	// 0b101;	// 4 MHz
		#elif _XTAL_FREQ == 8000000
			OSCCONbits.IRCF = 6;	// 0b110;	// 8 MHz
		#elif _XTAL_FREQ == 16000000
			OSCCONbits.IRCF = 7;	// 0b111;	// 16 MHz
		#else
			OSCCONbits.IRCF = 6;	// 0b110;	// 8 MHz
		#endif
		OSCCONbits.SCS = 2;	// 0b10;	// switch to the internal oscillator
		while(!OSCCONbits.OSTS);	// wait for the start-up timer to expire
		if(RCONbits.TO) stage = 0;	// reset not from the watchdog - clear the stage index

		ANCON0 = 0x1F;	// 0b00011111;	// enable AN0 - AN4, disable AN5 - AN7
		ANCON1 = 0;				// disable AN8 - AN15
		ANCON2 = 0;				// disable AN16 - AN23

		LED = 1; TRISGbits.TRISG4 = 0;	// LED output

		RCONbits.IPEN = 0;		// disable interrupt priorities
		INTCON2bits.RBPU = 0;	// enable pull-ups on the external interrupt lines
		PADCFG1bits.RDPU = 1;	// enable pull-ups on the external bus data lines
		PADCFG1bits.RJPU = 1;	// enable pull-ups on the external bus control signals

		EDL = 0; TRISBbits.TRISB4 = 0;	// the EDL# output must be kept low at all times
										// in order to enable proper operation of
										// the external memory bus

		OFF = 0; TRISBbits.TRISB5 = 1;	// the OFF# line will be configured as input so
										// it just keeps the input of the power supply module
										// open during normal operation

		A21 = 0; TRISHbits.TRISH4 = 0;	// initialise memory bank select A21 output
		A22 = 0; TRISHbits.TRISH5 = 0;	// initialise memory bank select A22 output
		A23 = 0; TRISHbits.TRISH6 = 0;	// initialise memory bank select A23 output

		SEL0 = 1; TRISFbits.TRISF4 = 0;	// initialise SEL0# output
		SEL1 = 1; TRISFbits.TRISF5 = 0;	// initialise SEL1# output
		SEL2 = 1; TRISFbits.TRISF6 = 0;	// initialise SEL2# output
		SEL3 = 1; TRISFbits.TRISF7 = 0;	// initialise SEL3# output

		// stage 0: check for external crystal
		if(stage == 0) {
			stage++;
			crystal_osc = 0;
			freq_cntr_int = 0;
			freq_cntr_ext = 0;
			WDTCONbits.SWDTEN = 1;	// enable the watchdog
			OSCCONbits.SCS = 0;	// 0b00;	// try to enable the external crystal oscillator
			asm("NOP");
			asm("NOP");
			asm("NOP");				// will get stuck here and restart if there is no external crystal
			crystal_osc = 1;		// set the crystal presence flag
			while(1);				// force the watchdog to restart the system
		}

		// checking the watchdog reset flag and further stages
		if(RCONbits.TO == 0) {
			WDTCONbits.SWDTEN = 1;	// enable the watchdog

			if(crystal_osc && (stage == 1)) {	// stage 1: count when on internal clock
				stage++;
				asm("NOP");
				asm("NOP");
				asm("NOP");
				asm("NOP");
				asm("NOP");
				while(1) freq_cntr_int++;	// this endless loop will be terminated by the watchdog
			}

			else if(crystal_osc && (stage == 2)) {	// stage 2: count when on external clock
				stage++;
				OSCCONbits.SCS = 0;	// 0b00;	// enable the external crystal oscillator
				asm("NOP");
				asm("NOP");
				asm("NOP");
				while(1) freq_cntr_ext++;	// this endless loop will be terminated by the watchdog
			}

			// stage 3: all finished so there is nothing else to do
		}
		WDTCONbits.SWDTEN = 0;		// disable the watchdog
		if(crystal_osc) {
			OSCCONbits.SCS = 0;	// 0b00;	// enable the external crystal oscillator
			XTAL_FREQ = (((_XTAL_FREQ / 10000) * freq_cntr_ext) / freq_cntr_int) * 10000;
		}
		else XTAL_FREQ = _XTAL_FREQ;
		INTchain = 0;			// initialise the interrupt function chain

		SSP1CON1bits.SSPEN = 0;		// disable SPI

		// SPI modes
		// 0: CKP=0, CKE=1
		// 1: CKP=0, CKE=0
		// 2: CKP=1, CKE=1
		// 3: CKP=1, CKE=0
		SSP1CON1bits.CKP = 1;
		SSP1STATbits.CKE = 0;

		SSP1CON1bits.SSPM = 0;	// 0b0000;	// default SPI mode: master, clock Fosc/4

		LATCbits.LATC3 = 1; TRISCbits.TRISC3 = 0;	// SCK1 output
							TRISCbits.TRISC4 = 1;	// SDI1 MISO input
		LATCbits.LATC5 = 1; TRISCbits.TRISC5 = 0;	// SDO1 MOSI output

		LATCbits.LATC6 = 1; TRISCbits.TRISC6 = 0;	// TX1 output
							TRISCbits.TRISC7 = 1;	// RX1 input
		PIE1bits.RC1IE = 1;		// enable interrupt on receive

		LATJ = 0xff; TRISJ = 0;	// port J is used for control signals in the external bus

		// initialise the external memory bus

		MEMCONbits.EBDIS = 0;	// external memory bus is always enabled
		MEMCONbits.WAIT = 0;	// 0b00;	// 3 Tcy in table reads
		MEMCONbits.WM = 0;		// 0b00;	// byte mode for table writes

		// initialise the main console for 115200,8N1 protocol

		LATGbits.LATG1 = 1;	TRISGbits.TRISG1 = 0;	// TX2 output
							TRISGbits.TRISG2 = 1;	// RX2 input
		{ // calculate the SPBRG value and load the baud rate register
			uint16_t brg = (((uint32_t) XTAL_FREQ / 4) / (uint32_t) CONSOLE_BAUDRATE) - 1;
			SPBRG2 = (uint8_t) brg;
			SPBRGH2 = (uint8_t) (brg >> 8);
		}
		BAUDCON2bits.BRG16 = 1;	// 16-bit baud rate register
		TXSTA2bits.BRGH = 1;	// high baud rate
		TXSTA2bits.TXEN = 1;	// enable transmit
		RCSTA2bits.CREN = 1;	// enable receive
		PIE3bits.RC2IE = 1;		// enable interrupt on receive
		RCSTA2bits.SPEN = 1;	// enable EUSART2

		printBanner();
		printf( "Platform: PIC18 (%li.%02li MHz)\r\n", (XTAL_FREQ / 1000000), ((XTAL_FREQ / 10000) % 100) );
		if(crystal_osc == 0) printf(">>> WARNING: No external oscillator found\r\n");

		{	// detect the size of the attached system RAM
			uint32_t offs = _ROMSIZE;
			uint8_t v1 = 0, v2 = 0, v3 = 0, v4 = 0;
			uint8_t ramf = 0;
			STORE_ADDR = 0;
			SYSRAM_ADDR = 0;
			SYSRAM_SIZE = 0;
			while(1) {
				if((offs & 0xffff) == 0) printf( "%lu KB tested\r", ((offs - _ROMSIZE) / 0x400) );
				memW(offs, 0xff); v1 = memR(offs);
				memW(offs, 0x96); v2 = memR(offs);
				memW(offs, 0x69); v3 = memR(offs);
				memW(offs, 0x00); v4 = memR(offs);
				if( (v1 == 0xff) && (v2 == 0x96) && (v3 == 0x69) && (v4 == 0x00) ) {	// RAM detected
					if(!ramf) {
						SYSRAM_ADDR = offs;
						ramf = 1;
					}
					SYSRAM_SIZE += 0x40;
					memSet(offs, 0, 0x40);
				}
				else if(ramf) break;
				offs += 0x40;
				if(offs >= 0x1000000) break;	// up to 2^24 addresses tested
			}
		}

		if(SYSRAM_SIZE) {
			printf( "Detected RAM: %lu (0x%04lx) bytes starting from 0x%04lx\r\n\n",
						SYSRAM_SIZE, SYSRAM_SIZE, SYSRAM_ADDR );
			if(SYSRAM_SIZE >= 0x1800) {	// in systems with 6 KB or more, a "store" area is allocated and used
				STORE_ADDR = SYSRAM_ADDR;
				SYSRAM_ADDR += 0x1000;
				SYSRAM_SIZE -= 0x1000;	// reserving area to store registers before execution
			}
		}
		else printf(">>> WARNING: No external RAM found\r\n\n");

	#else

		#if IFS_SIZE_KB >= FATFS_SIZE_MIN_KB
			fclose(IFS);
			IFS=NULL;
		#endif

		SYSRAM_ADDR = (uint32_t) sandbox;
		SYSRAM_SIZE = sizeof(sandbox);

		setbuf(stdin, NULL);
		setbuf(stdout, NULL);

		printBanner();
		printf("Platform CPU: SANDBOX\r\n");

	#endif

	// initialise the RAM disk
	RAM = 0;
	RAM_SIZE_KB = 0;

	// reset the date/time structure
	memset((uint8_t *) &date_time, 0, sizeof(date_time));

	// abstract level indexes for the console reception buffer
	console_rxi_index = 0;
	console_rxo_index = 0;

	// initialise the memory manager
	xmeminit( ((uint32_t) xmem), XMEM_SIZE, errAlloc );

	#ifdef INCLUDE_FILES

	// initialise the FatFs main structure
	f_mount(&FF, "", 0);
	f_setcp(850);	// set default code page 850 (Latin 1)

		#if IFS_SIZE_KB >= FATFS_SIZE_MIN_KB

		// start in the internal file system drive
		if(f_mount(&FF, "ifs:", 1) != FR_OK) {
			cputs("\r\nInitialising IFS: ... ");
			int64_t sz = fopsInit("ifs:");
			if(sz > 0) printf("%llu bytes free\r\n\n", (uint64_t) sz);
			else {
				printf("\r\n");
				fopsError((int8_t) -sz);
			}
		}

		// try to mount IFS: as default drive
		f_chdrive("ifs:");
		f_mount(&FF, "ifs:", 1);

		#endif

	#endif

	#ifndef SANDBOX

	// enable Timer 0 as 100 Hz system tick
	T0CONbits.TMR0ON = 0;
	T0CONbits.T08BIT = 0;	// 16-bit mode
	T0CONbits.T0CS = 0;		// internal clock Fosc/4
	T0CONbits.T0PS = 7;	// 0b111;	// 1:256 pre-scaler
	T0CONbits.PSA = 0;		// pre-scaler is assigned
	T0CONbits.TMR0ON = 1;	// enable Timer 0
	T0val = ~(XTAL_FREQ / 4 / 256 / 100);	// Fosc/4 --> pre-scaler 1:256 --> 100 Hz desired output
	TMR0H = (uint8_t) (T0val >> 8);
	TMR0L = (uint8_t) T0val;
	INTCONbits.TMR0IE = 1;	// enable Timer 0 interrupt

	// enable interrupts
	systick = 0;
	INTCONbits.PEIE = 1;	// enable peripheral interrupts
	INTCONbits.GIE = 1;		// enable all interrupts

	LED = 0;

	#endif

}
