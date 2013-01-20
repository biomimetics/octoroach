"""
authors: apullin

Modifications and additions to this file made by Andrew Pullin are copyright, 2013
Copyrights are acknowledged for portions of this code extant before modifications by Andrew Pullin 
Any application of BSD or other license to copyright content without the authors express approval
is invalid and void.

"""

from lib import command
from struct import pack,unpack
import time, sys

import shared

#Dictionary of packet formats, for unpack()
pktFormat = { \
    command.TX_DUTY_CYCLE:          'l3f', \
    command.GET_IMU_DATA:           'l6h', \
    command.TX_SAVED_STATE_DATA:    'l3f', \
    command.SET_THRUST_OPEN_LOOP:   '', \
    command.SET_THRUST_CLOSED_LOOP: '', \
    command.SET_PID_GAINS:          '10h', \
    command.GET_PID_TELEMETRY:      '', \
    command.SET_CTRLD_TURN_RATE:    '=h', \
    command.SET_MOVE_QUEUE:         '', \
    command.SET_STEERING_GAINS:     '6h', \
    command.SOFTWARE_RESET:         '', \
    command.SPECIAL_TELEMETRY:      '=LL'+13*'h'+'fhhffLLh', \
    command.ERASE_SECTORS:          'L', \
    command.FLASH_READBACK:         '', \
    command.SLEEP:                  'b', \
    command.ECHO:                   'c' ,\
    command.SET_VEL_PROFILE:        '24h' ,\
    command.WHO_AM_I:               '', \
    command.ZERO_POS:               '=2l', \
    command.SET_HALL_GAINS:         '10h', \
    command.SET_TAIL_GAINS:         '5h' \
    }
               
#XBee callback function, called every time a packet is recieved
def xbee_received(packet):
    rf_data = packet.get('rf_data')
    #rssi = ord(packet.get('rssi'))
    (src_addr, ) = unpack('>H', packet.get('source_addr'))
    #id = packet.get('id')
    #options = ord(packet.get('options'))
    
    #Only print pertinent SRC lines
    #This also allows us to turn off messages on the fly, for telem download
    for r in shared.ROBOTS:
        if r.DEST_ADDR_int == src_addr:
            if r.VERBOSE:
                print "SRC: 0x%02X | " % src_addr,
   
    status = ord(rf_data[0])
    type = ord(rf_data[1])
    data = rf_data[2:]
    
    
    #Record the time the packet is received, so command timeouts
    # can be done
    shared.last_packet_time = time.time()
    
    try:
        pattern = pktFormat[type]
    except KeyError:
        print "Got bad packet type: ",type
        return
    
    try:
        # GET_IMU_DATA
        if type == command.GET_IMU_DATA:
            datum = unpack(pattern, data)
            if (datum[0] != -1):
                shared.imudata.append(datum)
                print "got datum:",datum
        # TX_SAVED_STATE_DATA
        elif type == command.TX_SAVED_STATE_DATA:
            datum = unpack(pattern, data)
            if (datum[0] != -1):
                statedata.append(datum)
         # TX_DUTY_CYCLE
        elif type == command.TX_DUTY_CYCLE:
            datum = unpack(pattern, data)
            if (datum[0] != -1):
                dutycycles.append(datum)
                
        # ECHO
        elif type == command.ECHO:
            print "echo: status = ",status," type=",type," data = ",data
            
        # SET_PID_GAINS
        elif type == command.SET_PID_GAINS:
            gains = unpack(pattern, data)
            print "Set motor gains to ", gains
            for r in shared.ROBOTS:
                if r.DEST_ADDR_int == src_addr:
                    r.motor_gains_set = True
                    
        # SET_STEERING_GAINS
        elif type == command.SET_STEERING_GAINS:
            gains = unpack(pattern, data)
            print "Set steering gains to ", gains
            for r in shared.ROBOTS:
                if r.DEST_ADDR_int == src_addr:
                    r.steering_gains_set = True 
                    
        # SET_CTRLD_TURN_RATE
        elif type == command.SET_CTRLD_TURN_RATE:
            print "Set turning rate ",
            rate = unpack(pattern, data)[0]
            print ", deg: ",shared.count2deg * rate,
            print ", counts: ", rate
            for r in shared.ROBOTS:
                if r.DEST_ADDR_int == src_addr:
                    r.steering_rate_set = True 
        # GET_IMU_LOOP_ZGYRO
        elif type == command.GET_IMU_LOOP_ZGYRO:
            pp = 2;
            print "Z Gyro Data Packet"
            datum = unpack(pattern, data)
            if (datum[0] != -1):
                for i in range(pp):
                    shared.imudata.append(datum[4*i:4*(i+1)] )
        # SPECIAL_TELEMETRY
        elif type == command.SPECIAL_TELEMETRY:
            shared.pkts = shared.pkts + 1
            #print "Special Telemetry Data Packet, ",shared.pkts
            datum = unpack(pattern, data)
            datum = list(datum)
            telem_index = datum.pop(0) #pop removes this from data array
            #print "telem_index: ",telem_index
            #print "Special Telemetry Data Packet #",telem_index
            if (datum[0] != -1) and (telem_index) >= 0:
                for r in shared.ROBOTS:
                    if r.DEST_ADDR_int == src_addr:
                        r.imudata[telem_index] = datum
                
        # ERASE_SECTORS
        elif type == command.ERASE_SECTORS:
            datum = unpack(pattern, data)
            print "Erased flash for", datum[0], " samples."
            if datum[0] != 0:
                for r in shared.ROBOTS:
                    if r.DEST_ADDR_int == src_addr:
                        r.flash_erased = datum[0] 
            
        # SLEEP
        elif type == command.SLEEP:
            datum = unpack(pattern, data)
            print "Sleep reply: ",datum[0]
            if datum[0] == 0:
                shared.awake = True;
                
        # SET_HALL_GAINS
        elif type == command.SET_HALL_GAINS:
            print "Set Hall Effect PID gains"
            gains = unpack(pattern, data)
            print gains
            shared.motor_gains_set = True 
            
        # ZERO_POS
        elif type == command.ZERO_POS:
            print 'Hall zeros established; Previous motor positions:',
            motor = unpack(pattern,data)
            print motor
            
        # SET_VEL_PROFILE
        elif (type == command.SET_VEL_PROFILE):
            print "Set Velocity Profile readback:"
            temp = unpack(pattern, data)
            print temp
            
        # WHO_AM_I
        elif (type == command.WHO_AM_I):
            print "query : ",data
            for r in shared.ROBOTS:
                if r.DEST_ADDR_int == src_addr:
                    r.robot_queried = True 
            
        # SET_TAIL_GAINS
        elif type == command.SET_TAIL_GAINS:
            gains = unpack(pattern, data)
            print "Set TAIL gains to ", gains
            for r in shared.ROBOTS:
                if r.DEST_ADDR_int == src_addr:
                    r.tail_gains_set = True 
            
        else:    
            pass
    
    except Exception as args:
        print "\nGeneral exception from callbackfunc:",args
        print "Attemping to exit cleanly..."
        shared.xb.halt()
        sharedser.close()
        sys.exit()


