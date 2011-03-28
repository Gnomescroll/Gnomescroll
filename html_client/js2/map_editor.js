// map of tile_values -> coordinates of tile in cached canvas
var terrain_tiles = {
    
    // e.g.
    // '5': [48, 72],
    // tile_value: [x, y],
}

var map_editor = {
    
    init_panel: function() {
        var pane = $('#map_editor'),
            tile,
            tile_value,
            image_data,
            bc = board_canvas,
            cached_tile_coords;
        // loads tiles from tileset into a bunch of divs or span
        // which requires css spriting (adjusting background-image position
        // but that requires a copy of the tileset per tile
        // so may be best to just use the cached tiles and stick them in <canvas class="map_tile" id="4">
        
        for (tile_value in terrain_tiles) {
            tile = $('<canvas></canvas>').attr({'class':'map_tile', 'id':tile_value})
                                         .width(bc.tile_pixel_width)
                                         .height(bc.tile_pixel_height)
                                         .click(function() {
                                            var t = $(this);
                                            t.css('border','1px solid red');
                                            map_editor.current_tile = t.attr('id');
                                        });
            pane.append(tile);
            tile = pane.children('#'+tile_value)[0];
            
            cached_tile_coords = terrain_tiles[tile_value];
            image_data = drawingCache.getImageData(cached_tile_coords[0], cached_tile_coords[1],
                                       bc.tile_pixel_width, bc.tile_pixel_height);
            tile.putImageData(0, 0, bc.tile_pixel_width, bc.tile_pixel_height);
        }
    },
    
    init_lazy_panel: function () {
        var pane = $('#map_editor'),
            tile_values = tileset_state.tile_id_to_name,
            j = 0,
            name,
            table = $('<table></table>'),
            cells_wide = 1,
            tr, td;
            
        for (i in tile_values) {
            name = tile_values[i];
            if (name === undefined) continue;
            if (j%cells_wide === 0) {
                tr = $('<tr></tr>');
                table.append(tr);
            }
            td = $('<td></td>').attr('id',i).html(name);
            if (i == this.current_tile) td.attr('class','selected');
            tr.append(td);
            j++;
        }
        pane.css('float','left');
        pane.append(table);
    },
    
    init_lazy_panel_controls: function () {
        var cells = $('td');
            
        cells.click(function(event) {
            var cell = $(this),
                cls;
            map_editor.current_tile = parseInt(cell.attr('id'));
            $('td.selected').attr('class','');
            cell.attr('class','selected');
        });
        
        
    },
    
    set_tile: function(event) {
       console.log('window click');
       // find if click is inside canvas
       // if so, which coordinate tile
       // call admin.set_map for the current_tile_value and at coord
        
        var canvas = $('canvas#board'),
            canvas_offset = canvas.offset(),
            coord = {},
            board_coord = {},
            global_coord = {},
            bc = board_canvas;
            
        // change coordinate system relative to canvas
        //coord.x = event.offsetX - canvas_offset.left;
        //coord.y = event.offsetY - canvas_offset.top;
        coord.x = event.pageX - canvas_offset.left;
        coord.y = event.pageY - canvas_offset.top;
        console.log(event);
        console.log({pageX: event.pageX, pageY: event.pageY});
        console.log(canvas_offset);
        console.log(coord);
        // check if in canvas
        if (coord.x < 0 || coord.x > bc.canvas_tile_width*bc.tile_pixel_width) return false;
        if (coord.y < 0 || coord.y > bc.canvas_tile_height*bc.tile_pixel_height) return false;
        
        // convert to board coordinate
        board_coord.x = Math.floor(coord.x/bc.tile_pixel_width);
        board_coord.y = Math.floor(coord.y/bc.tile_pixel_height);
        
        // convert to global game coordinate
        global_coord.x = board_coord.x + board.x_offset;
        global_coord.y = board_coord.y + board.y_offset;
        
        console.log('admin set map');
        console.log(global_coord);
        admin.set_map(global_coord.x, global_coord.y, board.z_level, map_editor.current_tile, true);
    },
    
    current_tile: 0, // currently selected map tile for editor
}

