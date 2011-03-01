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
            agent[key+'_old'] = agent[key];
            this[key] = val;
        });
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
                
        if (state.positionInState(this.pos())) { // agent pos in state
            
            if (state.gameObjectKnown(this)) { // known agent, updating
                state.cleanLocation(this.old());
                state.setLocation(this);
            
            } else { // new agent, adding
                state.addGameObject(this);
                state.setLocation(this);
            }
            
        } else { // agent pos not in state
            
            if (state.gameObjectKnown(this)) { // known agent, removing
                state.cleanLocation(this.old());
                state.removeGameObject(this);
                
            } else { // don't know don't care
                return;
            }
        }
        
        this.cleanOld();
    },
    
    create:
    function (data) {
    
        var agent = Object.beget(this);
        
        $.each(data, function(key, val) {
            agent[key] = val;
        });
        
        return agent;
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

Obj = $.extend({}, GameObject);

Container = $.extend({}, GameObject);
Container = $.extend(Container, InventoryMethods);
