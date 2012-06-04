BS_COMPORT = 'COM3'
BS_BAUDRATE = 230400

motor_gains_set = False
steering_gains_set = False
steering_rate_set = False
pkts = 0
count2deg = 2000.0/(2**15-1)

imudata = []
statedata = []
dutycycles = []
