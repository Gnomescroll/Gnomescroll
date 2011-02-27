//prototypal inheritance

var checks = ['agent','game_object'];

var g, a, test;

test = function (g, a) {
    if (g !== 'game_object') console.log('game_object '+g+' failed');
    else console.log('game_object '+g+' passed');
    if (a !== 'agent') console.log('agent '+a+' failed');
    else console.log('agent '+a+' passed');
};

// prototype beget
if (typeof Object.beget !== 'function') {
     Object.beget = function (o) {
         var F = function () {};
         F.prototype = o;
         return new F();
     };
}

var game_object, agent, object;

game_object = {
    
    type: 'game_object',
                 
    getType: function () {
        return this.type;
    },
};

agent = Object.beget(game_object);
agent.type = 'agent';

console.log('prototype beget');
g = game_object.getType() // game_object
a = agent.getType();       // agent
test(g, a);                        //
// pass                             //
//////////////////////////////////////

//////////////////////////////////////
// prototypal w/ encapsulation      //
GameObject = function () {
    
    var type, getPosition, getType;
    
    type = 'game_object';
    
    getType = function () {
        return type;
    };
    
    return { 
             getType: getType,
           };
           
};

Agent = function () {
    
    var type = 'agent',
        agent;
        
    agent = Object.beget(GameObject());
    
    agent.type = 'agent';
    
    return agent;
    
};
    
game_object = GameObject();
agent = Agent();


console.log('--------------------------------');
console.log('prototypal w/ encapsulation test');
g = game_object.getType() // game_object
a = agent.getType();       // game_object
test(g, a);
// failed                           //
//////////////////////////////////////


//////////////////////////////////////
// prototypal + closure factory     //
// incomplete
GameObject = function () {
    
    var create,
        type = 'game_object',
        build,
        built;
    
    
    build = function () {
        
        this.create = function () {
           var getType;
            
            getType = function () {
                return type;
            };
            
            return { 
                     getType: getType,
                   };
        };
    };
    
    return {
             build: build,
           };
};

Agent = function (go) {
    
    var type = 'agent',
        agent;
        
    agent = Object.beget(go);//GameObject());
    
    agent.type = type;
    
    agent.build();
    
    return agent;
    
};
    
var go = GameObject();
console.log(go);
go.build();
console.log(go);
var ag = Agent(go);
console.log(ag);

game_object = go.create();
agent = ag.create();

console.log('----------------------------');
console.log('prototypal + closure factory');
g = game_object.getType(); // 
a = agent.getType();       // 
test(g, a);                        //
//                                  //
//////////////////////////////////////

function update_amount (msg) {
    var amt,
        auction = $('#auction-amount');
        
    amt = parseInt(msg.payload) + parseInt(auction.val());
    auction.val(amt);
};
    
    
    
