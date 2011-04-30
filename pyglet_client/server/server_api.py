import socket
import struct

import time

def _listen():
    TCP_IP = '127.0.0.1'
    TCP_PORT = 5052
    BUFFER_SIZE = 10  # Normally 1024, but we want fast response

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((TCP_IP, TCP_PORT))
    s.listen(1)

    conn, addr = s.accept()
    print 'Connected to:', addr
    return conn

M = [
'testmsg',
'testmsg2',
'asf343',
]

s = _listen()

while True:
    for data in M:
        (prefix) = struct.pack('I', len(data))
        message = prefix + data
        print "sending data: " + str(data)
        try:
            s.send(message)  # echo
        except socket.error, (value,message):
            print "socket_error: " + str(value) + ", " + message
            s = _listen()
    time.sleep(15)


#while 1:
#    data = conn.recv(BUFFER_SIZE)
#    if not data: break
#    print "received data:", data
