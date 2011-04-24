
//stores tileset drawing information

var tile_properties = {
    tiles: {},
    
    add : function(property_object) {
        var id = property_object.id;
        this.tiles[id] = property_object;
    },
    
    get : function(id) {
        if(!this.tiles.hasOwnProperty(id)){
            console.log("Tile id is not stored");
        }
        return this.tiles[id];
    }
};

var tileset_state = {
    tile_rendering  : {},
    tile_name_to_id : {},
    tile_id_to_name : {},

    default_tile : {
        tile_name      : 'non-existance tile',
        tile_id        : -1,  
        tilemap_id     : 0,
        draw_style     : 1,
        background_rgb : [0, 0, 0],
        symbol         : 63,
        symbol_rgb     : [0, 30, 30]
    },

    add_tile : function(param) {
        this.tile_name_to_id[param.tile_name] = param.tile_id;
        this.tile_id_to_name[param.tile_id]   = param.tile_name;

        var data = {
            //validate
            tile_name      : param.tile_name,
            tile_id        : param.tile_id,   
            tilemap_id     : param.tilemap_id,
            draw_style     : param.draw_style,
            background_rgb : param.background_rgb,
            symbol         : param.symbol,
            symbol_rgb     : param.symbol_rgb
        };
        this.tile_rendering[param.tile_id] = data;
    },
    
    get_tile_rendering_info : function(tile_id) {
        var data;
        if (this.tile_rendering.hasOwnProperty(tile_id)) {
            return this.tile_rendering[tile_id];
        } else {
            //return default if tile does not exist in rendering info
            //console.log("Tile rendering infomation missing: " + tile_id);
            return this.default_tile;
        }
    }
};

function TileCanvas(id, parent_selector, dim) {

    this.width = dim.width || 16;
    this.height = dim.height || 16;
    this.pixel_width = dim.pixel_width || 24;
    this.pixel_height = dim.pixel_height || 24;
    this.canvas = $('<canvas></canvas').attr('id', id);
    this.ctx = null;

    this.init = function () {
        $(parent_selector).append(this.canvas);
        var cache_canvas = $('canvas#'+id).attr({ 'width' : this.width * this.pixel_width,
                                                  'height' : this.height * this.pixel_height
                                          });
        this.canvas = cache_canvas[0];
        this.ctx = this.canvas.getContext("2d");
    };
        
    this.set_ctx = function () {
        console.log('set_ctx id: '+id);
        return;
        this.canvas = $('canvas#'+id)[0];
        this.ctx = this.canvas.getContext("2d");
    };
            
    this.resize = function () {
        //Will be called to handle tileset re-sizing on map
    };
};

// This canvas is scratch space for drawing tiles, before they are stored in the tile_cache
var tile_cache_canvas = new TileCanvas('cache', 'body', { width: 16,
                                                          height: 16,
                                                          pixel_width: 24,
                                                          pixel_height: 24 });

// constructor for a new tilemap/tileset <img>
function Tilemap(src, tilemap_id, tpw, tph, tw, th) {
    // enforce exact arguments count
    if (this.constructor.length !== arguments.length) {
        throw new Error('Tilemap() construction has invalid number of arguments.');
    }
    var img = new Image(),
        _dom_element = $('<img />').attr({ 'src': src,
                                           'id': 'tilemap_'+tilemap_id,
                                           'class': 'tilemap' });

    $('body').append(_dom_element);
    img.src = src;

    this.dom_element = _dom_element;
    this.image = img;
    this.pixel_width = tpw;  //in pixels
    this.pixel_height = tph;
    this.tile_width = tw;    //in tiles
    this.tile_height = th;
};

var drawingCache = {

    tilemaps : [],
    
    img_cache : [],
    img_cache_count : 0,
    
    //for tiles
    tlookup : {},
    //for sprites (items, objects, building, agent sprites)
    slookup : {},
    
    workspace_canvas_dom : null,
    ctx: null,
    
    init : function (board_canvas) {
        var workspace_canvas = $('canvas#DrawingCacheWorkspace').attr({ 'width' : board.canvas.width * board.canvas.pixel_width,
                                                                        'height' : board.canvas.height * board.canvas.pixel_height
                                                                      });
        this.workspace_canvas_dom = workspace_canvas[0];
        this.ctx = this.workspace_canvas_dom.getContext("2d");
    },
    
    resize : function () {
        //clears cache, changes size of tiles being drawn to cache
        this.img_cache   = [];
        this.cache_count = 0;
        this.tlookup     = {};
        this.slookup     = {};
        // implement the rest
    },

    //adds a tilemap to cache
    insertTilemap : function (src, tilemap_id, tpw, tph, tw, th) {
        if(tilemap_id in this.tilemaps) {
            console.log("Error: Attempt to insert same tilemap twice");
            return false;
        }
        
        var tilemap = new Tilemap(src, tilemap_id, tpw, tph, tw, th);
        
        this.tilemaps[tilemap_id] = tilemap;
        this.slookup[tilemap_id] = [];
    },

    //insert tile into tile_drawing_cache
    insertTile : function (tile_id) {
        var tile = tileset_state.get_tile_rendering_info(tile_id),
            tilemap_id = tile.tilemap_id,
            tile_num = tile.symbol, //need this
            tilemap = this.tilemaps[tilemap_id],
            tile_x_pos = tile_num % tilemap.tile_width,
            tile_y_pos = tile_num - tile_x_pos,
            x_offset,
            y_offset,
            imgd,
            pix,
            draw_style = tile.draw_style,
            background_rgb = tile.background_rgb,
            symbol_rgb = tile.symbol_rgb,
            a, r, g, b,
            b_r, b_g, b_b,
            pix_len,
            i = 0,
            cache_index = ++this.img_cache_count;

        if (tile_y_pos != 0) {
            tile_y_pos = tile_y_pos / tilemap.tile_width;
        }
        
        x_offset = tile_x_pos * tilemap.pixel_width;
        y_offset = tile_y_pos * tilemap.pixel_height;
        
        this.ctx.clearRect(0, 0, board.canvas.pixel_width, board.canvas.pixel_height); //needed?
        
        this.ctx.drawImage(tilemap.image, x_offset, y_offset, 
                    tilemap.pixel_width, tilemap.pixel_height,
                    0, 0, board.canvas.pixel_width, board.canvas.pixel_height);

        imgd = this.ctx.getImageData(0, 0, board.canvas.pixel_width, board.canvas.pixel_height);
        pix  = imgd.data;
        pix_len = pix.length;

        if (draw_style == 0) {
            //This is copy/paste unmodified
        }
        
        if (draw_style == 1) {
            b_r = background_rgb[0] / 255;
            b_g = background_rgb[1] / 255;
            b_b = background_rgb[2] / 255;
            
            s_r = symbol_rgb[0] / 255;
            s_g = symbol_rgb[1] / 255;
            s_b = symbol_rgb[2] / 255;

            for (i = 0; i < pix_len; i += 4) {

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
        
        if (draw_style == 2) {
            for (i = 0; i < pix_len; i += 4) {
                if (pix[i+3] == 0) {
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

        this.img_cache[cache_index] = imgd;
        this.tlookup[tile_id] = cache_index;
        return cache_index;
    },

    // draws tile to board canvas
    drawTile : function (x, y, tile_id) {
        var index;

        if(!this.tlookup.hasOwnProperty(tile_id)) {
            index = this.insertTile(tile_id);
        } else {
            index = this.tlookup[tile_id];
        }
        board.canvas.ctx.putImageData(this.img_cache[index], x*board.canvas.pixel_width, y*board.canvas.pixel_height);
    },

    //draw tiles to an arbritary canvas
    drawTileToCtx : function(ctx, tile_id, x_offset, y_offset) {
        var index;

        if (x_offset === undefined) {
            x_offset = 0;
        }
        if (y_offset === undefined) {
            y_offset = 0;
        }

        if(!this.tlookup.hasOwnProperty(tile_id)) {
            index = this.insertTile(tile_id);
        } else {
            index = this.tlookup[tile_id];
        }
        ctx.putImageData(this.img_cache[index], x_offset, y_offset);        
    },

    //insert tile into tile_drawing_cache
    insertSprite : function (tile_num, tilemap_id) {
        var tilemap = this.tilemaps[tilemap_id],
            tile_x_pos = tile_num % tilemap.tile_width,
            tile_y_pos = tile_num - tile_x_pos,
            x_offset,
            y_offset,
            imgd,
            pix,
            cache_index = ++this.img_cache_count;
        
        if (tile_y_pos != 0) {
            tile_y_pos = tile_y_pos / tilemap.tile_width;
        }
        x_offset = tile_x_pos * tilemap.pixel_width;
        y_offset = tile_y_pos * tilemap.pixel_height;

        this.ctx.clearRect(0, 0, board.canvas.pixel_width, board.canvas.pixel_height); //needed?
        this.ctx.drawImage(tilemap.image, x_offset, y_offset, 
                    tilemap.pixel_width, tilemap.pixel_height,
                    0, 0, board.canvas.pixel_width, board.canvas.pixel_height);

        imgd = this.ctx.getImageData(0, 0, board.canvas.pixel_width, board.canvas.pixel_height);
        pix = imgd.data;

        this.img_cache[cache_index] = imgd; 
        this.slookup[tilemap_id][tile_num] = cache_index;
        return cache_index;
    },

    drawSprite : function (x, y, sprite_num, spriteMap_id) {
        var index;
        if (!this.slookup.hasOwnProperty(spriteMap_id)) { 
            console.log("DrawingCache.drawSprite Error: Tilemap/spriteMap not loaded: " + spriteMap_id);
            return false;
        }
        if (!this.slookup[spriteMap_id].hasOwnProperty(sprite_num)) {
            index = this.insertSprite(sprite_num, spriteMap_id);
        } else {
            index = this.slookup[spriteMap_id][sprite_num];
        }
        board.canvas.ctx.putImageData(this.img_cache[index], x*board.canvas.pixel_width, y*board.canvas.pixel_height);
    },
    
    drawSpriteToCtx : function (ctx, sprite_num, spriteMap_id, x_offset, y_offset) {
        var index;
        if (x_offset === undefined) {
            x_offset = 0;
        }
        if (y_offset === undefined) {
            y_offset = 0;
        }
        
        if (!this.slookup.hasOwnProperty(spriteMap_id)) {
            console.log("DrawingCache.drawSprite Error: Tilemap/spriteMap not loaded: " + spriteMap_id)
            return false;
        }
        if (!this.slookup[spriteMap_id].hasOwnProperty(sprite_num)) {
            index = this.insertSprite(sprite_num, spriteMap_id);
        } else {
            index = this.slookup[spriteMap_id][sprite_num];
        }
        ctx.putImageData(this.img_cache[index], x_offset, y_offset);
    }
};
