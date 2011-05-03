var socket = {
    
    debug       : false,
    node_server : '127.0.0.1',
    node_port   : 8081,
    socket      : null,
    no_reconnect: false,
    first_connect: true,
    
    init : function (update) {
        this.socket = this.socket || new io.Socket(this.node_server, {
                                                       'port' : this.node_port,
                                             'connectTimeout' : 200,
                                          'rememberTransport' : false,
                                               'closeTimeout' : 12000
                                                  });
        var started = false,
            that = this,
            reconnect = function(event_name) {
                return function () {
                    globals.session_id = null;
                    if (that.debug) console.log(event_name);
                    if (! that.no_reconnect) {
                        that.socket.connect();
                    }
                    that.no_reconnect = false;
                };
            };
        
        this.socket.on('connect', function () {
            if (that.debug) console.log('connect');
            // send client id to server
            that.register(that.first_connect);
            that.first_connect = false;
        });

        this.socket.on('message', function (msg) {
            console.log('message received');
            msg = $.parseJSON(msg);
            if (that.debug) {
                console.log(msg.msg);
                console.log(msg);
            }
            var fn = route[msg.msg];
            if (fn !== undefined) {
                fn(msg);
            }
        });

        this.socket.on('close', reconnect('close'));

        this.socket.on('disconnect', reconnect('disconnect'));
        
        this.socket.on('connect_failed', reconnect('connect_failed'));

        this.socket.connect();
    },

    reset_init : function () {
        console.log('resetting socket');
        if (this.socket) {
            this.socket.disconnect();
        }
        this.init();
    },
    
    reset_delayed : function (delay) {
        console.log('resetting socket');
        if (this.socket) {
            this.no_reconnect = true;
            this.socket.disconnect();
        }
        delay = delay || 3500;
        setTimeout('socket.socket.connect();', delay);
    },

    reset : function () {
        if (this.socket) {
            this.socket.disconnect();
        }
        this.socket = null;
        this.first_connect = true;
    },

    register : function (update) {
        // send client id to server
        console.log('registering');
        var msg = JSON.stringify({
            world_id : globals.world_id,
           client_id : globals.client_id,
                 cmd : 'register',
                 msg : (globals.new_client) ? 'new' : '',
              update : (update) ? 1 : 0,
        })
        this.socket.send(msg);
    },

    disconnect : function (no_reconnect) {
        if (this.socket) {
            this.no_reconnect = no_reconnect || true;
            this.socket.disconnect();
        }
    },
};
