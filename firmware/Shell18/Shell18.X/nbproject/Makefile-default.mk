#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../FatFs/source/diskio.c ../FatFs/source/ff.c ../FatFs/source/ffsystem.c ../FatFs/source/ffunicode.c ../FatFs/source/sd_spi.c ../main.c ../xmem.c ../console.c ../hardware.c ../ride.c ../fileops.c ../monitor.c ../memory.c ../bios.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1904903315/diskio.p1 ${OBJECTDIR}/_ext/1904903315/ff.p1 ${OBJECTDIR}/_ext/1904903315/ffsystem.p1 ${OBJECTDIR}/_ext/1904903315/ffunicode.p1 ${OBJECTDIR}/_ext/1904903315/sd_spi.p1 ${OBJECTDIR}/_ext/1472/main.p1 ${OBJECTDIR}/_ext/1472/xmem.p1 ${OBJECTDIR}/_ext/1472/console.p1 ${OBJECTDIR}/_ext/1472/hardware.p1 ${OBJECTDIR}/_ext/1472/ride.p1 ${OBJECTDIR}/_ext/1472/fileops.p1 ${OBJECTDIR}/_ext/1472/monitor.p1 ${OBJECTDIR}/_ext/1472/memory.p1 ${OBJECTDIR}/_ext/1472/bios.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1904903315/diskio.p1.d ${OBJECTDIR}/_ext/1904903315/ff.p1.d ${OBJECTDIR}/_ext/1904903315/ffsystem.p1.d ${OBJECTDIR}/_ext/1904903315/ffunicode.p1.d ${OBJECTDIR}/_ext/1904903315/sd_spi.p1.d ${OBJECTDIR}/_ext/1472/main.p1.d ${OBJECTDIR}/_ext/1472/xmem.p1.d ${OBJECTDIR}/_ext/1472/console.p1.d ${OBJECTDIR}/_ext/1472/hardware.p1.d ${OBJECTDIR}/_ext/1472/ride.p1.d ${OBJECTDIR}/_ext/1472/fileops.p1.d ${OBJECTDIR}/_ext/1472/monitor.p1.d ${OBJECTDIR}/_ext/1472/memory.p1.d ${OBJECTDIR}/_ext/1472/bios.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1904903315/diskio.p1 ${OBJECTDIR}/_ext/1904903315/ff.p1 ${OBJECTDIR}/_ext/1904903315/ffsystem.p1 ${OBJECTDIR}/_ext/1904903315/ffunicode.p1 ${OBJECTDIR}/_ext/1904903315/sd_spi.p1 ${OBJECTDIR}/_ext/1472/main.p1 ${OBJECTDIR}/_ext/1472/xmem.p1 ${OBJECTDIR}/_ext/1472/console.p1 ${OBJECTDIR}/_ext/1472/hardware.p1 ${OBJECTDIR}/_ext/1472/ride.p1 ${OBJECTDIR}/_ext/1472/fileops.p1 ${OBJECTDIR}/_ext/1472/monitor.p1 ${OBJECTDIR}/_ext/1472/memory.p1 ${OBJECTDIR}/_ext/1472/bios.p1

# Source Files
SOURCEFILES=../FatFs/source/diskio.c ../FatFs/source/ff.c ../FatFs/source/ffsystem.c ../FatFs/source/ffunicode.c ../FatFs/source/sd_spi.c ../main.c ../xmem.c ../console.c ../hardware.c ../ride.c ../fileops.c ../monitor.c ../memory.c ../bios.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F87K22
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1904903315/diskio.p1: ../FatFs/source/diskio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1904903315" 
	@${RM} ${OBJECTDIR}/_ext/1904903315/diskio.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1904903315/diskio.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1904903315/diskio.p1 ../FatFs/source/diskio.c 
	@-${MV} ${OBJECTDIR}/_ext/1904903315/diskio.d ${OBJECTDIR}/_ext/1904903315/diskio.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1904903315/diskio.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1904903315/ff.p1: ../FatFs/source/ff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1904903315" 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ff.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ff.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1904903315/ff.p1 ../FatFs/source/ff.c 
	@-${MV} ${OBJECTDIR}/_ext/1904903315/ff.d ${OBJECTDIR}/_ext/1904903315/ff.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1904903315/ff.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1904903315/ffsystem.p1: ../FatFs/source/ffsystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1904903315" 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ffsystem.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ffsystem.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1904903315/ffsystem.p1 ../FatFs/source/ffsystem.c 
	@-${MV} ${OBJECTDIR}/_ext/1904903315/ffsystem.d ${OBJECTDIR}/_ext/1904903315/ffsystem.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1904903315/ffsystem.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1904903315/ffunicode.p1: ../FatFs/source/ffunicode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1904903315" 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ffunicode.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ffunicode.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1904903315/ffunicode.p1 ../FatFs/source/ffunicode.c 
	@-${MV} ${OBJECTDIR}/_ext/1904903315/ffunicode.d ${OBJECTDIR}/_ext/1904903315/ffunicode.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1904903315/ffunicode.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1904903315/sd_spi.p1: ../FatFs/source/sd_spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1904903315" 
	@${RM} ${OBJECTDIR}/_ext/1904903315/sd_spi.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1904903315/sd_spi.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1904903315/sd_spi.p1 ../FatFs/source/sd_spi.c 
	@-${MV} ${OBJECTDIR}/_ext/1904903315/sd_spi.d ${OBJECTDIR}/_ext/1904903315/sd_spi.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1904903315/sd_spi.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/main.p1: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/main.p1 ../main.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/main.d ${OBJECTDIR}/_ext/1472/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/xmem.p1: ../xmem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/xmem.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/xmem.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/xmem.p1 ../xmem.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/xmem.d ${OBJECTDIR}/_ext/1472/xmem.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/xmem.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/console.p1: ../console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/console.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/console.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/console.p1 ../console.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/console.d ${OBJECTDIR}/_ext/1472/console.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/console.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/hardware.p1: ../hardware.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/hardware.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/hardware.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/hardware.p1 ../hardware.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/hardware.d ${OBJECTDIR}/_ext/1472/hardware.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/hardware.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/ride.p1: ../ride.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/ride.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ride.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/ride.p1 ../ride.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/ride.d ${OBJECTDIR}/_ext/1472/ride.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/ride.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/fileops.p1: ../fileops.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/fileops.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/fileops.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/fileops.p1 ../fileops.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/fileops.d ${OBJECTDIR}/_ext/1472/fileops.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/fileops.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/monitor.p1: ../monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/monitor.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/monitor.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/monitor.p1 ../monitor.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/monitor.d ${OBJECTDIR}/_ext/1472/monitor.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/monitor.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/memory.p1: ../memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/memory.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/memory.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/memory.p1 ../memory.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/memory.d ${OBJECTDIR}/_ext/1472/memory.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/memory.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/bios.p1: ../bios.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/bios.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/bios.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/bios.p1 ../bios.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/bios.d ${OBJECTDIR}/_ext/1472/bios.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/bios.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1904903315/diskio.p1: ../FatFs/source/diskio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1904903315" 
	@${RM} ${OBJECTDIR}/_ext/1904903315/diskio.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1904903315/diskio.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1904903315/diskio.p1 ../FatFs/source/diskio.c 
	@-${MV} ${OBJECTDIR}/_ext/1904903315/diskio.d ${OBJECTDIR}/_ext/1904903315/diskio.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1904903315/diskio.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1904903315/ff.p1: ../FatFs/source/ff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1904903315" 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ff.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ff.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1904903315/ff.p1 ../FatFs/source/ff.c 
	@-${MV} ${OBJECTDIR}/_ext/1904903315/ff.d ${OBJECTDIR}/_ext/1904903315/ff.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1904903315/ff.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1904903315/ffsystem.p1: ../FatFs/source/ffsystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1904903315" 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ffsystem.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ffsystem.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1904903315/ffsystem.p1 ../FatFs/source/ffsystem.c 
	@-${MV} ${OBJECTDIR}/_ext/1904903315/ffsystem.d ${OBJECTDIR}/_ext/1904903315/ffsystem.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1904903315/ffsystem.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1904903315/ffunicode.p1: ../FatFs/source/ffunicode.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1904903315" 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ffunicode.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1904903315/ffunicode.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1904903315/ffunicode.p1 ../FatFs/source/ffunicode.c 
	@-${MV} ${OBJECTDIR}/_ext/1904903315/ffunicode.d ${OBJECTDIR}/_ext/1904903315/ffunicode.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1904903315/ffunicode.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1904903315/sd_spi.p1: ../FatFs/source/sd_spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1904903315" 
	@${RM} ${OBJECTDIR}/_ext/1904903315/sd_spi.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1904903315/sd_spi.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1904903315/sd_spi.p1 ../FatFs/source/sd_spi.c 
	@-${MV} ${OBJECTDIR}/_ext/1904903315/sd_spi.d ${OBJECTDIR}/_ext/1904903315/sd_spi.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1904903315/sd_spi.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/main.p1: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/main.p1 ../main.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/main.d ${OBJECTDIR}/_ext/1472/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/xmem.p1: ../xmem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/xmem.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/xmem.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/xmem.p1 ../xmem.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/xmem.d ${OBJECTDIR}/_ext/1472/xmem.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/xmem.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/console.p1: ../console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/console.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/console.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/console.p1 ../console.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/console.d ${OBJECTDIR}/_ext/1472/console.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/console.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/hardware.p1: ../hardware.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/hardware.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/hardware.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/hardware.p1 ../hardware.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/hardware.d ${OBJECTDIR}/_ext/1472/hardware.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/hardware.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/ride.p1: ../ride.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/ride.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/ride.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/ride.p1 ../ride.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/ride.d ${OBJECTDIR}/_ext/1472/ride.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/ride.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/fileops.p1: ../fileops.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/fileops.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/fileops.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/fileops.p1 ../fileops.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/fileops.d ${OBJECTDIR}/_ext/1472/fileops.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/fileops.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/monitor.p1: ../monitor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/monitor.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/monitor.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/monitor.p1 ../monitor.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/monitor.d ${OBJECTDIR}/_ext/1472/monitor.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/monitor.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/memory.p1: ../memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/memory.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/memory.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/memory.p1 ../memory.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/memory.d ${OBJECTDIR}/_ext/1472/memory.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/memory.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1472/bios.p1: ../bios.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/bios.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1472/bios.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF -Os -maddrqual=require -xassembler-with-cpp -mwarn=-1 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=+psect,+class,+mem,-hex,+file  -ginhx32 -Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto     -o ${OBJECTDIR}/_ext/1472/bios.p1 ../bios.c 
	@-${MV} ${OBJECTDIR}/_ext/1472/bios.d ${OBJECTDIR}/_ext/1472/bios.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1472/bios.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: createResponseFiles
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
nbproject/$(CND_CONF).$(IMAGE_TYPE):   nbproject/Makefile-${CND_CONF}.mk    
		@cmd /C "gnuecho -n "-mcpu=$(MP_PROCESSOR_OPTION) $(MP_EXTRA_LD_PRE) -Wl,-Map=dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.map " > nbproject/default.debug"
	@cmd /C "gnuecho -n "-D__DEBUG=1 " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "-fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "-Os -maddrqual=require " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "-xassembler-with-cpp -mwarn=-1 -Wa,-a " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "-DXPRJ_default=$(CND_CONF) " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "-msummary=+psect,+class,+mem,-hex,+file " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "-ginhx32 " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "-Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "-std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "-Wl,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "-o dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} " >> nbproject/default.debug"
	@cmd /C "gnuecho -n "${OBJECTDIR}/_ext/1904903315/diskio.p1 ${OBJECTDIR}/_ext/1904903315/ff.p1 ${OBJECTDIR}/_ext/1904903315/ffsystem.p1 ${OBJECTDIR}/_ext/1904903315/ffunicode.p1 ${OBJECTDIR}/_ext/1904903315/sd_spi.p1 ${OBJECTDIR}/_ext/1472/main.p1 ${OBJECTDIR}/_ext/1472/xmem.p1 ${OBJECTDIR}/_ext/1472/console.p1 ${OBJECTDIR}/_ext/1472/hardware.p1 ${OBJECTDIR}/_ext/1472/ride.p1 ${OBJECTDIR}/_ext/1472/fileops.p1 ${OBJECTDIR}/_ext/1472/monitor.p1 ${OBJECTDIR}/_ext/1472/memory.p1 ${OBJECTDIR}/_ext/1472/bios.p1 " >> nbproject/default.debug"

	
else
nbproject/$(CND_CONF).$(IMAGE_TYPE):   nbproject/Makefile-${CND_CONF}.mk   
		@cmd /C "gnuecho -n "-mcpu=$(MP_PROCESSOR_OPTION) $(MP_EXTRA_LD_PRE) -Wl,-Map=dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.map " > nbproject/default.production"
	@cmd /C "gnuecho -n "-fno-short-double -fno-short-float -memi=bytewrite -mram=-0-2F,-100-1FF " >> nbproject/default.production"
	@cmd /C "gnuecho -n "-Os -maddrqual=require " >> nbproject/default.production"
	@cmd /C "gnuecho -n "-xassembler-with-cpp -mwarn=-1 -Wa,-a " >> nbproject/default.production"
	@cmd /C "gnuecho -n "-DXPRJ_default=$(CND_CONF) " >> nbproject/default.production"
	@cmd /C "gnuecho -n "-msummary=+psect,+class,+mem,-hex,+file " >> nbproject/default.production"
	@cmd /C "gnuecho -n "-ginhx32 " >> nbproject/default.production"
	@cmd /C "gnuecho -n "-Wl,--data-init -mkeep-startup -mdownload -mno-default-config-bits -mc90lib " >> nbproject/default.production"
	@cmd /C "gnuecho -n "-std=c99 -gdwarf-3 -mstack=hybrid:auto:auto:auto " >> nbproject/default.production"
	@cmd /C "gnuecho -n "-Wl,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml " >> nbproject/default.production"
	@cmd /C "gnuecho -n "-o dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} " >> nbproject/default.production"
	@cmd /C "gnuecho -n "${OBJECTDIR}/_ext/1904903315/diskio.p1 ${OBJECTDIR}/_ext/1904903315/ff.p1 ${OBJECTDIR}/_ext/1904903315/ffsystem.p1 ${OBJECTDIR}/_ext/1904903315/ffunicode.p1 ${OBJECTDIR}/_ext/1904903315/sd_spi.p1 ${OBJECTDIR}/_ext/1472/main.p1 ${OBJECTDIR}/_ext/1472/xmem.p1 ${OBJECTDIR}/_ext/1472/console.p1 ${OBJECTDIR}/_ext/1472/hardware.p1 ${OBJECTDIR}/_ext/1472/ride.p1 ${OBJECTDIR}/_ext/1472/fileops.p1 ${OBJECTDIR}/_ext/1472/monitor.p1 ${OBJECTDIR}/_ext/1472/memory.p1 ${OBJECTDIR}/_ext/1472/bios.p1 " >> nbproject/default.production"

	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: linkWithResponseFileStandalone
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES} nbproject/$(CND_CONF).$(IMAGE_TYPE)  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC}  @nbproject/$(CND_CONF).$(IMAGE_TYPE) 
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.hex 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES} nbproject/$(CND_CONF).$(IMAGE_TYPE)  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC}  @nbproject/$(CND_CONF).$(IMAGE_TYPE) 
	
	@echo Normalizing hex file
	@"C:/Program Files (x86)/Microchip/MPLABX/v5.30/mplab_platform/platform/../mplab_ide/modules/../../bin/hexmate" --edf="C:/Program Files (x86)/Microchip/MPLABX/v5.30/mplab_platform/platform/../mplab_ide/modules/../../dat/en_msgs.txt" dist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.hex -odist/${CND_CONF}/${IMAGE_TYPE}/Shell18.X.${IMAGE_TYPE}.hex

endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
