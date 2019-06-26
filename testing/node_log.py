import serial
import os
import time
import sys

logStarted = False
logFile = os.getcwd() + "/"


if len(sys.argv) != 2:
	raise AttributeError

portNo = "COM" + sys.argv[1]

s = serial.Serial(portNo, 115200)

while(1):
	# Read from serial port
	res = s.readline().decode()
	print(res, end="")

	# Save log to file
	if res.find("LOG:") != -1:
		# If new session is starting
		if res.find("LOG:start Node") == 0:
			logFile = os.getcwd() + "/logs/"
			nodeNo = int(res[len("LOG:start Node"):len("LOG:start Node")+1])
			logTime = int(time.time())
			logFile = logFile + str(nodeNo) + "_" + str(logTime)
			extension = ".log"
			if(os.path.exists(logFile+extension)):
				raise NameError("FileName is invalid")
			logFile += extension
			outfile = open(logFile, 'w')
			outfile.close()
			logStarted = True
		elif logStarted:
			outfile = open(logFile, 'a')
			res = "["+ str(int(time.time())) + "]" + res
			res = res.replace( "\n", "")
			res = res.replace( "\r", "")
			outfile.write(res+"\n");
			outfile.close()