
//stores tileset drawing information
var tileset_state = {
	tile_rendering: [];
	tile_name_to_id : [];
	
	add_tile : function(param) {
	tile_name_to_id[ param.tile_name] =  param.tile_id;
		
	var data = {
		//validate
		tile_name: param.tile_name,
		tile_id :  param.tile_id,	
		tile_set:  0,
		draw_type: 2,
		background_rgb:  param.background_rgb,
		symbol:  param.symbol,
		symbol_rgb: param.symbol_rgb,
	};
	tile_rendering[param.tile_id] = data;
	}
	
	get_tile_rendering_info : function(tile_id) {
		if(tile_id in tile_rendering) { return tile_rendering[tile_id]; }
		else {
			//return default if tile does not exist in rendering info
			console.log("Tile rendering infomation missing: " + tile_id)
			data = {
				tile_name: 'non-existance tile',
				tile_id :  -1,	
				tile_set:  0,
				draw_type: 2,
				background_rgb: [0, 0, 0],
				symbol:  1,
				symbol_rgb: [0, 150, 150],	
			}
			return data;
		}
		
		
		
	}
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

