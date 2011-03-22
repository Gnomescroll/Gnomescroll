//this is where drawing occurs


var board = {
	
	z_level: 1,
	x_offset: 0,
	y_offset: 0,

	board_tile_width: 16,
	board_tile_height: 16,
	
	board_canvas : null,
	board_manager : null,
	
	init : function() {
		this.board_canvas = board_canvas;
		this.board_canvas.init();
		this.board_manager = board_manager;
		this.board_manager.init();
	},
	
	resize : function() {
		///resize board
	},
	
	scroll : function(dx, dy) {
		this.x_offset += dx;
		this.y_offset += dy;
		this.reset();
	},

	scroll_z : function(zLevel) {
		this.z_level = zLevel;
		this.reset();
	}
	
	///redraw
	reset : function() {
		this.board_manager.reset();		
	},
}

var board_manager = {
	
	board: null,
	board_canvas: null,
	
	x_min : null,
	x_max : null,
	y_min : null,
	y_max : null,
	z_level : null,
	
	index : [], //index stores drawing cursor information for each board position

	agents : [],
	objects : [],

	init : function(board) {
		this.board = board;
		this.board_canvas = board.board_canvas;
		
		this.x_min = this.board.x_offset;
		this.x_max = this.board.tile_height + this.x_min;
		this.y_min = this.board.y_offset;
		this.y_max = this.board.tile_height + this.x_max;
		this.z_level = this.board.z_level;

		this.reset_index();
		this.populate_index();
	},

	resize : function () {
		//resizing
	},
	
	reset : function () {
		//redraw everything
	},
	
	scroll : function (dx, dy) {
		var method = "slow";
			console.timeEnd('a')
		console.time('Scroll: ' + dx + " " + dy)
		if(method == slow) { this._easy_scroll(dx,dy); }
		if(method == fast) { this._fast_scroll(dx,dy); }
		console.timeEnd('Scroll: ' + dx + " " + dy)
	},
	
	_easy_scroll : function (dx, dy) {
		///implement the easy to implement way (drop everything and recompute from scratch)
	}
	
	_fast_scroll : function(dx, dy) {
		///implement scrolling the quick way
		//partial drop of data, get new data, then redraw
	},
	
	
	
	//does a full redraw
	blip : function() {
		for(var x=0; x < this.board_tile_width; x++) {
			for(var y=0; y < this.board_tile_height; y++) {
				this.draw_board(x,y);
			}	
		}
	},

	draw_board : function() {
		for(x in this.index) { this._draw_board_tile(x); }
	}
	
	draw_board_tile : function(bx,by) {
		this._advance_drawing_cursor(this.index[bx + by*this.board.board_tile_width);	
	}

	_draw_board_tile : function(x) {
		if(x.drawing_cursor[0] != -1) {
			//draw tile
			// x.tile_id, x.bx, x.by
		} else if(x.drawing_cursor[1] != -1) {
			//draw agent
			// x.agent_list[x.drawing_cursor[1]], x.bx, x.by
		} else if(x.drawing_cursor[2] != -1) {
			//draw object
			// x.object_list[x.drawing_cursor[2]], x.bx, x.by
		}
	},

	// tile -> agents -> objects -> tile
	advance_drawing_cursor : function(bx, by) {
		this._advance_drawing_cursor(this.index[bx + by*this.board.board_tile_width);
	},
	
	advance_all_drawing_cursor : function() {
		for(x in this.index) {
			this._advance_drawing_cursor(x)
		}
	},
	
	//internal method, not interface method
	// takes an this.index element
	_advance_drawing_cursor : function(x) {
		if(x.drawing_cursor[0] != -1) //if cursor is on tile/rendering tile
		{
			if(x.agent_num > 0) //then if agents are on tile, render agent
			{
				x.drawing_cursor[0] = -1;
				x.drawing_cursor[1] = 0;
			} 
			else if(x.object_num > 0)  //if no agents, then render objects if they else
			{
				x.drawing_cursor[0] = -1;
				x.drawing_cursor[2] = 0;
			}
			else //else keep rendering the tile
			{
				//do nothing, only the tile exists on this square
			}
		}
		else if(x.drawing_cursor[1] != -1) //if cursor is rendering a agent
		{
			console.log("board_manager.advance_drawing_cursor: WTF 0.0")
			x.drawing_cursor[1]++;
			if(x.drawing_cursor[1] < x.agent_num) //if more agents, switch to next agent
			{
				//do nothings
			}
			else if(x.drawing_cursor[1] == x.agent_num)
			{
				if(x.object_num > 0)
				{
					x.drawing_cursor[1] = -1;
					x.drawing_cursor[2] = 0;
				}
				else //if no objects on square, then render tile
				{
					x.drawing_cursor[1] = -1;
					x.drawing_cursor[0] = 0;
				}
			}
			else if(x.drawing_cursor[1] > x.agent_num)
			{
				console.log("board_manager.advance_drawing_cursor: WTF 1, absolute error, probably a race condition")
			}
		}
		else if(x.drawing_cursor[2] != -1)
		{
			console.log("board_manager.advance_drawing_cursor: WTF 0.1")
			x.drawing_cursor[2]++;
			if(x.drawing_cursor[2] < x.object_num)
			{
				//do nothing
			}
			else if(x.drawing_cursor[2] == x.object_num)
			{
				x.drawing_cursor[2] = -1;
				x.drawing_cursor[0] = 0;
			}
			else if(x.drawing_cursor[2] > x.object_num)
			{
				console.log("board_manager.advance_drawing_cursor: WTF 2, absolute error, probably a race condition")				
			}
		}
	},
	
	//
	//internal utility functions, non-interface functions
	//
	reset_index: function() {
		var i;
		for(var x=0; x < this.board_tile_width; x++) {
			for(var y=0; y < this.board_tile_height; y++) {
				i = x + y*board_tile_width;
				this.index[i] = {
					drawing_cursor: [0,-1,-1],
					//last_blip : 0, //needed?
					tile_id : 1,
					agent_num : 0,
					agent_list : [],
					object_num : 0,
					object_list: [],
					//debugging information
					bx : x,
					by : y,
					position : [x+this.x_min,y+y_min,this.z_level],
				}
			}	
		}
	},
	
	populate_index: function() {
		this.reset_index();
		
		
		///for each agent/ determine if agent is on board and if so, add it to the index
		var x_pos, y_pos, z_pos, pos; //x,y positions
		
		agents = state.agents;/// fill this in; get list of agents
		for( agent in agents) 
		{
			pos = agent.pos();
			x_pos = agent_pos[0];
			y_pos = agent_pos[1];
			z_pos = agent_pos[2];
			
			if(z_pos != this.z_level) {
				console.log("agent z level errr")
				continue;
			}
			
			if( this.x_min <= x_pos && x_pos < this.x_max && this.y_min <= y_pos && this.y_max > y_pos)
			{
				//if agent is with confines of the board, add object to index
				this._add_agent_to_index( agent.id, x_pos ,y_pos, z_pos); //agent_id and x,y,z position
			}
		}
		
		var x_pos, agent_y_pos, agent_z_pos;
		for( object in state.objects) 
		{
			pos = object.pos();
			x_pos = pos[0];
			y_pos = pos[1];
			z_pos = pos[2];
		

			if(z_pos != this.z_level) {
				console.log("object z level errr")
				continue;
			}
			
			if( this.x_min <= x_pos && x_pos < this.x_max && this.y_min <= y_pos && this.y_max > y_pos)
			{
				this._add_object_to_index( object.id, x_pos ,y_pos, z_pos);
			}
		}
	},
	
	
	_add_agent_to_index : function(id, x_pos, y_pos, z_pos) {
		
	},
	
	_add_object_to_index : function(id, x_pos, y_pos, z_pos) {
		
	},
	
}

var cursor_manager = {
	
	
	
	
}
//this is where drawing occurs to
var board_canvas = {
	
	board : null,
	board_manager: null,
	
	//needs init code
	//will re-init the cache when board is re-sized
	
	canvas_tile_width: 16,  //width of canvas in number of tiles
	canvas_tile_height: 16, //width of canvas in number of tiles
	
	tile_pixel_width: 24,
	tile_pixel_height: 24,
    
	cache_canvas_dom: $('<canvas></canvas>').attr('id', 'board'), // the thing used for reading/writing to canvas
                       
	ctx: null, // drawing surface
    
    init: function (board) {
            this.cache_canvas_dom = $('canvas#board')[0];
            this.ctx = this.cache_canvas_dom.getContext("2d")
            this.board = board;
            this.board_manager = board.board_manager;
        },
        
    resize: function() {
		///Will be called to handle resizing of board and/or zooming
		
		///resize can increase/decrease number of map tile or can increase/decrease tilepixel size
	},
}
