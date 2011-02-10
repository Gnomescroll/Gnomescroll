
//replace with more memory efficient version, later
function Terrain_map()
{
	this.map = [];
	this.x_size = 0;
	this.y_size = 0;
	this.z_size = 0;
	
	this.prototype.init = function(x_size, y_size, z_size) {
		this.x_size = x_size;
		this.y_size = y_size;
		this.z_size = z_size; //z is not used currently
		
		this.map = new Array(x_size*y_size);
	}
	
	this.prototype.set = function(x,y,z,tile_value) {
		this.map[this.y_size * y + x] = tile_value;
	}
	
	this.prototype.get = function(x,y,z) {
		return this.map[this.y_size*y + x];
	}
}

function Game_world(world_id)
{
	this.world_id = 0; //fixed for debugging
	this.agent_list = [];
	this.object_list = [];
	
	this.map = new Terrain_map;
	
	
	this.protype.load = function() {
		//load map from server
		
		//make request for map data from server
		info_get_map(this.world_id); //should be simple get method... but is call back function
		info_get_agent_list(this.world_id);
		info_get_object_list(this.world_id);
	}

}

function info_get_map(world_id) {
	client_id = 5;
	world_id = 0;
	
	data = { 
		command: "get_map",
		type: "info",
		client_id: client_id,
		world_id: world_id 
		};

	$.post(window.ajax_server, data);
}

function info_get_agent_list(world_id) {
	client_id = 5;
	
}

function info_get_object_list(world_id) {
	client_id = 5;
}
