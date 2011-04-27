var socket;

socket = {
    
    debug: true,
    node_server: '127.0.0.1',
    node_port: 8080,
    socket: null,
    
    init: function () {
        this.socket = this.socket || new io.Socket(this.node_server, {
                                                   'port' : this.node_port,
                                         'connectTimeout' : 200,
                                      'rememberTransport' : false,
                                           'closeTimeout' : 12000,
                                                  });
        var started = false,
            debug = this.debug,
            socket = this.socket;
        
        socket.on('connect', function () {
            if (debug) console.log('connect');
            // send client id to server
            socket.send(JSON.stringify({ world_id: globals.world_id, client_id: globals.client_id, cmd: 'register'}));
            if (!started) {
                game.init2();
                started = true;
            }
        });

        socket.on('message', function (msg) {
            console.log('message received');
            msg = $.parseJSON(msg);
            if (debug) {
                console.log(msg.msg);
                console.log(msg);
            }
            var fn = route[msg.msg];
            if (fn !== undefined) {
                fn(msg);
            }
        });

        socket.on('close', function () {
            globals.session_id = null;
            if (debug) console.log('close');
            socket.connect();
        });

        socket.on('disconnect', function () {
            globals.session_id = null;
            if (debug) console.log('disconnect');
            socket.connect();
        });
        
        socket.on('connect_failed', function () {
            globals.session_id = null;
            if (debug) console.log('connection failed. reconnecting...');
            socket.connect();
        })

        socket.connect();

        //setInterval('socket.socket.send("ping")', this.ping_delay); // start pinging the server to keep-alive
    },

    reset : function () {
        if (this.socket) {
            this.socket.disconnect();
        }
        this.init();
    }
};
