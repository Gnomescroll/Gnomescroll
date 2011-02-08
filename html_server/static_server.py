import SimpleHTTPServer
import SocketServer

PORT = 8080
dir = "../html_client"

class StaticHTMLServer(SimpleHTTPServer.SimpleHTTPRequestHandler):
	
	def do_GET(self):
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


def main():
    try:
		httpd = SocketServer.ForkingTCPServer(('', PORT), StaticHTMLServer)
		print "serving at port ", PORT
		httpd.serve_forever()
    except KeyboardInterrupt:
        print '^C received, shutting down server'
        server.socket.close()

if __name__ == '__main__':
    main()
