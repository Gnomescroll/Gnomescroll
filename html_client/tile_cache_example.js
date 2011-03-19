var tile_cache_canvas = {
	    
	//needs init code
	//will re-init the cache when board is re-sized
	
	canvas_tile_width: 16,  //width of canvas in number of tiles
	canvas_tile_height: 16, //width of canvas in number of tiles
	
	tile_pixel_width: 24,
	tile_pixel_height: 24,

	cache_canvas_dom: $('<canvas></canvas>').attr('id', 'cache'), // the thing used for reading/writing to canvas
                       
	ctx: null, // drawing surface
    
    init: function () {
            $('body').append(this.cache_canvas_dom);
            $('canvas#cache').width(this.canvas_tile_width * this.tile_pixel_width)
                             .height(this.canvas_tile_height * this.tile_pixel_width);
            this.cache_canvas_dom = $('canvas#cache')[0];
            this.ctx = this.cache_canvas_dom.getContext("2d");
        },
        
    set_ctx: function () {
                this.cache_canvas_dom = $('canvas#cache')[0];
                this.ctx = this.cache_canvas_dom.getContext("2d");
            },
}

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
            $('body').append(this.cache_canvas_dom);
            $('canvas#board').width(this.canvas_tile_width * this.tile_pixel_width)
                             .height(this.canvas_tile_height * this.tile_pixel_width);
            this.cache_canvas_dom = $('canvas#board')[0];
            this.ctx = this.cache_canvas_dom.getContext("2d")
        },
}

var tilemap = {
	
	elem: $('img#tilemap')[0],
	
	image: ( function() {
						var img = new Image();
						img.src = "static/tiles/Bisasam_24x24.png";
						return img;
					}()),
					
	//tiles_wide: 16,
	//tiles_high: 16,
	
	tile_pixel_width: 24,
	tile_pixel_width: 24,
	tile_width: 24,
	tile_height: 24
	}

var drawTileToCache = function (tile_cache_canvas, tile_num, tilemap) {

	var x_offset,
		y_offset,
		tile_x_pos,
		tile_y_pos;
	
	tile_x_pos = tile_num % tilemap.tile_width;
	tile_y_pos = tile_num - tile_x_pos;
	if(tile_y_pos != 0) { tile_y_pos = tile_y_pos / tilemap.tile_width; }
	
	x_offset = tile_x_pos * tilemap.tile_pixel_width;
	y_offset = tile_y_pos * tilemap.tile_pixel_height;
	
	tile_cache_canvas.ctx.drawImage(tilemap.image, x_offset, y_offset, 
				tilemap.tile_pixel_width, tilemap.tile_pixel_height,
				0, 0, tile_cache_canvas.tile_pixel_width, tile_cache_canvas.tile_pixel_height);

//	ctx.drawImage(tilemap.image, 
//				  x_offset, y_offset, tile_pixel_width, tile_pixel_width,
//				  cell_x, cell_y, cell_width, cell_height);
}

//this stores tile
var tile_drawing_properties = (function () {
	
	var tile_drawing_property_array = [];
	
	function get(tile_id) {
		return tile_drawing_property_array[tile_id];
	}
	
	function set(tile_id, symbol, symbol_r, symbol_g, symbol_b, background_r, background_g, background_b) {
		tile_drawing_property_array[tile_id] = [symbol, [symbol_r, symbol_g, symbol_b], [background_r, background_g, background_b]];
	}
    
    return { get: get,
             set: set,
             array: tile_drawing_property_array }
}());

var tile_cache = (function () {
		//alert('entry 0');
    var tcc = tile_cache_canvas,
        cache_dict = [], //maps tile_id to number, [tile_cache_position, x_offset, y_offset, width, height]
        cache_counter = 0, //gives the next free spot in cache
        tdp = tile_drawing_properties, //stores the rendering metadata returned by the get_tiles info command
        bc = board_canvas; // canvas that we are drawing to
	
	function free_index() {
        
		return ++cache_counter;
		
		//check to see if free_index is greater than number of spots in cache
		//if so, much do garabage collection on cache
	}
	
	function draw_tile(board_x, board_y, tile_id) { //takes the x,y position and id of tile type to draw

        var symbol,
            symbol_color,
            background_color,
            tile_cache_position,
            x_row,
            y_row,
            x_offset,
            y_offset,
            width,
            height;
        
		//check to see if tile is in cache, if not; add to cache
		
		if(!( tile_id in cache_dict)) {
			
			//alert("Works");
		}
		

		if(! (tile_id in cache_dict)) {
			//tile not in cache, draw tile into cache
			
			//tile drawing properies are symbol, symbol_color, and background color
			
			/*
			temp = tileset_metadata.get_tdp(tile_id) ;
			// returns [symbol_num, [symbol_color_r, symbol_color_g, symbol_color_b], [background_color_r, background_color_g, background_color_b]]
			symbol = temp[0];
			symbol_color = temp[1];
			background_color = temp[2];
			 */
			
			//hardcode for now, but get drawing properties from tileset_data eventually
			symbol = 1; 
			symbol_color = [256, 256, 256]; //rgb
			background_color = [200, 0, 0]; //rgb
			
			//use square (0,0) as temporary drawing canvas
			
			//DO: draw background color onto (x,y)
			
			tile_cache_position = free_index();
			
			console.log("index: " + tile_cache_position);
			
			x_row = tile_cache_position % tcc.canvas_tile_width;
			y_row = tile_cache_position - x_row;
			if (y_row != 0) { y_row = y_row / tcc.canvas_tile_width; }
						
			console.log("x_row: " + x_row)
			console.log("y_row: " + y_row)
			
			x_offset = x_row * tcc.tile_pixel_width;	//in pixels
			y_offset = y_row * tcc.tile_pixel_height;	// in pixels
			
			console.log("x_offset: " + x_offset)
			console.log("y_offset: " + y_offset)
			
			width = tcc.tile_pixel_width;	//in pixels
			height = tcc.tile_pixel_height;	//in pixels

			console.log("width: " + width)
			console.log("height: " + height)
						
			//possibly clear canvas at (x_offset, y_offset, width, height)
			
			//draw solid color background onto canvas 
			tcc.ctx.fillStyle = fillStyle =  'rgb(' + background_color .join(',') + ')';
            console.log('draw coords');
            console.log([x_offset, y_offset, width, height]);
            tcc.set_ctx();
			tcc.ctx.fillRect(x_offset, y_offset, width, height);
			
			//clear canvas cache at (0, 0, width, height)
			tcc.ctx.clearRect(0, 0, width, height);
			
			//draw symbol from tile map to (0, 0, width, height)
			drawTileToCache(tcc, symbol, tilemap);
	
			console.log(tcc);
			
			//color symbol based upon symbol_color
			/*INSERT CODE HERE*/
			
			//copy (0, 0, width, height) onto (x_offset, y_offset, width, height)
			/*INSERT CODE HERE*/
		}
		
		//copy tile from the tile_cache onto the board
		

		//copy (x_offset, y_offset, width, height) from canvas tcc.ctx
		/*INSERT CODE HERE*/
		
		//to position x,y on board cavnas (need to calculate region)
		/*INSERT CODE HERE*/
	}
	
    return { free_index: free_index,
             draw_tile: draw_tile,
             cache_dict: cache_dict,
             cache_counter: cache_counter,
           }
}());
