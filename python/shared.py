#Base station
BS_COMPORT = 'COM3'
BS_BAUDRATE = 230400
#XBee
#BS_COMPORT = 'COM5'
#BS_BAUDRATE = 57600

motor_gains_set = False
steering_gains_set = False
steering_rate_set = False
flash_erased = 0
pkts = 0
#count2deg = 2000.0/(2**15-1)  WRONG
count2deg = 1/14.375
deg2count = 14.375
bytesIn = 0

last_packet_time = 0
readback_timeout = 2 #seconds

awake = True;

imudata = []
statedata = []
dutycycles = []