#include "hardware.h"	// for SYSRAM_xxx
#include "fileops.h"
#include "console.h"
#include "xmem.h"

#ifndef SANDBOX
	#include "hwconfig.h"	// for SYSRAM_ADDR
#endif


#ifdef INCLUDE_FILES

// print errors from FatFs
// (fe) error code
void fopsError(int8_t fe) {
    if(fe == (FRESULT) FR_OK) return;
    printf(">>> DRIVE ERROR %i: ", fe);
    switch((FRESULT) fe) {
        default: break;
        /* (-1) */ case FR_DISK_ERR: cputs("unrecoverable error occurred in the media access layer"); break;
        /* (-2) */ case FR_INT_ERR: cputs("internal error in the drive access functions"); break;
        /* (-3) */ case FR_NOT_READY: cputs("device is not ready"); break;
        /* (-4) */ case FR_NO_FILE: cputs("file is not found"); break;
        /* (-5) */ case FR_NO_PATH: cputs("path is not found"); break;
        /* (-6) */ case FR_INVALID_NAME: cputs("invalid filename"); break;
        /* (-7) */ case FR_DENIED: cputs("access denied"); break;
        /* (-8) */ case FR_EXIST: cputs("file already exists"); break;
        /* (-9) */ case FR_INVALID_OBJECT: cputs("invalid file/directory structure"); break;
        /* (-10)*/ case FR_WRITE_PROTECTED: cputs("device is write-protected"); break;
        /* (-11)*/ case FR_INVALID_DRIVE: cputs("invalid drive"); break;
        /* (-12)*/ case FR_NOT_ENABLED: cputs("device is not mounted"); break;
        /* (-13)*/ case FR_NO_FILESYSTEM: cputs("no valid file system on the device"); break;
        /* (-14)*/ case FR_MKFS_ABORTED: cputs("function aborted"); break;
        /* (-15)*/ case FR_TIMEOUT: cputs("timeout error"); break;
        /* (-16)*/ case FR_LOCKED: cputs("object is locked for processing"); break;
        /* (-17)*/ case FR_NOT_ENOUGH_CORE: cputs("not enough memory for file functions"); break;
        /* (-18)*/ case FR_TOO_MANY_OPEN_FILES: cputs("too many open files"); break;
        /* (-19)*/ case FR_INVALID_PARAMETER: cputs("invalid parameter"); break;
    }
    cputs("\r\n\n");
}


// process (*str) with the provided wildcard (*wild) and return 1, if they match, or 0 otherwise
char wcMatch(char *str, char *wild) {
    char needed = 0;
    char *ps = str, *ws = wild;
    if(!ps || !ws) return 1;
    while(*ps && *ws) {
        if( ( (*ws == '\\') || (*ws == '/') ) &&
			( (*(ws+1) == '*') || (*(ws+1) == '\\') || (*(ws+1) == '/') ) ) ws++;
        if( (*ws != '*') && (*ws != '?') ) needed = *(ws++);
        else {
            if(*ws == '?') {
                needed = *ps;
                ws++;
            } else {
                while(*ws == '*') ws++;
				if( ( (*ws == '\\') || (*ws == '/') ) &&
					( (*(ws+1) == '*') || (*(ws+1) == '\\') || (*(ws+1) == '/') ) ) ws++;
                needed = *(ws++);
                while( *ps && (*ps != needed) ) ps++;
            }
        }
        if(*(ps++) != needed) return 0;
    }
    return 1;
}


// initialise storage drive and return error code (negative) or size in bytes (positive)
// (*dev) case-sensitive device identifier (eg. "ifs:", "ram:", etc.)
int64_t fopsInit(const char *dev) {
    int64_t res =- FR_MKFS_ABORTED;
    if(!dev || (*dev == 0)) return res;
	uint8_t ftype = (FM_ANY | FM_SFD);
    uint8_t *wbuf = NULL;
    xalloc( (uint8_t **) &wbuf, FF_MAX_SS );
    if(wbuf) {
        FRESULT r = f_mkfs(dev, ftype, 0, wbuf, FF_MAX_SS);
        xfree((uint8_t **) &wbuf);
        if(r == FR_OK) {	// get drive free space after initialisation
            uint64_t cl = 0;
            FATFS *fs = &FF;
            r = f_getfree(dev, (DWORD *) &cl, &fs);
            if(r == FR_OK) {
                res = (int64_t) (cl * fs->csize *
							#if FF_MIN_SS != FF_MAX_SS
								fs->ssize
							#else
								FF_MAX_SS
							#endif
						);
            }
            else res = -((int64_t) r);
        }
        else res = -((int64_t) r);
    }
	else res=-1;
    return res;
}


// copy file
// (*cur) current file name of the file to be copied
// (*path) path and file name of the new copy
// (pf) enables progress report flag
// return the number of copied bytes or negative number as FatFs error FRESULT
int64_t fopsCopy(char *cur, char *path, uint8_t pf) {

	#ifndef COPYBUF_SIZE
		#define COPYBUF_SIZE	FF_MIN_SS
	#endif

	if( !cur || !path || (*cur == 0) || (*path == 0) || !strcmp(cur, path) ) return 0;
	if(pf) pf = 1;

	FRESULT r = FR_OK;
	uint8_t *buf = NULL;
	FIL *ff = NULL;
	xalloc( (uint8_t **) &buf, COPYBUF_SIZE );	// copy buffer
	xalloc( (uint8_t **) &ff, sizeof(FIL) );
	if(!buf || !ff) {
		xfree((uint8_t **) &buf);
		xfree((uint8_t **) &ff);
		return -FR_NOT_ENOUGH_CORE;
	}

	if( cur[0] && cur[1] && cur[2] && (cur[3] == ':') ) {
		if(r == FR_OK) r = f_chdrive((char *) cur);
		if(r == FR_OK) r = f_mount(&FF, "", 1);
	}

	if(r == FR_OK) r = f_open(ff, cur, (FA_READ | FA_OPEN_EXISTING));
	f_close(ff);

	if( path[0] && path[1] && path[2] && (path[3] == ':') ) {
		if(r == FR_OK) r = f_chdrive((char *) path);
		if(r == FR_OK) r = f_mount(&FF, "", 1);
	}

	if(r == FR_OK) r = f_open(ff, path, (FA_WRITE | FA_CREATE_ALWAYS));
	f_close(ff);

	UINT read = COPYBUF_SIZE;
	UINT written = 0;
	uint32_t cpos = 0;
	while( (r == FR_OK) && (read == COPYBUF_SIZE) ) {

		if( cur[0] && cur[1] && cur[2] && (cur[3] == ':') ) {
			if(r == FR_OK) r = f_chdrive((char *) cur);
			if(r == FR_OK) r = f_mount(&FF, "", 1);
		}

		if(r == FR_OK) r = f_open(ff, cur, (FA_READ | FA_OPEN_EXISTING));
		if(r == FR_OK) r = f_lseek(ff, cpos);
		if(r == FR_OK) {
			read = 0;
			r = f_read(ff, buf, COPYBUF_SIZE, &read);
		}
		f_close(ff);
		if( (r != FR_OK) || (read == 0) ) break;

		if( path[0] && path[1] && path[2] && (path[3] == ':') ) {
			if(r == FR_OK) r = f_chdrive((char *) path);
			if(r == FR_OK) r = f_mount(&FF, "", 1);
		}

		if(r == FR_OK) r = f_open(ff, path, (FA_WRITE | FA_OPEN_APPEND));
		if(r == FR_OK) r = f_write(ff, buf, read, &written);
		f_close(ff);
		cpos += written;
		if( (r != FR_OK) || (written != read) ) break;

		if(pf == 1) { printf("|\b"); pf++; }
		else if(pf == 2) { printf("/\b"); pf++; }
		else if(pf == 3) { printf("-\b"); pf++; }
		else if(pf == 4) { printf("\\\b"); pf = 1; }

	}

	xfree((uint8_t **) &buf);
	xfree((uint8_t **) &ff);

	if( (r == FR_OK) || (r == -FR_OK) ) return (int64_t) written;
	else return -((int64_t) r);

}


// file ops entry point
uint8_t fopsMain(uint8_t *cmd, uint8_t execf) {
	uint8_t interactive = (cmd ? 0 : 1);	// this flag will be 1 when there is no external command
	FRESULT result = FR_OK;
	char *pcmd = NULL;
	do {
		result = FR_OK;

		// print prompt and get command line
		if(interactive) {
			uint8_t *temp = NULL;
			xalloc( (uint8_t **) &temp, FF_MAX_LFN );
			if(temp) f_getcwd( (TCHAR *) temp, (FF_MAX_LFN - strlen((char *) PROMPT) - 20) );
			if(temp && *temp && (*temp != ':')) {
				strcat((char *) temp, "\r\n");
				strcat((char *) temp, (char *) PROMPT);
				cputs((char *) temp);
			}
			else cputs((char *) PROMPT);
			xfree((uint8_t **) &temp);
			getLine(NULL);
			cmd = cLine;
		}
		pcmd = (char *) cmd;
		if(*pcmd == 0) {
			if(interactive) continue;
			else break;
		}

		// exit
		if(interactive && iscmd("exit", pcmd)) break;

		// help
		else if(iscmd("help", pcmd)) {
			printf("\r\n>>> Files are executed directly by file name\r\n");
			printf(">>> Supported file system commands\r\n");
			printf("dir [-sw] [path]   Show files in directory. Wildcards * and ? are supported\r\n");
			printf("                   Switch `-a`  show all files including hidden and system\r\n");
			printf("                   Switch `-d`  show directories only\r\n");
			printf("                   Switch `-f`  show files only\r\n");
			printf("chdir  <dir_name>  Change directory. Full path and drive are supported\r\n");
			printf("mkdir  <dir_name>  Make new directory\r\n");
			printf("rmdir  <dir_name>  Remove directory. Must be empty\r\n");
			printf("init   <drive:>    Initialise drive (ifs:, ram:, sd1:, sd2:, sd3:, sd4:)\r\n");
			printf("mount  <drive:>    Optional command for mounting. Only <drive:> also works\r\n");
			printf("ramdrv [size_KB]   Get/Set size of RAM drive. Current is destroyed when setting\r\n");
			printf("list   <file>      List text file\r\n");
			printf("hlist  <file>      Hexadecimal list of binary file\r\n");
			printf("delete <file>      Delete file with specified name\r\n");
			printf("rename <file> , <new>   Rename file\r\n");
			printf("copy   <file> , <new>   Copy file\r\n");
			if(interactive) printf("exit   Exit from FOS\r\n");
			printf("\r\n");
		}

		// mount <device>
		else if( iscmd("mount", pcmd) || ( (strlen(pcmd) == 4) && (pcmd[3] == ':') ) ) {
			if(strlen(pcmd) > 4) skip(&pcmd, 5);
			result = f_chdrive(pcmd);
			if(result == FR_OK) result = f_mount(&FF, pcmd, 1);
		}

		// mkdir <dir>
		else if(iscmd("mkdir", pcmd)) {
			skip(&pcmd, 5);
			fopsError((int8_t) f_mkdir(pcmd));
		}

		// rmdir <dir>
		else if(iscmd("rmdir", pcmd)) {
			skip(&pcmd, 5);
			fopsError((int8_t) f_rmdir(pcmd));
		}

		// chdir <dir>
		else if(iscmd("chdir", pcmd)) {
			skip(&pcmd, 5);
			fopsError((int8_t) f_chdir(pcmd));
		}

		// delete <file>
		else if(iscmd("delete", pcmd)) {
			skip(&pcmd, 6);
			fopsError((int8_t) f_unlink(pcmd));
		}

		// ramdrv <sizeKB>
		else if(iscmd("ramdrv", pcmd)) {
			skip(&pcmd, 6);
			if( (*pcmd < '0') || (*pcmd > '9') ) {
				printf(">>> Free memory: %lu bytes\r\n", SYSRAM_SIZE);
				printf(">>> Drive RAM:   %lu bytes\r\n", (1024UL * RAM_SIZE_KB));
				continue;
			}
			uint32_t sz = 0;
			while( (*pcmd >= '0') && (*pcmd <= '9') ) sz = (10 * sz) + (*(pcmd++) - '0');
			if(RAM) {
				SYSRAM_SIZE += (1024UL * RAM_SIZE_KB);	// reclaim the memory allocated for RAM:
				RAM = 0;
			}
			RAM_SIZE_KB = 0;
			if(sz) {
				if( ( (1024UL * sz) >= FATFS_SIZE_MIN_KB ) &&
					( (1024UL * sz) <= (SYSRAM_SIZE - 16384) ) ) RAM_SIZE_KB = sz;
				else fopsError((int8_t) FR_INVALID_PARAMETER);
			}
		}

		// init <device>
		else if(iscmd("init", pcmd)) {
			skip(&pcmd, 4);
			result = f_chdrive(pcmd);
			if(result == FR_OK) result = f_mount(&FF, "", 1);
			if((result != FR_OK) && (result != FR_NO_FILESYSTEM)) {
				if(interactive) {
					fopsError((int8_t) result);
					continue;
				}
				else break;
			}
			if(result == FR_OK) {
				pcmd = (char *) cLine;
				f_getcwd( pcmd, (CONSOLE_LINE_LEN-1) );
				printf("WARNING: This operation will destroy ALL data on drive %s\r\nconfirm initialisation (y/n): ", pcmd);
				int k = getch();
				printf("\r\n");
				if(k != 'y' && k != 'Y') {	// confirm initialisation
					*pcmd = 0;
					if(interactive) continue;
					else break;
				}
			}
			int64_t sz = fopsInit(pcmd);
			*pcmd = 0;
			if(sz >= 0) {
				result = FR_OK;
				printf(">>> initialised size %llu bytes\r\n\n", (uint64_t) sz);
			}
			else result = -((FRESULT) sz);
			if(interactive) {
				fopsError((int8_t) result);
				continue;
			}
			else break;
		}

		// dir [<path>]
		else if(iscmd("dir", pcmd)) {
			skip(&pcmd, 3);
			uint8_t dirflags = 6;	//0b00000110;	// [.0] show all files; [.1] show files; [.2] show directories
			while(*pcmd == '-') {	// check for parameters
				pcmd++;
				if( (*pcmd == 'a') || (*pcmd == 'A') ) {
					dirflags |= BIT(0);
					skip(&pcmd, 1);
				}
				else if( (*pcmd == 'f') || (*pcmd == 'F') ) {
					dirflags &= ~BIT(2);	// disable directories
					skip(&pcmd, 1);
				}
				else if( (*pcmd == 'd') || (*pcmd == 'D') ) {
					dirflags &= ~BIT(1);	// disable files
					skip(&pcmd, 1);
				}
				else {
					result = FR_INVALID_PARAMETER;
					break;
				}
			}
			if(result != FR_OK) {
				fopsError((int8_t) result);
				continue;
			}
			char *wildcard = NULL;
			xalloc((uint8_t **) &wildcard, CONSOLE_LINE_LEN);
			if(*pcmd == 0) {	// no parameter - get the current directory
				pcmd = (char *) cLine;	// using the console buffer as directory name buffer
				f_getcwd( pcmd, (CONSOLE_LINE_LEN-1) );
			}
			else if(wildcard) {
				if( strchr((char *) pcmd, '*') || strchr((char *) pcmd, '?') ) {
					char *s = pcmd + strlen(pcmd) - 1;
					while( (s > pcmd) && (*s != '\\') && (*s != '/') ) s--;
					if( (*s == '\\') || (*s == '/') ) memcpy( wildcard, (s+1), (strlen(s+1) + 1) );
					else memcpy( wildcard, s, (strlen(s) + 1) );
					*s = 0;
					s = wildcard;
					while(*s) {
						if( (*s >= 'a') && (*s <= 'z') ) *s = 'A' + (*s - 'a');
						s++;
					}
				}
			}
			FRESULT r = FR_OK;
			DIR *dir = NULL;
			FILINFO *finfo = NULL;
			xalloc( (uint8_t **) &dir, sizeof(DIR) );
			xalloc( (uint8_t **) &finfo, sizeof(FILINFO) );
			if(dir && finfo) {
				uint64_t p1 = 0;
				uint32_t s1 = 0;
				uint32_t s2 = 0;
				if(pcmd[0] && pcmd[1] && pcmd[2] && (pcmd[3] == ':')) {
					if(r == FR_OK) r = f_chdrive(pcmd);
					if(r == FR_OK) r = f_mount(&FF, "", 1);
				}
				if(r == FR_OK) r = f_opendir(dir, pcmd);
				if(r == FR_OK) printf("\r\ndirectory %s\r\n", pcmd);
				uint8_t succf = 0;
				for(; r == FR_OK; ) {
					if(r == FR_OK) r = f_readdir(dir, finfo);
					if(r == FR_OK) succf = 1;
					if( (r != FR_OK) || (finfo->fname[0] == 0)) break;
					if( (wildcard == NULL) || wcMatch(finfo->fname, wildcard) ) {
						if(finfo->fattrib & AM_DIR) {
							s2++;
						} else {
							s1++;
							p1 = p1 + (uint64_t) finfo->fsize;
						}
						if( ((finfo->fattrib & AM_DIR) && (dirflags & BIT(2))) ||
							(((finfo->fattrib & AM_DIR) == 0) && (dirflags & BIT(1))) ||
							(((finfo->fattrib & AM_HID) || (finfo->fattrib & AM_SYS)) && (dirflags & BIT(0))) ) {
							printf("%c%c%c%c%c %4u/%02u/%02u %02u:%02u %10lu %c %-12s  %c %s",
									(finfo->fattrib & AM_DIR) ? 'D' : '-',
									(finfo->fattrib & AM_RDO) ? 'R' : '-',
									(finfo->fattrib & AM_HID) ? 'H' : '-',
									(finfo->fattrib & AM_SYS) ? 'S' : '-',
									(finfo->fattrib & AM_ARC) ? 'A' : '-',
									(finfo->fdate >> 9) + 1980, (finfo->fdate >> 5) & 15, finfo->fdate & 31,
									(finfo->ftime >> 11), (finfo->ftime >> 5) & 63, finfo->fsize,
									(finfo->fattrib & AM_DIR) ? '>' : ' ',
									finfo->fname,
									#if _USE_LFN
										(finfo->fattrib & AM_DIR) ? '>' : ' ',
										Lfname);
									#else
										' ',
										"");
									#endif
							printf("\r\n");
						}
					}
				}
				f_closedir(dir);
				if(succf) {
					printf("\r\n%4lu file(s), %12llu bytes total\r\n%4lu dir(s)", s1, (uint64_t) p1, s2);
					printf(",  wait...");
					FATFS *fs = &FF;
					if( f_getfree(pcmd, (DWORD*) &p1, &fs) == FR_OK ) {
						printf("\b\b\b\b\b\b\b\b\b\b");
						printf(",  %12llu bytes free\r\n", (uint64_t) (p1 * fs->csize *
																#if FF_MIN_SS != FF_MAX_SS
																	fs->ssize
																#else
																	FF_MAX_SS
																#endif
															)
							);
					}
					printf("\r\n\n");
				}
			}
			*pcmd = 0;
			xfree((uint8_t **) &wildcard);
			xfree((uint8_t **) &finfo);
			xfree((uint8_t **) &dir);
		}

		// rename <file> , <file>
		else if(iscmd("rename", pcmd)) {
			skip(&pcmd, 6);
			char *cf = pcmd;
			char *nf = pcmd;
			while( *nf && (*nf > ' ') && (*nf != ',') ) nf++;
			if( (*nf != ' ') && (*nf != ',') ) {	// error
				if(interactive) {
					fopsError((int8_t) FR_INVALID_PARAMETER);
					continue;
				}
				else break;
			}
			if(*nf != ',') {
				*(nf++) = 0;	// this marks the end of the 'current' file name
				while( *nf && (*nf != ',') ) nf++;
				if( *(nf++) != ',' ) {	// error
					if(interactive) {
						fopsError((int8_t) FR_INVALID_PARAMETER);
						continue;
					}
					else break;
				}
			}
			else *(nf++) = 0;
			while( *nf && (*nf == ' ') ) nf++;
			if(  (*nf <= ' ') || (*nf == ',') ) {	// error
				if(interactive) {
					fopsError((int8_t) FR_INVALID_PARAMETER);
					continue;
				}
				else break;
			}
			fopsError((int8_t) f_rename(cf, nf));
		}

		// copy <file> , <path/file>
		else if(iscmd("copy", pcmd)) {
			skip(&pcmd, 4);
			char *cf = pcmd;
			char *nf = pcmd;
			while( *nf && (*nf > ' ') && (*nf != ',') ) nf++;
			if( (*nf != ' ') && (*nf !=',') ) {	// error
				if(interactive) {
					fopsError((int8_t) FR_INVALID_PARAMETER);
					continue;
				}
				else break;
			}
			if(*nf != ',') {
				*(nf++) = 0;	// this marks the end of the 'current' file name
				while( *nf && (*nf != ',') ) nf++;
				if( *(nf++) != ',' ) {	// error
					if(interactive) {
						fopsError((int8_t) FR_INVALID_PARAMETER);
						continue;
					}
					else break;
				}
			}
			else *(nf++) = 0;
			while( *nf && (*nf == ' ') ) nf++;
			if( (*nf <= ' ') || (*nf == ',') ) {	// error
				if(interactive) {
					fopsError((int8_t) FR_INVALID_PARAMETER);
					continue;
				}
				else break;
			}
			int64_t sz = fopsCopy(cf, nf, interactive);
			if(sz < 0) result = ((FRESULT) (0 - sz));
			if(interactive) {
				if(sz > 0) printf(">>> copied %llu bytes\r\n\n", (uint64_t) sz);
				else fopsError((int8_t) result);
				continue;
			}
			else break;
		}

		// list <file>
		else if(iscmd("list", pcmd)) {
			skip(&pcmd, 4);

			FRESULT r = FR_OK;
			uint8_t *buf = NULL;
			FIL *ff = NULL;
			xalloc( (uint8_t **) &buf, COPYBUF_SIZE );	// copy buffer
			xalloc( (uint8_t **) &ff, sizeof(FIL) );

			if(buf && ff) {

				if( pcmd[0] && pcmd[1] && pcmd[2] && (pcmd[3] == ':') ) {
					if(r == FR_OK) r = f_chdrive((char *) pcmd);
					if(r == FR_OK) r = f_mount(&FF, "", 1);
				}

				uint32_t ln = 0;
				uint32_t r0, r1;
				uint8_t *p, c;
				if(r == FR_OK) r = f_open(ff, pcmd, (FA_READ | FA_OPEN_EXISTING));
				if(r == FR_OK) printf("\r\n%3lu: ", ++ln);

				while(r == FR_OK) {
					r0 = 0;
					r = f_read(ff, buf, COPYBUF_SIZE, (UINT *) &r0);
					r1 = r0;
					p = buf;
					while(r1--) {
						char k = 0;
						while(kbhit() || (k == ' ')) {
							k = getch();
							while(kbhit()) getch();
							if(k == KEY_BREAK) break;
						}
						if(k == KEY_BREAK) {
							printf("\r\n^cancel\r\n");
							ln = 0;
							break;
						}
						c = *(p++);
						if(c == 0) {
							ln = 0;
							break;
						}
						printf("%c", c);
						if(c == '\n') printf("\r%3lu: ", ++ln);
					}
					if( (ln == 0) || (r0 < COPYBUF_SIZE) ) break;
				}
				f_close(ff);

			}
			else r = -FR_NOT_ENOUGH_CORE;

			xfree((uint8_t **) &buf);
			xfree((uint8_t **) &ff);
			result = r;
			printf("\r\n\n");

		}

		// hlist <file>
		else if(iscmd("hlist", pcmd)) {
			skip(&pcmd, 5);

			FRESULT r = FR_OK;
			uint8_t *buf = NULL;
			FIL *ff = NULL;
			xalloc( (uint8_t **) &buf, COPYBUF_SIZE );	// copy buffer
			xalloc( (uint8_t **) &ff, sizeof(FIL) );

			if(buf && ff) {

				if( pcmd[0] && pcmd[1] && pcmd[2] && (pcmd[3] == ':') ) {
					if(r == FR_OK) r = f_chdrive((char *) pcmd);
					if(r == FR_OK) r = f_mount(&FF, "", 1);
				}

				uint32_t ln = 0;
				uint32_t r0, r1;
				uint8_t ch[16];	// number of bytes in line
				uint8_t *p, z, f, pc = 0;
				if(r == FR_OK) r = f_open(ff, pcmd, (FA_READ | FA_OPEN_EXISTING));
				if(r == FR_OK) printf("\r\n%06lX: ", ln);

				while(r == FR_OK) {
					r0 = 0;
					r = f_read(ff, buf, COPYBUF_SIZE, (UINT *) &r0);
					r1 = r0;
					p = buf;
					while(r1--) {
						char k = 0;
						while(kbhit() || (k == ' ')) {
							k = getch();
							while(kbhit()) getch();
							if(k == KEY_BREAK) break;
						}
						if(k == KEY_BREAK) {
							printf("\r\n^cancel\r\n");
							ln = 0;
							break;
						}
						ch[pc++] = *p;
						printf(" %02X", (int) *(p++));
						if( (r1 == 0) || (pc >= sizeof(ch)) ) {
							f = 1;
							while(pc < sizeof(ch)) {
								printf("   ");
								ch[pc++] = ' ';
								f = 0;
							}
							printf("  |  ");
							for(z = 0; z < pc; z++) {
								if(ch[z] >= ' ') putch(ch[z]);
								else putch('.');
							}
							ln += pc; pc = 0;
							if(f) printf("\r\n%06lX: ", ln);
						}
					}
					if( (ln == 0) || (r0 < COPYBUF_SIZE) ) break;
				}
				f_close(ff);

			}
			else r = -FR_NOT_ENOUGH_CORE;

			xfree((uint8_t **) &buf);
			xfree((uint8_t **) &ff);
			result = r;
			printf("\r\n\n");

		}

		// unknown command -  try to execute as file
		else if(execf && SYSRAM_SIZE) {
			uint8_t *buf = NULL;
			FIL *ff = NULL;
			xalloc( (uint8_t **) &buf, COPYBUF_SIZE );	// copy buffer
			xalloc( (uint8_t **) &ff, sizeof(FIL) );

			if(buf && ff) {
				char *params = pcmd;
				while((*params) > ' ') params++;	// skip the file name
				if(*params == ' ') {
					*(params++) = 0;	// here finishes the file name
					while((*params) == ' ') params++;	// skip the spaces after the file name
				}
				if(*params < ' ') *params = 0;

				FRESULT fr = FR_OK;
				if( pcmd[0] && pcmd[1] && pcmd[2] && (pcmd[3] == ':') ) {
					if(fr == FR_OK) fr = f_chdrive(pcmd);
					if(fr == FR_OK) fr = f_mount(&FF, "", 1);
				}
				if(fr == FR_OK) fr = f_open(ff, pcmd, (FA_READ | FA_OPEN_EXISTING));

				if(fr == FR_OK) {
					uint32_t runl = (uint32_t) f_size(ff);
					if(runl) {

						#ifndef SANDBOX

						if(runl <= SYSRAM_SIZE) {

							f_rewind(ff);
							uint32_t toread = runl;
							if(fr == FR_OK) {
								uint32_t addr = SYSRAM_ADDR;
								uint32_t trb, rdt = COPYBUF_SIZE;
								while( toread && rdt && (fr == FR_OK) ) {
									if(toread >= COPYBUF_SIZE) trb = COPYBUF_SIZE;
									else trb = toread;
									rdt = 0;
									fr = f_read(ff, buf, trb, (UINT *) &rdt);
									if(rdt) {
										memMove( addr, (DATA_RAM + (uint32_t) buf), rdt );
										toread -= trb;
										addr += trb;
									}
								}
							}
							f_close(ff);
							xfree((uint8_t **) &buf);
							xfree((uint8_t **) &ff);

							if( (fr == FR_OK) && (toread == 0) ) {
								printf("\r\n");
								if(SYSRAM_ADDR) {
									regStore();
									LCALL(SYSRAM_ADDR);
									regRestore();
								}
								printf("\r\n\n");
							}

						}
						else fr = FR_NOT_ENOUGH_CORE;

						#else

						printf("Not executable in sandbox mode\r\n\n");

						#endif

					}
				}

				f_close(ff);	// extra assurance that the file is closed in any case
				result = fr;
			}
			else result = FR_NOT_ENOUGH_CORE;

			xfree((uint8_t **) &buf);
			xfree((uint8_t **) &ff);
		}

		if(interactive) fopsError((int8_t) result);
	} while(interactive);	// will exit after the only one command or loop forever
	return (uint8_t) result;
}

#endif
