import serial
import select
from socket import *

sock = socket(AF_INET, SOCK_DGRAM)
sock.bind(('', 7823))
sock.setblocking(0)
sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
ser = serial.Serial("/dev/ttyAMA0", 19200)
buf = ""
while True:
	ready = select.select([sock, ser], [], [])
	for device in ready[0]:
		if device == ser:
			buf += ser.read()
			pos = buf.find("\n")
			if pos > 0:
				line = buf[:pos]
				print(line)
				buf = buf[pos+1:]
				sock.sendto(line[:-1], ('<broadcast>', 7823))
		else:
			line = sock.recv(1024)
			try:
				typ, remainder = line.split(":")
				if typ == "1":
					ser.write("d" + remainder + " ")
			except:
				pass