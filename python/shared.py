#Base station
BS_COMPORT = 'COM3'
BS_BAUDRATE = 230400
#XBee
#BS_COMPORT = 'COM6'
#BS_BAUDRATE = 57600

motor_gains_set = False
steering_gains_set = False
steering_rate_set = False
pkts = 0
count2deg = 2000.0/(2**15-1)
bytesIn = 0

last_packet_time = 0
readback_timeout = 4 #seconds

imudata = []
statedata = []
dutycycles = []
