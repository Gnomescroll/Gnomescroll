// initialization chain methods
// methods listed in order of being called (except reset, which is called on reset)
var game = {

    started : false,

    init : function () {
        drawingCache.init();            // creates a canvas
        socket.init();                  // connects to socket, sends register message which will trigger 'registered' event when received
        dispatcher.trigger('game_init_complete');
    },
    
    registered : function () {
        drawingCache.loadTilesets();    // downloads and stores references to tilesets
        tileset_state.init();           // requests tileset rendering info
        dispatcher.trigger('game_registered');
    },
    
    update : function() {
        state.init();                   // requests terrain/game_objects; once all have been received, 'state_loaded' event triggers
        dispatcher.trigger('game_update_complete');
    },

    start : function () {
        //input.init();                   // start accepting user input
        board.init();                   // 
        //board.start();
        //map_editor.init();
        //options.init();
        dispatcher.trigger('game_start');
    },

    activate : function () {
        input.init();                   // start accepting user input
        map_editor.init();
        options.init();
        dispatcher.trigger('game_activate');
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

dispatcher.listen('board_start', function () {
    game.activate();
});
