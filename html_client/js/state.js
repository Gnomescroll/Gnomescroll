// need object mapping tile_values with (maybe) tile_type names
// and also mapping the tile_values to positions in the tileset

var state = {
    
    levels:     {}, // tile map state, 2x2 arrays indexed by z-level (levels[z][x][y] to access a specific tile by coordinate)
    agents:     {},
    objects:    {},
    containers: {},
    ao_map:     {}, // coordinate map; maps coordinate.toString() to a another object, { agent: [], object: [], container: [] }        
    z_lvls:     [], // z-levels that are loaded
    current_z_lvl: 1,

    gameObjectTypeMap: { // map from game object types to state's cache for the type (loaded on init)
        'agent'    : 'agents',
        'obj'      : 'objects',
        'container': 'containers'
    },
    
    map_width: 100,
    map_height: 100,
    
    started : false,

    fully_loaded : false,
    requests_waiting : {},

    reset : function () {
        this.gameObjectTypeMap = {};
        this.levels = {};
        this.agents = {};
        this.objects = {};
        this.containers = {};
        this.ao_map = {};
        this.z_lvls = [];
        this.started = false;
        this.fully_loaded = false;
        this.requests_waiting = {};
    },
    
    // requests state data from server
    init: function () {
        this._initGameObjectTypeMap();
        
        var z_levels_to_add = [ this.current_z_lvl-1, 
                                this.current_z_lvl, 
                                this.current_z_lvl+1 ],
            len = z_levels_to_add.length,
            i,
            z;

        if (! this.started) {
            // add z-level numbers to z_lvls[]
            for(i=0; i < len; i++) {
                z = z_levels_to_add[i];
                if ($.inArray(z, this.z_lvls) === -1) {
                    this.z_lvls.push(z);
                }
            }

            // init levels{} with zeroed array, request map for each lvl
            len = this.z_lvls.length;
            for (i=0; i < len; i++) {
                z = this.z_lvls[i];
                this.levels[z] = this.blocks();  // zero-d array for each z-level
                this.load_map(z);                // request z-levels
                this.requests_waiting[z] = true;
            }

            return this.load_game_objects(); // agent, objects && tileset
        } else {
            return this.load_game_state();
        }
    },

    _initGameObjectTypeMap : function () {
        this.gameObjectTypeMap = { // map from game object types to state's cache for the type (loaded on init)
            'agent'    : this.agents,
            'obj'      : this.objects,
            'container': this.containers
        };
    },

    load_game_state : function (z) {
        this.fully_loaded = false;
        this._initGameObjectTypeMap();
        this.load_map(z);
        this.requests_waiting[z] = true;
        return this.load_game_objects();
    },

    load_map : function (z) {   // call load map state
        // z can be a z-level number or array of z-level numbers
        // or left out, in which case it will load all z's in this.z_lvls
        if (z === undefined) {
            z = this.z_lvls;
        }
        var i = 0,
            len;
        if ($.isArray(z)) {
            len = z.length;
            for (i=0; i < len; i++) {
               info.map(z[i]); 
            }
        } else {
            info.map(z);
        }
    },

    load_game_objects : function () {
        this.fully_loaded = false;
        this.requests_waiting.agents = true;
        this.requests_waiting.objects = true;
        return (info.agents() &&    // request agents
                info.objects());    //         objects
    },

    check_loaded : function () { // check if there are outstanding requests
        var n;
        this.fully_loaded = true;
        for (n in this.requests_waiting) {
            if (!this.requests_waiting.hasOwnProperty(n)) continue;
            if (n === 'agents' || n === 'objects' || !isNaN(parseInt(n, 10))) {
                this.fully_loaded = false;
            }
        }
        if (this.fully_loaded) {
            dispatcher.trigger('state_loaded');
            this.fully_loaded = false;
        }
    },

    // generate zeroed matrix
    blocks: function (width, height) {
        var arr = [],
            row = [],
            i   = 0,
            j   = 0,
            width  = width || this.map_width,
            height = height || this.map_height;
            
         for (i=0; i < width; i++) {
             for (j=0; j < height; j++) {
                row.push(0);
            }
            arr.push(row);
            row = [];
         }
         return arr;   
    },
    
    // checks if a position is in current state bounds.
    // returns false or a row of y values from [z][x]
    contains: function(pos) {
        var x, y, z,
            x_, z_;

        if (typeof pos === 'object') {
            x = pos.x;
            y = pos.y;
            z = pos.z;
        } else {
            x = pos[0]
            y = pos[1];
            z = pos[2];
        }
      
        z_ = this.levels[z];
        if (z_ === undefined) return false;
        x_ = z_[x];
        if (x_ === undefined) return false;
        if (x_[y] === undefined) return false;
        return x_;  
    },
    
    // update a z-level with map info
    updateLevel: function (data) {
        // data object requires:
        // z_level, map, y_size, x_size
        var arr = [],
            col,
            i,
            j,
            data_map_length;
            
        for (j=0; j < data.x_size; j++) {
            col = [];
            data_map_length = data.map.length;
            for (i=j; i < data_map_length; i += data.y_size) {
                col.push(data.map[i]);
            }
            arr.push(col);
        }
        
        this.levels[data.z_level] = arr;    // assign new map block
        
        if ($.inArray(data.z_level, this.z_lvls) === -1) {  // add z_lvl number to this.z_lvls
            this.z_lvls.push(data.z_level);
        }
        delete this.requests_waiting[data.z_level];
        this.check_loaded();
        return true;
    },
    
    // update a block type
    updateBlock: function(block) {
        var x_ = this.contains(block);
        if (x_ === false) return false;
        x_[block.y] = block.value
        return block;
    },
    
    // checks if a game_object exists in state
    // returns false or the object
    gameObjectKnown: function(game_object, type) {
        var id,
            obj;
        
        if (typeof game_object === 'object') {
            if (type === undefined) {
                type = game_object.base_type;
            }
            id = game_object.id;
        } else {        
            id = game_object;
        }
        
        obj = this.gameObjectTypeMap[type][id];
        
        if (typeof obj !== 'object') {
            this._initGameObjectTypeMap();
            obj = this.gameObjectTypeMap[type][id];
            if (obj === undefined) return false;
        }
        else return obj;
    },
    
    // add game_object to lists
    addGameObject: function(game_object) {
        var type,
            id;
            
        type = game_object.base_type;
        id = game_object.id;
        this.gameObjectTypeMap[type][id] = game_object;
        return true;
    },
    
    // remove game_object from lists
    removeGameObject: function(game_object) {
        var type,
            id;
            
        type = game_object.obj_type;
        id = game_object.id;
        
        delete this.gameObjectTypeMap[type][id];
        return true;
    },
    
    // removes a game_object from the ao_loc map
    cleanLocation: function(game_object) {
        var type,
            pos,
            id,
            ao_loc,
            ao_loc_type,
            index,
            objs;
            
        type = game_object.base_type;
        id = game_object.id;
        objs = this.gameObjectTypeMap[game_object.base_type];
        
        if (!game_object.hasOwnProperty('pos')) {
            pos = GameObject.pos.apply(game_object);
        } else {
            pos = game_object.pos();
        }
        pos = pos.toString();
        
        ao_loc = this.ao_map[pos];
        
        if (ao_loc === undefined) { // if pos is not in ao_map
            return false; // leave
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
            delete this.ao_map[pos]; // remove it from ao_map
        }
        return true;
    },
    
    // sets a game_object (agent, obj, cont) in the ao_loc map
    setLocation: function (game_object) {
        var type,
            pos,
            ao_loc,
            ao_loc_type;
        
        type = game_object.base_type;
        id = game_object.id;
        
        pos = game_object.pos();
        pos = pos.toString();
        
        ao_loc = this.ao_map[pos];
        
        if (ao_loc === undefined) { // if pos not in ao_map
            ao_loc = {};
            ao_loc[type] = [game_object.id];
            this.ao_map[pos] = ao_loc // add new obj
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
        return true;
    },
    
    locationEmpty: function (loc) {
        var x, y, z,
            ao_loc,
            ao_loc_item,
            item,
            empty = true;
        
        if (loc.constructor.name === 'Array' && loc.length >= 3) {
            x = loc[0];
            y = loc[1];
            z = loc[2];
        } else if (typeof loc === 'object') {
            x = loc.x;
            y = loc.y;
            z = loc.z;
        } else {
            return true;
        }
        
        ao_loc = this.ao_map[[x,y,z].toString()]
        if (ao_loc !== undefined) {
            for(item in ao_loc) {
                if (ao_loc.hasOwnProperty(item)) {
                    ao_loc_item = ao_loc[item];
                    if (this.gameObjectTypeMap.hasOwnProperty(item) && $.isArray(ao_loc_item) && ao_loc_item.length > 0) {
                        empty = false;
                        return false;
                    }
                }
            }
        }
        return empty;
    }
};

state.event = new function StateEvent () {

    this.info_terrain_map = function (name, msg) {
        state.updateLevel(msg);
    };

    this.set_terrain_map = function (name, msg) {
        state.updateBlock(msg);
    };

    var obj_type_map = {
        'agent' : {
            _class : Agent,
            waiting_label : 'agents'
        },
        'obj' : {
            _class : Obj,
            waiting_label : 'objects'
        }
    };

    var _generic_list_event = function(obj_type) {
        return function (name, msg) {
            var msg_list = msg.list,
                msg_len = msg_list.length,
                obj,
                list_obj,
                i;

            for(i=0; i < msg_len; i++) {
                list_obj = msg_list[i];
                obj = state.gameObjectKnown(list_obj, obj_type);
                if (obj) {                  // update
                    obj.update(list_obj);
                } else {                    // create
                    obj = obj_type_map[obj_type]._class.create(list_obj);
                }
                obj.toState();
            }
            delete state.requests_waiting[obj_type_map[obj_type].waiting_label];
            state.check_loaded();
        };
    };

    this.info_object_list = _generic_list_event('obj');

    this.info_agent_list = _generic_list_event('agent');

    var _generic_object_change = function (obj_type) {
        return function (name, msg) {
            var obj = state.gameObjectKnown(msg.id, obj_type);
            if (obj) {
                obj.update(msg);
                obj.toState();
            } else if (state.contains(GameObject.pos.apply(msg))) {
                obj = obj_type_map[obj_type]._class.create(msg);
                obj.toState();
            }
        };
    };

    this.agent_position_change = _generic_object_change('agent');
    this.object_position_change = _generic_object_change('obj');
    
    this.agent_state_change = _generic_object_change('agent');
    this.object_state_change = _generic_object_change('obj');

    var _generic_object_create = function (obj_type) {
        return function (name, msg) {
            var obj;
            if (state.contains(GameObject.pos.apply(msg))) {
                obj = obj_type_map[obj_type]._class.create(msg);
                obj.toState();
            }
        };
    };

    this.agent_create = _generic_object_create('agent');
    this.object_create = _generic_object_create('obj');

    var _generic_object_delete = function (obj_type) {
        return function (name, msg) {
            var obj = state.gameObjectKnown(msg);
            if (obj) {
                obj.remove();
            }
        };
    };

    this.agent_delete = _generic_object_delete('agent');
    this.object_delete = _generic_object_delete('obj');

};

dispatcher.listen('info_terrain_map', state.event.info_terrain_map);
dispatcher.listen('set_terrain_map', state.event.set_terrain_map);

dispatcher.listen('info_agent_list', state.event.info_agent_list);
dispatcher.listen('info_object_list', state.event.info_object_list);

dispatcher.listen('agent_position_change', state.event.agent_position_change);
dispatcher.listen('object_position_change', state.event.object_position_change);

dispatcher.listen('agent_state_change', state.event.agent_state_change);
dispatcher.listen('object_state_change', state.event.object_state_change);

dispatcher.listen('agent_create', state.event.agent_create);
dispatcher.listen('object_create', state.event.object_create);

dispatcher.listen('agent_delete', state.event.agent_delete);
dispatcher.listen('object_delete', state.event.object_delete);
