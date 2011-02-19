var globals = {

    world_id: 0,
    client_id: 0,
    server_out: null,
    update: update = 
        function (params) {
            this.world_id = typeof params.world_id === 'number' ? params.world_id : this.world_id;
            this.client_id = typeof params.client_id === 'number' ? params.client_id : this.client_id;
            this.server_out = typeof params.server_out !== undefined ? params.server_out : this.server_out;
        }
}

var send = function (data) {
    
    data = JSON.stringify(data);
    $.post(globals.server_out, { json: data });
    
}

// e.g.

// send(admin_commands.create_item(5,5,6));

var admin_commands = ( function () {

    var type = 'info',
        create_agent,
        create_item,
        set_map,
        public_;
        	
	create_agent = function (x,y,z, player_id) {
		
        if (player_id === undefined) { 
            player_id = 0;
        }
	
		var data = { 
                cmd: 'create_agent',
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                player_id: player_id
               }
        
        return data;
	}

	create_item = function (x,y,z) {

		var data = { 
                cmd: "create_item",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                position: [0, x, y, z]
               }
               
        return data;
	}
	
	set_map = function (x,y,z, value) {

		data = { 
                cmd: "set_map",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                position: [0, x, y, z],
                value: value
               }
               
        return data;
	}
    
    public_ = {
                create_agent: create_agent,
                create_item: create_item,
                set_map: set_map
              }
              
    return public_;

}());


var info_commands = ( function () {
	
    var type = "info",
        get_map,
        get_agent,
        get_object,
        get_agent_list,
        get_object_list,
        public_;
		
	get_map = function () {

		data = { 
                cmd: "get_map",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id
               }
		
        return data;
	}

	get_agent = function (id) {

		data = { 
                cmd: "get_agent",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                agent_id: id
               }
		
		return data;	
	}

	get_object = function (id) {

		data = { 
                cmd: "get_object",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                object_id: id
               };
		
		return data;
	}
	
	get_agent_list = function() {
		
		data = { 
                cmd: "get_agent_list",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id
               };
		
		return data;
	}

	get_object_list = function() {
		
		data = { 
                cmd: "get_object_list",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id
               };
		
		return data;
	}
	
    public_ = {
                get_map: get_map,
                get_agent: get_agent,
                get_object: get_object,
                get_agent_list: get_agent_list,
                get_object_list: get_object_list
              }
              
    return public_;
    		
}());

var agent_commands = ( function () {

    var type = 'agent',
        move_0,
        public_;
		
	move_0 = function (agent_id, dx, dy, dz) {

		data = { 
                cmd: "move_0",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                agent_id: agent_id,
                dp: [dx, dy, dz]
               }
		
		return data;
	}
    
    public_ = {
                move_0: move_0
              }
              
    return public_;
    
}());
