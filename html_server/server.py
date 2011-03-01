import string,cgi,time
from os import curdir, sep
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import sys 
import urlparse

import redis
from marshal import dumps
import marshal
import sys
import SimpleHTTPServer
import SocketServer
import simplejson

PORT = 8055

r_client = redis.Redis('localhost')

def send_message(msg):
    world_id = msg['world_id']
    r_client.lpush("world_"+str(world_id), dumps(msg))

class MyHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):
     
    def do_GET(self):
        dir = "../html_client"
        print  "path= " + self.path
        path = self.path
        if path == "/":
            path = "/index.html"
        try:
            print "open: " + dir+path
            f = open(dir+path, 'r')
            self.copyfile(f, self.wfile)
        except:
            self.send_error(404,'404 Error: %s' % self.path)


    def do_POST(self):
        global rootnode
        try:
            ctype, pdict = cgi.parse_header(self.headers.getheader('content-type'))
            #print "ctype, pdict= " + ctype + ", " + str(pdict)
            if ctype == 'multipart/form-data':
                postvars = cgi.parse_multipart(self.rfile, pdict)
            elif ctype == 'application/x-www-form-urlencoded':
                length = int(self.headers.getheader('content-length'))
                postvars = cgi.parse_qs(self.rfile.read(length), keep_blank_values=1)
            else:
                postvars = {}
            print "postvars= " + str(postvars)
#            print str(postvars['gx'])

            #for x,y  in postvars.items():
                #if type(y)==type([]) and len(y) == 1:
                    #postvars[x] = y[0]
            ##strip []s
            #array_cases = ['dp', 'position'] #arrays that are passed through
            #for prefix in array_cases:
                #x= prefix + '[]'
                #if x in postvars.keys():
                    #postvars[prefix] = postvars[x]
                    #del postvars[x]
                    
            #single_ints = ['agent_id', 'id', 'world_id', 'player_id']
            #int_arrays = ['position', 'dp']
            
            #for x,y  in postvars.items():
                #if x in single_ints:
                    #postvars[x] = int(y)
                #if type(y)==type([]) and x in int_arrays:    
                    #for index, g in enumerate(y):
                        #y[index] = int(y[index])

            postvars = postvars['json'][0]
            postvars = simplejson.loads(postvars)
            print "postvars= " + str(postvars)
            
            send_message(postvars)

            self.send_response(301)

            self.end_headers()
            #upfilecontent = query.get('upfile')
            #print "filecontent", upfilecontent[0]
            self.wfile.write("<HTML>POST OK.<BR><BR>");
            #self.wfile.write(upfilecontent[0]);
            
        except Exception, err:
            print "Post Error: %s: %s" %(sys.stderr, err)
            self.send_error(404,'404 Error: %s' % self.path)
            pass

def main():
    try:
        server = HTTPServer(('', PORT), MyHandler)
        print 'started httpserver...'
        server.serve_forever()
    except KeyboardInterrupt:
        print '^C received, shutting down server'
        server.socket.close()
        
        



if __name__ == '__main__':
    main()
