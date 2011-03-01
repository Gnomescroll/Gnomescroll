var globals = {
    
    client_id: 0,
    world_id: 0,

};

var locationStateMap = {
    
    0: 'ground',
    1: 'agent',
    2: 'container',
    
};

var route, process, validate;


process = {};
    
// world_id, client_id, x_size, y_size, z_level, map
process.terrain_map = function (msg) {
        
    if (!validate.terrain_map(msg)) return;
    
    msg.x_size = parseInt(msg.x_size);
    msg.y_size = parseInt(msg.y_size);
    msg.z_level = parseInt(msg.z_level);
    
    state.updateLevel(msg);
    
};

route = {
    
    terrain_map: process.terrain_map,
    
};
