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
	
	scroll : function () {
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
		for(x in this.index) {
			if(x.drawing_cursor[0] != -1) {
				//draw tile
				// x.tile_id, x.bx, x.by
			} else if(x.drawing_cursor[1] != -1) {
				//draw agent
			} else if(x.drawing_cursor[2] != -1) {
				//draw object
			}		
		}
	}

	// tile -> agents -> objects -> tile
	advance_drawing_cursor : function(bx, by) {
		this._advance_drawing_cursor
		
	},
	
	advance_all_drawing_cursor : function() {
		for(x in this.index) {
			this._advance_drawing_cursor(x)
		}
	},
		
	// takes an this.index element
	_advance_drawing_cursor : function(x) {
		if(x.drawing_cursor[0] != -1) 
			{
				if(x.agent_num > 0) 
				{
					x.drawing_cursor[0] = -1;
					x.drawing_cursor[1] = 0;
				} 
				else if(x.object_num > 0) 
				{
					x.drawing_cursor[0] = -1;
					x.drawing_cursor[2] = 0;
				}
				else
				{
					//do nothing, only the tile exists on this square
				}
			}
			else if(x.drawing_cursor[1] != -1)
			{
				console.log("board_manager.advance_drawing_cursor: WTF 0")
				x.drawing_cursor[1]++;
				if(x.drawing_cursor[1] < x.agent_num)
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
					else
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
	}
	
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
		//for list of agents, if agent is on map add to list
		
	}
	
	
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
