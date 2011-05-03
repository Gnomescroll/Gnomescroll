var locationStateMap = {
    0: 'ground',
    1: 'agent',
    2: 'container',
};


var route, process, validate;

process = {};

process.register = function(msg) {
    console.log('register');
    console.log(msg);
    if (msg.session_id === 'taken') {
        console.log('client_id in use');
        globals.create_client_id();
        socket.register();
        return false;
    }
    dispatcher.trigger('register', msg);
    return true;
};

process.info = {};

//todo
//current stores rendering information for tile, but discards meta information
//retain/store tile_dict somewhere    
process.info.tileset = function(msg) {
    var param,
        data,
        x,
        tile,
        tr = msg.tile_rendering,
        tp = msg.tile_properties;

    for (x in tr) {
        if (!tr.hasOwnProperty(x)) continue;
        param = tr[x];
        
        data = {
            tile_name     : param.tile_name,
            tile_id       : param.tile_id,   
            tilemap_id    : param.tilemap.tilemap_id,
            draw_style    : param.tilemap.draw_style,
            background_rgb: param.tilemap.background_rgb,
            symbol        : param.tilemap.symbol,
            symbol_rgb    : param.tilemap.symbol_rgb,
        };
            
        tileset_state.add_tile(data);
    }
    tileset_state.loaded = true;
    
    for (x in tp) {
        if (!tp.hasOwnProperty(x)) continue;
        tile = tp[x];
        tile_properties.add(tile);
    }

    dispatcher.trigger('info_tileset', msg);
    
    //store this; contains tile rendering information 
    //msg.tile_rendering_dict

    //store this; contains tile meta information
    //msg.tile_dict
    
    // take tileset msg and process/copy to a native format
    // suitable for drawing.
    // ideally it is a map from object_type identifiers to drawing properties
    // these drawing properties should be easily checked by the tilecache
};

process.info.terrain_map = function (msg) {
    if (!validate.terrain_map(msg)) return;
    
    msg.x_size = parseInt(msg.x_size, 10);
    msg.y_size = parseInt(msg.y_size, 10);
    msg.z_level = parseInt(msg.z_level, 10);
    
    delete msg.world_id;
    delete msg.client_id;
    delete msg.msg;

    dispatcher.trigger('info_terrain_map', msg);
};

process._convert_position = function (obj, val) {
    val = val || obj;
    obj.x = val.position[1];
    obj.y = val.position[2];
    obj.z = val.position[3];
    obj.loc_type = val.position[0];
    delete obj.position;
    return obj;
}

process.info._generic_object_info = function (msg, obj_type) {
    var value = msg.value;
    process._convert_position(msg, value);
    msg.obj_type = obj_type;
    delete msg.msg;
    delete msg.client_id;
    $.extend(msg, value); // merge msg.value with msg
};

process.info.agent_info = function (msg) {
    //if(!validate.agent_info(msg)) return;
    process.info._generic_object_info(msg, 'agent');
    dispatcher.trigger('info_agent_info', msg);
};

process.info.object_info = function (msg) {
    //if (!validate.object_list(msg)) return;
    process.info._generic_object_info(msg, 'obj');
    // if the server does not distinguish containers from objects,
    // need to detect that here (or change state.js)
    dispatcher.trigger('info_object_info', msg);
};

process.info._generic_list = function (msg) {
    var msg_list = msg.list,
        msg_len = msg_list.length,
        i;
    for (i=0; i < msg_len; i++) {
        process._convert_position(msg_list[i]);
    }
    return msg;
};

process.info.agent_list = function (msg) {
    //if (!validate.agent_list(msg)) return;
    msg = process.info._generic_list(msg);
    dispatcher.trigger('info_agent_list', msg);
};

process.info.object_list = function (msg) {
    //if (!validate.object_list(msg)) return;
    msg = process.info._generic_list(msg);
    dispatcher.trigger('info_object_list', msg);
};

process.delta = {};

process.delta.set_terrain_map = function (msg) {
    //if (!validate.set_terrain_map(msg)) return;
    dispatcher.trigger('set_terrain_map', msg);
};

process.delta.agent_position_change = function (msg) {
    //if (!validate.agent_position_change(msg)) return;
    process._convert_position(msg);
    dispatcher.trigger('agent_position_change', msg);
};

process.delta.agent_state_change = function (msg) {
    //if (!validate.agent_state_change(msg)) return;
    process._convert_position(msg);    
    dispatcher.trigger('agent_state_change', msg);
};

process.delta.agent_create = function (msg) {
    //if (!validate.agent_create(msg)) return;    
    process._convert_position(msg);
    msg.type = ['agent'];
    dispatcher.trigger('agent_create', msg);
};

process.delta.agent_delete = function (msg) {
    //if (!validate.agent_delete(msg)) return;
    dispatcher.trigger('agent_delete', msg);
};

process.delta.object_position_change = function (msg) {
    //if (!validate.object_position_change(msg)) return;
    process._convert_position(msg);
    dispatcher.trigger('object_position_change', msg);
};

process.delta.object_state_change = function (msg) {
    //if (!validate.object_state_change(msg)) return;
    process._convert_position(msg);
    dispatcher.trigger('object_state_change', msg);
};

process.delta.object_create = function (msg) {
    //if (!validate.object_create(msg)) return;
    process._convert_position(msg);
    dispatcher.trigger('object_create', msg);
};

process.delta.object_delete = function (msg) {
    //if (!validate.object_delete(msg)) return;
    dispatcher.trigger('object_delete', msg);
};

route = {

    register: process.register,
    
    tileset: process.info.tileset,
    terrain_map: process.info.terrain_map,
    agent_info: process.info.agent_info,
    object_info: process.info.object_info,
    agent_list: process.info.agent_list,
    object_list: process.info.object_list,

    set_terrain_map: process.delta.set_terrain_map,
    
    agent_position_change: process.delta.agent_position_change,
    agent_state_change_update: process.delta.agent_state_change,
    agent_create: process.delta.agent_create,
    agent_delete: process.delta.agent_delete,

    object_position_change: process.delta.object_position_change,
    object_state_change: process.delta.object_state_change,
    object_create: process.delta.object_create,
    object_delete: process.delta.object_delete,
    
};
