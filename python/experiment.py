#!/usr/bin/env python
"""
authors: stanbaek, apullin

"""
import numpy as np
from lib import command 
from struct import *
import time,sys
from xbee import XBee
import serial
import glob
from math import ceil,floor
from callbackFunc import xbee_received
import shared
import datetime


DEST_ADDR = '\x20\x52'
imudata_file_name = 'imudata.txt'
#statedata_file_name = 'statedata.txt'
#dutycycle_file_name = 'dutycycle.txt'

try:
    ser = serial.Serial(shared.BS_COMPORT, shared.BS_BAUDRATE, \
                    timeout=3, rtscts=1)
except serial.serialutil.SerialException:
    print "Could not open serial port:",shared.BS_COMPORT
    sys.exit()
    
xb = XBee(ser, callback = xbee_received)

###### Operation Flags ####
SAVE_DATA = True
RESET_ROBOT = False


########## Helper functions #################
def xb_send(status, type, data):
    payload = chr(status) + chr(type) + ''.join(data)
    xb.tx(dest_addr = DEST_ADDR, data = payload)

def resetRobot():
    xb_send(0, command.SOFTWARE_RESET, pack('h',0))

def findFileName():
    filenames = glob.glob("*imudata*.txt");
    # Explicitly remove "imudata.txt", since that can mess up the pattern
    if 'imudata.txt' in filenames:
        filenames.remove('imudata.txt')
    
    if filenames == []:
        dataFileName = "imudata1.txt"
    else:
        filenames.sort()
        filenum = [int(fn[7:-4]) for fn in filenames]
        filenum.sort()
        filenum = filenum[-1] + 1
        dataFileName = "imudata" + str(filenum) + ".txt"
    return dataFileName

def writeFileHeader(dataFileName):
    global angRateDeg, angRate, motorgains, steeringGains, runtime, \
            n, moveq
    now = datetime.datetime.now()
    
    fileout = open(dataFileName,'w')
    #write out parameters
    fileout.write('% ' + now.strftime("%m/%d/%Y %H:%M") + '\n')
    fileout.write('%  Comments: \n')
    fileout.write('%  angrate       = ' + str(angRateDeg) + ' deg\n')
    fileout.write('%                = ' + str(angRate) + ' raw\n')
    fileout.write('%  motorgains    = ' + repr(motorgains) + '\n')
    fileout.write('%  steeringGains = ' + repr(steeringGains) + '\n')
    fileout.write('%  runtime       = ' + repr(runtime) + '\n')
    fileout.write('%  n             = ' + repr(n) + '\n')
    fileout.write('%  moveq         = ' + repr(moveq) + '\n')
    fileout.write('% Columns: \n')
    fileout.write('% time | Rlegs | Llegs | DCL | DCR | GyroX | GyroY | GyroZ | GryoZAvg | AccelX | AccelY |AccelZ | LBEMF | RBEMF | SteerOut\n')
    fileout.close()
    
def dlProgress(current, total):
      percent = int(100.0*current/total)
      dashes = int(floor(percent/100.0 * 45))
      stars = 45 - dashes - 1
      barstring = '|' + '-'*dashes + '>' + '*'*stars + '|'
      #sys.stdout.write("\r" + "Downloading ...%d%%   " % percent)
      sys.stdout.write("\r" + str(current).rjust(5) +"/"+ str(total).ljust(5) + "   ")
      sys.stdout.write(barstring)
      sys.stdout.flush()
########## END Helper functions ########################
    

def main():
    global angRateDeg, angRate, motorgains, steeringGains, runtime, \
            n, fthrust, moveq
            
    if SAVE_DATA:
        dataFileName = findFileName();   
        print "Data file:  ", dataFileName

    #tic = time.time()
    #xb_send(0, command.ECHO,  "abcdefgh")

    if RESET_ROBOT:
        print "Resetting robot..."
        resetRobot()
        time.sleep(1)  
    

    angRateDeg = 0
    angRate = round( angRateDeg / shared.count2deg)
    while not(shared.steering_rate_set):
        print "Setting steering rate..."
        xb_send(0, command.SET_CTRLD_TURN_RATE, pack('h',angRate))
        time.sleep(0.5)

    motorgains = [200,2,0,2,0,    200,2,0,2,0]
    while not(shared.motor_gains_set):
        print "Setting motor gains..."
        xb_send(0, command.SET_PID_GAINS, pack('10h',*motorgains))
        time.sleep(0.5)

    #steeringGains = [0,0,0,0,0]
    steeringGains = [5,1,0,1,0]
    #steeringGains = [2,1,0,1,0]
    while not (shared.steering_gains_set):
        print "Setting steering gains..."
        xb_send(0, command.SET_STEERING_GAINS, pack('5h',*steeringGains))
        time.sleep(0.5)
        

    runtime = 9000; #in milliseconds, whole numbers only
    leadinTime = 500;
    leadoutTime = 500;
    #calculate the number of telemetry packets we expect
    
    n = int(ceil(150 * (runtime + leadinTime + leadoutTime) / 1000.0))
    
    #allocate an array to write the downloaded telemetry data into
    shared.imudata = [ [] ] * n
    print "Samples: ",n
    delay = 0.025
    
    if SAVE_DATA:
        xb_send(0, command.ERASE_SECTORS, pack('H',n))
        print "started erase, 3 second dwell"
        time.sleep(3)
    
    raw_input("Press enter to start run ...")
    

    #fthrust = 0
    #moves = 1
    #moveq = [moves, \
    #         fthrust, fthrust, runtime]
    moves = 3
    moveq = [moves, \
             50, 50, 3000,
             75, 75, 3000,
             125, 125, 3000]

    if SAVE_DATA:
        print "started save"
        xb_send(0, command.SPECIAL_TELEMETRY, pack('H',n))
        time.sleep(leadinTime / 1000.0)
        
    xb_send(0, command.SET_MOVE_QUEUE, pack('=h'+moves*'hhL', *moveq))
    ###This section only for explict closed loop thrust commands
    #thrust = [f, runtime, f, runtime, 0]
    #xb_send(0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
    

    if SAVE_DATA:
        #time.sleep((1/300.)*n + 3) #300Hz sampling, 20% overage
        time.sleep((runtime + leadinTime + leadoutTime)/1000.0 + 1)
        raw_input("Press any key to start readback ...")
        print "started readback"
        xb_send(0, command.FLASH_READBACK, pack('=H',n))

        # While waiting, write parameters to start of file
        writeFileHeader(dataFileName)
  
        dlStart = time.time()
        shared.last_packet_time = dlStart
        shared.bytesIn = 0
        while shared.imudata.count([]) > 0:
            time.sleep(0.1)
            #print "Downloading ...",(n-shared.imudata.count([])),"/",n
            dlProgress(n-shared.imudata.count([]) , n)
            if (time.time() - shared.last_packet_time) > shared.readback_timeout:
                print "\nReadback timeout exceeded, restarting."
                raw_input("Press any key to start readback ...")
                print "started readback"
                dlStart = time.time()
                xb_send(0, command.FLASH_READBACK, pack('=H',n))
                
        dlEnd = time.time()
        dlProgress(n-shared.imudata.count([]) , n)
        print "\nTime: %.2f s ,  %.3f KBps" % ( (dlEnd - dlStart), \
                            shared.bytesIn / (1000*(dlEnd - dlStart)))
        
        
        print "readback done"
        fileout = open(dataFileName, 'a')
        np.savetxt(fileout , np.array(shared.imudata), '%d', delimiter = ',')
        #np.savetxt(fileout , np.array(flat_imudata), '%d', delimiter = ',')

        print "data saved to ",dataFileName

        
    print "Ctrl + C to exit"

    while True:
        try:
            time.sleep(1)
            #print ".",
        except KeyboardInterrupt:
            break

    xb.halt()
    ser.close()


    print "Done"

#Provide a try-except over the whole main function
# for clean exit. The Xbee module should have better
# provisions for handling a clean exit, but it doesn't.
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print "\nRecieved Ctrl+C, exiting."
        xb.halt()
        ser.close()
    #except Exception as args:
    #    print "\nGeneral exception:",args
    #    print "Attemping to exit cleanly..."
    #    xb.halt()
    #    ser.close()
    except serial.serialutil.SerialException:
        xb.halt()
        ser.close()



