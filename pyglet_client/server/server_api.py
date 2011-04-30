import socket
import struct

import time

def _listen():
    TCP_IP = '127.0.0.1'
    TCP_PORT = 5053
    BUFFER_SIZE = 10  # Normally 1024, but we want fast response

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((TCP_IP, TCP_PORT))
    s.listen(1)

    conn, addr = s.accept()
    print 'Connected to:', addr
    return conn

import simplejson as json

def pm(id, msg):
    return struct.pack('H',id) +msg

def create_agent_message(agent_id, player_id, x, y, z, x_angle, y_angle):
    t1 = struct.pack('IIfffhh', agent_id, player_id, x, y, z, x_angle, y_angle)
    t2 = pm(2, t1)
    return t2

def agent_position_update(agent_id, tick, x,y,z, vx, vy, vz, ax, ay, az, x_angle, y_angle):
    t1 = struct.pack('II fff fff fff hh', agent_id, tick, x, y, z, vx, vz, vz, ax, ay, az, x_angle, y_angle)
    t2 = pm(3, t1)
    return t2

#           CreateAgentMessage = namedtuple('CreateAgent', 'agent_id', 'player_id', 'x','y','z','x_angle','y_angle')
#            n = CreateAgentMessage(struct.unpack('IIfffhh', datagram))

M = [
pm(0,"test!"),
pm(1,json.dumps(['test1','test2','test3'])),
create_agent_message(0,1,5,5,5,0,0)
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
