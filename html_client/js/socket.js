var Socket;

Socket = ( function () {
    
    var init,
        initRequests,
        debug = true,
        ajax_server = 'http://127.0.0.1:8055',
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
        
        initRequests();
    };
    
    initRequests = function () {
        var data = { cmd:'get_map', type:'info',client_id:0,z:5, world_id:0 };
        data = JSON.stringify(data);
        $.post(ajax_server+'/api/get_map',{json: data});
        
        data = {cmd: 'get_agent_list', type:'info', client_id:0, world_id:0};
        data = JSON.stringify(data);
        $.post(ajax_server+'/api/agent_list', {json:data});
        
        data = {cmd: 'get_object_list', type:'info', client_id:0, world_id:0};
        data = JSON.stringify(data);
        $.post(ajax_server+'/api/object_list', {json:data});
    };
    
    return {
             init: init,
           }

}());

