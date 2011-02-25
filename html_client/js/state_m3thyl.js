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
    
    // public update methods to be routed to by message handler
    // data is an object with update data from server
    
    addZLevel = function(data) {
        
        // validate data
        if (data.z === undefined) {
            console.log('addZLevel data is missing .z');
            console.log(data);
        }
        if (data.level_info === undefined) {
            console.log('addZLevel data is missing .level_info');
            console.log(data);
        }
        
        // add z-lvl to z_lvls list
        if (data.level_info !== undefined) {
            z_lvls[data.z] = data.level_info;
        } else { // zeroed
            z_lvls[data.z] = blocks();
        }
    }
    
    updateLevel = function (data) {
        
        if (typeof data !== 'object') {
            console.log('Tried passing bullshit to updateLevel. Please send a data object.');
            console.log(data);
            return;
        }
        
        if (data.client_id !== globals.client_id) { // ignore immediately, actually move this to main message handler
            return;
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
            return;
        }
        
        data.x_size = parseInt(data.x_size);
        data.y_size = parseInt(data.y_size);
        var map_size = data.x_size*data.y_size;
        if (map_size !== data.map.length) {
            console.log('The map dimensions do not match the map sent.');
            console.log(data);
            return;
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
    
    updateBlock = function(data) {
        // a coordinate and block type
    };
    
    updateAgent = function (data) {
        // an agent id and updates
    };
    
    updateObject = function (data) {
        // an object id and updates
    };
    
    public_ = {
                init: init,
                updateBlock: updateBlock,
                updateAgent: updateAgent,
                updateObject: updateObject,
                updateLevel: updateLevel,
              }
              
    return public_
    
    
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
