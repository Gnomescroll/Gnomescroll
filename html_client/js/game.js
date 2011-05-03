var game = {

    started : false,

    init : function () {
        drawingCache.init();
        socket.init();
        dispatcher.trigger('game_init');
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

    update3 : function () {
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
        socket.reset();
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
    console.log('register triggered; game.update() being called');
    if (parseInt(msg.update, 10)) { // request updates
        game.update();
    }
});

dispatcher.listen('tileset_state_loaded', function () {
    game.update2();
});

dispatcher.listen('state_loaded', function () {
    game.update3();
});
