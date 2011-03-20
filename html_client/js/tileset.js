
//stores tileset drawing information
var tileset_state = {
	tile_rendering: [],
	tile_name_to_id : [],
	
	add_tile : function(param) {
		this.tile_name_to_id[ param.tile_name] =  param.tile_id;
			
		var data = {
			//validate
			tile_name: param.tile_name,
			tile_id :  param.tile_id,	
			tilemap:  param.tilemap,
			tilemap_id:  param.tilemap_id,
			draw_style: param.draw_style,
			background_rgb:  param.background_rgb,
			symbol:  param.symbol,
			symbol_rgb: param.symbol_rgb,
		};
		this.tile_rendering[param.tile_id] = data;
	},
	
	get_tile_rendering_info : function(tile_id) {
		if(tile_id in this.tile_rendering) { return this.tile_rendering[tile_id]; }
		else {
			//return default if tile does not exist in rendering info
			console.log("Tile rendering infomation missing: " + tile_id)
			data = {
				tile_name: 'non-existance tile',
				tile_id :  -1,	
				tilemap:  'default',
				tilemap_id: 0,
				draw_style: 2,
				background_rgb: [0, 0, 0],
				symbol:  1,
				symbol_rgb: [0, 150, 150],	
			}
			return data;
		}

	},
}

/// This canvas is scratch space for drawing tiles, before they are stored in the tile_cache
var tile_cache_canvas = {
	//will re-init the cache when board is re-sized
	
	canvas_tile_width: 16,  //width of canvas in number of tiles
	canvas_tile_height: 16, //width of canvas in number of tiles
	
	tile_pixel_width: 24,
	tile_pixel_height: 24,

	cache_canvas_dom: $('<canvas></canvas>').attr('id', 'cache'), // the thing used for reading/writing to canvas
                       
	ctx: null, // drawing surface
    
    init: function () {
            //$('body').append(this.cache_canvas_dom);
            //$('canvas#cache').width(this.canvas_tile_width * this.tile_pixel_width)
            //                 .height(this.canvas_tile_height * this.tile_pixel_height);
            this.cache_canvas_dom = $('canvas#cache')[0];
            this.ctx = this.cache_canvas_dom.getContext("2d");
        },
        
    set_ctx: function () {
                this.cache_canvas_dom = $('canvas#cache')[0];
                this.ctx = this.cache_canvas_dom.getContext("2d");
            },
            
	resize_map: function () {
		///Will be called to handle tileset re-sizing on map
	},
}

function create_tilemap(src, tilemap_id, tpw, tph, tw, th) {
	var _dom_element;
	_dom_element = $('<img />').attr('src', src).attr('id', 'tilemap_'+tilemap_id);
	$('body').append(_dom_element);
	var img = new Image();
	img.src = src; // "static/tiles/Bisasam_24x24.png";
	var tilemap = {
	dom_element: _dom_element,
	image: img,
				
	tile_pixel_width: tpw, //in pixels
	tile_pixel_height: tph,
	tile_width: tw, //in tiles
	tile_height: th,		
	};
	return tilemap;	
}

function create_default_tilemap() { return create_tilemap("static/tiles/Bisasam_24x24.png", 0, 24, 24, 16, 16); }

/*
var tilemap = {
	
	elem: $('img#tilemap')[0],
	
	///!?!? Is this being evaluated everytime it is called?
	image: ( function() {
						var img = new Image();
						img.src = "static/tiles/Bisasam_24x24.png";
						return img;
					}()),
				
	
	tile_pixel_width: 24, //in pixels
	tile_pixel_height: 24,
	tile_width: 16, //in tiles
	tile_height: 16
	}
*/

var drawingCache = {
	board : null,
	tilemaps : [],
	
	img_cache : [],
	img_cache_count : 0,
	
	//for tiles
	tlookup : [],
	
	workspace_canvas_dom : null,
	ctx: null,
	
	init : function init() {
            this.workspace_canvas_dom = $('canvas#DrawingCacheWorkspace')[0];
            this.ctx = this.workspace_canvas_dom.getContext("2d");
            this.board = board_canvas;
	},
	
	resize : function resize() {
		//clears cache, changes size of tiles being drawn to cache
		this.img_cache = [];
		this.cache_count = 0;
		
		for(index in this.tlookup) {
			this.tlookup = [];
		}
	},

	//adds a tilemap to cache
	insertTilemap : function insertTilemap(src, tilemap_id, tpw, tph, tw, th) {
		if(tilemap_id in this.tilemaps) {
			console.log("Error: Attempt to insert same tilemap twice")
			return 0;
		}
		
		//var _dom_element = $('<img />').attr('src', src).attr('id', 'tilemap_'+tilemap_id);
		//$('body').append(_dom_element);
		var _dom_element = $('img#tileset_'+tilemap_id)
		
		console.log(_dom_element)
		
		var img = new Image();
		
		console.time("loaded: "+tilemap_id)
		img.onload = function() { 
			console.timeEnd("loaded: "+tilemap_id)
			 }
		img.src = src; // "static/tiles/Bisasam_24x24.png";
		var tilemap = {
			dom_element: _dom_element,
			image: img,
			tile_pixel_width: tpw, //in pixels
			tile_pixel_height: tph,
			tile_width: tw, //in tiles
			tile_height: th,		
		};
		this.tilemaps[tilemap_id] = tilemap;
		this.tlookup[tilemap_id] = [];
	},

	//insert tile into tile_drawing_cache
	insertTile : function insertTile(tile_id) {

		var tile = tileset_state.get_tile_rendering_info(tile_id);
			
		var tilemap_id, tile_num; //need this

		var tile = tileset_state.get_tile_rendering_info(tile_id);
		tilemap_id = tile.tilemap_id;
		tile_num = tile.symbol;

		var tilemap,
			x_offset,
			y_offset,
			tile_x_pos,
			tile_y_pos;
		
		if(!(tilemap_id in this.tilemaps)) { console.log("tilemap does not exist:" + tilemap_id) }
		tilemap = this.tilemaps[tilemap_id];
		
		tile_x_pos = tile_num % tilemap.tile_width;
		tile_y_pos = tile_num - tile_x_pos;
		if(tile_y_pos != 0) { tile_y_pos = tile_y_pos / tilemap.tile_width; }
		
		x_offset = tile_x_pos * tilemap.tile_pixel_width;
		y_offset = tile_y_pos * tilemap.tile_pixel_height;
		
		this.ctx.clearRect(0, 0, this.board.tile_pixel_width, this.board.tile_pixel_height); //needed?
		
		console.log(tilemap)
		console.log(tilemap_id)
		console.log(tile_num)
		
		this.ctx.drawImage(tilemap.image, x_offset, y_offset, 
					tilemap.tile_pixel_width, tilemap.tile_pixel_height,
					0, 0, this.board.tile_pixel_width, this.board.tile_pixel_height);
		
		var imgd = this.ctx.getImageData(0, 0, this.board.tile_pixel_width, this.board.tile_pixel_height);
		var pix = imgd.data;
		
		//do manipulations
		var draw_style, background_rgb, symbol_rgb;
		
		draw_style = tile.draw_style;
		background_rgb = tile.background_rgb;
		symbol_rgb = tile.symbol_rgb;
		
		var cache_index = ++this.img_cache_count;
		this.img_cache[cache_index] = imgd;
		
		this.tlookup[tile_id] = cache_index;

	},
	
	drawTile : function drawTile(x, y, tile_id) {
		if(!(tile_id in this.tlookup)) {
			console.log("Tile not loaded: " + tile_id)
			var rvalue = this.insertTile(tile_id);
			if(rvalue == 0) return 0; //usually means tileset is not loaded
		}
		var index = this.tlookup[tile_id];
		this.board.ctx.putImageData(this.img_cache[index], x*this.board.tile_pixel_width, y*this.board.tile_pixel_height);
	
		console.log(this.img_cache[index])
		console.log(index)
	
	},
}

/// MOVE THIS to board.js
//this is where drawing occurs
var board_canvas = {
	    
	//needs init code
	//will re-init the cache when board is re-sized
	
	canvas_tile_width: 16,  //width of canvas in number of tiles
	canvas_tile_height: 16, //width of canvas in number of tiles
	
	tile_pixel_width: 24,
	tile_pixel_height: 24,
    
	cache_canvas_dom: $('<canvas></canvas>').attr('id', 'board'), // the thing used for reading/writing to canvas
                       
	ctx: null, // drawing surface
    
    init: function () {
            //$('body').append(this.cache_canvas_dom);
            //$('canvas#board').width(this.canvas_tile_width * this.tile_pixel_width)
            //                .height(this.canvas_tile_height * this.tile_pixel_height);
            this.cache_canvas_dom = $('canvas#board')[0];
            this.ctx = this.cache_canvas_dom.getContext("2d")
        },
        
    resize: function() {
		///Will be called to handle resizing of board and/or zooming
	},
}
