import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 5050
BUFFER_SIZE = 10  # Normally 1024, but we want fast response

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

conn, addr = s.accept()
print 'Connection address:', addr
#while 1:
#    data = conn.recv(BUFFER_SIZE)
#    if not data: break
#    print "received data:", data

M = [
'testmsg',
'testmsg2',
'asf343',
]

for data in M:
    (prefix) = struct.pack('I', len(data))
    message = prefix + data
    conn.send(message)  # echo
conn.close()


