// need object mapping tile_values with (maybe) tile_type names
// and also mapping the tile_values to positions in the tileset

var state = ( function () {
    
    var blocks,
        levels = [],
        agents = {},
        objects = {},
        ao_map = {}; // coordinate map; maps coordinate.toString() to a another object, { agent: [], object: [], container: [] }
        
    // need some translation function for global/map/server coords to block's coords
        
    var z_lvls = [5],
        current_z_lvl = 5;
        
    var updateBlock,
        updateAgent,
        updateObject,
        updateLevel;
    
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
        if (!validate.blockData(block)) {
            return false;            
        } 
        
        var x_;
    
        x_ = positionInState(block.x, block.y, block.z);
        if (x_ === false) return false;
        
        x_[data.y] = data.type
        
        return true;
    };
    
    agentKnown = function(agent_id) {
        var a = agents[agent_id];
        if (a === undefined) return false;
        else return a;
    };
    
    addAgent = function(agent) {
        agents[agent.id] = agent;
        
        pos_string = agent.pos.toString();
        ao_loc = ao_map[pos_string]
        if (ao_loc === undefined) {
            ao_map[pos_string] = { agent.pos: { 'agents': [agent.id] } }
        } else {
            var ag = ao_loc['agents'];
            if (ag === undefined) {
                ao_loc['agents'] = [agent.id];
            } else {
                ag.push(agent.id);
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
                updateBlock: updateBlock,
                updateAgent: updateAgent,
                updateObject: updateObject,
                updateLevel: updateLevel,
              }
              
    return public_;
    
}());

var validate = ( function () {
    
    var levelData,
        blockData,
        agentData,
        objectData,
        public_;
        
    levelData = function (data) {
        
        if (typeof data !== 'object') {
            console.log('Tried passing bullshit to updateLevel. Please send a data object.');
            console.log(data);
            return false;
        }
        
        if (data.client_id !== globals.client_id) { // ignore immediately, actually move this to main message handler
            console.log('Message client_ids don\'t match. Ignoring.');
            return false;
        }
        
        // map data for a z-level
        var required = ['map', // (array of int tile values)
                        'z_level', // (int)
                        'x_size', // (int)
                        'y_size', // (int)
                        'world_id', // (int)
                       ];
               
        var missing = [];
        $.each(required, function(i, req) {
            if (data[req] === undefined) {
                missing.push(req);
            }
        });
        
        if (missing.length > 0) {
            var msg = missing.join();
            console.log('Cannot update z-level. The following data attrs are missing: '+msg.slice(0, msg.length-2));
            console.log(data);
            return false;
        }
        
        data.x_size = parseInt(data.x_size);
        data.y_size = parseInt(data.y_size);
        var map_size = data.x_size*data.y_size;
        if (map_size !== data.map.length) {
            console.log('The map dimensions do not match the map sent.');
            console.log(data);
            return false;
        }

        return true;
    };
        
    var check = function(data, req, msg) {
        
        var missing = false,
            int_val;
        
        $.each(req, function(i, val) { // check for missing and NaN
            if (data[val] === undefined) {
                console.log(msg +' is missing: '+val);
                missing = true;
                return true; //continue
            }
            int_val = parseInt(data[val]);
            if (isNaN(int_val)) {
                console.log(msg +' is NaN: '+val);
                missing = true;
                return true;
            } else {
                data[val] = int_val;
            }
        });
        
        if (missing === true) { // log data if bad, bail
            console.log(data);
        }
        
        return !missing;
    }
        
    blockData = function (data) {
        return check(data, ['type','x','y','z'], 'blockData');
    };
    
    agentData = function (data) {
        // 'loc' is the location_type (e.g. ground, agent, container)
        return check(data, ['loc','x','y','z','id'], 'agentData');        
    };
    
    objectData = function (data) {
        return check(data, ['loc','x','y','z','id'], 'objectData');
    };
    
    public_ = {
                levelData: levelData,
                blockData: blockData,
                agentData: agentData,
                objectData: objectData
              }
              
    return public_;
    
}());

// agent and object methods bundled here
// should have a convenience method for creating an object
// and should have methods for applying mods to the object

// might need to do a bit different inheritance , like .beget()
// so we can keep methods and static properties in a single object
// that can be looked up from the instance object 
// (python object hierarchy; saves a lot of memory)

var agent = function () {
    
    var create, update;
    
    create = function (data) {
        // takes some data, adds any static class properties defined above
        return {};
    }
    
    // meant to be .apply'd -- e.g. agent.update.apply(an_agent, data);
    update = function (data) {
        // some kind of type checking here?
        var property;
        for (property in data) {
            this[property] = data[property]; //indiscriminately overwrite existing properties, and add new ones
        }
    }
}


var game_object = function () {
    
    var create, update;
    
    create = function (data) {
        // takes some data, adds any static class properties defined above
        return {};
    }
    
    // meant to be .apply'd -- e.g. agent.update.apply(an_agent, data);
    update = function (data) {
        // some kind of type checking here?
        var property;
        for (property in data) {
            this[property] = data[property]; //indiscriminately overwrite existing properties, and add new ones
        }
    }
    
}
