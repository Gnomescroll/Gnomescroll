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
var game = {

    started : false,

    init : function (callback) { // load z_level +/- 1. Load agents, objects. init render object.
        var board_init_eval = 'board.init();';
        drawingCache.init();
        socket.init();
        if (typeof callback === 'function') {
            window._game_init_callback = function () {
                callback();
                delete window._game_init_callback;
            }
            board_init_eval = 'board.init(_game_init_callback);';
        }
        board.init_board_interval = setInterval(board_init_eval, 200);
    },
    
    init2 : function () {
        this.retry.interval = setInterval('game.retry();', 500); // wait half a sec (does this work?)
        this.started = true;
    },

    retry : (function () {
        var success = false,
            wait_func = function () {
                console.log('wait_func');
                if (!success) {
                    console.log('state init call');
                    success = state.init();
                    console.log(success);
                } else {
                    console.log('input init call');
                    input.init();
                    clearInterval(arguments.callee.interval);
                }
            };
        return wait_func;
    }()),
    
    input_delay : 3000, // ms delay for input check
    
    input_interval : function () { // called in game.start setInterval
        processInput(input.next(this.input_delay));
    },
    
    start : function (callback) { // main game loop
        //var interval = setInterval('game.input_interval()', this.input_delay); // start input rate-limit queue
        if (typeof callback === 'function') {
            callback();
        }
    },

    update : function () { // game requests info updates after it has started
        state.load_game_state();
        board.reset();
    },

    reset : function () { // resets everything
        // reset:
        //  socket
        //  board
        //  tilesets
        //  state
        //  game
        //
        // essentially, wipe everything and run init/start
        controls.reset();
        socket.disconnect();
        tile_properties.reset();
        tileset_state.reset();
        drawingCache.reset();
        board.reset();
        state.reset();
        map_editor.reset();
        options.reset();

        controls.trigger_load();
    },
    
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
