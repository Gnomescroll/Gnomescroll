import string,cgi,time
#from os import curdir, sep
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import sys 
import urlparse

import redis
from marshal import dumps
import marshal
#import SimpleHTTPServer
#import SocketServer
import simplejson

import io

PORT = 8055

r_client = redis.Redis('localhost')

def send_message(msg):
    world_id = msg['world_id']
    r_client.lpush("world_"+str(world_id), dumps(msg))

class MyHandler(BaseHTTPRequestHandler):
	
    def do_GET(self):

		try:
			if self.path.find('?') != -1:
				self.path, self.query_string = self.path.split('?', 1)
			else:
				self.query_string = ''
							
			#params = dict(cgi.parse_qsl(self.query_string))   
	
			dir = "../html_client"
			print  "path= " + self.path
			path = self.path
			if path == "/":
				path = "/index.html"
			
			self.send_response(200)
			self.send_header('Content-type',	'text/html')
			self.end_headers()
			
			try:
				print "open: " + dir+path
				f = open(dir+path, 'r')
				
				#self.wfile.copyfile(f, self.wfile)
				self.wfile.write(f.read())
				
			except Exception, err:
				print "error: %s: %s" %(sys.stderr, err)
				self.send_error(404,'404 Error: %s' % self.path)

		except Exception, err:
			print "error: %s: %s" %(sys.stderr, err)
#		except IOError:
#			self.send_error(404,'404 Error: %s' % self.path)

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
