var sys = require('sys');
var http = require('http'),  
    io = require('./socket.io'), // for npm, otherwise use require('./path/to/socket.io') 
    //require( './lib/redis-client.js' )
  
server = http.createServer(function(request, response){ 
 // your normal server code 
 response.writeHead(200, {'Content-Type': 'text/html'}); 
 response.end('<h1>Hello world</h1>'); 
 response.end(); 
}); 
server.listen(8080);

var client = require('./lib/redis-client.js').createClient();
client.info(function (err, info) {
    if (err) throw new Error(err);
    sys.puts("Redis Version is: " + info.redis_version);
    client.close();
});

var socket = io.listen(server); 
console.log('Client Disconnect');

socket.on('connection', function(client) {
	//subscribe to client id channel when client connects
	
	/*
	r_client.subscribeTo("0_0", function(channel, message, pattern) {
    socket.broadcast(message);
});  
	 */
	
    client.on('message', function(message) {
      return socket.broadcast(message);
    });
    return client.on('disconnect', function() {
      return client.broadcast(JSON.stringify([['disconnect', client.sessionId]]));
      console.log('Client Disconnect');
      //unsubscribe from client channel when client disconnects
      //!!! implement!
    /* look up syntax
	r_client.unsubscribeTo("0_0", function(channel, message, pattern) {});  
	 */
    });
  });

redis = require('./lib/redis-client.js');
var r = redis.createClient(6379, '127.0.0.1');

//this client connects to the Redis instance for direct to client communications
var r_client = redis.createClient(6379, '127.0.0.1');
//subscribe to message stream for a whole map chunk
r.subscribeTo("0_0", function(channel, message, pattern) {
        socket.broadcast(message);
});

//global admin messages
r.subscribeTo("global_admin", function(channel, message, pattern) {
        socket.broadcast(message);
});
