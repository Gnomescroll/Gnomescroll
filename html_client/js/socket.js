var Socket;

Socket = ( function () {
    
    var init,
        debug = true,
        ajax_server = 'http://127.0.0.1:8055',
        node_server = '127.0.0.1',
        node_port = 8080;
    
    init = function () {
        
        var socket = new io.Socket(node_server,
                                  {'port': node_port});
	
        socket.on('connect', function () {
            if (debug) console.log('connect');
            var data = { cx:0,cy:0, world_id:1 };
            data = JSON.stringify(data);
            $.post(ajax_server+'/api/get_map',{json: data});
        });

        socket.on('message', function (msg) {
            if (debug) console.log(msg);
            routeMessage(msg);
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

