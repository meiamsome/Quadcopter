import pygame
import time
import sys
import os
import math
from socket import *


sock = socket(AF_INET, SOCK_DGRAM)
sock.bind(('', 0))
sock.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
pygame.init()
pygame.joystick.init()
joysticks = [pygame.joystick.Joystick(x) for x in range(pygame.joystick.get_count())]
joysticks[0].init()


power_level = -1;
prev = ""
last = 0
while True:
    for event in pygame.event.get():
        pass
    p_change = (joysticks[0].get_axis(5) - joysticks[0].get_axis(2)) * 50
    power_level = -1 if power_level == -1 else min(max(power_level + p_change/200, 0), 100)
    if joysticks[0].get_button(2):
        if not last:
            power_level = -1 if power_level != -1 else 0
        last = 1
    else: 
        last = 0
    val_x, val_y = [joysticks[0].get_axis(x) for x in (0,1)]
    mod = math.hypot(val_y, val_x)
    angle = math.atan2(-val_y, val_x) + math.pi/4
    opp_1 = mod * math.asin(math.sin(angle))
    opp_2 = - mod * math.asin(math.cos(angle))
    new = "1:%i %f %f" % (int(power_level), opp_1, opp_2)
    if new != prev:
        print(new[2:])
        sock.sendto(new, ('<broadcast>', 7824))
    prev = new
    time.sleep(0.02)