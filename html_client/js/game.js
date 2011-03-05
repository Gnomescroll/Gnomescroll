var game,
    renderState,
    processInput,
    delay = 300; // ms delay for input check
    
var interval_func = function () {
    console.log('called');
    processInput(input.next(delay))
}
    
/* *******************************
 * *******************************
 * 
 * Get rid of the game loop.
 * In init, render the starting game state after the messages have been received.
 * When later messages come in, render the update only.
 
 * Meanwhile, key inputs will be caught.
 * Still, add these to a queue, so can compare timestamps. 
 * (For some reason, keypress/keydown always trigger twice w/ same timestamp after the first press, and a simple tracking var doesn't catch the 2nd)
 * Have a setInterval for pulling from the queue.
 * Pick the first event w/ timestamp diff > delay, then flush queue
 *
 */ 


// main loop
game = {

    init: // load z_level +/- 1. Load agents, objects. init render object.
    function () {
        socket.init();
        state.init();
        input.init();
    },
    
    start: // main game loop.  is it async/threaded?
           // check user input, server updates, render game.
    function () {
        
        var done = false,
            interval;

        interval = setInterval('interval_func()', delay);

    },
        
};

var selected_agent = { x: 5,
                       y: 5,
                       z: 5,
                       id:0,
                       pos: function() { return [this.x, this.y, this.z]; } 
                     };

processInput = function (key) {
    
    if (!key) return false;
    
    console.log(key);
    
    switch (key) {
        
        case 'ESC':               //quit ? remove this
            return false;
            
        case 'LEFT':
            action.move(selected_agent.id, -1, 0, 0);
            break;
            
        case 'RIGHT':
            action.move(selected_agent.id, 1, 0, 0);
            break;
            
        case 'UP':
            action.move(selected_agent.id, 0, 1, 0);
            break;
            
        case 'DOWN':
            action.move(selected_agent.id, 0, -1, 0);
            break;
            
        case 'c':                   // set map solid
            admin.set_map(selected_agent.pos(), 1);
            break;
            
        case 'x':                   // set map empty
            admin.set_map(selected_agent.pos(), 0);
            break;
            
        case 'p':                   // create agent
            admin.create_agent(selected_agent.pos());
            break;
            
        case 't':
            admin.create_object(selected_agent.pos());
            break;
            
        case 'g':
            //admin.create_object(selected_agent.pos(), 
            break;
            
        case 'r':
            action.plant(selected_agent.id);
            break;
            
        case 'f':
            action.till(selected_agent.id);
            break;
            
        case 'v':
            action.harvest(selected_agent.id);
            break;
            
        case 'i':
            info.map(5);
            break;
    }
    
    //console.log(key);
    
    return true;
};

// links state to render functions
renderState = function () {
    
    
    
};
