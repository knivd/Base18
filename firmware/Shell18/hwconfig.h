#ifndef HWCONFIG_H
#define	HWCONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>

// default fall-back clock frequency derived from the internal oscillator
#define _XTAL_FREQ	4000000

// System: Base18
// ===========================================================================

// memory bank select lines
#define A21		LATHbits.LATH4
#define A22		LATHbits.LATH5
#define A23		LATHbits.LATH6

// SPI select lines
#define SEL0	LATFbits.LATF4
#define SEL1	LATFbits.LATF5
#define SEL2	LATFbits.LATF6
#define SEL3	LATFbits.LATF7

// other control lines
#define EDL		LATBbits.LATB4
#define OFF		LATBbits.LATB5
#define LED		LATGbits.LATG4

// Processor: PIC18F87K22
// ===========================================================================

// CONFIG1L
#pragma config RETEN = ON		// VREG Sleep Enable bit
#pragma config INTOSCSEL = LOW	// LF-INTOSC in Low-power mode during Sleep
#pragma config SOSCSEL = LOW	// Low Power SOSC circuit selected
#pragma config XINST = OFF		// Extended Instruction Set

// CONFIG1H
#pragma config FOSC = HS2		// External crystal
#pragma config PLLCFG = OFF		// PLL x4 Enable bit
#pragma config FCMEN = ON		// Fail-Safe Clock Monitor
#pragma config IESO = ON		// Internal External Oscillator Switch Over Mode

// CONFIG2L
#pragma config PWRTEN = ON		// Power Up Timer
#pragma config BOREN = NOSLP	// Enabled while active, disabled in SLEEP, SBOREN disabled
#pragma config BORV	= 2.7V		// Brown-out Reset Voltage bits
#pragma config BORPWR = HIGH	// BORMV set to high power level

// CONFIG2H
#pragma config WDTEN = ON		// WDT controlled by SWDTEN bit setting
#pragma config WDTPS = 64		// 1:64 watchdog postscaler (around 256 milliseconds)

// CONFIG3L
#pragma config RTCOSC = SOSCREF	// RTCC uses SOSC
#pragma config EASHFT = ON		// External Address Shift bit
#pragma config ABW = XM20		// 20-bit address bus
#pragma config BW = 8			// 8-bit external bus mode
#pragma config WAIT	= ON		// External Bus Wait states

// CONFIG3H
#pragma config CCP2MX =	PORTC	// CCP2 Mux
#pragma config ECCPMX = PORTH	// ECCP Mux
#pragma config MSSPMSK = MSK7	// 7 Bit address masking mode
#pragma config MCLRE = ON		// MCLR Enabled, RG5 Disabled

// CONFIG4L
#pragma config STVREN = ON		// Stack Overflow Reset
#pragma config BBSIZ = BB1K		// 1K word Boot Block size
#pragma config DEBUG = OFF		// Background Debug

// CONFIG5L
#pragma config CP0 = OFF		// Code Protect 00800-03FFF
#pragma config CP1 = OFF		// Code Protect 04000-07FFF
#pragma config CP2 = OFF		// Code Protect 08000-0BFFF
#pragma config CP3 = OFF		// Code Protect 0C000-0FFFF
#pragma config CP4 = OFF		// Code Protect 10000-13FFF
#pragma config CP5 = OFF		// Code Protect 14000-17FFF
#pragma config CP6 = OFF		// Code Protect 18000-1BFFF
#pragma config CP7 = OFF		// Code Protect 1C000-1FFFF

// CONFIG5H
#pragma config CPB = OFF		// Code Protect Boot
#pragma config CPD = OFF		// Data EE Read Protect

// CONFIG6L
#pragma config WRT0 = OFF		// Table Write Protect 00800-03FFF
#pragma config WRT1 = OFF		// Table Write Protect 04000-07FFF
#pragma config WRT2 = OFF		// Table Write Protect 08000-0BFFF
#pragma config WRT3 = OFF		// Table Write Protect 0C000-0FFFF
#pragma config WRT4 = OFF		// Table Write Protect 10000-13FFF
#pragma config WRT5 = OFF		// Table Write Protect 14000-17FFF
#pragma config WRT6 = OFF		// Table Write Protect 18000-1BFFF
#pragma config WRT7 = OFF		// Table Write Protect 1C000-1FFFF

// CONFIG6H
#pragma config WRTC = OFF		// Config. Write Protect
#pragma config WRTB = OFF		// Table Write Protect Boot
#pragma config WRTD = OFF		// Data EE Write Protect

// CONFIG7L
#pragma config EBRT0 = OFF		// Table Read Protect 00800-03FFF
#pragma config EBRT1 = OFF		// Table Read Protect 04000-07FFF
#pragma config EBRT2 = OFF		// Table Read Protect 08000-0BFFF
#pragma config EBRT3 = OFF		// Table Read Protect 0C000-0FFFF
#pragma config EBRT4 = OFF		// Table Read Protect 10000-13FFF
#pragma config EBRT5 = OFF		// Table Read Protect 14000-17FFF
#pragma config EBRT6 = OFF		// Table Read Protect 18000-1BFFF
#pragma config EBRT7 = OFF		// Table Read Protect 1C000-1FFFF

// CONFIG7H
#pragma config EBRTB = OFF		// Table Read Protect Boot

#ifdef	__cplusplus
}
#endif

#endif	// HWCONFIG_H
