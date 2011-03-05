var send, admin_out, agent_out, info_out;

send = function () {
    
    var url = '/api';
    this = JSON.stringify(this);
    $.post(globals.ajax_server+url, {json: this});
};


admin_out = {

    // required = ['position'];
    // optional = ['player_id'];
    create_agent: function (x, y, z, player_id) {
        //create msg to be sent to server;
        msg = {};
        //specify command type and command;
        msg.type = 'admin';
        msg.cmd = 'create_agent';
        //command parameters;
        msg.world_id = globals.world_id;
        msg.position = (0,x,y,z) 
        msg.player_id = player_id;
        //send message to server;
        return msg;
    },

    //Not implemented yet;
    create_item: function (x, y, z) {
        msg = {};
        msg.type = 'admin';
        msg.cmd = 'create_item';
        msg.world_id = globals.world_id;
        msg.position = (0,x,y,z) 
        return msg;
    },

    create_object: function (x, y, z, object_type, template) {
        msg = {};
        msg.type = 'admin';
        msg.cmd = 'create_object';
        msg.object_type = object_type;
        msg.template = template;
        msg.world_id = globals.world_id;
        msg.position = (0,x,y,z) 
        return msg;
    },

    create_crop: function (x, y, z, template) {
        this.create_object(x,y,z, 'crop', template);
    },

    //required ['position', 'value', 'world_id'];
    //optional [];
    set_map: function (x, y, z, value) {
        msg = {};
        msg.type = 'admin';
        msg.cmd = 'set_map';
        msg.world_id = globals.world_id;
        msg.position = (0,x,y,z) 
        msg.value = value;
        return msg;
    },

info_out = {

    //required = ['z', 'client_id', 'world_id'];
    //optional = ['z'];
    get_map: function (world_id, z) {
        msg = {};
        msg.type = 'info';
        msg.cmd = 'get_map';
        msg.world_id = world_id;
        msg.z = z;
        msg.client_id = globals.client_id;
        return msg;
    },

    //required = ['agent_id', 'client_id', 'world_id'];
    //optional = []    ;
    get_agent: function (id) {
        msg = {};
        msg.type = 'info';
        msg.cmd = 'get_agent';
        msg.world_id = globals.world_id;
        msg.client_id = globals.client_id;
        msg.agent_id = id;
        return msg;
    },

    //required = ['object_id', 'client_id', 'world_id'];
    //optional = []    ;
    get_object: function (id) {
        msg = {};
        msg.type = 'info';
        msg.cmd = 'get_object';
        msg.world_id = globals.world_id;
        msg.client_id = globals.client_id;
        msg.object_id = id;
        return msg;
    },

    //required = ['client_id', 'world_id'];
    //optional = [];
    get_agent_list: function (world_id) {
        msg = {};
        msg.type = 'info';
        msg.cmd = 'get_agent_list';
        msg.world_id = world_id;
        msg.client_id = globals.client_id;
        return msg;
    },

    //required = ['client_id', 'world_id'];
    //optional = [];
    get_object_list: function (world_id) {
        msg = {};
        msg.type = 'info';
        msg.cmd = 'get_object_list';
        msg.world_id = world_id;
        msg.client_id = globals.client_id;
        return msg;
    },
};

agent_out = {

    // required = ['dp', 'agent_id'];
    // optional = ['player_id'];
    move_0: function (agent_id, dx, dy, dz) {
        msg = {};
        msg.type = 'agent';
        msg.cmd = 'move_0';
        msg.world_id = globals.world_id;
        msg.agent_id = agent_id;
        msg.dp = (dx,dy,dz) 
        return msg;
    },

    //required = ['client_id', 'world_id'];
    //optional = [];
    till_soil: function (agent_id) {
        msg = {};
        msg.type = 'agent';
        msg.cmd = 'till_soil';
        msg.world_id = globals.world_id;
        msg.agent_id = agent_id;
        return msg;
    },

    //required = ['client_id', 'world_id'];
    //optional = [];
    plant_crop: function (agent_id) {
        msg = {};
        msg.type = 'agent';
        msg.cmd = 'plant_crop';
        msg.world_id = globals.world_id;
        msg.agent_id = agent_id;
        return msg;
    },

    harvest_crop: function (agent_id, crop_id) {
        msg = {};
        msg.type = 'agent';
        msg.cmd = 'harvest_crop';
        msg.world_id = globals.world_id;
        msg.agent_id = agent_id;
        msg.crop_id = crop_id;
        return msg;
    },
};
