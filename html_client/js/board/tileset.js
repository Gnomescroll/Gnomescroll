
//stores tileset drawing information
var tileset_state = {
	tile_render_info_by_id: [];
	tile_name_to_id : [];
	
	add_tile : function(param) {
	tile_name_to_id[ param.tile_name] =  param.tile_id
		
	var data = {
		//validate
		tile_name: param.tile_name,
		tile_id :  param.tile_id,	
		tile_set:  0,
		draw_type: 2,
		background_rgb:  param.background_rgb,
		symbol:  param.symbol,
		symbol_rgb: param.symbol_rgb,
	}
	
	tile_rendering[param.tile_id] = data;
	
	}
}

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
