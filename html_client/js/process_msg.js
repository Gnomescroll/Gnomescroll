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
    //if (parseInt(msg.update, 10)) { // request updates
        //game.update();
    //}
    //globals.session_id = msg.session_id;
    //globals.new_client = false;
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

    dispatcher.trigger('info_tileset');

    game.update2();
    //store this; contains tile rendering information 
    //msg.tile_rendering_dict

    //store this; contains tile meta information
    //msg.tile_dict
    
    // take tileset msg and process/copy to a native format
    // suitable for drawing.
    // ideally it is a map from object_type identifiers to drawing properties
    // these drawing properties should be easily checked by the tilecache
};

// world_id, client_id, x_size, y_size, z_level, map
process.info.terrain_map = function (msg) {
        //console.log(msg);
    if (!validate.terrain_map(msg)) return;
    
    msg.x_size = parseInt(msg.x_size, 10);
    msg.y_size = parseInt(msg.y_size, 10);
    msg.z_level = parseInt(msg.z_level, 10);
    
    delete msg.world_id;
    delete msg.client_id;
    delete msg.msg;

    dispatcher.trigger('info_terrain_map');
    
    if (state.updateLevel(msg)) {
        board.event.terrain_map(msg);
    }
};

process.info.agent_info = function (msg) {
    //if(!validate.agent_info(msg)) return;
    
    var value = msg.value,
        agent;
    
    msg.x = value.position[1];
    msg.y = value.position[2];
    msg.z = value.position[3];
    msg.loc_type = value.position[0];
    
    msg.obj_type = 'agent';
    
    delete msg.msg;
    delete msg.client_id;
    
    // merge msg.value with msg
    $.extend(msg, value);

    dispatcher.trigger('info_agent_info');
    
    agent = state.gameObjectKnown(msg);
    if (agent) {            // update
        agent.update(msg);
    } else {                // create
        agent = Agent.create(msg);
    }
    agent.toState();
};

process.info.object_info = function (msg) {
    //if (!validate.object_list(msg)) return;
    
    var value = msg.value;
    
    msg.x = value.position[1];
    msg.y = value.position[2];
    msg.z = value.position[3];
    msg.loc_type = value.position[0];

    // merge msg.value with msg
    $.extend(msg, value);
    
    // if the server does not distinguish containers from objects,
    // need to detect that here
    msg.obj_type = 'obj';
    
    delete msg.position;
    delete msg.msg;
    delete msg.client_id;

    dispatcher.trigger('info_object_info');
    
    var obj = state.gameObjectKnown(msg);
    if (obj) {              // update
        obj.update(msg);
    } else {                // create
        var obj = Obj.create(msg);
    }
    obj.toState();
};

process.info.agent_list = function (msg) {
    //if (!validate.agent_list(msg)) return;
    var agent,
        msg_len = msg.list.length,
        i = 0,
        list_agent;

    dispatcher.trigger('info_agent_list');
        
    for(i=0; i < msg_len; i++) {
        list_agent = msg.list[i];

        list_agent.x = list_agent.position[1];
        list_agent.y = list_agent.position[2];
        list_agent.z = list_agent.position[3];
        list_agent.loc_type = list_agent.position[0];
        delete list_agent.position;
        
        agent = state.gameObjectKnown(list_agent, 'agent');
        if (agent) {                    // update
            agent.update(list_agent);
        } else {                        // create
            agent = Agent.create(list_agent);
        }
        agent.toState();
    }
    delete state.requests_waiting.agents;
    state.check_loaded();
};

process.info.object_list = function (msg) {
    //if (!validate.object_list(msg)) return;
    var obj,
        msg_len = msg.list.length,
        i = 0,
        list_obj;

    dispatcher.trigger('info_object_list');
        
    for(i=0; i < msg_len; i++) {
        list_obj = msg.list[i];
        
        list_obj.x = list_obj.position[1];
        list_obj.y = list_obj.position[2];
        list_obj.z = list_obj.position[3];
        list_obj.loc_type = list_obj.position[0];
        delete list_obj.position;
        
        obj = state.gameObjectKnown(list_obj, 'obj');
        if (obj) {                  // update
            obj.update(list_obj);
        } else {                    // create
            obj = Obj.create(list_obj);
        }
        
        obj.toState();
    }
    delete state.requests_waiting.objects;
    state.check_loaded();
};

process.delta = {};

process.delta.agent_position_change = function (msg) {
    //if (!validate.agent_position_change(msg)) return;
    
    msg.x = msg.position[1];
    msg.y = msg.position[2];
    msg.z = msg.position[3];
    msg.loc_type = msg.position[0];
    delete msg.position;
    
    var agent = state.gameObjectKnown(msg.id, 'agent');
    
    if (agent) {
        agent.update(msg);
        agent.toState();

    } else if (state.contains(GameObject.pos.apply(msg))) {
        agent = Agent.create(msg);
        agent.toState();

    }

    //callback
    agent = state.gameObjectKnown(msg.id, 'agent');
    
    if(agent == false) {
        console.log("process.delta.agent_position_change : WTF, should not occur")
        console.log(agent)
    } else {
        board.event.agent_change(agent);
    }

};

process.delta.agent_state_change = function (msg) {
    //if (!validate.agent_state_change(msg)) return;
    
    msg.x = msg.position[1];
    msg.y = msg.position[2];
    msg.z = msg.position[3];
    msg.loc_type = msg.position[0];
    delete msg.position;
    
    var agent = state.gameObjectKnown(msg.id, 'agent');
    
    if (agent) {
        agent.update(msg);
        agent.toState();
    } else if (state.contains(GameObject.pos.apply(msg))) {
        agent = Agent.create(msg);
        agent.toState();
    }
    
};

process.delta.agent_create = function (msg) {
    //if (!validate.agent_create(msg)) return;
    var agent;
    
    msg.x = msg.position[1];
    msg.y = msg.position[2];
    msg.z = msg.position[3];
    msg.loc_type = msg.position[0];
    delete msg.position;
    msg.type = ['agent'];
    if (state.contains(GameObject.pos.apply(msg))) {
        agent = Agent.create(msg);
        agent.toState();
    }
    
};

process.delta.agent_delete = function (msg) {
    //if (!validate.agent_delete(msg)) return;
    
    var agent = state.gameObjectKnown(msg);
    if (agent) {
        agent.remove();
    }
};

process.delta.object_position_change = function (msg) {
    //if (!validate.object_position_change(msg)) return;
    
    msg.x = msg.position[1];
    msg.y = msg.position[2];
    msg.z = msg.position[3];
    msg.loc_type = msg.position[0];
    delete msg.position;
    
    var obj = state.gameObjectKnown(msg.id, 'obj');
    
    if (obj) {
        obj.update(msg);
        obj.toState();
    } else if (state.contains(GameObject.pos.apply(msg))) {
        obj = Obj.create(msg);
        obj.toState();
    }
};

process.delta.object_state_change = function (msg) {
    //if (!validate.object_state_change(msg)) return;
    
    msg.x = msg.position[1];
    msg.y = msg.position[2];
    msg.z = msg.position[3];
    msg.loc_type = msg.position[0];
    delete msg.position;
    
    var obj = state.gameObjectKnown(msg.id, 'obj');
    
    if (obj) {
        obj.update(msg);
        obj.toState();
    } else if (state.contains(GameObject.pos.apply(msg))) {
        obj = Obj.create(msg);
        obj.toState();
    }
    
};

process.delta.object_create = function (msg) {
    //if (!validate.object_create(msg)) return;
    var obj;
    
    msg.x = msg.position[1];
    msg.y = msg.position[2];
    msg.z = msg.position[3];
    msg.loc_type = msg.position[0];
    delete msg.position;
    
    if (state.contains(GameObject.pos.apply(msg))) {
        obj = Obj.create(msg);
        obj.toState();
    }
};

process.delta.object_delete = function (msg) {
    //if (!validate.object_delete(msg)) return;
    
    var obj = state.gameObjectKnown(msg);
    if (obj) {
        obj.remove();
    }
};

process.delta.set_terrain_map = function (msg) {
    //if (!validate.set_terrain_map(msg)) return;
    var block;
    if (state.contains(GameObject.pos.apply(msg))) {
        block = state.updateBlock(msg);
        if (block !== false) {
            board.event.terrain_map_change(block);
        }
    }

};

route = {

    register: process.register,
    
    tileset: process.info.tileset,
    terrain_map: process.info.terrain_map,
    agent_info: process.info.agent_info,
    object_info: process.info.object_info,
    agent_list: process.info.agent_list,
    object_list: process.info.object_list,
    
    agent_position_change: process.delta.agent_position_change,
    agent_state_change_update: process.delta.agent_state_change,
    agent_create: process.delta.agent_create,
    agent_delete: process.delta.agent_delete,

    object_position_change: process.delta.object_position_change,
    object_state_change: process.delta.object_state_change,
    object_create: process.delta.object_create,
    object_delete: process.delta.object_delete,

    set_terrain_map: process.delta.set_terrain_map
    
};
