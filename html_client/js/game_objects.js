// prototype beget
if (typeof Object.beget !== 'function') {
     Object.beget = function (o) {
         var F = function () {};
         F.prototype = o;
         return new F();
     };
}

var GameObject, Obj, Agent, Container;

var get_board_event_name = function (game_obj) {
    var event_name;
    switch (game_obj.base_type) {
        case 'agent':
            event_name = 'agent_change';
            break;
        case 'obj':
            event_name = 'object_change';
            break;
        case 'container':
            event_name = 'object_change';
            break;
    }
    return event_name;
}

// interface
GameObject = {
    
    pos: // return pos array
    function () {
        return [this.x, this.y, this.z];
    },
        
    update: // update an agent instance attributes. do not call on Agent, only agent instance
    function (data) {
        var that = this,
            key,
            val;

        for(key in data) {
            if (data.hasOwnProperty(key)) {
                val = data[key];
                that[key+'_old'] = that[key];
                that[key] = val;
            }
        }
        // emit message to renderer
        board.event[get_board_event_name(that)](that);
    },
    
    old: // past state, after update
    function () {
        var dummy = {},
            key,
            val;

        for(key in this) {
            if (this.hasOwnProperty(key)) {
                val = this[key];
                if (key.slice(key.length-4) === '_old') {
                    dummy[key.slice(0, key.length-4)] = val;
                }
            }
        }
        dummy.isOld = true;
        
        dummy = $.extend({}, this, dummy);
        return dummy;
    },
    
    cleanOld: // removing past state, after not needed
    function () {
        var to_delete = [],
            key,
            val,
            i = 0,
            len;

        for(key in this) {
            if (this.hasOwnProperty(key)) {
                val = this[key];
                if (key.slice(key.length-4) === '_old') {
                    to_delete.push(key);
                }
            }
        }

        len = to_delete.length;
        for(i=0; i < len; i++) {
            key = to_delete[i];
            delete this[key];
        }
    },
    
    toState: // set object to the state
    function () {
        
        var that = this;
        
        if (state.contains(this.pos())) { // agent pos in state
            
            if (state.gameObjectKnown(this)) { // known agent, updating
                state.cleanLocation(this.old());
                state.setLocation(this);
            
            } else { // new agent, adding
                state.addGameObject(this);
                state.setLocation(this);
            }
            
        } else { // agent pos not in state
            
            if (state.gameObjectKnown(this)) { // known agent, removing
                this.remove();
                
            } else { // don't know don't care
                return;
            }
        }
        
        this.cleanOld();
        
        // emit message to renderer
        board.event[get_board_event_name(that)](that);
    },

    instance_properties: ['inventory'],
    defaults:
        function(name) {
            var defaults = {
                inventory: [],
            }
            return defaults[name];
    },
    
    create:
    function (data) {

        var obj = Object.beget(this); // link prototype to GameObject
        obj = $.extend(obj, data);    // add instance properties

        // add inventory here, until more elegant solution is found for creating
        // default instance values
        var len = this.instance_properties.length,
            i = 0,
            name;
        for (i=0; i < len; i++) {
            name = this.instance_properties[i];
            if ((!obj.hasOwnProperty(name)) && this.hasOwnProperty(name)) {
                obj[name] = this.defaults(name);
            }
        }
         
        return obj;
    
    },
    
    remove:
    function () {
        var that = $.extend({}, this);
        
        state.cleanLocation(this);
        // find what inventory it is stored in (if it is at all), and remove it
        // IMPLEMENT
    
        state.removeGameObject(this);
        
        // emit message to renderer
        board.event[get_board_event_name(that)](that);
        delete that;
    },

};

InventoryMethods = {

    inventory: [],
    
    addInventory:
    function(game_object) {
        this.inventory.push(game_object.id);
    },
    
    removeInventory:
    function (game_object) {
        var index = $.inArray(game_object.id);
        if (index > -1) {
            this.inventory.splice(index, 1);
        }
    },
};


// interface
Agent = $.extend({}, GameObject);
Agent = $.extend(Agent, InventoryMethods);
Agent.tile_num = 7;
Agent.base_type = 'agent';

Obj = $.extend({}, GameObject);
Obj.tile_num = 8;
Obj.base_type = 'obj';

Container = $.extend({}, GameObject);
Container = $.extend(Container, InventoryMethods);
Container.tile_num = 9;
Container.base_type = 'container';

function _generic_object_info (obj_type) {
    var obj_class = {
        'agent' : Agent,
        'obj'   : Obj
    };
    return function (name, msg) {
        var obj = state.gameObjectKnown(msg);
        if (obj) {            // update
            obj.update(msg);
        } else {                // create
            obj = obj_class[obj_type].create(msg);
        }
        obj.toState();
    };
};

dispatcher.listen('info_agent_info', _generic_object_info('agent'));
dispatcher.listen('info_object_info', _generic_object_info('obj'));
