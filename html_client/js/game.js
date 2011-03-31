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
            ///board_canvas.init();
			///drawingCache.init();
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
        
        //renderState.start();
        ///start drawing
        var interval;

        // input check interval
        interval = setInterval('game.input_interval()', this.delay);

    },
        
};
/*
var renderState = {
    
    view_x_offset: 0,
    view_y_offset: 0, // offsets against state
    
    render_dim: function() { return Math.sqrt(render.cells.length); }, // dims of render area. for some reason it is not working in render itself; fix that, and remove this
    
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
            cell_num,
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
                if (block == 1) {block = 8; color="red";}
                if (block == 0) {block = 7; color="red";}
                
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
                            if (type == 'agent') { block = 1;}
                            else {block = 15;}
                            console.log('agent render');
                            return false;
                        }
                    });
                }
                
                cell_num = render_dim*(y_ - this.view_y_offset) + (x_ - this.view_x_offset);
                render.colorTile(ctx, cell_num, block, color);
            }
        }
        
        // now draw cursor
        this.updateTile(cursor);
        
    },
    
    updateTile: function(msg) {
        
        if (typeof msg !== 'object') return false;
        
        var color = "red",
            cell_num = this.posInView(msg);
        
        if (cell_num !== null) {
            console.log('update tile');
            
            if (msg.type === 'cursor') {
                render.colorTile(render.canvasContext(), cell_num, msg.value, color);
            } else if (state.locationEmpty(msg) && GameObject.pos.apply(msg).toString() !== cursor.pos().toString()) {
                render.colorTile(render.canvasContext(), cell_num, msg.value, color);
            }
            
        }
            
        return true;
    },
    
    drawLoc: function(pos) {
        console.log('drawLoc');
        console.log(pos);
        
        if (pos.constructor.name === 'Array' && pos.length >= 3) {
            var pos_ = {}
            pos_.x = pos[0];
            pos_.y = pos[1];
            pos_.z = pos[2];
            pos = pos_;
        }
        console.log(pos);
        
        var cell_num = this.posInView(pos),
            color = 'red',
            ao_types = ['agent', 'object', 'container'];
        
        if (cell_num !== null) {
            var block = state.levels[pos.z][pos.x][pos.y];
            
            if (block <= 0) { // draw block in z_lvl below (just drawing a black square for now)
                //block = state.levels[z-1][x_][y_];
                block = 0;
                color = 'black';
            }
            if (block == 1) {block = 8; color="red";}
            if (block == 0) {block = 7; color="red";}
            
            ao = [pos.x, pos.y, pos.z].toString();
            ao_loc = state.ao_map[ao];
            if (ao_loc !== undefined) {                    
                
                console.log(ao);
                $.each(ao_types, function (i, type) {
                    var ao_type = ao_loc[type];
                    console.log(ao_type);
                    if (ao_type !== undefined && ao_type.length > 0) {
                        //block = ao_type[0].tile_num; // draw the first game_object in the first ao_types to exist
                        color = 'green';
                        if (type == 'agent') { block = 1;}
                        else {block = 15;}
                        console.log('agent render');
                        return false;
                    }
                });
            }
            
            render.colorTile(render.canvasContext(), cell_num, block, color);
        }
        
    },
    
    posInView: function (msg) {
        
        if (msg.constructor.name === 'Array' && msg.length >= 3) {
            var msg_ = {}
            msg_.x = msg[0];
            msg_.y = msg[1];
            msg_.z = msg[2];
            msg = msg_;
        }
        
        var render_dim = this.render_dim(),
            max_x = Math.min(this.view_x_offset + render_dim, state.map_width),
            max_y = Math.min(this.view_y_offset + render_dim, state.map_height),
            cell_num = render_dim*msg.y+msg.x;
        
        if (this.view_x_offset < msg.x && msg.x < max_x) {
            if (this.view_y_offset < msg.y && msg.y < max_y) {
                if (msg.z == state.current_z_lvl) {
                    return cell_num;
                }
            }
        }
        
        return null;
    },
    
    // takes a cursor and updates view_A_offset if at the boundary
    scrollMap: function(cursor) {
        
        var render_dim = this.render_dim() - 1,
            scrollAmt = {},
            x_delta = 0,
            y_delta = 0,
            scrollEdge = 0; // how far inside the map to start scrolling, when cursor pushes that boundary
        
        if (cursor.x) {
            x_delta = cursor.x - (this.view_x_offset + render_dim - scrollEdge);
            if (x_delta > 0) {
                this.view_x_offset += x_delta;
                scrollAmt.x = x_delta;
                console.log('x upper bound');
            } else {
                x_delta = cursor.x - (this.view_x_offset + scrollEdge);
                if (x_delta < 0) {
                    this.view_x_offset += x_delta;
                    scrollAmt.x = x_delta;
                    console.log('x lower bound');
                }
            }
        }
        console.log(x_delta);
        
        if (cursor.y) {
            y_delta = cursor.y - (this.view_y_offset + render_dim - scrollEdge);
            if (y_delta > 0) {
                this.view_y_offset += y_delta;
                scrollAmt.y = y_delta;
                console.log('y upper bound');
            } else {
                y_delta = cursor.y - (this.view_y_offset + scrollEdge);
                if (y_delta < 0) {
                    this.view_y_offset += y_delta;
                    scrollAmt.y = y_delta;
                    console.log('y lower bound');
                }
            }
        }
        console.log(y_delta);
        
        console.log('scrollmap');
        console.log(scrollAmt);
        console.log(this.view_x_offset);
        console.log(this.view_y_offset);
        console.log(cursor);
        if (scrollAmt.x || scrollAmt.y) {
            //render.pan(scrollAmt);
            this.start();
        }
        
    },
    
    renderColumn: function (col) {
        var cells = this.render_dim(),
            i = 0;
        
        while (i < cells) {
            this.drawLoc([i, col, cursor.z]);
            i++;
        }
    },
    
    renderRow: function (row) {
        var cells = this.render_dim(),
            i = 0;
        
        while (i < cells) {
            this.drawLoc([i, row, cursor.z]);
            i++;
        }
    },
        
};
*/
// temporary, just to keep processInput action calls from throwing
var selected_agent = { x: 15,
                       y: 15,
                       z: 5,
                       id:0,
                       pos: function() { return [this.x, this.y, this.z]; } 
                     };
                     
var cursor = { x: 25,
               y: 12,
               z: 5,
               value: 176,
               type: 'cursor',
               pos: function() { return [this.x, this.y, this.z]; },
               moveX: function(amt) {
                        this.x += amt;
                        this.x = Math.max(0, this.x);
                        renderState.scrollMap(this);
                    },
               moveY: function(amt) {
                        this.y += amt;
                        this.y = Math.max(0, this.y);
                        renderState.scrollMap(this);
                    },
             };

processInput = function (key) {
    
    if (!key) return false;
    
    console.log(key);
    
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
    
    //console.log(key);
    
    return true;
};
