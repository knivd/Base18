#include "sd_spi.h"
#include "../../main.h"
#include "../../hardware.h"

// modified by (c) KnivD, 2019

//-------------------------------------------------------------
// Private Functions
//-------------------------------------------------------------

// these should be replaced with references to actual ports if needed
#define CD	(1)		// Card detected   (yes:true, no:false, default:true)
#define WP	(0)		// Write protected (yes:true, no:false, default:false)

// Definitions for the MMC/SDC commands
#define CMD0   (0)			// GO_IDLE_STATE
#define CMD1   (1)			// SEND_OP_COND
#define ACMD41 (41|0x80)	// SEND_OP_COND (SDC)
#define CMD8   (8)			// SEND_IF_COND
#define CMD9   (9)			// SEND_CSD
#define CMD10  (10)			// SEND_CID
#define CMD12  (12)			// STOP_TRANSMISSION
#define ACMD13 (13|0x80)	// SD_STATUS (SDC)
#define CMD16  (16)			// SET_BLOCKLEN
#define CMD17  (17)			// READ_SINGLE_BLOCK
#define CMD18  (18)			// READ_MULTIPLE_BLOCK
#define CMD23  (23)			// SET_BLOCK_COUNT
#define ACMD23 (23|0x80)	// SET_WR_BLK_ERASE_COUNT (SDC)
#define CMD24  (24)			// WRITE_BLOCK
#define CMD25  (25)			// WRITE_MULTIPLE_BLOCK
#define CMD41  (41)			// SEND_OP_COND (ACMD)
#define CMD55  (55)			// APP_CMD
#define CMD58  (58)			// READ_OCR

// card type flags (MMC_GET_TYPE)
#define CT_MMC		0x01    // MMC ver 3
#define CT_SD1		0x02    // SD ver 1
#define CT_SD2		0x04    // SD ver 2
#define CT_SDC		(CT_SD1|CT_SD2) // SD
#define CT_BLOCK	0x08    // Block addressing

DSTATUS Stat = STA_NOINIT;	// current status
UINT CardType = 0;			// card type as taken from the device
UINT sectSize = FF_MIN_SS;	// sector size in bytes (512 bytes)
BYTE currentSD = 0;			// current selected drive
BYTE slowclkf = 0;			// flag: 0: fast SPI clock, 1: slow SPI clock
int32_t DlyCntr = 0;		// delay counter decreased in the xchg_spi() function


#ifndef SANDBOX

// SPI clock constants
uint32_t SPI_SLOW_CLK = 0;	// this will be later initialised with its actual value
#define SPI_FAST_CLK	(XTAL_FREQ / 4)


// master mode, clock around 200 kHz
void FCLK_SLOW() {
	slowclkf = 1;
	T2CON = 0;	// pre-scaler 1:1, post-scaler 1:1
	PR2 = (XTAL_FREQ / 4) / 400000;	// set timer period: Fosc/4 -> pre 1:1 -> PR2 -> post 1:1
	T2CONbits.TMR2ON = 1;		// enable TMR2
	SSP1CON1bits.SSPEN = 0;
	SSP1CON1bits.SSPM = 2;	// 0b0010;	// SPI clock source is now from TMR2/2 (around 200 kHz)
	SSP1CON1bits.SSPEN = 1;
	SPI_SLOW_CLK = XTAL_FREQ / 4 / PR2 / 2;
}


// master mode, clock Fosc/4
void FCLK_FAST() {
	slowclkf = 0;
	T2CONbits.TMR2ON = 0;		// disable TMR2 (there is no need for it anymore)
	SSP1CON1bits.SSPEN = 0;
	SSP1CON1bits.SSPM = 0;	// 0b0000;
	SSP1CON1bits.SSPEN = 1;
}


// automatically initialise and set CS# line low
static void CS_LOW(void) {
	if(currentSD == 0) SEL0 = 0;
	else if(currentSD == 1) SEL1 = 0;
	else if(currentSD == 2) SEL2 = 0;
	else if(currentSD == 3) SEL3 = 0;
	Nop();
}


// automatically initialise and set CS# line high
static void CS_HIGH(void) {
	if(currentSD == 0) SEL0 = 1;
	else if(currentSD == 1) SEL1 = 1;
	else if(currentSD == 2) SEL2 = 1;
	else if(currentSD == 3) SEL3 = 1;
	Nop();
}


// transmit/receive data byte through SPI
// (tx) byte to be transmitted
// will return the received byte during the same transaction
uint8_t spiTR(uint8_t tx) {
	if(SSP1BUF) {}	// dummy read the buffer in order to clear the .BF flag
	SSP1BUF = tx;
	while(SSP1STATbits.BF == 0) continue;	// wait until the transaction is complete
	return SSP1BUF;
}


// Transmit/Receive data to/from SD/MMC via SPI
static BYTE xchg_spi(BYTE txd) {
    if(DlyCntr > 0) DlyCntr--;
    return (BYTE) spiTR((uint8_t) txd);
}


// transmit block
void trmt_block(const BYTE *src, UINT cnt) {
    while(cnt--) xchg_spi(*(src++));
}


// receive block
void recv_block(BYTE *dst, UINT cnt) {
    while(cnt--) *(dst++) = xchg_spi(0xFF);
}


// calculate and set (DlyCntr) the number of xchg_spi() possible within the given number of milliseconds
void setDly(unsigned long ms) {
	DWORD x = ( (slowclkf) ? SPI_SLOW_CLK : SPI_FAST_CLK ) / 8;
    DlyCntr = ( (x * ms) / 1000 );
}


// Wait for card ready
static int wait_ready(void) {
    BYTE d;
    int32_t oldDly = DlyCntr;
    setDly(500);	// timeout in ms
	do {
		d = xchg_spi(0xFF);
	} while( (d != 0xFF) && (DlyCntr > 0) );
    DlyCntr = oldDly;
	return ((d == 0xFF) ? 1 : 0);
}


// Deselect the card and release SPI bus
static void deselect(void) {
	CS_HIGH();
	xchg_spi(0xFF);		// Dummy clock (force DO hi-z for multiple slave SPI)
}


// Select the card and wait for ready
static int select(void) {	// 1:Successful, 0:Timeout
	CS_LOW();
	xchg_spi(0xFF);			// Dummy clock (force DO enabled)
	if( wait_ready() ) return 1;	// OK
	deselect();
	return 0;		// Timeout
}


// Receive a data packet from MMC
static int recv_datablock (	// 1:OK, 0:Failed
	BYTE *buff,			// Data buffer to store received data
	UINT btr			// Byte count (must be multiple of 4)
) {
	BYTE token;
	setDly(100);
	do {                // Wait for data packet in timeout
		token = xchg_spi(0xFF);
	} while( (token == 0xFF) && (DlyCntr > 0) );

	if(token != 0xFE) return 0;	// If not valid data token, return with error

	recv_block(buff, btr);	// Receive the data block into buffer
	xchg_spi(0xFF);			// Discard CRC
	xchg_spi(0xFF);

	return 1;				// Return with success
}


// Send a data packet to MMC
#if FF_FS_READONLY == 0
static int trmt_datablock (	// 1:OK, 0:Failed
	const BYTE *buff,	// (sectSize) byte data block to be transmitted
	BYTE token			// Data token
) {
	BYTE resp;
	if( !wait_ready() ) return 0;

	xchg_spi(token);	// Xmit a token
	if(token != 0xFD) {	// Not StopTran token
		trmt_block(buff, sectSize); // Xmit the data block to the SD/MMC
		xchg_spi(0xFF);				// CRC (Dummy)
		xchg_spi(0xFF);
		resp = xchg_spi(0xFF);		// Receive a data response
		if((resp & 0x1F) != 0x05) return 0;	// If not accepted, return with error
	}

	return 1;
}
#endif


// Send a command packet to MMC
static BYTE send_cmd0 (
	BYTE cmd,		// Command byte
	DWORD arg		// Argument
) {
	BYTE n, res;
	/* removed here to avoid recursive calls
	if(cmd & 0x80) {	// ACMD<n> is the command sequence of CMD55-CMD<n>
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if(res > 1) return res;
	}
	*/

	// Select the card and wait for ready except to stop multiple block read
	if(cmd != CMD12) {
		deselect();
		if( select() == 0 ) return 0xFF;	// timeout
	}

	// Send command packet
	xchg_spi(0x40 | cmd);			// Start + Command index
	xchg_spi((BYTE)(arg >> 24));	// Argument[31..24]
	xchg_spi((BYTE)(arg >> 16));	// Argument[23..16]
	xchg_spi((BYTE)(arg >> 8));		// Argument[15..8]
	xchg_spi((BYTE)arg);			// Argument[7..0]
	n = 0x01;						// Dummy CRC + Stop
	if(cmd == CMD0) n = 0x95;		// Valid CRC for CMD0(0) + Stop
	if(cmd == CMD8) n = 0x87;		// Valid CRC for CMD8(0x1AA) + Stop
	xchg_spi(n);

	// Receive command response
	if(cmd == CMD12) xchg_spi(0xFF);	// Skip a stuff byte on stop to read
	n = 10;		// Wait for a valid response in timeout of 10 attempts
	do {
		res = xchg_spi(0xFF);
	} while((res & 0x80) && --n);

	return res;		// Return with the response value
}


// Send a command packet to MMC
static BYTE send_cmd (
	BYTE cmd,		// Command byte
	DWORD arg		// Argument
) {
	BYTE n, res;
	if(cmd & 0x80) {	// ACMD<n> is the command sequence of CMD55-CMD<n>
		cmd &= 0x7F;
		res = send_cmd0(CMD55, 0);
		if(res > 1) return res;
	}
	return send_cmd0(cmd, arg);
}

#endif

//-------------------------------------------------------------
// Public Functions
//-------------------------------------------------------------

// Get Disk Status
DSTATUS sd_status (
	BYTE pdrv		// Physical drive number (0)
) {
	if(pdrv != 0) return STA_NOINIT;	// Supports only single drive
	if(CD) Stat &= ~STA_NODISK; else Stat |= (STA_NODISK | STA_NOINIT);
	if(WP) Stat |= STA_PROTECT; else Stat &= ~STA_PROTECT;
	return Stat;
}


// Initialise Disk Drive
DSTATUS sd_init (
	BYTE pdrv		// Physical drive number (0)
) {

	if(pdrv != 0) return STA_NOINIT;       // Supports only single drive
	if(sd_status(0) & STA_NODISK) return Stat; // No card in the socket

	#ifndef SANDBOX

	BYTE n, cmd, ty, ocr[4];

	CS_HIGH();
	FCLK_SLOW();
	for(n = 10; n; n--) xchg_spi(0xFF);		// 80 dummy clocks
	ty = 0;

	if(send_cmd(CMD0, 0) == 1) {			// Enter Idle state
		setDly(1500);       				// Initialisation timeout in ms
		if(send_cmd(CMD8, 0x1AA) == 1) {	// SDv2
			for(n = 0; n < 4; n++) ocr[n] = xchg_spi(0xFF);	// Get trailing return value of R7 resp
			if(ocr[2] == 0x01 && ocr[3] == 0xAA) {			// The card can work at Vdd range of 2.7-3.6V
				while( (DlyCntr > 0) && send_cmd(ACMD41, 0x40000000) );	// Wait for leaving idle state (ACMD41 with HCS bit)
				if( (DlyCntr > 0) && (send_cmd(CMD58, 0) == 0) ) {		// Check CCS bit in the OCR
					for(n = 0; n < 4; n++) ocr[n] = xchg_spi(0xFF);
					ty = ( (ocr[0] & 0x40) ? (CT_SD2 | CT_BLOCK) : CT_SD2 ); 	// SDv2
				}
			}
		} else {							// SDv1 or MMCv3
			if(send_cmd(ACMD41, 0) <= 1) {
				ty = CT_SD1; cmd = ACMD41;	// SDv1
			} else {
				ty = CT_MMC; cmd = CMD1;	// MMCv3
			}
			while( (DlyCntr > 0) && send_cmd(cmd, 0) );	// Wait for leaving idle state
			if( (DlyCntr <= 0) || (send_cmd(CMD16, sectSize) != 0) ) ty = 0; 	// Set read/write block length
		}
	}
    deselect();

	CardType=ty;
	if(ty) Stat &= ~STA_NOINIT;	// Function succeeded - clear STA_NOINIT
	FCLK_FAST();

	#endif

	return Stat;
}


// Read Sector(s)
DRESULT sd_read (
	BYTE pdrv,		// Physical drive number (0)
	BYTE *buff,		// Pointer to the data buffer to store read data
	DWORD sector,	// Start sector number (LBA)
	UINT count		// Sector count (1..128)
) {

	#ifndef SANDBOX

	if(pdrv || !count) return RES_PARERR;
	if(Stat & STA_NOINIT) return RES_NOTRDY;

	if( !(CardType & CT_BLOCK) ) sector *= sectSize;	// Convert to byte address if needed

	if(count == 1) {		// Single block read
		if( (send_cmd(CMD17, sector) == 0) && recv_datablock(buff, sectSize) ) count = 0;	// READ_SINGLE_BLOCK
	}
	else {				// Multiple block read
		if(send_cmd(CMD18, sector) == 0) {	// READ_MULTIPLE_BLOCK
			do {
				if( !recv_datablock(buff, sectSize) ) break;
				buff += sectSize;
			} while(--count);
			send_cmd(CMD12, 0);				// STOP_TRANSMISSION
		}
	}
	deselect();

	#endif

	return count ? RES_ERROR : RES_OK;
}


// Write Sector(s)
#if FF_FS_READONLY == 0
DRESULT sd_write (
	BYTE pdrv,				// Physical drive number (0)
	const BYTE *buff,		// Pointer to the data to be written
	DWORD sector,			// Start sector number (LBA)
	UINT count				// Sector count (1..128)
) {

	#ifndef SANDBOX

	if(pdrv || !count) return RES_PARERR;
	if(Stat & STA_NOINIT) return RES_NOTRDY;
	if(Stat & STA_PROTECT) return RES_WRPRT;

	if( !(CardType & CT_BLOCK) ) sector *= sectSize;	// Convert to byte address if needed

	if(count == 1) {		// Single block write
		if( (send_cmd(CMD24, sector) == 0) && trmt_datablock(buff, 0xFE) ) count = 0;	// WRITE_BLOCK
	}
	else {				// Multiple block write
        if(CardType & CT_SDC) send_cmd(ACMD23, count);
		if(send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if( !trmt_datablock(buff, 0xFC) ) break;
				buff += sectSize;
			} while(--count);
			if( !trmt_datablock(0, 0xFD) ) 	/* STOP_TRAN token */
				count = 1;
		}
	}
	deselect();

	#endif

	return count ? RES_ERROR : RES_OK;
}
#endif


// Miscellaneous Functions
DRESULT sd_ioctl (
	BYTE pdrv,		// Physical drive number (0)
	BYTE cmd,		// Control code
	void *buff		// Buffer to send/receive data block
) {

	if(pdrv) return RES_PARERR;
	if(Stat & STA_NOINIT) return RES_NOTRDY;
	DRESULT res = RES_ERROR;

	#ifndef SANDBOX

	BYTE n, *ptr = buff;
    DWORD csz, dwbuf[4];
    BYTE *csd = (BYTE *) dwbuf;

	switch (cmd) {

		case GET_SECTOR_SIZE :  // Get sector size (WORD)
            sectSize = FF_MIN_SS;
            *(WORD*) buff = (WORD) sectSize;
            res = RES_OK;
            break;

        case GET_SECTOR_COUNT :	// Get number of sectors on the disk (WORD
            if( (send_cmd(CMD9, 0) == 0) && recv_datablock(csd, 16) ) {
                if((csd[0] >> 6) == 1) {	// SDv2
                    csz = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
                    *(DWORD*)buff = csz << 10;
                } else {					// SDv1 or MMCv3
                    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                    csz = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                    *(DWORD*)buff = csz << (n - 9);
                }
                res = RES_OK;
            }
            break;

        case GET_BLOCK_SIZE :	// Get erase block size in unit of sectors (DWORD)
            if(CardType & CT_SD2) {	// SDv2
                if(send_cmd(ACMD13, 0) == 0) {		// Read SD status
                    xchg_spi(0xFF);
                    if(recv_datablock(csd, 16)) {	// Read partial block
                        for(n = 64 - 16; n; n--) xchg_spi(0xFF);	// Purge trailing data
                        *(DWORD*) buff = 16UL << (csd[10] >> 4);
                        res = RES_OK;
                    }
                }
            } else {					// SDv1 or MMCv3
                if((send_cmd(CMD9, 0) == 0) && recv_datablock(csd, 16)) {	// Read CSD
                    if(CardType & CT_SD1) {	// SDv1
                        *(DWORD*) buff = (((csd[10] & 63) << 1) +
										((WORD) (csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
                    } else {					// MMCv3
                        *(DWORD*) buff = ((WORD) ((csd[10] & 124) >> 2) + 1) *
										(((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
                    }
                    res = RES_OK;
                }
            }
            break;

        case MMC_GET_TYPE :		// Get card type flags (1 byte)
            *ptr = CardType;
            res = RES_OK;
            break;

        case MMC_GET_CSD :	// Receive CSD as a data block (16 bytes)
            if( (send_cmd(CMD9, 0) == 0) && (recv_datablock(buff, 16)) ) res = RES_OK;	// READ_CSD
            break;

        case MMC_GET_CID :	// Receive CID as a data block (16 bytes)
            if( (send_cmd(CMD10, 0) == 0) && (recv_datablock(buff, 16)) ) res = RES_OK;	// READ_CID
            break;

        case MMC_GET_OCR :	// Receive OCR as an R3 resp (4 bytes)
            if(send_cmd(CMD58, 0) == 0) {	// READ_OCR
                for(n = 0; n < 4; n++) *((BYTE*) buff+n) = xchg_spi(0xFF);
                res = RES_OK;
            }
            break;

        case MMC_GET_SDSTAT :	// Receive SD status as a data block (64 bytes)
            if( (CardType & CT_SD2) && (send_cmd(ACMD13, 0) == 0) ) {	// SD_STATUS
                xchg_spi(0xFF);
                if(recv_datablock(buff, 64)) res = RES_OK;
            }
            break;

		case CTRL_SYNC :	// Flush write-back cache, Wait for end of internal process
            if( select() ) res = RES_OK;
            break;

        default:
            res = RES_PARERR;

	}
	deselect();

	#endif

	return res;
}
