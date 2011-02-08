import string,cgi,time
from os import curdir, sep
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

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
            if ctype == 'multipart/form-data':
                query=cgi.parse_multipart(self.rfile, pdict)
            self.send_response(301)
            
            self.end_headers()
            upfilecontent = query.get('upfile')
            print "filecontent", upfilecontent[0]
            self.wfile.write("<HTML>POST OK.<BR><BR>");
            self.wfile.write(upfilecontent[0]);
            
        except :
            pass

def main():
    try:
        server = HTTPServer(('', 8080), MyHandler)
        print 'started httpserver...'
        server.serve_forever()
    except KeyboardInterrupt:
        print '^C received, shutting down server'
        server.socket.close()

if __name__ == '__main__':
    main()
