# import bluetooth
import socket

# print(bluetooth.read_local_bdaddr())

# sock = bluetooth.BluetoothSocket()
# sock.bind(("", bluetooth.PORT_ANY))
# sock.listen(1)
sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
sock.bind(("", 4))
sock.listen(1)

port = sock.getsockname()[1]

uuid = "2b14e374-9105-4ebb-b15c-39900ca95ab2"

# bluetooth.advertise_service(sock, "test", service_id=uuid,
#                             service_classes=[uuid, bluetooth.SERIAL_PORT_CLASS],
#                             profiles=[bluetooth.SERIAL_PORT_PROFILE])

print("on port", port)

client_sock, client_info = sock.accept()
print("connected w/", client_info)

try:
    while True:
        data = client_sock.recv(1024)
        if not data:
            break
        print("Recieved", data)

except OSError:
    pass

print("done")

client_sock.close()
sock.close()
