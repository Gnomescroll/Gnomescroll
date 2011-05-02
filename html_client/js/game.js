var game = {

    started : false,

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
                } else {
                    console.log('input init call');
                    input.init();
                    clearInterval(arguments.callee.interval);
                }
            };
        return wait_func;
    }()),
    
    start : function (callback) {
        //input.run();
        if (typeof callback === 'function') {
            callback();
        }
    },

    update : function (callback) {
        drawingCache.loadTilesets();
        tileset_state.init();
        if (typeof callback === 'function') {
            callback();
        }
    },
    
    update2 : function() {
        state.init();
    },

    waiting_for_state : true,

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

dispatcher.listen('register', function (event_name, msg) {
    console.log('game listener triggered');
    console.log(msg);
    if (parseInt(msg.update, 10)) { // request updates
        game.update();
    }
}, 1);
