from __future__ import print_function
import smbus
import datetime
import itertools
from math import *
from time import sleep
from socket import *
"""
i2cbus = smbus.SMBus(1)
#Configure i2C accelerometer
accel_addr = 0x53
i2cbus.write_byte_data(accel_addr, 0x2C, 0x0F)
i2cbus.write_byte_data(accel_addr, 0x2D, 0x08)
i2cbus.write_byte_data(accel_addr, 0x31, 0x0B)
"""
def read_accel():
	factor = 0.0039
	return [1] * 4
	return [factor*(-(x^0xFFFE) if x&0x8000 else x) for x in [i2cbus.read_word_data(accel_addr, x) for x in [0x32, 0x34, 0x36]]]
#End accel cnf
#Configure Gyro
"""
gyro_addr = 0x69
i2cbus.write_byte_data(gyro_addr, 0x20, 0xFF)
i2cbus.write_byte_data(gyro_addr, 0x21, 0x00)
i2cbus.write_byte_data(gyro_addr, 0x24, 0x00)
"""
def read_gyro():
	return [1] * 4
	factor = 8.75/1000
	return [factor*(-(x^0xFFFE) if x&0x8000 else x) for x in [i2cbus.read_word_data(gyro_addr, x) for x in [0xA8, 0xAA, 0xAC]]]
#End conf gyro
def apply_rotations(rotations, vector):
	angle = atan2(vector[1], vector[2]) + rotations[0]
	mod = hypot(vector[1], vector[2])
	vector[1] = sin(angle) * mod
	vector[2] = cos(angle) * mod
	angle = atan2(vector[0], vector[2]) + rotations[1]
	mod = hypot(vector[0], vector[2])
	vector[0] = sin(angle) * mod
	vector[2] = cos(angle) * mod
	angle = atan2(vector[1], vector[0]) + rotations[2]
	mod = hypot(vector[1], vector[0])
	vector[1] = sin(angle) * mod
	vector[0] = cos(angle) * mod
	return vector
sock = socket(AF_INET, SOCK_DGRAM)
sock.bind(('', 7824))
sock.setblocking(0)
sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
i = 0
start_time = datetime.datetime.now()
fil = open("/dev/servoblaster", 'w')
last_accel = read_accel()
accel_sum = [0,0,0]
last_time = datetime.datetime.now()
last_stash = read_accel()
last_stash_time = last_time
base_speed = -1
k = 1.0/100
kp = 0
kd = 0
ki = 0
gyro_factor = 0.95
accel_integral = [0,0,0]
on_land = [0,0,0]
on_land_sum = 0
while True:
	try:
		message = sock.recv(1024)
		if message[:2] == "1:":
			data = message[2:].split()
			base_speed = int(data[0])
		elif message[:2] == "2:":
			data = message[2:].split()
			kp, kd, ki, gyro_factor = [float(x) for x in data]
			print("Controls set: %f %f %f %f" % (kp, kd, ki, gyro_factor))
	except error:
		pass
	i+=1
	"""if i % 5000 == 0:
		for j in range(0, 4):
			print("%i=%i" % (j, 100), file = fil)
		fil.flush()
		print(max_error)
		print(min_error)
		raise Exception()"""
	this_accel = read_accel()
	this_gyro = read_gyro()
	time = datetime.datetime.now()
	delta = (time - last_time).total_seconds()
	last_time = time
	gyro_projection = apply_rotations([x*delta for x in this_gyro], last_accel)
	this_combined = [gyro_projection[j] * gyro_factor + (1 - gyro_factor) * this_accel[j] for j in range(3)]
	last_accel = this_combined
	accel_sum = [accel_sum[j] + this_combined[j] for j in range(3)]
	if i % 10 == 0:
		a_tot = sqrt(sum(map(lambda x: x**2,accel_sum)))
		accel_sum = map(lambda x: degrees(asin(x / a_tot)), accel_sum)
		delta = (time - last_stash_time).total_seconds()
		accel_diff = [(accel_sum[j] - last_stash[j])/delta for j in range(3)]
		last_stash = accel_sum
		last_stash_time = time
		if base_speed == -1:
			for j in range(3):
				on_land[j] += last_stash[j]
			on_land_sum += 1
		for j in range(3):
			last_stash[j] -= on_land[j] / on_land_sum
		accel_integral = [accel_integral[j] + last_stash[j] * delta for j in range(3)]
		accel_sum = [0,0,0]
		powers = [base_speed]*4
		powers[1] += k*(last_stash[0]*kp + accel_diff[0]*kd + accel_integral[0]*ki)
		powers[2] -= k*(last_stash[0]*kp + accel_diff[0]*kd + accel_integral[0]*ki)
		powers[0] += k*(last_stash[1]*kp + accel_diff[1]*kd + accel_integral[1]*ki)
		powers[3] -= k*(last_stash[1]*kp + accel_diff[1]*kd + accel_integral[1]*ki)
		for j in range(0, 4):
			powers[j] = int(min(100, max(powers[j], 0)) * 80/100 + 120)
			if base_speed == -1:
				powers[j] = 100
				accel_integral = [0, 0, 0]
			print(powers)
			print("%i=%i" % (j, powers[j]), file = fil)
		fil.flush()
		if i % 100 == 0:
			delta = datetime.datetime.now() - start_time
			print(str(i) + " in " + str(delta) + ": " + str(i/delta.total_seconds()))
		text = "0: %i %f %f %f" % tuple(itertools.chain([i], last_stash))
		sock.sendto(text, ('<broadcast>', 7823))
	"""
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
	"""