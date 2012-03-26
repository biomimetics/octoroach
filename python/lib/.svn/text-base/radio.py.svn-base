#!/usr/bin/env python
"""
radio module
authors: stanbaek, ahoover
Modification on 2010-07-07:

"""
#ser = serial.Serial('/dev/tty.Stan1-SPP-1', 230400, timeout=15, rtscts=1)

from serial import *
import stxetx
import multiprocessing
from Queue import Empty

__all__ = ['RadioPacket', 'Radio', 'RadioProcess']
__version__ = '1.0'

CONNECT = "CONNECT"
DISCONNECT = "DISCONNECT"


CMD_NACK = chr(0x00)
MAX_RECEIVE_BUFFER_LEN = 512

WRITE_TIMEOUT = 0.4
READ_TIMEOUT = 0.5

class RadioPacket(object):
    """An object for holding a packet consisting of status, type, and data."""
    def __init__(self, status, type, data):
        self.status = status
        self.type = type
        self.data = data


class Radio(Serial):
    
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
        
        super(Radio, self).__init__(*args, **kwargs)
        self.packet_proto = stxetx.StxEtx()
        self.packet_proto.set_data_out(self.write)
        self.rx_buffer = []

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


    def send_packet(self, status, type, data):
        self.packet_proto.send(status, type, data)

    def receive_packet(self):

        if self.inWaiting():
            self.rx_buffer.extend(list(self.read(self.inWaiting())))
            # print self.rx_buffer
        if len(self.rx_buffer) > stxetx.HEADER_LENGTH + stxetx.TRAILER_LENGTH:
            found_packet, next_idx = self.packet_proto.process(
                                     self.rx_buffer[0:MAX_RECEIVE_BUFFER_LEN])

            if found_packet:
                status = self.packet_proto.get_rx_packet_status()
                type = self.packet_proto.get_rx_packet_type()
                data = self.packet_proto.get_rx_packet_data()
                del(self.rx_buffer[0:next_idx])
                return True, RadioPacket(status, type, data)

        return False, None
                


class RadioProcess(multiprocessing.Process):

    def __init__(self, rx_queue, tx_queue, connected):
        multiprocessing.Process.__init__(self)
        self.rx_queue = rx_queue
        self.tx_queue = tx_queue
        self.conn = None
        self.connected = connected

    def Disconnect(self):
        if self.conn is None:
            return False

        try:
            self.conn.close()
        except SerialException:
            pass
        finally:
            return True

    def Connect(self, port, baudrate, handshake):
        try:
            if self.conn is not None:  # already connected
                self.conn.open()
                self.conn.timeout = READ_TIMEOUT # nonzero value before handshake
            else:
                self.conn = Radio(port, baudrate, timeout=READ_TIMEOUT, rtscts=1)

            self.conn.writeTimeout = WRITE_TIMEOUT

            if handshake is True:
                if self.conn.handshake() is True:
                    
                    # jump out from bootloader and run user application
                    self.conn.write(CMD_START_APPLICATION) 
                    
                    # zero value after handshake
                    self.conn.timeout = 0  
                    print "Connected"
                    return True
                
                else:
                    self.conn = None
                    print "Handshake failed."
                    return False
            else:
                print "Connected"
                return True

        except SerialException, e:
            print "SerialException: " + e.message
            self.conn = None
            return False
        except SerialTimeoutException, e:
            print "SerialTimeoutException: " + e.message
            self.conn = None
            return False
               

    def run(self):
        print "Serial Process is running"
        success = None

        while True:
            try:
                packet = self.tx_queue.get_nowait() 
                #print "Got a packet"
                if packet.type == CONNECT:
                    handshake = packet.status
                    port = packet.data[0]
                    baudrate = packet.data[1]
                    success = self.Connect(port, baudrate, handshake)
                    if success: 
                        self.connected.value = 1
                    print "Radio is connected."
                elif packet.type == DISCONNECT:
                    self.Disconnect()
                    self.connected.value = 0
                    success = True
                    print "Radio is disconnected."
                else: # status & type must be one byte integer (0-255)
                    # Sending commands
                    if self.connected.value:
                        self.conn.send_packet(packet.status, packet.type, packet.data)
                        sucess = None
                    else:
                        print "Did not attempt to send command because " + \
                            "there is no radio connection."
                        success = False
            except Empty:
                success = None
            except SerialTimeoutException, e:
                print "Radio could not send command because of " + e.message
            except SerialException, e:
                print "Radio Error " + e.message
            except Exception as inst:
                print "Unexpected Error:" 
                print type(inst)     # the exception instance
                #print inst.args      # arguments stored in .args
                print inst    
            finally:
                if success != None:
                    self.rx_queue.put_nowait(RadioPacket(success, packet.type, ""))
                success = None

            if self.connected.value:                    
                new_packet, packet = self.conn.receive_packet()
                if new_packet:
                    self.rx_queue.put_nowait(packet)



if __name__ == '__main__':
    
    import time
    import command
    
    rx_queue = multiprocessing.JoinableQueue()
    tx_queue = multiprocessing.JoinableQueue()
    connected = multiprocessing.Value('i', 0)
    radiotalk = RadioProcess(rx_queue, tx_queue, connected)
    radiotalk.daemon = True
    radiotalk.start()

    tx_queue.put_nowait(RadioPacket(False, CONNECT, ['COM4', 230400]))

    tic = time.time()
    count = 1
    while (time.time() - tic < 5):
        #tx_queue.put_nowait(RadioPacket(0, command.ECHO, "You want a piece of me, boy?" + str(count)))
        #tx_queue.put_nowait(RadioPacket(0, command.ECHO, "Jacked up and good to go." + str(count)))
        #tx_queue.put_nowait(RadioPacket(0, command.ECHO, "Need medical attention?" + str(count))) 
        tx_queue.put_nowait(RadioPacket(0, command.ECHO, "Come again, Cap'n..." + str(count))) 
        #tx_queue.put_nowait(RadioPacket(0, command.ECHO, "How do I get out of this chickenshit outfit?" + str(count))) 
        tx_queue.put_nowait(RadioPacket(0, command.ECHO, "Rock and Roll..." + str(count))) 
        count += 1
        tx_queue.put_nowait(RadioPacket(0, command.GET_IMU_DATA, "0")) 
        try:
            packet = rx_queue.get_nowait()
            #print "... A packet received."
            print packet.status, packet.type, "".join(packet.data)
        except Empty:
            pass

    #tx_queue.join();
    tx_queue.put_nowait(RadioPacket(0, DISCONNECT, []))
    packet = rx_queue.get()
    print "... A packet received."
    print packet.status, packet.type, packet.data

    print "terminating"
    radiotalk.terminate()
    print "Done"
