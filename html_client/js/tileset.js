
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
    },

    reset : function () {
        this.tiles = {};
    },
};

var tileset_state = {
    loaded : false,
    tile_rendering  : new Dict(),
    tile_name_to_id : {},
    tile_id_to_name : {},

    init : function () {
        info.tileset();
    },

    default_tile : {
        tile_name      : 'non-existant tile',
        is_default     : true,
        tile_id        : -1,  
        tileset_id     : 0,
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
            tileset_id     : param.tilemap_id,
            draw_style     : param.draw_style,
            background_rgb : param.background_rgb,
            symbol         : param.symbol,
            symbol_rgb     : param.symbol_rgb
        };
        this.tile_rendering[param.tile_id] = data;
    },
    
    get_tile_rendering_info : function(tile_id) {
        return this.tile_rendering.getDefault(tile_id, this.default_tile);
    },

    reset : function () {
        this.loaded = false;
        this.tile_rendering = {};
        this.tile_name_to_id = {};
        this.tile_id_to_name = {};
    },
};

// constructor for a new tileset <img>
function Tileset(src, tileset_id, tpw, tph, tw, th) {
    if (this.constructor.length !== arguments.length) { // enforce exact arguments count
        throw new Error(this.constructor.name +' construction has invalid number of arguments.');
    }
    if (!(this instanceof arguments.callee)) {
        return new Tileset(src, tileset_id, tpw, tph, tw, th);
    }

    var img = new Image(),
        _elem_id = 'tileset_' + tileset_id,
        _dom_element = $('<img />').attr({ 'src'  : src,
                                           'id'   : _elem_id,
                                           'class': 'tileset' });

    $('body').append(_dom_element);
    img.src = src;
    this.selector = '#' + _elem_id;
    this.dom_element = _dom_element;
    this.image = img;
    this.pixel_width = tpw;  //in pixels
    this.pixel_height = tph;
    this.tile_width = tw;    //in tiles
    this.tile_height = th;
};

// constructor for a Canvas
function TileCanvas(id, parent_selector, dim) {
    // id is the html id attr, parent_selector is a selector for the container
    // where the canvas will be placed in, and dim is an object of properties
    if (!(this instanceof arguments.callee)) {
        return new TileCanvas(id, parent_selector, dim);
    }
    if (dim && typeof dim === 'object') {
        this.width = dim.width || 16;
        this.height = dim.height || 16;
        this.pixel_width = dim.pixel_width || 24;
        this.pixel_height = dim.pixel_height || 24;
    }
    this.ctx = null;
    this.id = id;
    this.parent_selector = parent_selector;

    //this.prototype = this.constructor.methods; // assign class methods
    this.init();
};

TileCanvas.prototype = {
    
    init : function () {
        var w = this.width * this.pixel_width,
            h = this.height * this.pixel_height;
        w = w || 24*16;
        h = h || 24*16; // defaults
        
        if (!this.canvas) {
            this.canvas = $('<canvas></canvas>').attr('id', this.id);
        }

        if (this.canvas.constructor.name === 'HTMLCanvasElement') { // already init'd
            return false;
        }
        
        parent_selector = this.parent_selector || 'body';
        $(parent_selector).append(this.canvas);
        this.canvas.attr({ 'width' : w,
                           'height' : h });
        this.canvas = this.canvas[0];
        this.ctx = this.canvas.getContext("2d");
    },
        
    set_ctx : function () {
        if (this.canvas.constructor.name !== 'HTMLCanvasElement') {
            this.canvas = $('canvas#'+this.id)[0];
        }
        this.ctx = this.canvas.getContext("2d");
    },
            
    resize : function () {
        //Will be called to handle tileset re-sizing on map
    },

    reset : function () {
        $('canvas#'+this.id).remove();
        this.canvas = null;
        this.ctx = null;
        this.init();
    },
};

// This canvas is scratch space for drawing tiles, before they are stored in the tile_cache
var tile_cache_canvas = new TileCanvas('cache', '#caches', { width : 16,
                                                             height: 16,
                                                             pixel_width : 24,
                                                             pixel_height: 24 });


// canvas workspace for building a tile image
var drawingCache = {

    tilesets : [],
    
    img_cache : [],
    img_cache_count : 0,
    
    //for tiles
    tlookup : {},
    //for sprites (items, objects, building, agent sprites)
    slookup : {},
    
    canvas: null,
    ctx: null,
    id: 'DrawingCacheWorkspace',
    parent_selector: '#caches',
    
    init : function () {
        TileCanvas.prototype.init.call(this);
        //this.loadTilesets();
    },
    
    resize : function () {
        //clears cache, changes size of tiles being drawn to cache
        this.reset();
        // implement the rest
    },

    reset : function () {   // resets drawingCache (but does not re-insert tilesets)
        this.img_cache   = [];
        this.img_cache_count = 0;
        this.tlookup     = {};
        this.slookup     = {};
    },

    reset_full : function () { // reset everything, including re-downloading tilesets
        this.removeTilesets();
        this.reset();
    },

    removeTilesets : function () {
        var t;
        for (t in this.tilesets) {
            if (!this.tilesets.hasOwnProperty(t)) continue;
            $(this.tilesets[t].selector).remove();
        }
        this.tilesets = [];
    },

    loadTilesets : function () {
        drawingCache.insertTileset("/static/tiles/Bisasam_24x24.png", 0, 24, 24, 16, 16);
        drawingCache.insertTileset("/static/tiles/dwarves.png", 1, 18, 20, 16, 16);
    },

    //adds a tileset to cache
    insertTileset : function (src, tileset_id, tpw, tph, tw, th) {
        if(tileset_id in this.tilesets) {
            console.log("Error: Attempt to insert same tileset twice");
            return false;
        }
        
        var tileset = new Tileset(src, tileset_id, tpw, tph, tw, th);
        
        this.tilesets[tileset_id] = tileset;
        this.slookup[tileset_id] = [];
    },

    //insert tile into tile_drawing_cache
    insertTile : function (tile_id) {
        var tile       = tileset_state.get_tile_rendering_info(tile_id),
            tileset_id = tile.tileset_id,
            tile_num   = tile.symbol, //need this
            tileset    = this.tilesets[tileset_id],
            tile_x_pos = tile_num % tileset.tile_width,
            tile_y_pos = tile_num - tile_x_pos,
            x_offset,
            y_offset,
            imgd,
            pix,
            draw_style = tile.draw_style,
            symbol_rgb = tile.symbol_rgb,
            background_rgb = tile.background_rgb,
            a, r, g, b,
            b_r, b_g, b_b,
            pix_len,
            i = 0,
            cache_index;

        if (tile_y_pos != 0) {
            tile_y_pos = tile_y_pos / tileset.tile_width;
        }
        
        x_offset = tile_x_pos * tileset.pixel_width;
        y_offset = tile_y_pos * tileset.pixel_height;
        
        this.ctx.clearRect(0, 0, board.canvas.pixel_width, board.canvas.pixel_height); //needed?
        
        this.ctx.drawImage(tileset.image, x_offset, y_offset, 
                    tileset.pixel_width, tileset.pixel_height,
                    0, 0, board.canvas.pixel_width, board.canvas.pixel_height);

        imgd = this.ctx.getImageData(0, 0, board.canvas.pixel_width, board.canvas.pixel_height);
        pix  = imgd.data;
        pix_len = pix.length;

        if (draw_style == 0) {
            //This is copy/paste unmodified
        }
        else if (draw_style == 1) {
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
        else if (draw_style == 2) {
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

        if (! tile.is_default) {    // don't cache default (non-existant) tile
            cache_index = ++this.img_cache_count;
            this.img_cache[cache_index] = imgd;
            this.tlookup[tile_id] = cache_index;
        } else {
            cache_index = this.img_cache_count;
        }
        return cache_index;
    },

    // draws tile to board canvas
    drawTile : function (x, y, tile_id) {
        var index,
            imgd;
        if(!this.tlookup.hasOwnProperty(tile_id)) {
            index = this.insertTile(tile_id);
        } else {
            index = this.tlookup[tile_id];
        }
        imgd = this.img_cache[index];
        if (imgd !== undefined) {
            board.canvas.ctx.putImageData(this.img_cache[index], x*board.canvas.pixel_width, y*board.canvas.pixel_height);
        } else {
            console.log('Attempted to drawTile, but there was no data in the image cache');
        }
    },

    //draw tiles to an arbritary canvas
    drawTileToCtx : function(ctx, tile_id, x_offset, y_offset) {
        var index,
            imgd;

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
        
        imgd = this.img_cache[index];
        if (imgd !== undefined) {
            ctx.putImageData(this.img_cache[index], x_offset, y_offset);
        } else {
            console.log('Attempted to drawTileToCtx, but there was no data in the image cache');
        }     
    },

    //insert tile into tile_drawing_cache
    insertSprite : function (tile_num, tileset_id) {
        var tileset = this.tilesets[tileset_id],
            tile_x_pos = tile_num % tileset.tile_width,
            tile_y_pos = tile_num - tile_x_pos,
            x_offset,
            y_offset,
            imgd,
            pix,
            cache_index = ++this.img_cache_count;
        
        if (parseInt(tile_y_pos, 10) !== 0) {
            tile_y_pos = tile_y_pos / tileset.tile_width;
        }
        x_offset = tile_x_pos * tileset.pixel_width;
        y_offset = tile_y_pos * tileset.pixel_height;

        this.ctx.clearRect(0, 0, board.canvas.pixel_width, board.canvas.pixel_height); //needed?
        this.ctx.drawImage(tileset.image, x_offset, y_offset, 
                    tileset.pixel_width, tileset.pixel_height,
                    0, 0, board.canvas.pixel_width, board.canvas.pixel_height);

        imgd = this.ctx.getImageData(0, 0, board.canvas.pixel_width, board.canvas.pixel_height);
        pix = imgd.data;

        this.img_cache[cache_index] = imgd; 
        this.slookup[tileset_id][tile_num] = cache_index;
        return cache_index;
    },

    drawSprite : function (x, y, sprite_num, spriteMap_id) {
        var index,
            imgd;
        if (!this.slookup.hasOwnProperty(spriteMap_id)) { 
            console.log("DrawingCache.drawSprite Error: tileset/spriteMap not loaded: " + spriteMap_id);
            return false;
        }
        if (!this.slookup[spriteMap_id].hasOwnProperty(sprite_num)) {
            index = this.insertSprite(sprite_num, spriteMap_id);
        } else {
            index = this.slookup[spriteMap_id][sprite_num];
        }

        imgd = this.img_cache[index];
        if (imgd !== undefined) {
            board.canvas.ctx.putImageData(this.img_cache[index], x*board.canvas.pixel_width, y*board.canvas.pixel_height);
        } else {
            console.log('Attempted to drawSprite, but there was no data in the image cache');
        }  
    },
    
    drawSpriteToCtx : function (ctx, sprite_num, spriteMap_id, x_offset, y_offset) {
        var index,
            imgd;
        if (x_offset === undefined) {
            x_offset = 0;
        }
        if (y_offset === undefined) {
            y_offset = 0;
        }
        
        if (!this.slookup.hasOwnProperty(spriteMap_id)) {
            console.log("DrawingCache.drawSprite Error: tileset/spriteMap not loaded: " + spriteMap_id)
            return false;
        }
        if (!this.slookup[spriteMap_id].hasOwnProperty(sprite_num)) {
            index = this.insertSprite(sprite_num, spriteMap_id);
        } else {
            index = this.slookup[spriteMap_id][sprite_num];
        }

        imgd = this.img_cache[index];
        if (imgd !== undefined) {
            ctx.putImageData(this.img_cache[index], x_offset, y_offset);
        } else {
            console.log('Attempted to drawSpriteToCtx, but there was no data in the image cache');
        } 
    }
};
