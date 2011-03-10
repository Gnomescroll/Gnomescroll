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
            render.init();
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
        
        renderState.start();
        
        var interval;

        // input check interval
        interval = setInterval('game.input_interval()', this.delay);

    },
        
};

var renderState = {
    
    view_x_offset: 0,
    view_y_offset: 0, // offsets against state
    
    start: // initial rendering of state
    function () {
        
        // for each block in state's curr_z_lvl
        // if block == 0
        //      draw the block underneath it (white/black if also empty)
        // else
        //      draw block's tile png
        // for each 3-ple location in view
        //      check whats there
        //      draw it
        
        var z = state.current_z_lvl,
            ctx = render.canvasContext();
        
        console.log(render.grid_cells);
        
        var x_, y, y_, 
            block, 
            ao, ao_loc,
            cell_num = 0,
            render_dim = Math.sqrt(render.cells.length),
            max_x_ = Math.min(this.view_x_offset + render_dim, state.map_width),
            max_y_ = Math.min(this.view_y_offset + render_dim, state.map_height);
        
        console.log('state levels');
        //console.log(state.levels);
        
        console.log(render.cells.length);
        console.log('ao_map');
        console.log(state.ao_map);
        var color='black', ao_types = ['agent', 'object', 'container'];
        for (x_ = this.view_x_offset; x_ < max_x_; x_++) {
            
           // console.log('inner x_ loop');
            
            y = state.levels[z][x_];
            
           // console.log(y);
            for (y_ = this.view_y_offset; y_ < max_y_; y_++) {
                
              //  console.log('inner y_ loop');
               // console.log(cell_num);
                block = y[y_];
                color = 'red';
                
                if (block <= 0) { // draw block in z_lvl below (just drawing a black square for now)
                    //block = state.levels[z-1][x_][y_];
                    block = 0;
                    color = 'black';
                }
                
                ao = [x_, y_, z].toString();
                ao_loc = state.ao_map[ao];
                if (ao_loc !== undefined) {                    
                    
                    console.log(ao);
                    $.each(ao_types, function (i, type) {
                        var ao_type = ao_loc[type];
                        console.log(ao_type);
                        if (ao_type !== undefined && ao_type.length > 0) {
                            //block = ao_type[0].tile_num; // draw the first game_object in the first ao_types to exist
                            color = 'green';
                            if (type == 'agent') { block = 17;}
                            else {block = 20;}
                            console.log('agent render');
                            return false;
                        }
                    });
                }
                
                if (block == 1) {block = 3; color="blue"; console.log(color);}
                
                if (block == 0) {block = 7; color="red";}
                //console.log('block: '+block);
                if (color == 'green') console.log(color);
                render.colorTile(ctx, cell_num, block, color);
                cell_num += 1;
            }
        }
        
    },
};

// temporary, just to keep processInput action calls from throwing
var selected_agent = { x: 15,
                       y: 15,
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
            console.log('set map');
            //admin.set_map(selected_agent.pos(), 1);
            admin.set_map([5, 5, 5], 1);
            break;
            
        case 'x':                   // set map empty
            //admin.set_map(selected_agent.pos(), 0);
            admin.set_map([5, 5, 5], 0);
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
