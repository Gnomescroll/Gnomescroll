// prototype beget
if (typeof Object.beget !== 'function') {
     Object.beget = function (o) {
         var F = function () {};
         F.prototype = o;
         return new F();
     };
}

var GameObject, Obj, Agent, Container;


// interface
GameObject = {
    
    pos: // return pos array
    function () {
        return [this.x, this.y, this.z];
    },
        
    update: // update an agent instance attributes. do not call on Agent, only agent instance
    function (data) {
        $.each(data, function(key, val) {
            this[key+'_old'] = this[key];
            this[key] = val;
        });
        
        // emit message to renderer
        //board_event.objectUpdate(this);
    },
    
    old: // past state, after update
    function () {
        var dummy = {};
        
        $.each(this, function(key, val) {
            if (key.slice(key.length-4) === 'old') {
                dummy[key] = val;
            }
        });
        
        return dummy;
    },
    
    cleanOld: // removing past state, after not needed
    function () {
        var to_delete = [];
        
        $.each(this, function(key, val) {
            if (key.slice(key.length-4) === '_old') {
                to_delete.push(key);
            }
        });
        
        $.each(to_delete, function(i, key) {
            delete this[key];
        });
    },
    
    toState: // set object to the state
    function () {
                
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
        //board_event.objectToState(this);
    },
    
    create:
    function (data) {
    
        //var agent = Object.beget(this);
        console.log('gameObject create');
        console.log(this);
        var gobj = $.extend({},this);
        
        $.each(data, function(key, val) {
            gobj[key] = val;
        });
        
        delete gobj.create;
        
        return gobj;
    },
    
    remove:
    function () {
        state.cleanLocation(this);
        // find what inventory it is stored in (if it is at all), and remove it
        // IMPLEMENT
    
        state.removeGameObject(this);
        
        // emit message to renderer
        //board_event.removedObject(this);
    },

}

InventoryMethods = {
    
    addInventory:
    function(game_object) {
        this.inventory.push(game_object.id);
    },
    
    removeInventory:
    function (game_object) {
        var index = $.inArray(game_object.id);
        if (index > -1) {
            this.inventory.splice(index,1);
        }
    },
},

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
