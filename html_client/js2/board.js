//this is where drawing occurs


var board = {
	
	board_z_level: 1,
	board_x_offset: 0,
	board_y_offset: 0,

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
		this.board_x_offset += dx;
		this.board_y_offset += dy;
		this.reset();
	},

	scroll_z : function(zLevel) {
		this.board_z_level = zLevel;
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
		this.y_min = this.board.y_offset;
		this.x_max = this.board.tile_height + this.x_min;
		this.y_max = this.board.tile_height + this.x_max;
		
		var i;
		for(var x=0; x < this.board_tile_width; x++) {
			for(var y=0; y < this.board_tile_height; y++) {
				i = x + y*board_tile_width;
				this.index[i] = {
					x_pos : x,
					y_pos : y,
					drawing_cursor: [0,-1,-1],
					tile : 1,
					agent_num : 0,
					agent_list : [],
					object_num : 0,
					object_list: [],
				}
			}	
		}
		this.tile = [];
		this.agents = [];
		this.objects = [];
		
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
	
	redraw_all : function() {
		for(var x=0; x < this.board_tile_width; x++) {
			for(var y=0; y < this.board_tile_height; y++) {
				
				
			}	
		}
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
