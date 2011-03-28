var sys = require('sys'),
    http = require('http'),  
    io = require('./socket.io'),
    redis = require("redis"),
    r = redis.createClient(6379, '127.0.0.1'),
    server,
    socket;
  
server = http.createServer(function(request, response){ 
 // your normal server code 
 response.writeHead(200, {'Content-Type': 'text/html'}); 
 response.writeBody('<h1>Hello world</h1>'); 
 response.finish(); 
}); 
server.listen(8080);


//this client connects to the Redis instance for direct to client communications
//var r_client = redis.createClient(6379, '127.0.0.1');
//subscribe to message stream for map/world

r.subscribe("world_0_out", function(channel, message, pattern) {
        client.broadcast(message);
});

//global admin messages
r.subscribe("global_admin", function(channel, message, pattern) {
        socket.broadcast(message);
});


socket = io.listen(server); 
//console.log('Client Disconnect');
console.log('Server Listening');

socket.on('connection', function(client) {
	//subscribe to client id channel when client connects
	console.log('Client Connected');
	/*
	r_client.subscribeTo("0_0", function(channel, message, pattern) {
    socket.broadcast(message);
});  
	 */
	const subscribe = redis.createClient();
    subscribe.subscribe('world_0_out');
    
    subscribe.on('message', function(channel, message) {
        client.send(message);
    });
    
    client.on('message', function(message) {
        console.log('message');
        client.send(message);
    });
    
    client.on('disconnect', function() {
      
      client.send(JSON.stringify([['disconnect', client.sessionId]]));
      console.log('Client Disconnect');
      subscribe.quit();
      //unsubscribe from client channel when client disconnects
      // !!! implement!
    // look up syntax
	//r_client.unsubscribeTo("0_0", function(channel, message, pattern) {});  
	 
    });
  });

//console.log('Not Crashed Yet');

        //socket.broadcast(message);
        //console.log("message received!");

