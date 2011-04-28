var http = require('http'),
    port = 8081,
    server = http.createServer(function(request, response) {
        response.statusCode = 200;
        response.setHeader('Content-Type', 'text/html');
        response.write('socket server');
        response.end();
    });
server.listen(port);

/*
 * Global broadcast
 */
var redis = require("redis"),
    redis_port = 6379,
    redis_host = '127.0.0.1';

var r_global = redis.createClient(redis_port, redis_host);

//subscribe to message stream for map/world
r_global.subscribe("world_0_out", function(channel, message, pattern) {
    //client.broadcast(message);
});

//global admin messages
r_global.subscribe("global_admin", function(channel, message, pattern) {
    socket.broadcast(message);
});

/*
 * Socket.io
 */

var io = require('socket.io'),
    socket,
    clients = {}, // maps client_id to client objects
    client_id_to_session = {},
    session_id_to_client = {},
    confirm_register = function (msg) { // respond to the client after receipt of client_id
        console.log('client registering: '+ msg.client_id);
        var confirmed = false,
            old_client;
        if (msg.msg === 'new' && client_id_to_session.hasOwnProperty(msg.client_id)) {
            msg.session_id = 'taken';
        } else {
            this.client_id = msg.client_id;
            msg.session_id = this.sessionId;
            client_id_to_session[msg.client_id] = { session_id: msg.session_id,
                                                    timestamp : Date.now() };
            session_id_to_client[msg.session_id] = msg.client_id;
            
            old_client = clients[this.client_id];
            if (old_client) {
                if (old_client.redis_client !== undefined) {
                    this.redis_client = old_client.redis_client;
                }
                this.queue = old_client.queue || [];
            }
            clients[msg.client_id] = this;
            confirmed = true;
        }
        msg.msg = msg.cmd;
        delete msg.cmd;
        this.send(JSON.stringify(msg));
        this.confirmed = confirmed;
        this.queue = this.queue || [];
        return this;
    };

console.log(server);
socket = io.listen(server, { websocket: { closeTimeout: 15000 }}); 
console.log('Socket.io Listening');

/*
 * TODO: culling clients that have disconnected too long
 */

// update client's redis_client
var update_redis = function (data) {

    var that = this,
        bind_message = function() {
            that.redis_client.on('message', function(channel, message) {
                if (that.connected) {
                    that.send(message);
                } else {
                    that.queue.push(message);
                }
            });
        };
    if (!that.redis_client) { // new redis client
        that.redis_client = redis.createClient(redis_port, redis_host);
        bind_message();
        that.redis_client.subscribe('world_'+data.world_id+'_out');
        that.redis_client.subscribe('client_'+that.client_id);
    } else {    // update client object in redis_client.on('message');
        bind_message();
    }
    return that;
};

var flush_queue = function () {
    console.log('flushing queue');
    console.log(this.queue.length + ' items in queue');
    while (this.queue.length) {
        this.send(this.queue.shift());
    }
    console.log('queue flushed');
};

socket.on('connection', function(client) {
    //subscribe to client id channel when client connects
    console.log('Client Connected');
    if (client.confirm_register === undefined) {
        client.confirm_register = confirm_register;
    }
    if (client.update_redis === undefined) {
        client.update_redis = update_redis;
    }
    if (client.flush_queue === undefined) {
        client.flush_queue = flush_queue;
    }
    
    client.on('message', function(message) {
        var redis_client,
            old_client;
        if (message === undefined) return;
        message = JSON.parse(message);
        if (typeof message !== 'object') return;
        if (message.cmd === 'register') {
            if (client.confirm_register(message).confirmed) {
                client.update_redis(message);
                client.flush_queue();
            }
        } else {
            tell_redis(message);
        }
    });

    var exit_func = function() {
        console.log('Client Disconnect or Close');
        
        delete session_id_to_client[client.sessionId];
        delete client_id_to_session[client.client_id];
        client.send(JSON.stringify([{'disconnect': client.sessionId}]));
    };
    
    client.on('disconnect', exit_func);
    client.on('close', exit_func);  /* "Be careful with using this event, as some transports will fire it even under temporary, expected disconnections (such as XHR-Polling)." */
});


// clean up clients that have been gone too long
var cull_clients = function () {
    //client.redis_client.quit();
};

cull_clients.timeout = 5000; // 5 seconds

//setInterval('cull_clients();', cull_clients.timeout);

////Client Connected
////a sample client object:
//{ listener: 
   //{ server: 
      //{ connections: 1,
        //allowHalfOpen: true,
        //watcher: [Object],
        //_events: [Object],
        //httpAllowHalfOpen: false,
        //type: 'tcp4',
        //fd: 7 },
     //options: 
      //{ origins: '*:*',
        //resource: 'socket.io',
        //transports: [Object],
        //transportOptions: [Object],
        //log: [Function],
        //websocket: [Object] },
     //clientsIndex: { '09413910284638405': [Circular] },
     //clients: { '09413910284638405': [Circular] },
     //_clientFiles: {},
     //_events: { connection: [Function] } },
  //options: 
   //{ timeout: null,
     //heartbeatInterval: 10000,
     //closeTimeout: 8000,
     //duration: 20000 },
  //connections: 1,
  //_open: false,
  //_heartbeats: 0,
  //connected: true,
  //upgradeHead: undefined,
  //request: 
   //{ socket: 
      //{ bufferSize: 0,
        //fd: 8,
        //type: 'tcp4',
        //allowHalfOpen: true,
        //_readWatcher: [Object],
        //readable: true,
        //_writeQueue: [],
        //_writeQueueEncoding: [],
        //_writeQueueFD: [],
        //_writeQueueCallbacks: [],
        //_writeWatcher: [Object],
        //writable: true,
        //_writeImpl: [Function],
        //_readImpl: [Function],
        //_shutdownImpl: [Function],
        //remoteAddress: '127.0.0.1',
        //remotePort: 36300,
        //server: [Object],
        //ondrain: [Function],
        //_idleTimeout: 120000,
        //_idleNext: [Object],
        //_idlePrev: [Object],
        //_idleStart: Mon, 25 Apr 2011 21:18:06 GMT,
        //_events: [Object],
        //ondata: [Function],
        //onend: [Function],
        //_httpMessage: null },
     //connection: 
      //{ bufferSize: 0,
        //fd: 8,
        //type: 'tcp4',
        //allowHalfOpen: true,
        //_readWatcher: [Object],
        //readable: true,
        //_writeQueue: [],
        //_writeQueueEncoding: [],
        //_writeQueueFD: [],
        //_writeQueueCallbacks: [],
        //_writeWatcher: [Object],
        //writable: true,
        //_writeImpl: [Function],
        //_readImpl: [Function],
        //_shutdownImpl: [Function],
        //remoteAddress: '127.0.0.1',
        //remotePort: 36300,
        //server: [Object],
        //ondrain: [Function],
        //_idleTimeout: 120000,
        //_idleNext: [Object],
        //_idlePrev: [Object],
        //_idleStart: Mon, 25 Apr 2011 21:18:06 GMT,
        //_events: [Object],
        //ondata: [Function],
        //onend: [Function],
        //_httpMessage: null },
     //httpVersion: '1.1',
     //complete: false,
     //headers: 
      //{ host: '127.0.0.1:8080',
        //connection: 'keep-alive',
        //referer: 'http://127.0.0.1:8055/',
        //origin: 'http://127.0.0.1:8055',
        //'user-agent': 'Mozilla/5.0 (X11; Linux i686) AppleWebKit/534.30 (KHTML, like Gecko) Ubuntu/10.04 Chromium/12.0.743.0 Chrome/12.0.743.0 Safari/534.30',
        //accept: '*/*',
        //'accept-encoding': 'gzip,deflate,sdch',
        //'accept-language': 'en-US,en;q=0.8',
        //'accept-charset': 'ISO-8859-1,utf-8;q=0.7,*;q=0.3' },
     //trailers: {},
     //readable: true,
     //url: '/socket.io/xhr-polling//1303766286791',
     //method: 'GET',
     //statusCode: null,
     //client: 
      //{ bufferSize: 0,
        //fd: 8,
        //type: 'tcp4',
        //allowHalfOpen: true,
        //_readWatcher: [Object],
        //readable: true,
        //_writeQueue: [],
        //_writeQueueEncoding: [],
        //_writeQueueFD: [],
        //_writeQueueCallbacks: [],
        //_writeWatcher: [Object],
        //writable: true,
        //_writeImpl: [Function],
        //_readImpl: [Function],
        //_shutdownImpl: [Function],
        //remoteAddress: '127.0.0.1',
        //remotePort: 36300,
        //server: [Object],
        //ondrain: [Function],
        //_idleTimeout: 120000,
        //_idleNext: [Object],
        //_idlePrev: [Object],
        //_idleStart: Mon, 25 Apr 2011 21:18:06 GMT,
        //_events: [Object],
        //ondata: [Function],
        //onend: [Function],
        //_httpMessage: null },
     //httpVersionMajor: 1,
     //httpVersionMinor: 1,
     //upgrade: false },
  //response: 
   //{ output: [],
     //outputEncodings: [],
     //writable: true,
     //_last: false,
     //chunkedEncoding: false,
     //shouldKeepAlive: true,
     //useChunkedEncodingByDefault: true,
     //_hasBody: true,
     //_trailer: '',
     //finished: true,
     //socket: null,
     //connection: null,
     //_events: { finish: [Function] },
     //_header: 'HTTP/1.1 200 OK\r\nContent-Type: text/plain; charset=UTF-8\r\nContent-Length: 25\r\nAccess-Control-Allow-Origin: http://127.0.0.1:8055\r\nConnection: keep-alive\r\n\r\n',
     //_headerSent: true },
  //connection: 
   //{ bufferSize: 0,
     //fd: 8,
     //type: 'tcp4',
     //allowHalfOpen: true,
     //_readWatcher: 
      //{ socket: [Circular],
        //callback: [Function: onReadable] },
     //readable: true,
     //_writeQueue: [],
     //_writeQueueEncoding: [],
     //_writeQueueFD: [],
     //_writeQueueCallbacks: [],
     //_writeWatcher: 
      //{ socket: [Circular],
        //callback: [Function: onWritable] },
     //writable: true,
     //_writeImpl: [Function],
     //_readImpl: [Function],
     //_shutdownImpl: [Function],
     //remoteAddress: '127.0.0.1',
     //remotePort: 36300,
     //server: 
      //{ connections: 1,
        //allowHalfOpen: true,
        //watcher: [Object],
        //_events: [Object],
        //httpAllowHalfOpen: false,
        //type: 'tcp4',
        //fd: 7 },
     //ondrain: [Function],
     //_idleTimeout: 120000,
     //_idleNext: 
      //{ repeat: 120,
        //_idleNext: [Circular],
        //_idlePrev: [Circular],
        //callback: [Function] },
     //_idlePrev: 
      //{ repeat: 120,
        //_idleNext: [Circular],
        //_idlePrev: [Circular],
        //callback: [Function] },
     //_idleStart: Mon, 25 Apr 2011 21:18:06 GMT,
     //_events: 
      //{ timeout: [Function],
        //error: [Function],
        //close: [Function] },
     //ondata: [Function],
     //onend: [Function],
     //_httpMessage: null },
  //_closeTimeout: 
   //{ _idleTimeout: 20000,
     //_onTimeout: null,
     //_idlePrev: null,
     //_idleNext: null,
     //_idleStart: Mon, 25 Apr 2011 21:18:06 GMT,
     //callback: null },
  //sessionId: '09413910284638405',
  //handshaked: true,
  //_writeQueue: [],
  //_disconnectTimeout: 
   //{ _idleTimeout: 8000,
     //_onTimeout: [Function],
     //_idlePrev: 
      //{ repeat: 8,
        //_idleNext: [Circular],
        //_idlePrev: [Circular],
        //callback: [Function] },
     //_idleNext: 
      //{ repeat: 8,
        //_idleNext: [Circular],
        //_idlePrev: [Circular],
        //callback: [Function] },
     //_idleStart: Mon, 25 Apr 2011 21:18:06 GMT } }
