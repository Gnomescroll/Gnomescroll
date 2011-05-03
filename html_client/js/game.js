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
        board.cursor_manager.init();
        state.init();                   // requests terrain/game_objects; once all have been received, 'state_loaded' event triggers
        dispatcher.trigger('game_update_complete');
    },

    start : function () {
        board.init();                   // Inits if tileset_state is loaded, else continues to attempt to init until tileset_state is loaded. Calls board.start() once ready, which triggers 'board_start'
        dispatcher.trigger('game_start');
    },

    activate : function () {    // triggered by 'board_start'
        input.init();                   // start accepting user input
        map_editor.init();              // load map editor
        options.init();                 // load UI options
        dispatcher.trigger('game_activate');
    },

    reset : function () { // resets everything
        // wipe everything and run init/start
        this.quit();
        dispatcher.trigger('game_reset_complete');
        controls.trigger_init(); // begin chain of init events
    },

    quit : function (err) { // wipes everything, notifies user of error. Use for things like completely failing to connect to server, or some other serious issue.
        this.started = false;
        controls.reset();
        input.reset();
        socket.reset(true);
        tile_properties.reset();
        tileset_state.reset();
        drawingCache.reset_full();
        board.reset();
        state.reset();
        map_editor.reset();
        options.reset();
        dispatcher.trigger('game_quit_complete', err);
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

dispatcher.listen('board_init_fail', function (name, err) {
    game.quit(err);
});

dispatcher.listen('game_quit_complete', function (name, err) {
    if (err) {
        alert(err);
    }
});
