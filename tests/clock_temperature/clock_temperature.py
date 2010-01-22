import serial
import time
import sys
import os

ser = serial.Serial('/dev/ttyUSB0', 57600, timeout=1)

for board in ['b1', 'b2', 'b3', 'b4', 'b5'] :
	print 'Please connect board ', board, ', and follow the programming instructions'
	#os.system('sam3u-samba-program.py -b /home/lucas/var/sam3/apps/power_consumption_temperature/bin/test-power-consumption-sam3-at91sam3u-ek-at91sam3u4-flash.bin')

	while 1 :
		ser.readline()	
		print 'Please set the temperature, reset the board and hit [Enter]'
		sys.stdin.readline()
		count = 50
		idx = 0
		start = 0
		while count > 0 : 
			msg = ser.readline()
			if len(msg) > 0 :
				if(msg[0:7] == 'Start: ') :
					clock = round(float(msg[7:len(msg)])/1000000)
					count = 50
					os.system('/usr/bin/python /home/lucas/lib/mclib/SCPI/power_measurement_suite.py > output/tmp')
				elif(msg[0:7] == 'Stop:  ') :
					temperature = float(msg[7:len(msg)])
					os.system('mv output/tmp output/'+board+'_'+str(int(clock))+'_'+str(temperature))
			else : 
				count -= 1

		print 'Test another temperature [y/n]?'	
		reply = sys.stdin.readline()
		if reply.find('y') == -1 :
			break		


