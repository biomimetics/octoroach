#!/usr/bin/env python
"""
authors: stanbaek, apullin

"""
import numpy as np
from lib import command 
from struct import *
import time
from xbee import XBee
import serial
import glob
from math import ceil
from callbackFunc import xbee_received
import shared

DEST_ADDR = '\x20\x52'
imudata_file_name = 'imudata.txt'
#statedata_file_name = 'statedata.txt'
#dutycycle_file_name = 'dutycycle.txt'


ser = serial.Serial(shared.BS_COMPORT, shared.BS_BAUDRATE,timeout=3, rtscts=1)
xb = XBee(ser, callback = xbee_received)


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
    fileout = open(dataFileName,'w')
    #write out parameters in format which can be imported to Excel
 # updated to include accelerations
    fileout.write('"%  angrate       = ' + str(angRateDeg) + ' deg\n')
    fileout.write('"%                = ' + str(angRate) + ' raw\n')
    fileout.write('"%  motorgains    = ' + repr(motorgains) + '\n')
    fileout.write('"%  steeringGains = ' + repr(steeringGains) + '\n')
    fileout.write('"%  runtime       = ' + repr(runtime) + '\n')
    fileout.write('"%  n             = ' + repr(n) + '\n')
    fileout.write('"%  fthrust       = ' + repr(fthrust) + '\n')
    fileout.write('"% Columns: \n')
    fileout.write('"% time | Rlegs | Llegs | DCL | DCR | GyroX | GryoY | GryoZ | GryoZAvg | AX | AY | AZ | LBEMF | RBEMF | SteerOut\n')
    fileout.write('time, Rlegs, Llegs, DCL, DCR, GyroX, GryoY, GryoZ, GryoZAvg, AX, AY, AZ, LBEMF, RBEMF, SteerOut\n')
    fileout.close()


if __name__ == '__main__':
    print "experiment-ronf.py version Dec. 22, 2011\n"
    dataFileName = findFileName();
 
    print "Data file:  ", dataFileName

    print "using robot address", hex(256* ord(DEST_ADDR[0])+ ord(DEST_ADDR[1]))

    #tic = time.time()
    #xb_send(0, command.ECHO,  "abcdefgh")

    resetRobot()
    time.sleep(1)
    xb_send(0, command.WHO_AM_I, "Robot Echo")    
    time.sleep(1)

    angRateDeg = 0
    angRate = round( angRateDeg / shared.count2deg)
    while not(shared.steering_rate_set):
        print "Setting steering rate..."
        xb_send(0, command.SET_CTRLD_TURN_RATE, pack('h',angRate))
        time.sleep(1)

    motorgains = [200,2,0,2,0,    200,2,0,2,0]
    while not(shared.motor_gains_set):
        print "Setting motor gains..."
        xb_send(0, command.SET_PID_GAINS, pack('10h',*motorgains))
        time.sleep(1)

    #steeringGains = [0,0,0,0,0]
    #steeringGains = [5,1,0,1,0]
    steeringGains = [2,1,0,1,0]
    while not (shared.steering_gains_set):
        print "Setting steering gains..."
        xb_send(0, command.SET_STEERING_GAINS, pack('5h',*steeringGains))
        time.sleep(1)

    xb_send(0, command.ERASE_SECTORS, pack('h',0))
    print "started erase, 3 second dwell"
    time.sleep(3)

    runtime = 3000; #in milliseconds, whole numbers only
    n = ceil(runtime / 1000.0 * 150 + 50)
    print "Samples: ",n
    delay = 0.025
    raw_input("Press enter to start run ...")
    

    fthrust = 150
    moves = 1
    moveq = [moves, \
             fthrust, 0, runtime]

    print "started save"
    xb_send(0, command.SPECIAL_TELEMETRY, pack('h',n))
    time.sleep(0.5)
    xb_send(0, command.SET_MOVE_QUEUE, pack('=h'+moves*'hhL', *moveq))
    ###This section only for explict closed loop thrust commands
    #thrust = [f, runtime, f, runtime, 0]
    #xb_send(0, command.SET_THRUST_CLOSED_LOOP, pack('5h',*thrust))
    time.sleep((1/300.)*n + 3) #300Hz sampling, 20% overage
    raw_input("Press any key to start readback ...")
    
    print "started readback"
    xb_send(0, command.FLASH_READBACK, pack('=h',n))

    # While waiting, write parameters to start of file
    writeFileHeader(dataFileName)
  
        
    time.sleep(delay*n + 3)
    while shared.pkts != n:
        print "Retry"
        shared.imudata = []
        shared.pkts = 0
        xb_send(0, command.FLASH_READBACK, pack('=h',n))
        time.sleep(delay*n + 3)
        if shared.pkts > n:
            print "too many packets"
            break

    

    print "readback done"
    fileout = open(dataFileName, 'a')
    np.savetxt(fileout , np.array(shared.imudata), '%d', delimiter = ',')
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

    #print imudata
    
    #Old method
    #np.savetxt(dataFileName , np.array(imudata), '%d', delimiter = ',')
    #np.savetxt(dutycycle_file_name, np.array(dutycycles), '%f', delimiter = ',')
    #np.savetxt(statedata_file_name, np.array(statedata), '%f', delimiter = ',')

    print "Done"



