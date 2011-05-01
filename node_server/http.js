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
    if (msg.world_id === undefined) return;
    
    channel = channel || 'world_' + msg.world_id;
    console.log('tell redis');
    console.log(json);
    r_api.lpush(channel, json);
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

		_static_cache = {};

        views = {
            hello : function (request) {
                return '<h1><a href="/">Gnomescroll</a></h1>';
            },

            game : function (request) {
                return fs.readFileSync('../html_client/index.html', 'utf8');
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

            _read_static: function (request, response, type, fp) {
                var path_prefix = '../html_client',
                    encoding = response.getHeader('Content-Type').split('/')[0],
                    fn = request.url.split('/'),
                    f,
                    body,
                    cache_key;
                if (type === 'javascript') type = 'js';
                fn = fn[fn.length-1];
                fp = cache_key = (fp) ? fp : '/' + type + '/' + fn;
                fp = path_prefix + fp;
                encoding = (encoding === 'text') ? 'utf8' : '';
                if (_static_cache[cache_key] !== undefined) {
					body = _static_cache[cache_key];
				} else {
					body = fs.readFileSync(fp, encoding);
					_static_cache[cache_key] = body;
				}
                return body;
            },

            static_media : function(request, response) {    // static files
                var filepath = request.url.split('?')[0],
                    type = response.getHeader('Content-Type').split('/')[1];
                return this.views._read_static(request, response, type, filepath);
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
            ''       : views.game,
            '/'      : views.game,
            '/api'   : views.api,

            media    : {
                '/static': views.static_media,
                '/js'    : views.js,
                '/css'   : views.css,
            },
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
    };
}(http_port));
