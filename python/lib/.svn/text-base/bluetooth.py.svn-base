#!/usr/bin/env python
"""
Bluetooth module
author: stanbaek
Modification on 2010-01-20:
    readInteger can read mutiple integers at a time.

"""

import wx, time, os, sys
from serial import *

#ser = serial.Serial('/dev/tty.Stan1-SPP-1', 230400, timeout=15, rtscts=1)

class Bluetooth(Serial):
    
    def __init__(self, *args, **kwargs):

        # __init__(port=None, baudrate=9600, bytesize=EIGHTBITS, 
        # parity=PARITY_NONE, stopbits=STOPBITS_ONE, timeout=None, 
        # xonxoff=0, rtscts=0, interCharTimeout=None)

        # the max buadrate is 230400
        # baudrate = kwargs.get('baudrate', 230400)
        # if baudrate > 230400: baudrate = 230400
        # kwargs['baudrate'] = baudrate

        # ensure that a reasonable timeout is set, blocking is no good
        # timeout = kwargs.get('timeout',1)
        # if timeout < 0.1: timeout = 1
        # kwargs['timeout'] = timeout
        
        super(Bluetooth, self).__init__(*args, **kwargs)
       

    def readInteger(self, size = 1):
        bytes = self.read(size*2)
        ret = []
        for i in range(size):
            lowerbyte = ord(bytes[2*i])
            upperbyte = ord(bytes[2*i+1])
            value = lowerbyte + ((upperbyte << 8) & 0xff00)
            ret.append(value)
        return ret

    def handshake(self):
        self.flushInput()
        self.flushOutput()
            
        self.read(self.inWaiting()) # read out all the garbage data if any.
        
        # self.write('X') # send a dummy char
        self.write('###')

        if self.read() == "#":
            return True
        else:
            return False


