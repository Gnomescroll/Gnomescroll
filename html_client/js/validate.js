var validate = ( function () {
    
    var levelData,
        blockData,
        agentData,
        objectData,
        public_;
        
    levelData = function (data) {
        
        if (typeof data !== 'object') {
            console.log('Tried passing bullshit to updateLevel. Please send a data object.');
            console.log(data);
            return false;
        }
        
        if (data.client_id !== globals.client_id) { // ignore immediately, actually move this to main message handler
            console.log('Message client_ids don\'t match. Ignoring.');
            return false;
        }
        
        // map data for a z-level
        var required = ['map', // (array of int tile values)
                        'z_level', // (int)
                        'x_size', // (int)
                        'y_size', // (int)
                        'world_id', // (int)
                       ];
               
        var missing = [];
        $.each(required, function(i, req) {
            if (data[req] === undefined) {
                missing.push(req);
            }
        });
        
        if (missing.length > 0) {
            var msg = missing.join();
            console.log('Cannot update z-level. The following data attrs are missing: '+msg.slice(0, msg.length-2));
            console.log(data);
            return false;
        }
        
        data.x_size = parseInt(data.x_size);
        data.y_size = parseInt(data.y_size);
        var map_size = data.x_size*data.y_size;
        if (map_size !== data.map.length) {
            console.log('The map dimensions do not match the map sent.');
            console.log(data);
            return false;
        }

        return true;
    };
        
    var check = function(data, req, msg) {
        
        var missing = false,
            int_val;
        
        $.each(req, function(i, val) { // check for missing and NaN
            if (data[val] === undefined) {
                console.log(msg +' is missing: '+val);
                missing = true;
                return true; //continue
            }
            int_val = parseInt(data[val]);
            if (isNaN(int_val)) {
                console.log(msg +' is NaN: '+val);
                missing = true;
                return true;
            } else {
                data[val] = int_val;
            }
        });
        
        if (missing === true) { // log data if bad, bail
            console.log(data);
        }
        
        return !missing;
    }
        
    blockData = function (data) {
        return check(data, ['type','x','y','z'], 'blockData');
    };
    
    agentData = function (data) {
        // 'loc' is the location_type (e.g. ground, agent, container)
        return check(data, ['loc','x','y','z','id'], 'agentData');        
    };
    
    objectData = function (data) {
        return check(data, ['loc','x','y','z','id'], 'objectData');
    };
    
    public_ = {
                levelData: levelData,
                blockData: blockData,
                agentData: agentData,
                objectData: objectData
              }
              
    return public_;
    
}());
