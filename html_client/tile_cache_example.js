function tile_cache_canvas() {
	
	//needs init code
	//will re-init the cache when board is re-sized
	
	cache_canvas_dom = ??? // the thing used for reading/writing to canvas
	
	canvas_tile_width = 16;  //width of canvas in number of tiles
	canvas_tile_height = 16; //width of canvas in number of tiles
	
	tile_pixel_width = 20;
	tile_pixel_width = 20;
	
}

function tile_cache() {

	tile_cache_canvas = ??? // assign an instance
		
	cache_dict = []; //maps tile_id to number
	
	tileset_metadata = ??? //stores the rendering metadata returned by the get_tiles info command
	
	board_canvas // canvas that we are drawing to
	
	function draw_tile(board_x, board_y, tile_id) { //takes the x,y position and id of tile type to draw
		
		//check to see if tile is in cache, if not; add to cache
		if(! tile_id in cache_dict) {
			//tile not in cache, draw tile into cache
			
			//tile drawing properies are symbol, symbol_color, and background color
			
			//hardcode for now, but will get from tileset_metadata eventually
			symbol = 1; 
			symbol_color = [256, 256, 256] //rgb
			background_color = [100, 0, 0] //rgb
			
			
			
		}
		
		//copy tile from the tile_cache onto the board
		
		
		
	}
	
}
