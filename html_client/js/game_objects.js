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

var game_object,
    agent,
    object;

// make a game_object base class object literal
game_object = {
    
    id: 0,     
    x: 0,
    y: 0,
    z: 0,
    loc_state: '',
    
    

    pos: function () {
            return [this.x, this.y, this.z];
         },
         
    serialize: function () {
                },

};

agent = Object.beget(game_object);
agent = function () {
    
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
