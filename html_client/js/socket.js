var socket;

socket = ( function () {
    
    var init,
        initRequests,
        debug = true,
        node_server = '127.0.0.1',
        node_port = 8080;
    
    init = function () {
        
        var socket = new io.Socket(node_server,
                                  {'port': node_port});

        socket.on('connect', function () {
            if (debug) console.log('connect');
            
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
        });

        socket.on('disconnect', function () {
            if (debug) console.log('disconnect');
        });

        socket.connect();	
        
    };
    
    return {
             init: init,
           }

}());

