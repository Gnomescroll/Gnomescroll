
//stores tileset drawing information
var tileset_state = {
	tile_rendering: [],
	tile_name_to_id : [],
	tile_id_to_name : [],

	add_tile : function(param) {
		this.tile_name_to_id[ param.tile_name] =  param.tile_id;
		this.tile_id_to_name[ param.tile_id ] = param.tile_name;

		var data = {
			//validate
			tile_name: param.tile_name,
			tile_id :  param.tile_id,	
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
		//if(-1 in this.tile_rendering) { return this.tile_rendering[-1]; }
		else {
			//return default if tile does not exist in rendering info
			//console.log("Tile rendering infomation missing: " + tile_id)
			data = {
				tile_name: 'non-existance tile',
				tile_id :  -1,	
				tilemap_id: 0,
				draw_style: 1,
				background_rgb: [0, 0, 0],
				symbol:  63,
				symbol_rgb: [0, 30, 30],	
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

var drawingCache = {
	board : null,
	tilemaps : [],
	
	img_cache : [],
	img_cache_count : 0,
	
	//for tiles
	tlookup : [],
	//for sprites (items, objects, building, agent sprites)
	slookup : [],
	
	workspace_canvas_dom : null,
	ctx: null,
	
	init : function init(board_canvas) {
            this.workspace_canvas_dom = $('canvas#DrawingCacheWorkspace')[0];
            this.ctx = this.workspace_canvas_dom.getContext("2d");
            this.board = board_canvas;
	},
	
	resize : function resize() {
		//clears cache, changes size of tiles being drawn to cache
		this.img_cache = [];
		this.cache_count = 0;
		
		this.tlookup = [];
		for(index in this.slookup) {
			this.slookup = [];
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
		//console.log(_dom_element)
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
		this.slookup[tilemap_id] = [];
	},

	//insert tile into tile_drawing_cache
	insertTile : function insertTile(tile_id) {

/*
		if(!(tild_id in this.tileset_state.tile_rendering)) {
			console.log("Tile drawing information not loaded: " + tile_id)
			//return 0;
			tile_id = -1;
		}
*/

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

		if(!(tilemap_id in this.tilemaps)) { 
			console.log("Tilemap does not exist:" + tilemap_id) 
			}
		
		tilemap = this.tilemaps[tilemap_id];
		
		
		tile_x_pos = tile_num % tilemap.tile_width;
		tile_y_pos = tile_num - tile_x_pos;
		if(tile_y_pos != 0) { tile_y_pos = tile_y_pos / tilemap.tile_width; }
		
		x_offset = tile_x_pos * tilemap.tile_pixel_width;
		y_offset = tile_y_pos * tilemap.tile_pixel_height;
		
		this.ctx.clearRect(0, 0, this.board.tile_pixel_width, this.board.tile_pixel_height); //needed?
		
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
		
	/// MATH MATH MATH		
			console.log("draw_style: " + draw_style)
			if(draw_style == 0) {
				//This is copy/paste unmodified
			}
			
			if(draw_style == 1) {
				var a, r, g, b;
				var b_r, b_g, b_b;
				
				b_r = background_rgb[0] / 255;
				b_g = background_rgb[1] / 255;
				b_b = background_rgb[2] / 255;
				
				s_r = symbol_rgb[0] / 255;
				s_g = symbol_rgb[1] / 255;
				s_b = symbol_rgb[2] / 255;

				for (var i = 0, n = pix.length; i < n; i += 4) {

					r = pix[i  ] / 255;
					g = pix[i+1] / 255;
					b = pix[i+2] / 255;
					a = pix[i+3] / 255;
					a_ = 1 - a;
					
					pix[i  ] = Math.floor( (s_r*r*a + b_r*a_)* 255 ); // red
					pix[i+1] = Math.floor( (s_g*g*a + b_g*a_)* 255 ); // green
					pix[i+2] = Math.floor( (s_b*b*a + b_b*a_)* 255 ); // blue
					pix[i+3] = 255;	
				}	
			}
			
			if(draw_style == 2) {
				for (var i = 0, n = pix.length; i < n; i += 4) {
				  
				  if(pix[i+3] == 0) {
				//alpha channel is 0, show background
					pix[i  ] = background_rgb[0]; // red
					pix[i+1] = background_rgb[1]; // green
					pix[i+2] = background_rgb[2]; // blue				  
					pix[i+3] = 255;
					} else {
	
				  pix[i  ] = Math.floor( pix[i  ] * symbol_rgb[0] / 256 ); // red
				  pix[i+1] = Math.floor( pix[i+1] * symbol_rgb[1] / 256 ); // green
				  pix[i+2] = Math.floor( pix[i+2] * symbol_rgb[2] / 256 ); // blue
				  // i+3 is alpha (the fourth element)
					}
				}
			}
			
		/// MATH MATH MATH

		var cache_index = ++this.img_cache_count;
		this.img_cache[cache_index] = imgd;
		
		this.tlookup[tile_id] = cache_index;

	},
	
	drawTile : function drawTile(x, y, tile_id) {
		if(!(tile_id in this.tlookup)) {
			//if(tileset_state.tile_id_to_name[tile_id] == undefined) {
			//	console.log("Tile metadata does not exist: " + tile_id)
				//return 0;
				//tile_id = -1; //render unknown tile
			//} else {
			console.log("Tile not loaded: " + tile_id)
			var rvalue = this.insertTile(tile_id);
			if(rvalue == 0) return 0; //usually means tileset is not loaded
			//}
		}
		var index = this.tlookup[tile_id];
		this.board.ctx.putImageData(this.img_cache[index], x*this.board.tile_pixel_width, y*this.board.tile_pixel_height);

	},

	//draw tiles to an arbritary canvas
	drawTileToCtx : function drawTileToCTX(ctx, tile_id, x_offset, y_offset) {
		if(x_offset !== undefined || y_offset !== undefined) {
			x_offset = 0;
			y_offset = 0;
		}

		if(!(tile_id in this.tlookup)) {
			var rvalue = this.insertTile(tile_id);
			if(rvalue == 0) return 0;
		}
		var index = this.tlookup[tile_id];			
		ctx.putImageData(this.img_cache[index], x_offset, y_offset);		
	},
/*
	///used for drawing a tile to the sidebar or other canvas
	drawTileToCtx : function drawTileToCtx(ctx, tile_id, ctr
*/

	//insert tile into tile_drawing_cache
	insertSprite : function insertSprite(tile_num, tilemap_id) {

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
		this.ctx.drawImage(tilemap.image, x_offset, y_offset, 
					tilemap.tile_pixel_width, tilemap.tile_pixel_height,
					0, 0, this.board.tile_pixel_width, this.board.tile_pixel_height);
		var imgd = this.ctx.getImageData(0, 0, this.board.tile_pixel_width, this.board.tile_pixel_height);
		var pix = imgd.data;
		//do manipulations (assume copy/paste for sprites)
		var cache_index = ++this.img_cache_count;
		this.img_cache[cache_index] = imgd;	
		this.slookup[tilemap_id][tile_num] = cache_index;
	},

	drawSprite : function drawSprite(x, y, sprite_num, spriteMap_id) {
		if(!(spriteMap_id in this.slookup)) { 
			console.log("DrawingCache.drawSprite Error: Tilemap/spriteMap not loaded: " + spriteMap_id)
			return 0;
			}
		if(!(sprite_num in this.slookup[spriteMap_id])) { this.insertSprite(sprite_num, spriteMap_id); }
		var index = this.slookup[spriteMap_id][sprite_num];
		this.board.ctx.putImageData(this.img_cache[index], x*this.board.tile_pixel_width, y*this.board.tile_pixel_height);
	}
}

