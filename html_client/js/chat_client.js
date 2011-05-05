$('#submit').click(function (event) {
    event.preventDefault();
    var input = $('#input');
    if (input.val()) {
        chat.send(input.val());
    }
    input.val('');
});

$('#input').attr('maxlength', chat.max_msg_length);

var globals = {

    _generate_id : function () {    // generate random 16 char string
        var chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXTZabcdefghiklmnopqrstuvwxyz",
            num_chars = chars.length,
            string_length = 16,
            randomstring = '',
            i = 0,
            rnum;
        for (i=0; i<string_length; i++) {
            rnum = Math.floor(Math.random() * num_chars);
            randomstring += chars.charAt(rnum);
        }
        return randomstring;
    },

    create_client_id : function () {
        this.client_id = this._generate_id();
        localStorage.client_id = this.client_id;
        this.new_client = true;
    },
    
    update : function (params) {
        this.world_id   = typeof parseInt(params.world_id, 10)  === 'number' ? params.world_id  : this.world_id;
        this.client_id  = typeof parseInt(params.client_id, 10) === 'number' ? params.client_id : this.client_id;
        this.server_out = typeof params.server_out !== undefined ? params.server_out : this.server_out;
    },
};

$.extend(globals, {
    world_id   : 0,
    new_client : !!localStorage.client_id,
    client_id  : localStorage.client_id || globals.create_client_id(),
    player_id  : 0,
    session_id : null,
    api_path : '/chat_api',
    server_out : '',//'http://127.0.0.1:8082',
});

var chat = {
    send : function (msg) {
        data = JSON.stringify({
            cmd: 'chat',
            msg: 'chat',
            content: msg,
            client_id: globals.client_id,
            player_id: globals.player_id,
            world_id: globals.world_id,
            name: $('#name').val(),
        });
        $.post(globals.server_out + globals.api_path, { json: data });
        return true;
    },

    history : [],
    max_history : 200,

    max_msg_length : 500,

    blacklist : (function () {
        localStorage.blacklist = localStorage.blacklist || {};
        return localStorage.blacklist;
    }()),

    
    
}

var route = {};
route.chat = function (msg) {
    console.log('received msg');
    $('#messages').append($('<div></div>').attr('class', 'message').html(msg.name +': '+msg.content));
};
