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
    },
    
    init2: function () {
        var wait_func = function () {
            console.log('wait_func');
            state.init();
            input.init();
            //render.init();
            //board_canvas.init();
            //drawingCache.init();
        }
        setTimeout(wait_func(), 2000); // wait half a sec (does this work?)
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
        // input check interval
        var interval = setInterval('game.input_interval()', this.delay);
    }
        
};


// temporary, just to keep processInput action calls from throwing
var selected_agent = {
    x: 15,
    y: 15,
    z: 5,
    id:0,
    pos: function() {
        return [this.x, this.y, this.z];
    } 
};
                     
var cursor = {
    x: 25,
    y: 12,
    z: 5,
    value: 176,
    type: 'cursor',
    pos: function() {
        return [this.x, this.y, this.z];
    },
    moveX: function(amt) {
            this.x += amt;
            this.x = Math.max(0, this.x);
            renderState.scrollMap(this);
    },
    moveY: function(amt) {
            this.y += amt;
            this.y = Math.max(0, this.y);
            renderState.scrollMap(this);
    }
};

processInput = function (key) {
    
    if (!key) return false;
    
    var old;
    
    switch (key) {
        
        case 'ESC':               //quit ? remove this
            map_editor.clear_current();
            return false;
            
        case 'LEFT':
            //action.move(selected_agent.id, -1, 0, 0);
            /*
            old = cursor.pos();
            cursor.moveX(-1);
            renderState.drawLoc(old);
            renderState.updateTile(cursor);*/
            break;
            
        case 'RIGHT':
            //action.move(selected_agent.id, 1, 0, 0);
            /*
            old = cursor.pos();
            cursor.moveX(1);
            renderState.drawLoc(old);
            renderState.updateTile(cursor);*/
            break;
            
        case 'UP':
            //action.move(selected_agent.id, 0, 1, 0);
            /*
            old = cursor.pos();
            cursor.moveY(-1);
            renderState.drawLoc(old);
            renderState.updateTile(cursor);*/
            break;
            
        case 'DOWN':
            //action.move(selected_agent.id, 0, -1, 0);
            /*
            old = cursor.pos();
            cursor.moveY(1);
            renderState.drawLoc(old);
            renderState.updateTile(cursor);*/
            break;
            
        case 'c':                   // set map solid
            console.log('set map');
            //admin.set_map(selected_agent.pos(), 1);
            admin.set_map(cursor.pos(), 1);
            break;
            
        case 'x':                   // set map empty
            //admin.set_map(selected_agent.pos(), 0);
            admin.set_map(cursor.pos(), 0);
            break;
            
        case 'p':                   // create agent
            admin.create_agent(cursor.pos());
            break;
            
        case 't':
            admin.create_object(cursor.pos());
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
    return true;
};
