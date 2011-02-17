
/*
//globals.update( { world_id: 7, client_id: 15 } ) and it will update that

var globals = {

world_id: 0,
client_id: 0,
server_out: null,
update: update = function(params) {
this.world_id = typeof params.world_id === 'number' ? params.world_id : this.world_id;
this.client_id = typeof params.client_id === 'number' ? params.client_id : this.client_id;
this.server_out = typeof params.server_out !== undefined ? params.server_out : this.server_out;
}
}
*/


var globals = {

    world_id: 0,
    client_id: 0,
    server_out: null,
    update: update = function(params) {
                        this.world_id = typeof params.world_id === 'number' ? params.world_id : this.world_id;
                        this.client_id = typeof params.client_id === 'number' ? params.client_id : this.client_id;
                        this.server_out = typeof params.server_out !== undefined ? params.server_out : this.server_out;
                    }
}
           

/*
function Globals()
{
	this.world_id = 0;
	this.client_id = 0;
	this.server_out = null;

	this.init = function(world_id, client_id, server_out) {
	this.world_id = world_id;
	this.client_id = client_id;
	this.server_out = server_out;
	}	
}
*/


function Admin_commands(globals)
{
	this.globals = globals;
	this.type = 'info';
	
	this.create_agent = function(x,y,z, player_id) {
		if(player_id == null) { player_id=0;}
	
		data = { 
		cmd: 'create_agent',
		type: this.type,
		client_id: this.globals.client_id,
		world_id: this.globals.world_id,
		player_id: player_id
		};
		
		$.post(this.globals.server_out, data);	
	}

	this.create_item = function(x,y,z) {

		data = { 
		cmd: "create_item",
		type: this.type,
		client_id: this.globals.client_id,
		world_id: this.globals.world_id,
		position: [0, x, y, z]
		};
		
		$.post(this.globals.server_out, data);	
				
	}
	
	this.set_map = function(x,y,z, value) {

		data = { 
		cmd: "set_map",
		type: this.type,
		client_id: this.globals.client_id,
		world_id: this.globals.world_id,
		position: [0, x, y, z],
		value: value
		};
		
		$.post(this.globals.server_out, data);	
				
	}	
}

function Info_commands(globals)
{
	this.globals = globals;
	this.type = "info";
	
	this.init = function(globals) {	this.globals = globals; }
	
	this.get_map = function() {

		data = { 
		cmd: "get_map",
		type: this.type,
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
				
	}

	this.get_agent = function(id) {

		data = { 
		cmd: "get_agent",
		type: this.type,
		client_id: this.globals.client_id,
		world_id: this.globals.world_id,
		agent_id: id
		};
		
		$.post(this.globals.server_out, data);	
				
	}

	this.get_object = function(id) {

		data = { 
		cmd: "get_object",
		type: this.type,
		client_id: this.globals.client_id,
		world_id: this.globals.world_id,
		object_id: id
		};
		
		$.post(this.globals.server_out, data);	
				
	}
	
	this.get_agent_list = function() {
		
		data = { 
		cmd: "get_agent_list",
		type: this.type,
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
				
	}

	this.get_object_list = function() {
		
		data = { 
		cmd: "get_object_list",
		type: this.type,
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
	}
			
}

function Agent_commands(globals)
{
	this.globals = globals;
	this.type = 'agent'
	
	this.init = function(globals) {	this.globals = globals; }
	
	this.move_0 = function(agent_id, dx, dy, dz) {

		data = { 
		cmd: "move_0",
		type: this.type,
		client_id: this.globals.client_id,
		world_id: this.globals.world_id,
		agent_id: agent_id,
		dp: [dx, dy, dz]
		};
		
		$.post(this.globals.server_out, data);	

	}
}
