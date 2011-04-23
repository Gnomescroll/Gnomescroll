var globals = {

    world_id: 0,
    client_id: 0,
    player_id: 0,
    server_out: '',
    update: function (params) {
                this.world_id = typeof params.world_id === 'number' ? params.world_id : this.world_id;
                this.client_id = typeof params.client_id === 'number' ? params.client_id : this.client_id;
                this.server_out = typeof params.server_out !== undefined ? params.server_out : this.server_out;
    },
};

var send = function (data) {
    var url = '/api';
    data = JSON.stringify(data);
    $.post(globals.server_out+url, { json: data });
};

var admin, info, action;

admin = {

    type: 'admin',
            
    create_agent: function (x, y, z, location) {
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
            type: this.type,
            client_id: globals.client_id,
            world_id: globals.world_id,
            player_id: globals.player_id,
            position: position,
        };
        send(data);
    },
    
    create_object: function (x, y, z, location, object_type, template) {
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
            cmd: 'create_item',
            type: this.type,
            object_type: object_type,
            template: template,
            client_id: globals.client_id,
            world_id: globals.world_id,
            position: position,
        };
        send(data);
    },
    
    set_map: function (x, y, z, value, debug) {
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
            type: this.type,
            client_id: globals.client_id,
            world_id: globals.world_id,
            position: position,
            value: value
        };
        if (debug) console.log(data);
        send(data);
    }
};

info = {
    
    type: 'info',
    consts: {
        type: 'info',
        client_id: globals.client_id,
        world_id: globals.world_id
    },
        
    tileset: function () {
        var data = $.extend({}, this.consts, { cmd: 'get_tiles' });
        send(data);
    },
        
    map: function (z) {
        var data = $.extend({}, this.consts, { cmd: 'get_map' });
               
        if (z !== undefined) {
            data.z = z;
        }
        send(data);
    },

    agent: function (id) {
        var data = $.extend({}, this.consts, { cmd: 'get_agent',
                                      agent_id: id });
        send(data); 
    },

    object: function (id) {
        var data = $.extend({}, this.consts, { cmd: 'get_object',
                                      object_id: id, });
        send(data);
    },
    
    agents: function () {
        var data = $.extend({}, this.consts, { cmd: 'get_agent_list' });
        send(data);
    },

    objects: function() {
        var data = $.extend({}, this.consts, { cmd: 'get_object_list' });
        send(data);
    }
};

action = {

    type: 'agent',
        
    move: function (agent_id, dx, dy, dz) {
        var data = { 
            cmd: "move_0",
            type: this.type,
            client_id: globals.client_id,
            world_id: globals.world_id,
            agent_id: agent_id,
            dp: [dx, dy, dz]
        };
        
        send(data);
    },
    
    till: function (agent_id) {
        var data = {
            cmd: 'till_soil',
            type: this.type,
            world_id: globals.world_id,
            agent_id: agent_id,
        };
        send(data);
    },

    plant: function (agent_id) {
        var data = {
            cmd: 'plant_crop',
            type: this.type,
            world_id: globals.world_id,
            agent_id: agent_id,
        };
        send(data);
    },

    harvest: function (agent_id, crop_id) {
        //temporary
        if (crop_id === undefined) {
            crop_id = 1;
        }
        
        var data = {
            cmd: 'harvest_crop',
            type: this.type,
            world_id: globals.world_id,
            agent_id: agent_id,
            crop_id: crop_id,
        };
        send(data);
    }
};
