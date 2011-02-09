
function Terrain_map()
{
	this.map = [];
	this.x_size = 0;
	this.y_size = 0;
	this.z_size = 0;
	
	this.prototype.init = function(x_size, y_size) {
		
		
	}
	
	
	this.prototype.set = function(x,y,z,tile_value) {
		
	}
	
	this.prototype.get = function(x,y,z) {
		
		
	}
}

function Game_world(world_id)
{
	this.world_id = world_id;
	this.agent_list = [];
	this.object_list = [];
	
	this.map = null;
	
	
	this.protype.load = function() {
		//load map from server
		
		//
	}

}
