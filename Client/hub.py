import matplotlib.pyplot as plt
import numpy
from socket import *

sock = socket(AF_INET, SOCK_DGRAM)
sock.bind(('<broadcast>', 7823))
sock.setblocking(0)

plt.legend()
plt.grid(True)
plt.ion()
t_data = []
x_data = []
y_data = []
z_data = []
while True:
    try:
        line = sock.recv(1024)
    except error:
        plt.clf()
        if len(t_data) >= 50:
            moving_avg = [sum([x_data[x] for x in xrange(y-2, y+3)])/5 for y in xrange(-48, -2)]
            plt.plot([t_data[x] for x in xrange(-48, -2)], moving_avg, color='b')
            moving_avg = [sum([y_data[x] for x in xrange(y-2, y+3)])/5 for y in xrange(-48, -2)]
            plt.plot([t_data[x] for x in xrange(-48, -2)], moving_avg, color='g')
            moving_avg = [sum([z_data[x] for x in xrange(y-2, y+3)])/5 for y in xrange(-48, -2)]
            plt.plot([t_data[x] for x in xrange(-48, -2)], moving_avg, color='r')
        plt.scatter(t_data[-50:], x_data[-50:], color='b')
        plt.scatter(t_data[-50:], y_data[-50:], color='g')
        plt.scatter(t_data[-50:], z_data[-50:], color='r')
        plt.ylim(-2, 2)
        plt.draw()
        t_data = t_data[-50:]
        x_data = x_data[-50:]
        y_data = y_data[-50:]
        z_data = z_data[-50:]
        continue
    print(line)
    typ, remainder = line.split(":")
    if typ != "0":
        continue
    data = remainder.split()
    t_data.append(int(data[0]))
    x_data.append(float(data[2]))
    y_data.append(float(data[3]))
    z_data.append(float(data[4]))
