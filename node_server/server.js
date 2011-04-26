/*
 *
 *
 * Todo:
 *      Clients generate unique clientID and send it in first message (server checks it is unique and replies)
 *      clientID is stored in a mapping from sessionID -> {clientID: clientID, timestamp: sys.time(), connected: boolean}
 *      A redis channel is created for the client, and is stored in a mapping from clientID -> {channel: channel, queue: [] }
 *      Channel sends message -
 *          client connected: send
 *          client disconnected: add to queue
 *      ...more
 *      
 *      
 */


/*
 * 
 * Redis
 * 
 */
var redis = require("redis"),
    r = redis.createClient(6379, '127.0.0.1');

//subscribe to message stream for map/world
r.subscribe("world_0_out", function(channel, message, pattern) {
    client.broadcast(message);
});

//global admin messages
r.subscribe("global_admin", function(channel, message, pattern) {
    socket.broadcast(message);
});

function tell_redis(json, msg, channel) {    // publish json or a js object to redis
    // json can be either encoded or decoded json.
    // if encoded, msg can be the decoded object.
    //      if msg is the decoded object, channel can be the redis channel, or undefined
    // if decoded, msg can be the channel, or undefined
    if (typeof json !== 'object') {
        try {
            msg = JSON.parse(json);
            if (typeof msg !== 'object') return;
        } catch (e) {
            return;
        }
    } else {
        msg = json;
        json = JSON.stringify(msg);
    }
    if (msg.world_id === undefined) return;
    
    var r = redis.createClient();
    channel = channel || 'world_'+msg.world_id;
    r.lpush(channel, json);
}


/*
 * 
 * HTTP Server
 * 
 */
var http_port = 8080,
    http = new (function(port) {
    return function () {
        var http = require('http'),
            URL = require('url'),
            fs = require('fs'),
            views,
            urls;

        if (URL.Query === undefined) {
            URL.Query = function(url) {
                return URL.parse(url, true).query;
            };
        }

        function set_mimetype(request, response) {
            var ext = request.url.split('.'),
                typemap = { // extensions to mimetypes
                    'js'  : 'text/javascript',
                    'css' : 'text/css',
                    'html': 'text/html',
                },
                content_type = typemap[ext[ext.length-1]];
            content_type = content_type || 'text/html';
            response.setHeader('Content-Type', content_type);
        }

        views = {
            hello : function (request) {
                return '<h1>Gnomescroll</h1>';
            },

            api : function (request, response) {
                response.setHeader('Access-Control-Allow-Origin', 'http://127.0.0.1:8055');
                var json = URL.Query(request.message.content).json,
                  vars;
                if (!json) {
                  return 'api - no json received';
                }
                tell_redis(json);
                return 'api received: ' + json;
            },

            post : function(request) { // example post method
                var msg  = '',
                    vars = URL.Query(request.message.content);
                msg += '<form method="post" action="/post">';
                msg += '<input type="text" name="message" />';
                msg += '<br>';
                msg += '<input type="submit" value="Send Message" />';
                msg += '</form>';
                msg += '<br>';
                msg += '<strong>Message you last sent:</strong>';
                msg += '<br>';
                msg += vars.message || '';
                return msg;
            },

            _read_static: function (request, response, type, fp) {
                var path_prefix = '../html_client',
                    fn = request.url.split('/'),
                    f,
                    body = '';

                fn = fn[fn.length-1];

                fp = (fp) ? path_prefix + fp : path_prefix + '/' + type + '/' + fn;
                body = fs.readFileSync(fp, 'utf-8');
                return body;
            },

            static_media : function(request, response) {    // static files
                var filepath = request.url.split('?')[0];

                if (response.getHeader('Content-Type') === 'text/javascript') { // this breaks if set_mimetype() isn't called before routing to url
                    return this.views._read_static(request, response, 'js', filepath);
                }
                if (response.getHeader('Content-Type') === 'text/css') {
                    return this.views._read_static(request, response, 'css', filepath);
                }
            },

            js : function (request, response, filepath) {
                return this.views._read_static(request, response, 'js');
            },

            css : function (request, response) {
                return this.views._read_static(request, response, 'css');
            },
        };

        this.views = views;
        
        urls = {
            ''       : views.hello,
            '/'      : views.hello,
            '/api'   : views.api,
            '/post'  : views.post,

            media    : {
                '/static': views.static_media,
                '/js'    : views.js,
            },
        };

        this.urls = urls;

        var that = this;
        this.server = http.createServer(function(request, response){
            var message = {'content': request.url+'?'};
            request.message = message;
            request.on('data', function (chunk) {
                message.content += chunk.toString();
            });
            
            request.on('end', function () {
                set_mimetype(request, response);
                var status = 200,
                    body = urls[request.url],
                    media_path;

                if (!body) {    // route to files for static urls
                    for (media_path in urls.media) {
                        if (!urls.media.hasOwnProperty(media_path)) continue;
                        if (request.url.slice(0, media_path.length) === media_path) {
                            body = urls.media[media_path];
                        }
                    }
                }

                status = (body) ? status : 404;
                response.statusCode = status;
                body = (body) ? body.call(that, request, response) : '';
                response.write(body);
                response.end();
            });
        });

        this.port = port || 8080;
        this.server.listen(this.port);
    };
}(http_port));


/*
 * 
 * Socket.io
 * 
 */
var io = require('socket.io'),
    socket,
    clients = {};

socket = io.listen(http.server, { websocket: { closeTimeout: 15000 }}); 
console.log('Socket.io Listening');

socket.on('connection', function(client) {
    //subscribe to client id channel when client connects
    console.log('Client Connected');
    //console.log(client);
    clients[client.sessionId] = client;
    const redisClient = redis.createClient();
    redisClient.subscribe('world_0_out');
    
    redisClient.on('message', function(channel, message) {
        client.send(message);
    });
    
    client.on('message', function(message) {
        console.log('client sent message: '+ message.toString());
        client.send(message);
    });
    
    client.on('disconnect', function() {
        client.send(JSON.stringify([{'disconnect': client.sessionId}]));
        console.log('Client Disconnect');
        redisClient.quit();
        //r_client.unsubscribeTo("0_0", function(channel, message, pattern) {});  
    });
});


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
