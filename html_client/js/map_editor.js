// map editor panels & controls
var map_editor = {
    
    panel_canvas_tile: function (id) {
        var bc = board.canvas,
            canvas = $('<canvas></canvas>').attr({ 'id' : id,
                                                   'class' : 'panel_tile',
                                                   'width' : bc.pixel_width,
                                                   'height': bc.pixel_height });
        return canvas;
    },

    init: function () {
        this._init_panel();
        this._init_panel_controls();
    },
    
    _init_panel: function () {
        var pane = $('#map_editor'),
            tile_values = tileset_state.tile_id_to_name,
            table = $('<table></table>').attr('class','panel tiles'),
            cells_wide = 1,
            j = 0,
            i,
            name,
            canvas,
            tr,
            td;
            
        for (i in tile_values) {
            if (!tile_values.hasOwnProperty(i)) continue;
            name = tile_values[i];
            i = parseInt(i, 10)
            if (isNaN(i) || i < 0 || name === undefined) continue;
            if (j%cells_wide === 0) {
                tr = $('<tr></tr>');
                table.append(tr);
            }
            td = $('<td></td>').attr('class','canvas');
            canvas = this.panel_canvas_tile(i);
            tr.append(td.append(canvas));
            
            td = $('<td></td>').attr('id',i).html(name);
            if (i == this.current_tile) td.attr('class','selected');
            tr.append(td);
            j++;
        }
        pane.append(table);
        
        pane.append($('<hr>'));

        // agent
        table = $('<table></table>').attr('class','panel agents');
        tr = $('<tr></tr>');
        td = $('<td></td>').attr('class','canvas');
        canvas = this.panel_canvas_tile('agent');
        tr.append(td.append(canvas));
        td = $('<td></td>').attr('id','agent').html('agent');
        tr.append(td);
        table.append(tr);
        
        pane.append(table);
        
        table = $('table.panel.tiles');
        table.find('canvas.panel_tile')
             .each(function (i) {
                    var tile = $(this),
                        ctx = this.getContext('2d'),
                        tile_id = tile.attr('id');
                   // console.log('draw panel tile');
                   // console.log(tile_id);
                    drawingCache.drawTileToCtx(ctx, tile_id);
              });
        
        table = $('table.panel.agents');
        table.find('canvas.panel_tile')
             .each(function (i) {
                    var ctx = this.getContext('2d');
                  //  console.log('draw agent tile');
                    drawingCache.drawSpriteToCtx(ctx, 1, 1);
             });
        
    },
    
    _init_panel_controls: function () {
        var cells = $('td');
            
        cells.click(function(event) {
            var cell = $(this),
                cls,
                selected = false;
            if (cell.attr('class') === 'canvas') cell = cell.next();
            map_editor.current_tile = cell.attr('id');
            selected = (cell.attr('class') === 'selected');
            $('td.selected').attr('class','');
            if (!selected) {
                cell.attr('class','selected');
            } else {
                map_editor.current_tile = null;
            }
        });
    },
    
    set_tile: function (event) {
       // find if click is inside canvas
       // if so, which coordinate tile
       // call admin.set_map for the current_tile_value and at coord
        
        if (this.current_tile === null) return;
        
        var canvas = $('canvas#board'),
            canvas_offset = canvas.offset(),
            coord = {},
            board_coord = {},
            global_coord = {},
            bc = board.canvas;
            
        // change coordinate system relative to canvas
        coord.x = event.pageX - canvas_offset.left;
        coord.y = event.pageY - canvas_offset.top;

        // check if in canvas
        if (coord.x < 0 || coord.x > bc.width*bc.pixel_width) return false;
        if (coord.y < 0 || coord.y > bc.height*bc.pixel_height) return false;
        
        // convert to board coordinate
        board_coord.x = Math.floor(coord.x/bc.pixel_width);
        board_coord.y = Math.floor(coord.y/bc.pixel_height);
        
        // convert to global game coordinate
        global_coord.x = board_coord.x + board.x_offset;
        global_coord.y = board_coord.y + board.y_offset;

        if (this.current_tile === 'agent') {
            admin.create_agent(global_coord.x, global_coord.y, board.z_level);
        } else {
            admin.set_map(global_coord.x, global_coord.y, board.z_level, this.current_tile);
        }
    },
    
    current_tile: null, // currently selected map tile for editor
    
    clear_current: function () {
        $('td.selected').attr('class', '');
        this.current_tile = null;
    }
};
