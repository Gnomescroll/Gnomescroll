// get all the terrain tiles
// make an img div for them
// use css spriting ^
// onclick methods for each
//   border = red
//   current_map_tile = img tile value


// canvas click methods
// when click, take coordinate -> board coordinate -> global map coordinate
// send admin.set_map(x,y,z,tile_value) msg

var tile_pane,
    tileset_coordinates,
    select_tile,
    canvas_click;
    
    
tile_pane = {
    
    init: function () {
            // iterate over map tiles
            // create span for each
            // with id=tile_value
            // bind click method
            var len,
                i,
                tile,
                pane = $('#map_tile_editor');
            
            len = tileset_coordinates.length;
            for(i=0; i < len; i++) {
                
                tile = $('<span></span>').attr({'id':tile_value, 'class':'pane_tile'});
                
                
                pane.append(tile);
            }
            
        },
        
    
    
    
}
