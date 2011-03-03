var globals = {

    world_id: 0,
    client_id: 0,
    player_id: 0,
    server_out: '',
    update: update = 
        function (params) {
            this.world_id = typeof params.world_id === 'number' ? params.world_id : this.world_id;
            this.client_id = typeof params.client_id === 'number' ? params.client_id : this.client_id;
            this.server_out = typeof params.server_out !== undefined ? params.server_out : this.server_out;
        }
}

var send = function (data) {
    
    var url = '/api';
    data = JSON.stringify(data);
    $.post(globals.server_out+url, { json: data });
    
}

// e.g.

// send(admin_commands.create_item(5,5,6));

var admin, info, action;

admin = ( function () {

    var type = 'info',
        create_agent,
        create_item,
        set_map,
        public_;
            
	create_agent = function (location, x, y, z, player_id) {
		
        if (player_id === undefined) { 
            player_id = 0;
        }
	
		var data = { 
                cmd: 'create_agent',
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                player_id: player_id,
                position: [location, x, y, z],
               }
        
        send(data);
	}

	create_item = function (x,y,z) {

		var data = { 
                cmd: "create_item",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                position: [0, x, y, z]
               }
               
        send(data);
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
               
        send(data);
	}
    
    public_ = {
                create_agent: create_agent,
                create_item: create_item,
                set_map: set_map
              }
              
    return public_;

}());


info = ( function () {
	
    var type = "info",
        get_map,
        get_agent,
        get_object,
        get_agents,
        get_objects,
        public_;
		
	get_map = function (z) {

		data = { 
                cmd: "get_map",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id
               }
               
        if (z !== undefined) {
            data.z = z;
        }
		
        send(data);
	}

	get_agent = function (id) {

		data = { 
                cmd: "get_agent",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                agent_id: id
               }
		
		send(data);	
	}

	get_object = function (id) {

		data = { 
                cmd: "get_object",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                object_id: id
               };
		
		send(data);
	}
	
	get_agents = function() {
		
		data = { 
                cmd: "get_agent_list",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id
               };
		
		send(data);
	}

	get_objects = function() {
		
		data = { 
                cmd: "get_object_list",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id
               };
		
		send(data);
	}
	
    public_ = {
                get_map: get_map,
                get_agent: get_agent,
                get_object: get_object,
                get_agents: get_agents,
                get_objects: get_objects
              };
              
    return public_;
    		
}());

action = ( function () {

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
		
		send(data);
	}
    
    public_ = {
                move_0: move_0
              };
              
    return public_;
    
}());
