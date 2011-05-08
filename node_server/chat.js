/* Chat todo:
 *
 * server:
 * 
 *  History (ignoring)
 *  PMs DONE
 *  anti-spam (ratelimit)
 *  sanitize msgs (no html tags) DONE
 *
 *  client:
 *      blacklist/ignore DONE
 *      Message history buffer DONE
 *      Fixed size windows  DONE
 *      CSS
 *      message div attributes DONE
 *      Error notifications
 *      PM interface
 *
 *
 */

var redis = require("redis"),
    redis_port = 6379,
    redis_host = '127.0.0.1',
    r_api = redis.createClient(redis_port, redis_host);

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
        msg  = json;
        json = JSON.stringify(msg);
    }
    if (!validate_chat_message(msg)) {
        return;
    }
    clean(msg);
    json = JSON.stringify(msg);
    channel = channel || (msg.pm) ? 'chat_user_'+msg.pm : 'chat_' + msg.world_id;
    console.log('channel '+ channel);
    console.log('tell redis');
    console.log(json);
    r_api.publish(channel, json);
}

var message_counter = 1;
function validate_chat_message (msg) {
    if (msg.world_id === undefined) return false;
    if (msg.content === undefined || msg.content === '') return false;
    if (msg.name === undefined || msg.name === '') return false;
    msg.id = message_counter++;
    return true;
}

function clean(msg) {
    msg.content = stripHTML(msg.content);
}

function stripHTML (str)  {
    return str.replace(/<\/?[a-z][a-z0-9]*[^<>]*>/ig, "");
}

var http_port = 8082,
    http = new (function(port) {
    return function () {
        var http = require('http'),
            URL  = require('url'),
            fs   = require('fs'),
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
                    'png' : 'image/png',
                    'jpg' : 'image/jpeg',
                    'jpeg': 'image/jpeg',
                    'gif' : 'image/gif',
                    'bmp' : 'image/bmp',
                },
                content_type = typemap[ext[ext.length-1]];
            content_type = content_type || 'text/html';
            response.setHeader('Content-Type', content_type);
        }

        views = {

            hello : function () {
                return 'hello';
            },
            
            chat : function (request) {
                return fs.readFileSync('../html_client/chat.html', 'utf8');
            },

            chat_api : function (request, response) {
                var json = URL.Query(request.message.content).json,
                    vars;
                if (!json) {
                  return 'chat api - no json received';
                }
                tell_redis(json);
                return 'chat api received: ' + json;
            },
        };

        this.views = views;
        
        urls = {
            '/'        : views.chat,
            '/chat'    : views.chat,
            '/chat_api': views.chat_api,
        };

        this.urls = urls;

        var that = this;
        this.port = port || 8080;
        this.server = http.createServer(function(request, response){
            var message = {'content': request.url+'?'};
            request.message = message;
            request.on('data', function (chunk) {
                message.content += chunk.toString();
            });
            
            request.on('end', function () {
                var ext    = set_mimetype(request, response),
                    status = 200,
                    body   = urls[request.url],
                    media_path;

                if (!body) {    // route to files for static urls
                    for (media_path in urls.media) {
                        if (!urls.media.hasOwnProperty(media_path)) continue;
                        if (request.url.slice(0, media_path.length) === media_path) {
                            body = urls.media[media_path];
                        }
                    }
                }

                response.statusCode = status;
                body = (body) ? body.call(that, request, response) : views.hello();
                response.write(body);
                response.end();
            });
        });
        this.server.listen(this.port);
        console.log('HTTP Server now listening at port '+this.port);
    };
}(http_port));

/*
 * Socket.io
 */

var io = require('socket.io'),
    socket,
    clients = {}, // maps client_id to client objects
    client_id_to_session = {},
    session_id_to_client = {},
    disconnected_clients = {}, // maps timestamp -> [client_id, ]
    confirm_register = function (msg) { // respond to the client after receipt of client_id
        console.log('client registering: '+ msg.client_id);
        var confirmed = false,
            old_client,
            disconnect_time,
            dc_clients,
            index;
        if (msg.msg === 'new' && client_id_to_session.hasOwnProperty(msg.client_id)) {
            msg.session_id = 'taken';
        } else {
            this.client_id = msg.client_id;
            msg.session_id = this.sessionId;
            if (msg.update) {
                console.log('requested update');
                msg.requested_update = 1;
            }
            msg.update = 1; // 0 or 1, tells client if it should request updates or not (i.e. if there is no queue of messages waiting for it)
            client_id_to_session[msg.client_id] = { session_id: msg.session_id,
                                                    timestamp : Date.now() };
            session_id_to_client[msg.session_id] = msg.client_id;
            
            old_client = clients[this.client_id];
            if (old_client) {
                if (old_client.redis_client !== undefined) {
                    this.redis_client = old_client.redis_client;
                    msg.update = msg.requested_update || 0;
                }
                this.queue = (msg.requested_update) ? [] : old_client.queue || [];
                // remove from disconnected clients
                disconnect_time = old_client.disconnect_time
                if (disconnect_time) {
                    dc_clients = disconnected_clients[disconnect_time];
                    index = dc_clients.indexOf(this.client_id);
                    dc_clients.splice(index, 1);
                }
            }
            clients[msg.client_id] = this;
            confirmed = true;
        }
        msg.msg = msg.cmd;
        delete msg.cmd;
        delete msg.requested_update;
        this.send(JSON.stringify(msg));
        this.confirmed = confirmed;
        this.queue = this.queue || [];
        return this;
    };


// clean up clients that have been gone too long
var cull_clients = function () {
    var now = Date.now() - arguments.callee.timeout,
        start = arguments.callee.last_timestamp || now-1,
        clients_array,
        client_id,
        client,
        i = 0,
        len;

    while (++start <= now) {
        clients_array = disconnected_clients[start];
        if (!clients_array) continue;
        i = 0;
        len = clients_array.length;
        for (i=0; i < len; i++) {
            client_id = clients_array[i];
            //console.log(clients);
            client = clients[client_id];
            if (client !== undefined) {
                if (client.redis_client !== undefined) {
                    client.redis_client.quit();
                }
            }
            delete clients[client_id];
            console.log('culled client '+client_id);
        }
        delete disconnected_clients[start];
    }
    
    arguments.callee.last_timestamp = now;
};

cull_clients.timeout = 5000; // 5 seconds.  Time to wait before removing client
var cull_interval = setInterval(cull_clients, 1000);

socket = io.listen(http.server, { websocket: { closeTimeout: 15000 }}); 
console.log('Socket.io Listening');

// update client's redis_client
var update_redis = function (data) {

    var that = this,
        bind_message = function() {
            that.redis_client.on('message', function(channel, message) {
                if (that.connected) {
                    console.log('sending redis msg');
                    that.send(message);
                } else {
                    console.log('queuing');
                    that.queue.push(message);
                }
            });
        };
    if (!that.redis_client) { // new redis client
        that.redis_client = redis.createClient(redis_port, redis_host);
        bind_message();
        that.redis_client.subscribe('chat_'+data.world_id);
        that.redis_client.subscribe('client_global');
        that.redis_client.subscribe('chat_user_'+data.client_id);
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

    var disconnect_func = function(client) {
        return function () {
            console.log('Client Disconnect');
            var timestamp = Date.now();
            client.disconnect_time = timestamp;
            disconnected_clients[timestamp] = disconnected_clients[timestamp] || [];
            disconnected_clients[timestamp].push(client.client_id);
            delete session_id_to_client[client.sessionId];
            delete client_id_to_session[client.client_id];
            client.send(JSON.stringify([{'disconnect': client.sessionId}]));
        };
    };

    var close_func = function (client) {
        return function () {
            console.log('Client closed');
            delete session_id_to_client[client.sessionId];
            delete client_id_to_session[client.client_id];
            delete clients[client.client_id];
        };
    };
    
    client.on('disconnect', disconnect_func(client));
    client.on('close', close_func(client));  /* "Be careful with using this event, as some transports will fire it even under temporary, expected disconnections (such as XHR-Polling)." */
});
