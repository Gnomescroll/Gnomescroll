// initialization chain methods
// methods listed in order of being called (except reset, which is called on reset)
var game = {

    started : false,

    init : function () {
        drawingCache.init();
        socket.init();
        dispatcher.trigger('game_init_complete');
    },
    
    registered : function () {
        drawingCache.loadTilesets();
        tileset_state.init();
        dispatcher.trigger('game_registered');
    },
    
    update : function() {
        state.init();
        dispatcher.trigger('game_update_complete');
    },

    start : function () {
        input.init();
        board.init();
        board.start();
        map_editor.init();
        options.init();
        dispatcher.trigger('game_start');
    },

    reset : function () { // resets everything
        // wipe everything and run init/start
        this.started = false;
        controls.reset();
        socket.reset();
        tile_properties.reset();
        tileset_state.reset();
        drawingCache.reset_full();
        board.reset();
        state.reset();
        map_editor.reset();
        options.reset();
        dispatcher.trigger('game_reset_complete');
        controls.trigger_init(); // begin chain of init events
    },
    
};

dispatcher.listen('game_init', function () {
    game.init();
});

dispatcher.listen('register', function (name, msg) {
    console.log('register triggered; game.update() being called');
    if (parseInt(msg.update, 10)) { // request updates
        game.registered();
    }
});

dispatcher.listen('tileset_state_loaded', function () {
    game.update();
});

dispatcher.listen('game_update', function () {
    game.update();
});

dispatcher.listen('state_loaded', function () {
    game.start();
});

dispatcher.listen('game_reset', function () {
    game.reset();
});
