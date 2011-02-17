

function Globals()
{
	this.world_id = 0;
	this.client_id = 0;
	this.server_out = null;

	this.prototype.init = function(world_id, client_id, server_out) {
	this.world_id = world_id;
	this.client_id = client_id;
	this.server_out = server_out;
	}	
}

function Admin_commands()
{
	this.globals = null;
	this.type = 'info';
	
	this.prototype.init = function(globals) {	this.globals = globals }
	
	this.prototype.create_agent = function(x,y,z, player_id) {
		if(player_id == null) { player_id=0;}
	
		data = { 
		cmd: "create_agent",
		type: this.type
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
	}

	this.prototype.create_item = function(x,y,z) {

		data = { 
		cmd: "create_item",
		type: this.type
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
				
	}
	
	this.prototype.set_map = function(x,y,z, value) {

		data = { 
		cmd: "set_map",
		type: this.type
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
				
	}	
}

function Info_commands()
{
	this.globals = null;
	this.type = "info";
	
	this.prototype.init = function(globals) {	this.globals = globals }
	
	this.prototype.get_map = function() {

		data = { 
		cmd: "get_map",
		type: this.type
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
				
	}

	this.prototype.get_agent = function(id) {

		data = { 
		cmd: "get_agent",
		type: this.type
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
				
	}

	this.prototype.get_object = function(id) {

		data = { 
		cmd: "get_object",
		type: this.type
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
				
	}
	
	this.prototype.get_agent_list = function() {
		
		data = { 
		cmd: "get_agent_list",
		type: this.type
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
				
	}

	this.prototype.get_object_list = function() {
		
		data = { 
		cmd: "get_object_list",
		type: this.type
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	
	}
			
}

function Agent_commands()
{
	this.globals = null;
	this.type = 'agent'
	
	this.prototype.init = function(globals) {	this.globals = globals }
	
	this.prototype.move_0 = function(agent_id, dx, dy, dz) {

		data = { 
		cmd: "move_0",
		type: this.type
		client_id: this.globals.client_id,
		world_id: this.globals.world_id
		};
		
		$.post(this.globals.server_out, data);	

	}
}
