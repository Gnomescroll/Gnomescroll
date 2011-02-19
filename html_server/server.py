import string,cgi,time
from os import curdir, sep
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import sys 
import urlparse

import redis
from marshal import dumps
import marshal
import sys

import simplejson

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
	
	#Setup Global Environment
	        #self.globals = dict(cgi.parse_qsl(self.query_string))
			params = dict(cgi.parse_qsl(self.query_string))          
			#params = dict(urlparse.parse_qs(self.query_string))
			print "path= " + self.path
			self.send_response(200)
			self.send_header('Content-type',	'text/html')
			self.end_headers()
			#self.wfile.write("hey, today is the" + str(time.localtime()[7]))
			#self.wfile.write(" day in the year " + str(time.localtime()[0]))
			self.wfile.write(str(params))
			return
                
		except IOError:
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
#			print str(postvars['gx'])

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
