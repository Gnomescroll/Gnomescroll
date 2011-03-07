// need object mapping tile_values with (maybe) tile_type names
// and also mapping the tile_values to positions in the tileset

var state = ( function () {
    
    var blocks,
        levels = {},
        agents = {},
        objects = {},
        containers = {},
        ao_map = {}; // coordinate map; maps coordinate.toString() to a another object, { agent: [], object: [], container: [] }
        
    // need some translation function for global/map/server coords to block's coords
        
    var z_lvls = [],
        current_z_lvl = 5;
        
    var updateBlock,
        updateLevel;
    
    var gameObjectKnown,
        gameObjectTypeMap = { 'agent': agents,
                              'obj': objects,
                              'container': containers
                            };
    
    var addGameObject,
        removeGameObject;
    
    var setLocation,
        cleanLocation;
    
    var init,
        public_;
    
    
    // generate blank block matrix
    // stores tile values
    
    var map_width = 100,
        map_height = 100;
    
    // generate zeroed matrix
    blocks = function (width, height) {
        var arr = [],
            row = [],
            i = 0,
            j = 0,
            width = width || map_width,
            height = height || map_height;
            
         for (i=0; i < width; i++) {
             for (j=0; j < height; j++) {
                row.push(0);
            }
            arr.push(row);
            row = [];
         }
         
         return arr;   
    };
    
    // requests state data from server
    init = function () {
        
        var z_levels_to_add = [ current_z_lvl-1, 
                                current_z_lvl, 
                                current_z_lvl+1 ];
        
        $.each(z_levels_to_add, function (i, z) {
            if ($.inArray(z, z_lvls) === -1) {
                z_lvls.push(z);
            }
        });
        
        $.each(z_lvls, function (i, z) {
            levels[z] = blocks();       // zero-d array for each z-level
            info.map(z);                // request z-levels
        });
        
        
        info.agents();                  // request agents
        info.objects();                 //         objects
    };
    
    // checks if a position is in current state bounds.
    // returns false or a row of y values from [z][x]
    contains = function(pos) {
    
        var x, y, z,
            x_, z_;
      
        x = pos[0]
        y = pos[1];
        z = pos[2];
      
        z_ = levels[z];
        if (z_ === undefined) return false;
        x_ = z_[x];
        if (x_ === undefined) return false;
        if (x_[y] === undefined) return false;
        
        return x_;  
    };
    
    // update a z-level with map info
    updateLevel = function (data) {
        
        // data object requires:
        // z_level, map, y_size, x_size
        
        var arr = [],
            col = [],
            i = 0,
            j = 0;
            
        for (j=0; j < data.x_size; j += 1) {
            for (i=j; i < data.map.length; i += data.y_size) {
                col.push(data.map[i]);
            }
            arr.push(col);
            col = [];
        }
        
        levels[data.z_level] = arr;
        
        if ($.inArray(data.z_level, z_lvls) === -1) {
            z_lvls.push(data.z_level);
        }
        
    };
    
    // update a block type
    updateBlock = function(block) {

        var x_;
    
        x_ = contains(block.x, block.y, block.z);
        if (x_ === false) return false;
        
        x_[data.y] = data.value
        
        return true;
    };
    
    // checks if a game_object exists in state
    // returns false or the object
    gameObjectKnown = function(game_object, type) {
        
        var type,
            id,
            obj;
        
        if (typeof game_object === 'object') {
            type = game_object.obj_type;
            id = game_object.id;
        } else {        
            id = game_object;
        }
        
        obj = gameObjectTypeMap[type][id];
        
        if (obj === undefined) return false;
        else return obj;
    };
    
    // add game_object to lists
    addGameObject = function(game_object) {
        
        var type,
            id;
            
        type = game_object.obj_type;
        id = game_object.id;
            
        gameObjectTypeMap[type][id] = game_object;
        
    };
    
    // remove game_object from lists
    removeGameObject = function(game_object) {
        
        var type,
            id;
            
        type = game_object.obj_type;
        id = game_object.id;
        
        delete gameObjectTypeMap[type][id];
        
    };
    
    // removes a game_object from the ao_loc map
    cleanLocation = function(game_object) {
        
        var type,
            pos,
            id,
            ao_loc,
            ao_loc_type,
            index;
            
        type = game_object.obj_type;
        id = game_object.id;
        
        pos = game_object.pos();
        pos = pos.toString();
        
        ao_loc = ao_map[pos];
        
        if (ao_loc === undefined) { // if pos is not in ao_map
            return; // leave
        }
        
        ao_loc_type = ao_loc[type];
        index = $.inArray(id, objs);
        if (index > -1) { // if game_object is in its type's array
            ao_loc_type.slice(index,1); // remove it
        }
        
        if (ao_loc_type.length === 0) { // if array is now empty
            delete ao_loc[type]; // remove the key to type
        }
        
        if ($.isEmptyObject(ao_loc)) { // if ao_loc is now just {}
            delete ao_map[pos]; // remove it from ao_map
        }
        
    };
    
    // sets a game_object (agent, obj, cont) in the ao_loc map
    setLocation = function (game_object) {
        
        var type,
            pos,
            ao_loc,
            ao_loc_type;
        
        type = game_object.obj_type; // agent, object, container
        id = game_object.id;
        
        pos = game_object.pos();
        pos = pos.toString();
        
        ao_loc = ao_map[pos];
        
        if (ao_loc === undefined) { // if pos not in ao_map
            ao_map[pos] = { type: [game_object.id] } // add new obj
        } else {
            ao_loc_type = ao_loc[type]; // get existing obj
            if (ao_loc_type === undefined) { // if there is no key for type
                ao_loc[type] = [game_object.obj_type]; // add it w/ new id array
            } else {
                if ($.inArray(id, ao_loc_type) === -1) { // check if it is here already
                    ao_loc_type.push(id); // if not, add it
                }
            }
        }
    };
    
    public_ = {
                init: init,
                contains: contains,
                setLocation: setLocation,
                cleanLocation: cleanLocation,
                gameObjectKnown: gameObjectKnown,
                addGameObject: addGameObject,
                removeGameObject: removeGameObject,
                updateBlock: updateBlock,
                updateLevel: updateLevel,
                ao_map: ao_map,
                levels: levels,
                current_z_lvl: current_z_lvl,
                map_width: map_width,
                map_height: map_height,
              };
              
    return public_;
    
}());
