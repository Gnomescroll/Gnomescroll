var game,
    renderState,
    processInput;

// main loop
game = {

    init: // load z_level +/- 1. Load agents, objects. init render object.
    function () {
        socket.init();
        state.init();
    },
    
    start: // main game loop.  is it async/threaded?
           // check user input, server updates, render game.
    function () {
        
        var done = false
        
        while (!done) {
            
            done = processInput();
            renderState();
            
        }
    },
        
};

processInput = function () {
    var key;
    
    key = input.next();
    
    switch (key) {
        
        case 'q':               //quit
            return false;
            
        case     
            
            
        
        default:
            return true;
            
    }
};

// links state to render functions
renderState = function () {
    
    
    
};
