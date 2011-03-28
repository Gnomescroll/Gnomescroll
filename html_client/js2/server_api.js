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
    console.log('socket.socket');
    console.log(socket.socket);
}

var admin, info, action;

admin = ( function () {

    var type = 'admin',
        create_agent,
        create_item,
        create_object,
        set_map,
        public_;
            
	create_agent = function (x, y, z, location) {
	
        var position;
    
        if (location === undefined) {
            location = 0;
        }
    
        if (x.constructor.name === 'Array') {
            if (x.length === 3) {
                x.splice(0,0,location);
            }
            position = x;
        } else {
            position = [location, x, y, z];
        }
    
		var data = { 
                cmd: 'create_agent',
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                player_id: globals.player_id,
                position: position,
               }
        
        send(data);
	};
    
    create_object = function (x, y, z, location, object_type, template) {
        
        var position;
    
        if (location === undefined) {
            location = 0;
        }
    
        if (x.constructor.name === 'Array') {
            if (x.length === 3) {
                x.splice(0,0,location);
            }
            position = x;
        } else {
            position = [location, x, y, z];
        }
        
        var data = {
                type: type,
                cmd: 'create_item',
                object_type: object_type,
                template: template,
                client_id: globals.client_id,
                world_id: globals.world_id,
                position: position,
            }
        
        send(data);
    };
	
	set_map = function (x, y, z, value, debug) {
        
        if (debug) console.log(arguments);
        var position;
    
        if (value === undefined) {
            value = 1;
        }
    
        if (x.constructor.name === 'Array') {
            position = x;
            if (position.length < 4 && value !== undefined) {
                value = y;
                position.splice(0,0,value);
            }
        } else {
            position = [value, x, y, z];
        }

		var data = { 
                cmd: "set_map",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                position: position,
                value: value
               }
        if (debug) console.log(data);
        send(data);
	};
    
    public_ = {
                create_agent: create_agent,
                create_item: create_item,
                create_object: create_object,
                set_map: set_map
              };
              
    return public_;

}());


info = ( function () {
	
    var type = "info",
        consts = {},
        tileset, 
        map,
        agent,
        object,
        agents,
        objects,
        public_;
        
    consts = { type: type,
               client_id: globals.client_id,
               world_id: globals.world_id
             };
		
    tileset = function () {
        var data = $.extend(consts, { cmd: 'get_tiles', });
        send(data);
    };
        
	map = function (z) {

		var data = $.extend(consts, { cmd: "get_map", });
               
        if (z !== undefined) {
            data.z = z;
        }
		
        send(data);
	}

	agent = function (id) {

		var data = $.extend(consts, { cmd: "get_agent",
                                      agent_id: id, });
		send(data);	
	}

	object = function (id) {

		var data = $.extend(consts, { cmd: "get_object",
                                      object_id: id, });
		send(data);
	}
	
	agents = function() {
		
		var data = $.extend(consts, { cmd: "get_agent_list", });
		send(data);
	}

	objects = function() {
		
		var data = $.extend(consts, { cmd: "get_object_list", });
		send(data);
	}
	
    public_ = {
                tileset: tileset,
                map: map,
                agent: agent,
                object: object,
                agents: agents,
                objects: objects,
              };
              
    return public_;
    		
}());

action = ( function () {

    var type = 'agent',
        move,
        till,
        plant,
        harvest,
        public_;
		
	move = function (agent_id, dx, dy, dz) {

		var data = { 
                cmd: "move_0",
                type: type,
                client_id: globals.client_id,
                world_id: globals.world_id,
                agent_id: agent_id,
                dp: [dx, dy, dz]
               }
		
		send(data);
	}
    
    till = function (agent_id) {

        var data = {
                    type: type,
                    cmd: 'till_soil',
                    world_id: globals.world_id,
                    agent_id: agent_id,
                   }
                   
        send(data);
    },

    plant = function (agent_id) {

        var data = {
                    type: type,
                    cmd: 'plant_crop',
                    world_id: globals.world_id,
                    agent_id: agent_id,
                   }
                   
        send(data);
    },

    harvest = function (agent_id, crop_id) {
        
        //temporary
        if (crop_id === undefined) {
            crop_id = 1;
        }
        
        var data = {
                    type: type,
                    cmd: 'harvest_crop',
                    world_id: globals.world_id,
                    agent_id: agent_id,
                    crop_id: crop_id,
                   }
                   
        send(data);
    };
    
    public_ = {
                move: move,
                till: till,
                plant: plant,
                harvest: harvest,
              };
              
    return public_;
    
}());
