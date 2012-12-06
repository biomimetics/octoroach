from lib import command
from struct import pack,unpack
import time

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
    command.GET_IMU_LOOP_ZGYRO:     '='+2*'Lhhh', \
    command.SET_MOVE_QUEUE:         '', \
    command.SET_STEERING_GAINS:     '6h', \
    command.SOFTWARE_RESET:         '', \
    command.SPECIAL_TELEMETRY:      '=LL'+13*'h'+'f'+'hhf'+'f'+'LL'+'h', \
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
    #(src_addr, ) = unpack('H', packet.get('source_addr'))
    #id = packet.get('id')
    #options = ord(packet.get('options'))
   
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
            print "Set PID gains"
            gains = unpack(pattern, data)
            print gains
            shared.motor_gains_set = True 
        # SET_STEERING_GAINS
        elif type == command.SET_STEERING_GAINS:
            print "Set Steering gains"
            gains = unpack(pattern, data)
            print gains
            shared.steering_gains_set = True
        # SET_CTRLD_TURN_RATE
        elif type == command.SET_CTRLD_TURN_RATE:
            print "Set turning rate"
            rate = unpack(pattern, data)[0]
            print "degrees: ",shared.count2deg * rate
            print "counts: ", rate
            shared.steering_rate_set = True
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
            telem_index = datum.pop(0)
            #print "Special Telemetry Data Packet #",telem_index
            if (datum[0] != -1) and (telem_index) >= 0:
                shared.imudata[telem_index] = datum
                shared.bytesIn = shared.bytesIn + (2*4 + 15*2)
        # ERASE_SECTORS
        elif type == command.ERASE_SECTORS:
            datum = unpack(pattern, data)
            #if datum[0] == 0:
            #    shared.flash_erased = True
            shared.flash_erased = datum[0]
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
            #print "whoami:",status, hex(type), data
            print "whoami:",data
            shared.robotQueried = True
        # SET_TAIL_GAINS
        elif type == command.SET_TAIL_GAINS:
            print "Set Tail gains"
            gains = unpack(pattern, data)
            print gains
            shared.tail_gains_set = True
        else:    
            pass
    
    except Exception as args:
        print "\nGeneral exception from callbackfunc:",args
        print "Attemping to exit cleanly..."
        self.xb.halt()
        self.ser.close()
        sys.exit()


