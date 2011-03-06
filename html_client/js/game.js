var game,
    processInput;
    
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
 * 
 * UPDATE 3/5/2011
 *      User input checking w/ delay seems to work well
 *      
 *      Next, implement rendering of initial state.
 *      Then, render updates on message received.
 *      Then, efficient rendering on view change
 *      Then, scrolling based on position & movement of selected agents/moving things
 *      Then, mouse input clicks for selecting agent
 *      Then, mouse input for onHover tooltips/cell info
 *      Then, mouse input for drag selection (multiagent)
 */     


// main loop
game = {

    init: // load z_level +/- 1. Load agents, objects. init render object.
    function () {
        socket.init();
        state.init();
        input.init();
    },
    
    delay: 300, // ms delay for input check
    
    input_interval: // called in game.start setInterval
    function () { 
        //console.log('called');
        processInput(input.next(this.delay))
    },
    
    start: // main game loop.  is it async/threaded?
           // check user input, server updates, render game.
    function () {
        
        render.start();
        
        var interval;

        // input check interval
        interval = setInterval('game.input_interval()', this.delay);

    },
        
};

var render = {
    
    start: // initial rendering of state
    function () {
        
        // for each block in state's curr_z_lvl
        // if block == 0
        //      draw the block underneath it (white/black if also empty)
        // for each 3-ple location in view
        //      check whats there
        //      draw it
        
    },
    
    
    
    
};

// temporary, just to keep processInput action calls from throwing
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
