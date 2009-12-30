import serial
import time
import sys
import os

ser = serial.Serial('/dev/ttyUSB0', 57600, timeout=1)



ser.readline()
#print 'Please reset the board and hit [Enter]'
sys.stdin.readline()

prefix = '20_'

count = 50
idx = 0
start = 0
while count > 0 : 
	msg = ser.readline()
	ts = time.time()
	if len(msg) > 0 :
		if(msg[0:7] == 'Start: ') :
			start = ts
			clock = round(float(msg[7:len(msg)])/1000000)
			count = 50
			os.system('/usr/bin/python /home/lucas/lib/mclib/SCPI/power_measurement_suite.py > output/'+prefix+str(int(clock)))
		elif(msg[0:7] == 'Stop:  ') :
			interval = ts - start
			iterations = int(msg[7:len(msg)])
			performance = (iterations/interval)/1000
			print clock, performance
	else : 
		count -= 1
