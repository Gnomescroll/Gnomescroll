// need object mapping tile_values with (maybe) tile_type names
// and also mapping the tile_values to positions in the tileset

var state = ( function () {
    
    var blocks,
        levels = [],
        agents = {},
        objects = {},
        containers = {},
        ao_map = {}; // coordinate map; maps coordinate.toString() to a another object, { agent: [], object: [], container: [] }
        
    // need some translation function for global/map/server coords to block's coords
        
    var z_lvls = [5],
        current_z_lvl = 5;
        
    var updateBlock,
        updateAgent,
        updateObject,
        updateLevel;
    
    var gameObjectKnown,
        gameObjectTypeMap = { 'agent': agents,
                              'object': objects,
                              'container': containers
                            }
    
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
    
        
    init = function () {
        // zero-d array for each z-level
        $.each(z_lvls, function (i, val) {
            levels[val] = blocks();
        });
    };
    
    positionInState = function(x, y, z) {
        // checks if the position is in current state.
        // if it is, returns a row of y values from level
        // else false
      
        if (typeof x === 'object') {
            if (g.length === undefined) {
                y = x.y;
                z = x.z;
                x = x.x;
            } else {
                y = x[1];
                z = x[2];
                x = x[0];
            }
        }
      
        z_ = levels[z];
        if (z_ === undefined) return false;
        x_ = z_[x];
        if (x_ === undefined) return false;
        if (x_[y] === undefined) return false;
        
        return x_;  
    };
    
    // public update methods to be routed to by message handler
    // data is an object with update data from server
    
    // update a z-level with map info
    updateLevel = function (data) {
        
        if (!validate.levelData(data)) {
            return false;
        }
        
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
        
        data.z_level = parseInt(data.z_level);
        levels[data.z_level] = arr;
        
        if ($.inArray(data.z_level, z_lvls) === -1) {
            z_lvls.push(data.z_level);
        }
    };
    
    // update a block type
    updateBlock = function(block) {
        // a coordinate and block type

        var x_;
    
        x_ = positionInState(block.x, block.y, block.z);
        if (x_ === false) return false;
        
        x_[data.y] = data.type
        
        return true;
    };
    
    // checks if a game_object exists in state
    gameObjectKnown = function(game_object) {
        
        var type,
            id,
            obj;
        
        type = game_object.type;
        id = game_object.id;
        
        obj = gameObjectTypeMap[type][id];
        
        if (obj === undefined) return false;
        else return a;
    };
    
    addGameObject = function(game_object) {
        
        var type,
            id;
            
        type = game_object.type;
        id = game_object.id;
            
        gameObjectTypeMap[type][id] = game_object;
        
    }
    
    // removes a game_object from the ao_loc map
    cleanLocation = function(game_object) {
        
        var type,
            pos,
            id,
            ao_loc,
            ao_loc_type,
            index;
            
        type = game_object.type;
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
        
        type = game_object.type; // agent, object, container
        id = game_object.id;
        
        pos = game_object.pos();
        pos = pos.toString();
        
        ao_loc = ao_map[pos];
        
        if (ao_loc === undefined) { // if pos not in ao_map
            ao_map[pos] = { type: [game_object.id] } // add new obj
        } else {
            ao_loc_type = ao_loc[type]; // get existing obj
            if (ao_loc_type === undefined) { // if there is no key for type
                ao_loc[type] = [game_object.type]; // add it w/ new id array
            } else {
                if ($.inArray(id, ao_loc_type) === -1) { // check if it is here already
                    ao_loc_type.push(id); // if not, add it
                }
            }
        }
    };
        
    // update agent status
    updateAgent = function (agent) {
        // an agent id and updates
        if (!validate.agentData(data)) {
            return false;
        }
        
        if (positionInState(agent.pos) !== false) {
            
            
        } else {
            
            
        }
        
        
        
        // if agent in state
        //   if agent was in state
        //      move it
        //   else
        //      add it
        //
        // else if agent not in state
        //   if it was in state
        //      remove it
        //   else
        //      ignore
        
        return true;
    };
    
    // update object status
    updateObject = function (data) {
        // an object id and updates
        
        if (!validate.objectData(data)) {
            return false;
        }
        
        return true;
    };
    
    public_ = {
                init: init,
                setLocation: setLocation,
                cleanLocation: cleanLocation,
                gameObjectKnown: gameObjectKnown,
                addGameObject: addGameObject,
                updateBlock: updateBlock,
                updateLevel: updateLevel,
              }
              
    return public_;
    
}());
