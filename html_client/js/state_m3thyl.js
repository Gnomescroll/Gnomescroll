// need object mapping tile_values with (maybe) tile_type names
// and also mapping the tile_values to positions in the tileset

var state = ( function () {
    
    var blocks,
        agents = {},
        objects = {},
        ao_map = {}; // coordinate map; maps coordinate.toString() to a another object, { agent: [], object: [], container: [] }
        
    // need some translation function for global/map/server coords to block's coords
        
    var z_lvl = 5;
        
    var updateBlock,
        updateAgent,
        updateObject;
    
    var init,
        public_;
    
    
    // generate blank block matrix
    // stores tile values
    
    var map_width = 100,
        map_height = 100;
    
    blocks = ( function () {
        var arr = [],
            row = [],
            i = 0,
            j = 0;
         
         for (i=0; i < map_height; i++) {
             for (j=0; j < map_width; j++) {
                row.push(0);
            }
            arr.push(row);
            row = [];
         }
         
         return arr;   
    }());
    
    
    // public update methods to be routed to by message handler
    // data is an object with update data from server
    
    updateBlock = function (data) {
        
    }
    
    updateAgent = function (data) {
        
    }
    
    updateObject = function (data) {
        
    }
    
    init = function () {
        
    }
    
    
    public_ = {
                init: init,
                updateBlock: updateBlock,
                updateAgent: updateAgent,
                updateObject: updateObject
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
