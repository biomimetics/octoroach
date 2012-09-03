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
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1290873929/xl.o ${OBJECTDIR}/_ext/1290873929/battery.o ${OBJECTDIR}/_ext/1290873929/stopwatch.o ${OBJECTDIR}/_ext/1290873929/packet_queue.o ${OBJECTDIR}/_ext/1290873929/queue.o ${OBJECTDIR}/_ext/1290873929/at86rf.o ${OBJECTDIR}/_ext/1290873929/payload.o ${OBJECTDIR}/_ext/1290873929/ipspi1.o ${OBJECTDIR}/_ext/1290873929/radio.o ${OBJECTDIR}/_ext/1290873929/payload_queue.o ${OBJECTDIR}/_ext/1290873929/init_default.o ${OBJECTDIR}/_ext/1290873929/dfmem.o ${OBJECTDIR}/_ext/1290873929/i2c_driver.o ${OBJECTDIR}/_ext/1290873929/dfilter_avg.o ${OBJECTDIR}/_ext/1290873929/gyro.o ${OBJECTDIR}/_ext/1290873929/pid_hw.o ${OBJECTDIR}/_ext/1290873929/pid.o ${OBJECTDIR}/_ext/1290873929/version.o ${OBJECTDIR}/_ext/1290873929/delay.o ${OBJECTDIR}/_ext/1360930230/adc_pid.o ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o ${OBJECTDIR}/_ext/1360930230/traps.o ${OBJECTDIR}/_ext/1360930230/move_queue.o ${OBJECTDIR}/_ext/1360930230/sensors.o ${OBJECTDIR}/_ext/1360930230/estop.o ${OBJECTDIR}/_ext/1360930230/telem.o ${OBJECTDIR}/_ext/1360930230/steering.o ${OBJECTDIR}/_ext/1360930230/leg_ctrl.o ${OBJECTDIR}/_ext/1360930230/tail_queue.o ${OBJECTDIR}/_ext/1360930230/tail_ctrl.o ${OBJECTDIR}/_ext/1360930230/sys_service.o ${OBJECTDIR}/_ext/1360930230/hall.o ${OBJECTDIR}/_ext/1360930230/ams-enc.o ${OBJECTDIR}/_ext/1360930230/imu.o ${OBJECTDIR}/source/cmd.o ${OBJECTDIR}/source/main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1290873929/xl.o.d ${OBJECTDIR}/_ext/1290873929/battery.o.d ${OBJECTDIR}/_ext/1290873929/stopwatch.o.d ${OBJECTDIR}/_ext/1290873929/packet_queue.o.d ${OBJECTDIR}/_ext/1290873929/queue.o.d ${OBJECTDIR}/_ext/1290873929/at86rf.o.d ${OBJECTDIR}/_ext/1290873929/payload.o.d ${OBJECTDIR}/_ext/1290873929/ipspi1.o.d ${OBJECTDIR}/_ext/1290873929/radio.o.d ${OBJECTDIR}/_ext/1290873929/payload_queue.o.d ${OBJECTDIR}/_ext/1290873929/init_default.o.d ${OBJECTDIR}/_ext/1290873929/dfmem.o.d ${OBJECTDIR}/_ext/1290873929/i2c_driver.o.d ${OBJECTDIR}/_ext/1290873929/dfilter_avg.o.d ${OBJECTDIR}/_ext/1290873929/gyro.o.d ${OBJECTDIR}/_ext/1290873929/pid_hw.o.d ${OBJECTDIR}/_ext/1290873929/pid.o.d ${OBJECTDIR}/_ext/1290873929/version.o.d ${OBJECTDIR}/_ext/1290873929/delay.o.d ${OBJECTDIR}/_ext/1360930230/adc_pid.o.d ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d ${OBJECTDIR}/_ext/1360930230/traps.o.d ${OBJECTDIR}/_ext/1360930230/move_queue.o.d ${OBJECTDIR}/_ext/1360930230/sensors.o.d ${OBJECTDIR}/_ext/1360930230/estop.o.d ${OBJECTDIR}/_ext/1360930230/telem.o.d ${OBJECTDIR}/_ext/1360930230/steering.o.d ${OBJECTDIR}/_ext/1360930230/leg_ctrl.o.d ${OBJECTDIR}/_ext/1360930230/tail_queue.o.d ${OBJECTDIR}/_ext/1360930230/tail_ctrl.o.d ${OBJECTDIR}/_ext/1360930230/sys_service.o.d ${OBJECTDIR}/_ext/1360930230/hall.o.d ${OBJECTDIR}/_ext/1360930230/ams-enc.o.d ${OBJECTDIR}/_ext/1360930230/imu.o.d ${OBJECTDIR}/source/cmd.o.d ${OBJECTDIR}/source/main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1290873929/xl.o ${OBJECTDIR}/_ext/1290873929/battery.o ${OBJECTDIR}/_ext/1290873929/stopwatch.o ${OBJECTDIR}/_ext/1290873929/packet_queue.o ${OBJECTDIR}/_ext/1290873929/queue.o ${OBJECTDIR}/_ext/1290873929/at86rf.o ${OBJECTDIR}/_ext/1290873929/payload.o ${OBJECTDIR}/_ext/1290873929/ipspi1.o ${OBJECTDIR}/_ext/1290873929/radio.o ${OBJECTDIR}/_ext/1290873929/payload_queue.o ${OBJECTDIR}/_ext/1290873929/init_default.o ${OBJECTDIR}/_ext/1290873929/dfmem.o ${OBJECTDIR}/_ext/1290873929/i2c_driver.o ${OBJECTDIR}/_ext/1290873929/dfilter_avg.o ${OBJECTDIR}/_ext/1290873929/gyro.o ${OBJECTDIR}/_ext/1290873929/pid_hw.o ${OBJECTDIR}/_ext/1290873929/pid.o ${OBJECTDIR}/_ext/1290873929/version.o ${OBJECTDIR}/_ext/1290873929/delay.o ${OBJECTDIR}/_ext/1360930230/adc_pid.o ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o ${OBJECTDIR}/_ext/1360930230/traps.o ${OBJECTDIR}/_ext/1360930230/move_queue.o ${OBJECTDIR}/_ext/1360930230/sensors.o ${OBJECTDIR}/_ext/1360930230/estop.o ${OBJECTDIR}/_ext/1360930230/telem.o ${OBJECTDIR}/_ext/1360930230/steering.o ${OBJECTDIR}/_ext/1360930230/leg_ctrl.o ${OBJECTDIR}/_ext/1360930230/tail_queue.o ${OBJECTDIR}/_ext/1360930230/tail_ctrl.o ${OBJECTDIR}/_ext/1360930230/sys_service.o ${OBJECTDIR}/_ext/1360930230/hall.o ${OBJECTDIR}/_ext/1360930230/ams-enc.o ${OBJECTDIR}/_ext/1360930230/imu.o ${OBJECTDIR}/source/cmd.o ${OBJECTDIR}/source/main.o


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
ProjectDir="C:\Users\pullin\Desktop\GIT code\octoroach\firmware"
ConfName=default
ImagePath="dist\default\${IMAGE_TYPE}\firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ImageDir="dist\default\${IMAGE_TYPE}"
ImageName="firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"

.build-conf:  .pre ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33FJ128MC706A
MP_LINKER_FILE_OPTION=,--script=p33FJ128MC706A.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1290873929/xl.o: ../../imageproc-lib/xl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/xl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/xl.c  -o ${OBJECTDIR}/_ext/1290873929/xl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/xl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/xl.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/battery.o: ../../imageproc-lib/battery.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/battery.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/battery.c  -o ${OBJECTDIR}/_ext/1290873929/battery.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/battery.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/battery.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/stopwatch.o: ../../imageproc-lib/stopwatch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/stopwatch.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/stopwatch.c  -o ${OBJECTDIR}/_ext/1290873929/stopwatch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/stopwatch.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/stopwatch.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/packet_queue.o: ../../imageproc-lib/packet_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/packet_queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/packet_queue.c  -o ${OBJECTDIR}/_ext/1290873929/packet_queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/packet_queue.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/packet_queue.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/queue.o: ../../imageproc-lib/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/queue.c  -o ${OBJECTDIR}/_ext/1290873929/queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/queue.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/queue.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/at86rf.o: ../../imageproc-lib/at86rf.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/at86rf.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/at86rf.c  -o ${OBJECTDIR}/_ext/1290873929/at86rf.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/at86rf.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/at86rf.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/payload.o: ../../imageproc-lib/payload.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/payload.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/payload.c  -o ${OBJECTDIR}/_ext/1290873929/payload.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/payload.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/payload.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/ipspi1.o: ../../imageproc-lib/ipspi1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/ipspi1.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/ipspi1.c  -o ${OBJECTDIR}/_ext/1290873929/ipspi1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/ipspi1.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/ipspi1.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/radio.o: ../../imageproc-lib/radio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/radio.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/radio.c  -o ${OBJECTDIR}/_ext/1290873929/radio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/radio.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/radio.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/payload_queue.o: ../../imageproc-lib/payload_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/payload_queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/payload_queue.c  -o ${OBJECTDIR}/_ext/1290873929/payload_queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/payload_queue.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/payload_queue.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/init_default.o: ../../imageproc-lib/init_default.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/init_default.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/init_default.c  -o ${OBJECTDIR}/_ext/1290873929/init_default.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/init_default.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/init_default.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/dfmem.o: ../../imageproc-lib/dfmem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/dfmem.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/dfmem.c  -o ${OBJECTDIR}/_ext/1290873929/dfmem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/dfmem.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/dfmem.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/i2c_driver.o: ../../imageproc-lib/i2c_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/i2c_driver.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/i2c_driver.c  -o ${OBJECTDIR}/_ext/1290873929/i2c_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/i2c_driver.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/i2c_driver.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/dfilter_avg.o: ../../imageproc-lib/dfilter_avg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/dfilter_avg.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/dfilter_avg.c  -o ${OBJECTDIR}/_ext/1290873929/dfilter_avg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/dfilter_avg.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/dfilter_avg.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/gyro.o: ../../imageproc-lib/gyro.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/gyro.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/gyro.c  -o ${OBJECTDIR}/_ext/1290873929/gyro.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/gyro.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/gyro.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/pid_hw.o: ../../imageproc-lib/pid_hw.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/pid_hw.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/pid_hw.c  -o ${OBJECTDIR}/_ext/1290873929/pid_hw.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/pid_hw.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -D__IMAGEPROC2 -DPID_HARDWARE -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/pid_hw.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/pid.o: ../../imageproc-lib/pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/pid.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/pid.c  -o ${OBJECTDIR}/_ext/1290873929/pid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/pid.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -mlarge-data -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -D__IMAGEPROC2 -DPID_HARDWARE -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/pid.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/version.o: ../../imageproc-lib/version.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/version.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/version.c  -o ${OBJECTDIR}/_ext/1290873929/version.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/version.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/version.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/adc_pid.o: ../lib/adc_pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/adc_pid.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/adc_pid.c  -o ${OBJECTDIR}/_ext/1360930230/adc_pid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/adc_pid.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/adc_pid.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/motor_ctrl.o: ../lib/motor_ctrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/motor_ctrl.c  -o ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/traps.o: ../lib/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/traps.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/traps.c  -o ${OBJECTDIR}/_ext/1360930230/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/traps.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/traps.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/move_queue.o: ../lib/move_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/move_queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/move_queue.c  -o ${OBJECTDIR}/_ext/1360930230/move_queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/move_queue.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/move_queue.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/sensors.o: ../lib/sensors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/sensors.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/sensors.c  -o ${OBJECTDIR}/_ext/1360930230/sensors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/sensors.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/sensors.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/estop.o: ../lib/estop.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/estop.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/estop.c  -o ${OBJECTDIR}/_ext/1360930230/estop.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/estop.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/estop.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/telem.o: ../lib/telem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/telem.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/telem.c  -o ${OBJECTDIR}/_ext/1360930230/telem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/telem.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/telem.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/steering.o: ../lib/steering.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/steering.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/steering.c  -o ${OBJECTDIR}/_ext/1360930230/steering.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/steering.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -fast-math -mlarge-data -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/steering.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/leg_ctrl.o: ../lib/leg_ctrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/leg_ctrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/leg_ctrl.c  -o ${OBJECTDIR}/_ext/1360930230/leg_ctrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/leg_ctrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -D__IMAGEPROC2 -DPID_HARDWARE -fast-math -mlarge-data -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/leg_ctrl.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/tail_queue.o: ../lib/tail_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/tail_queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/tail_queue.c  -o ${OBJECTDIR}/_ext/1360930230/tail_queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/tail_queue.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/tail_queue.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/tail_ctrl.o: ../lib/tail_ctrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/tail_ctrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/tail_ctrl.c  -o ${OBJECTDIR}/_ext/1360930230/tail_ctrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/tail_ctrl.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -mlarge-data -O0 -I".\source" -I"..\lib" -I"..\..\imageproc-lib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/tail_ctrl.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/sys_service.o: ../lib/sys_service.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/sys_service.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/sys_service.c  -o ${OBJECTDIR}/_ext/1360930230/sys_service.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/sys_service.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/sys_service.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/hall.o: ../lib/hall.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/hall.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/hall.c  -o ${OBJECTDIR}/_ext/1360930230/hall.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/hall.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/hall.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/ams-enc.o: ../lib/ams-enc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/ams-enc.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/ams-enc.c  -o ${OBJECTDIR}/_ext/1360930230/ams-enc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/ams-enc.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/ams-enc.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/imu.o: ../lib/imu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/imu.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/imu.c  -o ${OBJECTDIR}/_ext/1360930230/imu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/imu.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -D__IMAGEPROC2 -DPID_HARDWARE -fast-math -mlarge-data -O0 -I"source" -I"..\lib" -I"..\..\imageproc-lib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/imu.o.d" $(SILENT) 
	
${OBJECTDIR}/source/cmd.o: source/cmd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/source 
	@${RM} ${OBJECTDIR}/source/cmd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/cmd.c  -o ${OBJECTDIR}/source/cmd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/cmd.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/source/cmd.o.d" $(SILENT) 
	
${OBJECTDIR}/source/main.o: source/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/source 
	@${RM} ${OBJECTDIR}/source/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/main.c  -o ${OBJECTDIR}/source/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/main.o.d"        -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/source/main.o.d" $(SILENT) 
	
else
${OBJECTDIR}/_ext/1290873929/xl.o: ../../imageproc-lib/xl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/xl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/xl.c  -o ${OBJECTDIR}/_ext/1290873929/xl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/xl.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/xl.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/battery.o: ../../imageproc-lib/battery.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/battery.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/battery.c  -o ${OBJECTDIR}/_ext/1290873929/battery.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/battery.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/battery.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/stopwatch.o: ../../imageproc-lib/stopwatch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/stopwatch.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/stopwatch.c  -o ${OBJECTDIR}/_ext/1290873929/stopwatch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/stopwatch.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/stopwatch.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/packet_queue.o: ../../imageproc-lib/packet_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/packet_queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/packet_queue.c  -o ${OBJECTDIR}/_ext/1290873929/packet_queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/packet_queue.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/packet_queue.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/queue.o: ../../imageproc-lib/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/queue.c  -o ${OBJECTDIR}/_ext/1290873929/queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/queue.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/queue.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/at86rf.o: ../../imageproc-lib/at86rf.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/at86rf.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/at86rf.c  -o ${OBJECTDIR}/_ext/1290873929/at86rf.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/at86rf.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/at86rf.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/payload.o: ../../imageproc-lib/payload.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/payload.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/payload.c  -o ${OBJECTDIR}/_ext/1290873929/payload.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/payload.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/payload.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/ipspi1.o: ../../imageproc-lib/ipspi1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/ipspi1.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/ipspi1.c  -o ${OBJECTDIR}/_ext/1290873929/ipspi1.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/ipspi1.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/ipspi1.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/radio.o: ../../imageproc-lib/radio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/radio.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/radio.c  -o ${OBJECTDIR}/_ext/1290873929/radio.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/radio.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/radio.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/payload_queue.o: ../../imageproc-lib/payload_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/payload_queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/payload_queue.c  -o ${OBJECTDIR}/_ext/1290873929/payload_queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/payload_queue.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/payload_queue.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/init_default.o: ../../imageproc-lib/init_default.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/init_default.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/init_default.c  -o ${OBJECTDIR}/_ext/1290873929/init_default.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/init_default.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/init_default.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/dfmem.o: ../../imageproc-lib/dfmem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/dfmem.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/dfmem.c  -o ${OBJECTDIR}/_ext/1290873929/dfmem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/dfmem.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/dfmem.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/i2c_driver.o: ../../imageproc-lib/i2c_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/i2c_driver.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/i2c_driver.c  -o ${OBJECTDIR}/_ext/1290873929/i2c_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/i2c_driver.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/i2c_driver.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/dfilter_avg.o: ../../imageproc-lib/dfilter_avg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/dfilter_avg.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/dfilter_avg.c  -o ${OBJECTDIR}/_ext/1290873929/dfilter_avg.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/dfilter_avg.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/dfilter_avg.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/gyro.o: ../../imageproc-lib/gyro.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/gyro.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/gyro.c  -o ${OBJECTDIR}/_ext/1290873929/gyro.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/gyro.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/gyro.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/pid_hw.o: ../../imageproc-lib/pid_hw.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/pid_hw.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/pid_hw.c  -o ${OBJECTDIR}/_ext/1290873929/pid_hw.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/pid_hw.o.d"        -g -omf=elf -mlarge-data -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -D__IMAGEPROC2 -DPID_HARDWARE -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/pid_hw.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/pid.o: ../../imageproc-lib/pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/pid.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/pid.c  -o ${OBJECTDIR}/_ext/1290873929/pid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/pid.o.d"        -g -omf=elf -mlarge-data -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -D__IMAGEPROC2 -DPID_HARDWARE -msmart-io=1 -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/pid.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1290873929/version.o: ../../imageproc-lib/version.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/version.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../imageproc-lib/version.c  -o ${OBJECTDIR}/_ext/1290873929/version.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1290873929/version.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/version.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/adc_pid.o: ../lib/adc_pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/adc_pid.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/adc_pid.c  -o ${OBJECTDIR}/_ext/1360930230/adc_pid.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/adc_pid.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/adc_pid.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/motor_ctrl.o: ../lib/motor_ctrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/motor_ctrl.c  -o ${OBJECTDIR}/_ext/1360930230/motor_ctrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/motor_ctrl.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/traps.o: ../lib/traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/traps.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/traps.c  -o ${OBJECTDIR}/_ext/1360930230/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/traps.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/traps.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/move_queue.o: ../lib/move_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/move_queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/move_queue.c  -o ${OBJECTDIR}/_ext/1360930230/move_queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/move_queue.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/move_queue.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/sensors.o: ../lib/sensors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/sensors.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/sensors.c  -o ${OBJECTDIR}/_ext/1360930230/sensors.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/sensors.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/sensors.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/estop.o: ../lib/estop.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/estop.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/estop.c  -o ${OBJECTDIR}/_ext/1360930230/estop.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/estop.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/estop.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/telem.o: ../lib/telem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/telem.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/telem.c  -o ${OBJECTDIR}/_ext/1360930230/telem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/telem.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/telem.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/steering.o: ../lib/steering.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/steering.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/steering.c  -o ${OBJECTDIR}/_ext/1360930230/steering.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/steering.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -fast-math -mlarge-data -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/steering.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/leg_ctrl.o: ../lib/leg_ctrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/leg_ctrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/leg_ctrl.c  -o ${OBJECTDIR}/_ext/1360930230/leg_ctrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/leg_ctrl.o.d"        -g -omf=elf -D__IMAGEPROC2 -DPID_HARDWARE -fast-math -mlarge-data -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/leg_ctrl.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/tail_queue.o: ../lib/tail_queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/tail_queue.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/tail_queue.c  -o ${OBJECTDIR}/_ext/1360930230/tail_queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/tail_queue.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/tail_queue.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/tail_ctrl.o: ../lib/tail_ctrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/tail_ctrl.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/tail_ctrl.c  -o ${OBJECTDIR}/_ext/1360930230/tail_ctrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/tail_ctrl.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -mlarge-data -O0 -I".\source" -I"..\lib" -I"..\..\imageproc-lib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/tail_ctrl.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/sys_service.o: ../lib/sys_service.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/sys_service.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/sys_service.c  -o ${OBJECTDIR}/_ext/1360930230/sys_service.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/sys_service.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/sys_service.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/hall.o: ../lib/hall.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/hall.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/hall.c  -o ${OBJECTDIR}/_ext/1360930230/hall.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/hall.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/hall.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/ams-enc.o: ../lib/ams-enc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/ams-enc.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/ams-enc.c  -o ${OBJECTDIR}/_ext/1360930230/ams-enc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/ams-enc.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/ams-enc.o.d" $(SILENT) 
	
${OBJECTDIR}/_ext/1360930230/imu.o: ../lib/imu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1360930230 
	@${RM} ${OBJECTDIR}/_ext/1360930230/imu.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../lib/imu.c  -o ${OBJECTDIR}/_ext/1360930230/imu.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1360930230/imu.o.d"        -g -omf=elf -D__IMAGEPROC2 -DPID_HARDWARE -fast-math -mlarge-data -O0 -I"source" -I"..\lib" -I"..\..\imageproc-lib" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360930230/imu.o.d" $(SILENT) 
	
${OBJECTDIR}/source/cmd.o: source/cmd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/source 
	@${RM} ${OBJECTDIR}/source/cmd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/cmd.c  -o ${OBJECTDIR}/source/cmd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/cmd.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/source/cmd.o.d" $(SILENT) 
	
${OBJECTDIR}/source/main.o: source/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/source 
	@${RM} ${OBJECTDIR}/source/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE)  source/main.c  -o ${OBJECTDIR}/source/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/source/main.o.d"        -g -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -O0 -I"..\lib" -I"..\..\imageproc-lib" -I"source" -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/source/main.o.d" $(SILENT) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1290873929/delay.o: ../../imageproc-lib/delay.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/delay.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../imageproc-lib/delay.s  -o ${OBJECTDIR}/_ext/1290873929/delay.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -I"." -Wa,-MD,"${OBJECTDIR}/_ext/1290873929/delay.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,-g,--no-relax,-g $(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/delay.o.d"  $(SILENT) 
	
else
${OBJECTDIR}/_ext/1290873929/delay.o: ../../imageproc-lib/delay.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1290873929 
	@${RM} ${OBJECTDIR}/_ext/1290873929/delay.o.d 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../../imageproc-lib/delay.s  -o ${OBJECTDIR}/_ext/1290873929/delay.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -I"." -Wa,-MD,"${OBJECTDIR}/_ext/1290873929/delay.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g $(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1290873929/delay.o.d"  $(SILENT) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--heap=8192,--stack=512,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}\firmware.${IMAGE_TYPE}.map",--report-mem,-ldsp-elf$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DPID_HARDWARE -D__IMAGEPROC2 -Wl,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--heap=8192,--stack=512,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}\firmware.${IMAGE_TYPE}.map",--report-mem,-ldsp-elf$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/firmware.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf 
	
endif

.pre:
	@echo "--------------------------------------"
	@echo "User defined pre-build step: [python ..\..\imageproc-lib\version.py APULLIN-TAYLROACH-DEVEL;PID-HARD;STEER-HARD;TAIL-HARD]"
	@python ..\..\imageproc-lib\version.py APULLIN-TAYLROACH-DEVEL;PID-HARD;STEER-HARD;TAIL-HARD
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

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
