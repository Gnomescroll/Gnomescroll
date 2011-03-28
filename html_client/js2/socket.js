var socket;

socket = {
    
    
    debug: true,
    node_server: '127.0.0.1',
    node_port: 8080,
    socket: null,
    
    init: function () {
        
        var started = false,
            debug = this.debug,
            socket = new io.Socket(this.node_server, {
                                   'port' : this.node_port,
                         'connectTimeout' : 200,
                      'rememberTransport' : false,     
                                  });
        
        // this will adjust the rate of connection cycling by a websocket
        // the default is 15000
        //socket.transport.options.timeout = 1500;
        
        this.socket = socket;
        
        socket.on('connect', function () {
            if (debug) console.log('connect');
            if (!started) {
                game.init2();
                started = true;
            }
        });

        socket.on('message', function (msg) {
            msg = $.parseJSON(msg);
            if (debug) console.log(msg.msg);
            var fn = route[msg.msg];
            if (fn !== undefined) {
                fn(msg);
            }
        });

        socket.on('close', function () {
            if (debug) console.log('close');
            socket.connect();
        });

        socket.on('disconnect', function () {
            if (debug) console.log('disconnect');
            socket.connect();
        });
        
        socket.on('connect_failed', function () {
            if (debug) console.log('connection failed. reconnecting...');
            socket.connect();
        })

        socket.connect();	
        
    },
    
};

