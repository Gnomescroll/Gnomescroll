var game = {

    started : false,

    init : function () {
        drawingCache.init();
        socket.init();
        dispatcher.trigger('game_init');
    },
    
    init2 : function () {
        this.retry.interval = setInterval('game.retry();', 500);
        this.started = true;
        dispatcher.trigger('game_init2');
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
    
    start : function () {
        //input.run();
        dispatcher.trigger('game_start');
    },

    update : function () {
        drawingCache.loadTilesets();
        tileset_state.init();
        dispatcher.trigger('game_update');
    },
    
    update2 : function() {
        state.init();
        dispatcher.trigger('game_update2');
    },

    waiting_for_state : true,

    update3 : function () {
        this.waiting_for_state = false;
        input.init();
        board.init();
        board.start();
        map_editor.init();
        options.init();
        dispatcher.trigger('game_update3');
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
        dispatcher.trigger('game_reset');
        controls.trigger_load(); // begin chain of init events
    },
    
};

dispatcher.listen('register', function (name, msg) {
    console.log('game listener triggered');
    console.log(msg);
    if (parseInt(msg.update, 10)) { // request updates
        game.update();
    }
});

dispatcher.listen('info_tileset', function () {
    game.update2();
});

dispatcher.listen('state_loaded', function () {
    game.update3();
});
