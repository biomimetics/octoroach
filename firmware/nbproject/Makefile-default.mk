#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360930230/adc_pid.o ${OBJECTDIR}/_ext/1360930230/gyro.o ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o ${OBJECTDIR}/_ext/1360930230/move_queue.o ${OBJECTDIR}/_ext/1360930230/traps.o ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o ${OBJECTDIR}/_ext/1360930230/pid.o ${OBJECTDIR}/_ext/1360930230/steering.o ${OBJECTDIR}/_ext/1360930230/hall.o ${OBJECTDIR}/_ext/1290873929/at86rf.o ${OBJECTDIR}/_ext/1290873929/payload.o ${OBJECTDIR}/_ext/1290873929/ipspi1.o ${OBJECTDIR}/_ext/1290873929/queue.o ${OBJECTDIR}/_ext/1290873929/packet_queue.o ${OBJECTDIR}/_ext/1290873929/delay.o ${OBJECTDIR}/_ext/1290873929/stopwatch.o ${OBJECTDIR}/_ext/1290873929/battery.o ${OBJECTDIR}/_ext/1290873929/xl.o ${OBJECTDIR}/_ext/1290873929/radio.o ${OBJECTDIR}/_ext/1290873929/dfmem.o ${OBJECTDIR}/_ext/1290873929/init_default.o ${OBJECTDIR}/source/main.o ${OBJECTDIR}/source/cmd.o ${OBJECTDIR}/version.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360930230/adc_pid.o.d ${OBJECTDIR}/_ext/1360930230/gyro.o.d ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d ${OBJECTDIR}/_ext/1360930230/move_queue.o.d ${OBJECTDIR}/_ext/1360930230/traps.o.d ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.d ${OBJECTDIR}/_ext/1360930230/pid.o.d ${OBJECTDIR}/_ext/1360930230/steering.o.d ${OBJECTDIR}/_ext/1360930230/hall.o.d ${OBJECTDIR}/_ext/1290873929/at86rf.o.d ${OBJECTDIR}/_ext/1290873929/payload.o.d ${OBJECTDIR}/_ext/1290873929/ipspi1.o.d ${OBJECTDIR}/_ext/1290873929/queue.o.d ${OBJECTDIR}/_ext/1290873929/packet_queue.o.d ${OBJECTDIR}/_ext/1290873929/delay.o.d ${OBJECTDIR}/_ext/1290873929/stopwatch.o.d ${OBJECTDIR}/_ext/1290873929/battery.o.d ${OBJECTDIR}/_ext/1290873929/xl.o.d ${OBJECTDIR}/_ext/1290873929/radio.o.d ${OBJECTDIR}/_ext/1290873929/dfmem.o.d ${OBJECTDIR}/_ext/1290873929/init_default.o.d ${OBJECTDIR}/source/main.o.d ${OBJECTDIR}/source/cmd.o.d ${OBJECTDIR}/version.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360930230/adc_pid.o ${OBJECTDIR}/_ext/1360930230/gyro.o ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o ${OBJECTDIR}/_ext/1360930230/move_queue.o ${OBJECTDIR}/_ext/1360930230/traps.o ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o ${OBJECTDIR}/_ext/1360930230/pid.o ${OBJECTDIR}/_ext/1360930230/steering.o ${OBJECTDIR}/_ext/1360930230/hall.o ${OBJECTDIR}/_ext/1290873929/at86rf.o ${OBJECTDIR}/_ext/1290873929/payload.o ${OBJECTDIR}/_ext/1290873929/ipspi1.o ${OBJECTDIR}/_ext/1290873929/queue.o ${OBJECTDIR}/_ext/1290873929/packet_queue.o ${OBJECTDIR}/_ext/1290873929/delay.o ${OBJECTDIR}/_ext/1290873929/stopwatch.o ${OBJECTDIR}/_ext/1290873929/battery.o ${OBJECTDIR}/_ext/1290873929/xl.o ${OBJECTDIR}/_ext/1290873929/radio.o ${OBJECTDIR}/_ext/1290873929/dfmem.o ${OBJECTDIR}/_ext/1290873929/init_default.o ${OBJECTDIR}/source/main.o ${OBJECTDIR}/source/cmd.o ${OBJECTDIR}/version.o


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

# The following macros may be used in the pre and post step lines
Device=dsPIC33FJ128MC706A
ProjectDir=/Users/fgb/Research/Source/embedded/octoroach/firmware
ConfName=default
ImagePath=dist/default/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
ImageDir=dist/default/${IMAGE_TYPE}
ImageName=firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

.build-conf:  .pre ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ128MC706A
MP_LINKER_FILE_OPTION=,-Tp33FJ128MC706A.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1290873929/delay.o: ../../imageproc-lib/delay.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/delay.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/delay.o.ok ${OBJECTDIR}/_ext/1290873929/delay.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/delay.o.d" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE)  ../../imageproc-lib/delay.s -o ${OBJECTDIR}/_ext/1290873929/delay.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 --defsym=__MPLAB_DEBUG=1 --defsym=__ICD2RAM=1 --defsym=__DEBUG=1 --defsym=__MPLAB_DEBUGGER_PK3=1 -g  -MD "${OBJECTDIR}/_ext/1290873929/delay.o.d"$(MP_EXTRA_AS_POST)
	
else
${OBJECTDIR}/_ext/1290873929/delay.o: ../../imageproc-lib/delay.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/delay.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/delay.o.ok ${OBJECTDIR}/_ext/1290873929/delay.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/delay.o.d" $(SILENT) -c ${MP_AS} $(MP_EXTRA_AS_PRE)  ../../imageproc-lib/delay.s -o ${OBJECTDIR}/_ext/1290873929/delay.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 -g  -MD "${OBJECTDIR}/_ext/1290873929/delay.o.d"$(MP_EXTRA_AS_POST)
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360930230/adc_pid.o: ../lib/adc_pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/adc_pid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/adc_pid.o.ok ${OBJECTDIR}/_ext/1360930230/adc_pid.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/adc_pid.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/adc_pid.o.d" -o ${OBJECTDIR}/_ext/1360930230/adc_pid.o ../lib/adc_pid.c  
	
${OBJECTDIR}/_ext/1360930230/gyro.o: ../lib/gyro.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/gyro.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/gyro.o.ok ${OBJECTDIR}/_ext/1360930230/gyro.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/gyro.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/gyro.o.d" -o ${OBJECTDIR}/_ext/1360930230/gyro.o ../lib/gyro.c  
	
${OBJECTDIR}/_ext/1360930230/motor_ctrl.o: ../lib/motor_ctrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.ok ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d" -o ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o ../lib/motor_ctrl.c  
	
${OBJECTDIR}/_ext/1360930230/move_queue.o: ../lib/move_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/move_queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/move_queue.o.ok ${OBJECTDIR}/_ext/1360930230/move_queue.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/move_queue.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/move_queue.o.d" -o ${OBJECTDIR}/_ext/1360930230/move_queue.o ../lib/move_queue.c  
	
${OBJECTDIR}/_ext/1360930230/traps.o: ../lib/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/traps.o.ok ${OBJECTDIR}/_ext/1360930230/traps.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/traps.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/traps.o.d" -o ${OBJECTDIR}/_ext/1360930230/traps.o ../lib/traps.c  
	
${OBJECTDIR}/_ext/1360930230/dfmem_extra.o: ../lib/dfmem_extra.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.ok ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.d" -o ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o ../lib/dfmem_extra.c  
	
${OBJECTDIR}/_ext/1360930230/pid.o: ../lib/pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/pid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/pid.o.ok ${OBJECTDIR}/_ext/1360930230/pid.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/pid.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/pid.o.d" -o ${OBJECTDIR}/_ext/1360930230/pid.o ../lib/pid.c  
	
${OBJECTDIR}/_ext/1360930230/steering.o: ../lib/steering.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/steering.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/steering.o.ok ${OBJECTDIR}/_ext/1360930230/steering.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/steering.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/steering.o.d" -o ${OBJECTDIR}/_ext/1360930230/steering.o ../lib/steering.c  
	
${OBJECTDIR}/_ext/1360930230/hall.o: ../lib/hall.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/hall.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/hall.o.ok ${OBJECTDIR}/_ext/1360930230/hall.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/hall.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/hall.o.d" -o ${OBJECTDIR}/_ext/1360930230/hall.o ../lib/hall.c  
	
${OBJECTDIR}/_ext/1290873929/at86rf.o: ../../imageproc-lib/at86rf.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/at86rf.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/at86rf.o.ok ${OBJECTDIR}/_ext/1290873929/at86rf.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/at86rf.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/at86rf.o.d" -o ${OBJECTDIR}/_ext/1290873929/at86rf.o ../../imageproc-lib/at86rf.c  
	
${OBJECTDIR}/_ext/1290873929/payload.o: ../../imageproc-lib/payload.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/payload.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/payload.o.ok ${OBJECTDIR}/_ext/1290873929/payload.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/payload.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/payload.o.d" -o ${OBJECTDIR}/_ext/1290873929/payload.o ../../imageproc-lib/payload.c  
	
${OBJECTDIR}/_ext/1290873929/ipspi1.o: ../../imageproc-lib/ipspi1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/ipspi1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/ipspi1.o.ok ${OBJECTDIR}/_ext/1290873929/ipspi1.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/ipspi1.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/ipspi1.o.d" -o ${OBJECTDIR}/_ext/1290873929/ipspi1.o ../../imageproc-lib/ipspi1.c  
	
${OBJECTDIR}/_ext/1290873929/queue.o: ../../imageproc-lib/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/queue.o.ok ${OBJECTDIR}/_ext/1290873929/queue.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/queue.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/queue.o.d" -o ${OBJECTDIR}/_ext/1290873929/queue.o ../../imageproc-lib/queue.c  
	
${OBJECTDIR}/_ext/1290873929/packet_queue.o: ../../imageproc-lib/packet_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/packet_queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/packet_queue.o.ok ${OBJECTDIR}/_ext/1290873929/packet_queue.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/packet_queue.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/packet_queue.o.d" -o ${OBJECTDIR}/_ext/1290873929/packet_queue.o ../../imageproc-lib/packet_queue.c  
	
${OBJECTDIR}/_ext/1290873929/stopwatch.o: ../../imageproc-lib/stopwatch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/stopwatch.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/stopwatch.o.ok ${OBJECTDIR}/_ext/1290873929/stopwatch.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/stopwatch.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/stopwatch.o.d" -o ${OBJECTDIR}/_ext/1290873929/stopwatch.o ../../imageproc-lib/stopwatch.c  
	
${OBJECTDIR}/_ext/1290873929/battery.o: ../../imageproc-lib/battery.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/battery.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/battery.o.ok ${OBJECTDIR}/_ext/1290873929/battery.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/battery.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/battery.o.d" -o ${OBJECTDIR}/_ext/1290873929/battery.o ../../imageproc-lib/battery.c  
	
${OBJECTDIR}/_ext/1290873929/xl.o: ../../imageproc-lib/xl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/xl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/xl.o.ok ${OBJECTDIR}/_ext/1290873929/xl.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/xl.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/xl.o.d" -o ${OBJECTDIR}/_ext/1290873929/xl.o ../../imageproc-lib/xl.c  
	
${OBJECTDIR}/_ext/1290873929/radio.o: ../../imageproc-lib/radio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/radio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/radio.o.ok ${OBJECTDIR}/_ext/1290873929/radio.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/radio.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/radio.o.d" -o ${OBJECTDIR}/_ext/1290873929/radio.o ../../imageproc-lib/radio.c  
	
${OBJECTDIR}/_ext/1290873929/dfmem.o: ../../imageproc-lib/dfmem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/dfmem.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/dfmem.o.ok ${OBJECTDIR}/_ext/1290873929/dfmem.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/dfmem.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/dfmem.o.d" -o ${OBJECTDIR}/_ext/1290873929/dfmem.o ../../imageproc-lib/dfmem.c  
	
${OBJECTDIR}/_ext/1290873929/init_default.o: ../../imageproc-lib/init_default.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/init_default.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/init_default.o.ok ${OBJECTDIR}/_ext/1290873929/init_default.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/init_default.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/init_default.o.d" -o ${OBJECTDIR}/_ext/1290873929/init_default.o ../../imageproc-lib/init_default.c  
	
${OBJECTDIR}/source/main.o: source/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/source 
	@${RM} ${OBJECTDIR}/source/main.o.d 
	@${RM} ${OBJECTDIR}/source/main.o.ok ${OBJECTDIR}/source/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/source/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/source/main.o.d" -o ${OBJECTDIR}/source/main.o source/main.c  
	
${OBJECTDIR}/source/cmd.o: source/cmd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/source 
	@${RM} ${OBJECTDIR}/source/cmd.o.d 
	@${RM} ${OBJECTDIR}/source/cmd.o.ok ${OBJECTDIR}/source/cmd.o.err 
	@${FIXDEPS} "${OBJECTDIR}/source/cmd.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/source/cmd.o.d" -o ${OBJECTDIR}/source/cmd.o source/cmd.c  
	
${OBJECTDIR}/version.o: version.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/version.o.d 
	@${RM} ${OBJECTDIR}/version.o.ok ${OBJECTDIR}/version.o.err 
	@${FIXDEPS} "${OBJECTDIR}/version.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/version.o.d" -o ${OBJECTDIR}/version.o version.c  
	
else
${OBJECTDIR}/_ext/1360930230/adc_pid.o: ../lib/adc_pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/adc_pid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/adc_pid.o.ok ${OBJECTDIR}/_ext/1360930230/adc_pid.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/adc_pid.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/adc_pid.o.d" -o ${OBJECTDIR}/_ext/1360930230/adc_pid.o ../lib/adc_pid.c  
	
${OBJECTDIR}/_ext/1360930230/gyro.o: ../lib/gyro.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/gyro.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/gyro.o.ok ${OBJECTDIR}/_ext/1360930230/gyro.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/gyro.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/gyro.o.d" -o ${OBJECTDIR}/_ext/1360930230/gyro.o ../lib/gyro.c  
	
${OBJECTDIR}/_ext/1360930230/motor_ctrl.o: ../lib/motor_ctrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.ok ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d" -o ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o ../lib/motor_ctrl.c  
	
${OBJECTDIR}/_ext/1360930230/move_queue.o: ../lib/move_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/move_queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/move_queue.o.ok ${OBJECTDIR}/_ext/1360930230/move_queue.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/move_queue.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/move_queue.o.d" -o ${OBJECTDIR}/_ext/1360930230/move_queue.o ../lib/move_queue.c  
	
${OBJECTDIR}/_ext/1360930230/traps.o: ../lib/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/traps.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/traps.o.ok ${OBJECTDIR}/_ext/1360930230/traps.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/traps.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/traps.o.d" -o ${OBJECTDIR}/_ext/1360930230/traps.o ../lib/traps.c  
	
${OBJECTDIR}/_ext/1360930230/dfmem_extra.o: ../lib/dfmem_extra.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.ok ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/dfmem_extra.o.d" -o ${OBJECTDIR}/_ext/1360930230/dfmem_extra.o ../lib/dfmem_extra.c  
	
${OBJECTDIR}/_ext/1360930230/pid.o: ../lib/pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/pid.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/pid.o.ok ${OBJECTDIR}/_ext/1360930230/pid.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/pid.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/pid.o.d" -o ${OBJECTDIR}/_ext/1360930230/pid.o ../lib/pid.c  
	
${OBJECTDIR}/_ext/1360930230/steering.o: ../lib/steering.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/steering.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/steering.o.ok ${OBJECTDIR}/_ext/1360930230/steering.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/steering.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/steering.o.d" -o ${OBJECTDIR}/_ext/1360930230/steering.o ../lib/steering.c  
	
${OBJECTDIR}/_ext/1360930230/hall.o: ../lib/hall.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/hall.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360930230/hall.o.ok ${OBJECTDIR}/_ext/1360930230/hall.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/hall.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1360930230/hall.o.d" -o ${OBJECTDIR}/_ext/1360930230/hall.o ../lib/hall.c  
	
${OBJECTDIR}/_ext/1290873929/at86rf.o: ../../imageproc-lib/at86rf.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/at86rf.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/at86rf.o.ok ${OBJECTDIR}/_ext/1290873929/at86rf.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/at86rf.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/at86rf.o.d" -o ${OBJECTDIR}/_ext/1290873929/at86rf.o ../../imageproc-lib/at86rf.c  
	
${OBJECTDIR}/_ext/1290873929/payload.o: ../../imageproc-lib/payload.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/payload.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/payload.o.ok ${OBJECTDIR}/_ext/1290873929/payload.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/payload.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/payload.o.d" -o ${OBJECTDIR}/_ext/1290873929/payload.o ../../imageproc-lib/payload.c  
	
${OBJECTDIR}/_ext/1290873929/ipspi1.o: ../../imageproc-lib/ipspi1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/ipspi1.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/ipspi1.o.ok ${OBJECTDIR}/_ext/1290873929/ipspi1.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/ipspi1.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/ipspi1.o.d" -o ${OBJECTDIR}/_ext/1290873929/ipspi1.o ../../imageproc-lib/ipspi1.c  
	
${OBJECTDIR}/_ext/1290873929/queue.o: ../../imageproc-lib/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/queue.o.ok ${OBJECTDIR}/_ext/1290873929/queue.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/queue.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/queue.o.d" -o ${OBJECTDIR}/_ext/1290873929/queue.o ../../imageproc-lib/queue.c  
	
${OBJECTDIR}/_ext/1290873929/packet_queue.o: ../../imageproc-lib/packet_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/packet_queue.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/packet_queue.o.ok ${OBJECTDIR}/_ext/1290873929/packet_queue.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/packet_queue.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/packet_queue.o.d" -o ${OBJECTDIR}/_ext/1290873929/packet_queue.o ../../imageproc-lib/packet_queue.c  
	
${OBJECTDIR}/_ext/1290873929/stopwatch.o: ../../imageproc-lib/stopwatch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/stopwatch.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/stopwatch.o.ok ${OBJECTDIR}/_ext/1290873929/stopwatch.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/stopwatch.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/stopwatch.o.d" -o ${OBJECTDIR}/_ext/1290873929/stopwatch.o ../../imageproc-lib/stopwatch.c  
	
${OBJECTDIR}/_ext/1290873929/battery.o: ../../imageproc-lib/battery.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/battery.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/battery.o.ok ${OBJECTDIR}/_ext/1290873929/battery.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/battery.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/battery.o.d" -o ${OBJECTDIR}/_ext/1290873929/battery.o ../../imageproc-lib/battery.c  
	
${OBJECTDIR}/_ext/1290873929/xl.o: ../../imageproc-lib/xl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/xl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/xl.o.ok ${OBJECTDIR}/_ext/1290873929/xl.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/xl.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/xl.o.d" -o ${OBJECTDIR}/_ext/1290873929/xl.o ../../imageproc-lib/xl.c  
	
${OBJECTDIR}/_ext/1290873929/radio.o: ../../imageproc-lib/radio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/radio.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/radio.o.ok ${OBJECTDIR}/_ext/1290873929/radio.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/radio.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/radio.o.d" -o ${OBJECTDIR}/_ext/1290873929/radio.o ../../imageproc-lib/radio.c  
	
${OBJECTDIR}/_ext/1290873929/dfmem.o: ../../imageproc-lib/dfmem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/dfmem.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/dfmem.o.ok ${OBJECTDIR}/_ext/1290873929/dfmem.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/dfmem.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/dfmem.o.d" -o ${OBJECTDIR}/_ext/1290873929/dfmem.o ../../imageproc-lib/dfmem.c  
	
${OBJECTDIR}/_ext/1290873929/init_default.o: ../../imageproc-lib/init_default.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/init_default.o.d 
	@${RM} ${OBJECTDIR}/_ext/1290873929/init_default.o.ok ${OBJECTDIR}/_ext/1290873929/init_default.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/init_default.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/_ext/1290873929/init_default.o.d" -o ${OBJECTDIR}/_ext/1290873929/init_default.o ../../imageproc-lib/init_default.c  
	
${OBJECTDIR}/source/main.o: source/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/source 
	@${RM} ${OBJECTDIR}/source/main.o.d 
	@${RM} ${OBJECTDIR}/source/main.o.ok ${OBJECTDIR}/source/main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/source/main.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/source/main.o.d" -o ${OBJECTDIR}/source/main.o source/main.c  
	
${OBJECTDIR}/source/cmd.o: source/cmd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/source 
	@${RM} ${OBJECTDIR}/source/cmd.o.d 
	@${RM} ${OBJECTDIR}/source/cmd.o.ok ${OBJECTDIR}/source/cmd.o.err 
	@${FIXDEPS} "${OBJECTDIR}/source/cmd.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/source/cmd.o.d" -o ${OBJECTDIR}/source/cmd.o source/cmd.c  
	
${OBJECTDIR}/version.o: version.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/version.o.d 
	@${RM} ${OBJECTDIR}/version.o.ok ${OBJECTDIR}/version.o.err 
	@${FIXDEPS} "${OBJECTDIR}/version.o.d" $(SILENT) -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -D__IMAGEPROC2 -D__DFMEM_32MBIT -I"./" -I"../lib" -I"../../imageproc-lib" -MMD -MF "${OBJECTDIR}/version.o.d" -o ${OBJECTDIR}/version.o version.c  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1,--heap=2048,--stack=2048,--no-check-sections,-Map="${DISTDIR}/octoroach.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}        -Wl,--defsym=__MPLAB_BUILD=1,--heap=2048,--stack=2048,--no-check-sections,-Map="${DISTDIR}/octoroach.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}/pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
endif

.pre:
	@echo "--------------------------------------"
	@echo "User defined pre-build step: [python ../python/version.py 'octoroach with hall effect']"
	@python ../python/version.py 'octoroach with hall effect'
	@echo "--------------------------------------"

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

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
