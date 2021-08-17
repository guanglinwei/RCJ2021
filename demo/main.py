# import bluetooth
import socket
import pygame

pygame.init()
pygame.joystick.init()
joysticks = [pygame.joystick.Joystick(x) for x in range(pygame.joystick.get_count())]
clock = pygame.time.Clock()
print(len(joysticks), "joysticks total")

addr = "00:14:03:05:0D:3E" # gw
port = 1
sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
print("connecting...")
sock.connect((addr, port))
print("connected")

while len(joysticks) > 0:
    clock.tick(2)
    pygame.event.get()
    x_axis, y_axis, lt, rt = [joysticks[0].get_axis(i) for i in [0, 1, 2, 5]]
    x_button = joysticks[0].get_button(2)

    sock.sendall(bytes([255]))
    sock.sendall(bytes([int(v * 100 + 100) for v in [x_axis, y_axis, lt, rt]] + [x_button]))
    # sock.sendall(",".join([str(s) for s in [x_axis, y_axis, lt, rt]]))


    #     # for j in joysticks:
    #         # print(", ".join([str(j.get_axis(i)) for i in range(j.get_numaxes())]))


# left -1 right 1 up -1 down 1
# left stick axis 0, 1
# right stick axis 3, 4

# resting position -1, fully pressed 1
# LT axis 2, RT axis 5
# Buttons 0-10
# A, B, X, Y, LB, RB, BACK, START, LSTICK, RSTICK, GUIDE

print("done")
pygame.joystick.quit()