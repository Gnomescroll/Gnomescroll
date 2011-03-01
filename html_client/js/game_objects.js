// agent and object methods bundled here
// should have a convenience method for creating an object
// and should have methods for applying mods to the object

// might need to do a bit different inheritance , like .beget()
// so we can keep methods and static properties in a single object
// that can be looked up from the instance object 
// (python object hierarchy; saves a lot of memory)

// prototype beget
if (typeof Object.beget !== 'function') {
     Object.beget = function (o) {
         var F = function () {};
         F.prototype = o;
         return new F();
     };
}

var locationStateMap = {
    
    0: 'ground',
    1: 'agent',
    2: 'container',
    
}

// need agent interface methods
// and agent object creator

// interface
Agent = {
    
    pos: // return pos array
    function () {
        return [this.x, this.y, this.z];
    },
        
    update: // update an agent instance attributes. do not call on Agent, only agent instance
    function (data) {
        $.each(data, function(key, val) {
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
    
    toState:
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
    },
    
    
}

// agent obj creator
Agent.create = function (data) {
    
    var agent = Object.beget(this);
    
    $.each(data, function(key, val) {
        agent[key+'_old'] = agent[key];
        agent[key] = val;
    });
    
    return agent;
}
