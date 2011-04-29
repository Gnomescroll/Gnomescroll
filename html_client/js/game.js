var game = {

    started : false,

    //init : function (callback) { // load z_level +/- 1. Load agents, objects. init render object.
        //var board_init_eval = 'board.init();';
        //drawingCache.init();
        //socket.init();
        //if (typeof callback === 'function') {
            //var args = [],
                //len = arguments.length,
                //i = 1;
            //for (i=1; i < len; i++) {
                //args[i-1] = arguments[i];
            //}
            //window._game_init_callback = function () {
                //callback.apply(this, args);
                //delete window._game_init_callback;
            //}
            //board_init_eval = 'board.init(_game_init_callback);';
        //}
        //board.init_board_interval = setInterval(board_init_eval, 200);
    //},

    init : function (callback) {
        drawingCache.init();
        socket.init();
        if (typeof callback === 'function') {
            callback();
        }
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
    
    start : function (callback) { 
        //var interval = setInterval('game.input_interval()', this.input_delay); // start input rate-limit queue
        if (typeof callback === 'function') {
            callback();
        }
    },

    //update : function (callback) { // game requests info updates after it has started
        //state.init();
        //input.init();
        //board.reset();
        //board.init();
        //if (typeof callback === 'function') {
            //callback();
        //}
    //},

    update : function (callback) {
        drawingCache.loadTilesets();
        tileset_state.init();
        if (typeof callback === 'function') {
            callback();
        }
    },

    waiting_for_state : true,
    update2 : function() {
        state.init();
    },

    update3 : function () {
        this.waiting_for_state = false;
        input.init();
        board.init();
        board.start();
        map_editor.init();
        options.init();
    },

    reset : function (callback) { // resets everything
        // reset:
        //  socket
        //  board
        //  tilesets
        //  state
        //  game
        //
        // essentially, wipe everything and run init/start
        this.started = false;
        controls.reset();
        //socket.disconnect();
        delete socket.socket;
        tile_properties.reset();
        tileset_state.reset();
        drawingCache.reset_full();
        board.reset();
        state.reset();
        map_editor.reset();
        options.reset();
        
        controls.trigger_load();

        if (typeof callback === 'function') {
            callback();
        }
    },
    
};
