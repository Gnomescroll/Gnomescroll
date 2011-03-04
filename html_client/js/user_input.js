var input;

input = ( function () {

    var queue = [],
        init,
        next;
        
    init = function () {
        
        $('body').keypress(function (event) {
            var key = keymap[event.which];
            queue.push({key: key, timestamp: event.timeStamp});
        });
    };
        
    next = (function () {
        
        var timestamp = 0,
            delay = 300; // millisecond delay between input
        
        return
        function () {
            var event = queue.shift();
            
            if (event === undefined) return false;
            if (! event.timestamp - timestamp > delay) return false;
            
            timestamp = event.timestamp;
            
            return event.key;
            
        };
        
    }());
        
    return { init: init,
             next: next,
           };

}());

keymap = {
    
    81: 'q',
    87: 'w',
    69: 'e',
    82: 'r',
    84: 't',
    89: 'y',
    85: 'u',
    73: 'i',
    79: 'o',
    80: 'p',
    65: 'a',
    83: 's',
    68: 'd',
    70: 'f',
    71: 'g',
    72: 'h',
    74: 'j',
    75: 'k',
    76: 'l',
    90: 'z',
    88: 'x',
    67: 'c',
    86: 'v',
    66: 'b',
    78: 'n',
    77: 'm',
    
}
