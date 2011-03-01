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

process.info = {}
    
// world_id, client_id, x_size, y_size, z_level, map
process.info.terrain_map = function (msg) {
        
    if (!validate.terrain_map(msg)) return;
    
    msg.x_size = parseInt(msg.x_size);
    msg.y_size = parseInt(msg.y_size);
    msg.z_level = parseInt(msg.z_level);
    
    delete msg.world_id;
    delete msg.client_id;
    delete msg.msg;
    
    state.updateLevel(msg);
    
};

process.info.agent_info = function (msg) {
    
    console.log(msg);
    
    //if(!validate.agent_info(msg)) return;
    
    var value = msg.value;
    
    msg.x = value.pos[1];
    msg.y = value.pos[2];
    msg.z = value.pos[3];
    msg.loc_type = value.pos[0];
    
    msg.obj_type = 'agent';
    
    delete msg.msg;
    delete msg.client_id;
    
    // unpack value
    $.each(value, function (key, val) {
        msg[key] = val;
    }
    
    var agent = state.gameObjectKnown(msg);
    if agent { // update
        agent.update(msg);
    } else { // create
        var agent = Agent.create(msg);
    }
    
    agent.toState();
    
}

process.info.object_info = function (msg) {
    
    console.log(msg);
    
    //if (!validate.object_list(msg)) return;
    
    var value = msg.value;
    
    msg.x = value.pos[1];
    msg.y = value.pos[2];
    msg.z = value.pos[3];
    msg.loc_type = value.pos[0];
    
    // unpack value
    $.each(value, function (key, val) {
        msg[key] = val;
    }
    
    // if the server does not distinguish containers from objects,
    // need to detect that here
    msg.obj_type = 'object';
    
    delete msg.msg;
    delete msg.client_id;
    
    var obj = state.gameObjectKnown(msg);
    if obj { // update
        obj.update(msg);
    } else { // create
        var obj = obj.create(msg);
    }
    
    obj.toState();
    
}

process.info.agent_list = function (msg) {
    
    console.log(msg);
    
    //if (!validate.agent_list(msg)) return;
    
    $.each(msg.list, function (i, list_agent) {
        var agent = state.gameObjectKnown(list_agent);
        if agent { // update
            agent.update(list_agent);
        } else { // create
            var agent = Agent.create(list_agent);
        }
        
        agent.toState();
    });
    
    delete msg.list;
    delete msg.msg;
    delete msg.client_id;
    delete msg.world_id;

}

process.info.object_list = function (msg) {
    
    console.log(msg);
    
    //if (!validate.object_list(msg)) return;
    
    $.each(msg.list, function(i, list_obj) {
        var obj = state.gameObjectKnown(list_obj);
        if obj { // update
            obj.update(list_obj);
        } else { // create
            var obj = obj.create(list_obj);
        }
        
        obj.toState();
    });
    
    delete msg.list;
    delete msg.msg;
    delete msg.client_id;
    delete msg.world_id;
}

process.delta.agent_position_change = function (msg) {
    
    console.log(msg);
    
    //if (!validate.agent_list(msg)) return;
};

process.delta.agent_state_change = function (msg) {
    
    console.log(msg);
    
    //if (!validate.agent_list(msg)) return;
    
}

process.delta.agent_create = function (msg) {
    
    console.log(msg);
    
    //if (!validate.agent_list(msg)) return;
    
}

process.delta.agent_delete = function (msg) {
    
    console.log(msg);
    
    //if (!validate.agent_list(msg)) return;
    
}

process.delta.object_position_change = function (msg) {
    
        console.log(msg);
    
    //if (!validate.agent_list(msg)) return;
}

process.delta.object_state_change = function (msg) {
        console.log(msg);
    
    //if (!validate.agent_list(msg)) return;
}

process.delta.object_create = function (msg) {
        console.log(msg);
    
    //if (!validate.agent_list(msg)) return;
}

process.delta.object_delete = function (msg) {
        console.log(msg);
    
    //if (!validate.agent_list(msg)) return;
}

process.delta.set_terrain_map = function (msg) {
        console.log(msg);
    
    //if (!validate.agent_list(msg)) return;
}

route = {
    
    terrain_map: process.info.terrain_map,
    agent_info: process.info.agent_info,
    object_info: process.info.object_info,
    agent_list: process.info.agent_list,
    object_list: process.info.object_list,
    
    agent_position_change: process.delta.agent_position_change,
    agent_state_change: process.delta.agent_state_change,
    agent_create: process.delta.agent_create,
    agent_delete: process.delta.agent_delete,

    object_position_change = process.delta.object_position_change,
    object_state_change = process.delta.object_state_change,
    object_create = process.delta.object_create,
    object_delete = process.delta.object_delete,

    set_terrain_map = process.delta.set_terrain_map,
    
};
