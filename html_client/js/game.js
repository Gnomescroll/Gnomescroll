var game;

game = {

    init: // load z_level +/- 1. Load agents, objects. init render object.
    function () {
        socket.init();
        state.init();
    },
    
    start: // main game loop.  is it async/threaded?
           // check user input, server updates, render game.
    function () {
        
    },
        
};
