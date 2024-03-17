import socket
import msgpack
import numpy as np

count = 0
x_bytes = 327685

for i in range(1000):

    print(f"count = {count}", flush = True)

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('localhost', 8080)
    k = np.ones([128, 128, 4]).astype(np.float32)
    k = k.flatten()

    num = k.tolist()
    server_socket.connect(server_address)
    packed_data = msgpack.packb(num)

    while packed_data:
        sent = server_socket.send(packed_data)
        packed_data = packed_data[sent:]


    received_data_ = b""

    while len(received_data_) < x_bytes:
        chunk = server_socket.recv(x_bytes - len(received_data_))
        if not chunk:
            raise RuntimeError("socket connection broken")
        received_data_ += chunk

    unpacked_data = msgpack.unpackb(received_data_)

    print(f"received sum = ", np.sum(unpacked_data))

    server_socket.close()

    count = count + 1





