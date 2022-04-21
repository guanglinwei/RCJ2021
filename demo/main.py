# import bluetooth
import socket
from time import sleep
import pygame

# from inputs import get_gamepad

# while 1:
#         events = get_gamepad()
#         sleep(.4)
#         for event in events:
#             print(event.ev_type, event.code, event.state)

pygame.init()
pygame.joystick.init()
joysticks = [pygame.joystick.Joystick(x) for x in range(pygame.joystick.get_count())]
clock = pygame.time.Clock()
print(len(joysticks), "joysticks total")
for j in joysticks:
    j.init()

#addrs = ["00:14:03:05:0D:3E", "00:14:03:05:0b:72"] # gw, peter
#addrs = ["00:14:03:05:0b:72"]#, "00:14:03:05:0D:3E"] # peter, gw
addrs = ["00:14:03:05:0D:3E"] # gw
port = 1
socks = []
for addr in addrs:
    sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
    socks.append(sock)
    print("Connecting to", addr, "...")
    sock.connect((addr, port))
    print("Connected\n")
    port += 2


def send_data(joystick_index):
    x_axis, y_axis, lt, rt = [joysticks[joystick_index].get_axis(i) for i in [0, 1, 4, 5]]
    print([joysticks[0].get_axis(i) for i in range(6)])
    x_button = joysticks[joystick_index].get_button(2)

    socks[joystick_index].sendall(bytes([255]))
    socks[joystick_index].sendall(bytes([int(v * v * (1 if v > 0 else -1) * 100 + 100) for v in [x_axis, y_axis, lt, rt]] + [x_button]))

if len(joysticks) != len(addrs):
    print(f"Have {len(joysticks)} joysticks, and {len(addrs)} receivers")

while len(joysticks) > 0:
    clock.tick(8)
    pygame.event.get()
    for j in range(min(len(socks), len(joysticks))):
        send_data(j)
    # sock.sendall(",".join([str(s) for s in [x_axis, y_axis, lt, rt]]))


    #     # for j in joysticks:
    #         # print(", ".join([str(j.get_axis(i)) for i in range(j.get_numaxes())]))


# left -1 right 1 up -1 down 1
# left stick axis 0, 1
# right stick axis 2, 3

# resting position -1, fully pressed 1
# LT axis 4, RT axis 5
# Buttons 0-10
# A, B, X, Y, LB, RB, BACK, START, LSTICK, RSTICK, GUIDE

print("done")
pygame.joystick.quit()