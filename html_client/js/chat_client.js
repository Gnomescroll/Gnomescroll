$('#submit').click(function (event) {
    event.preventDefault();
    var input = $('#input');
    if (input.val()) {
        chat.send(input.val());
    }
    input.val('');
});

$('#input').attr({
    'maxlength': chat.max_msg_length
});

$('#name').attr({
    value : localStorage.name || ''
}).blur(function (event) {
    localStorage.name = $(this).val();
});

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
    server_out : '',
});

var chat = {
    send : function (msg) {
        if (typeof msg !== 'string') {
            return;
        }
        if (msg.length > this.max_msg_length) {
            console.log('message too long');
            return;
        }
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

    max_msg_length : 500,

    received : {}, // used to filter out mistaken duplicate msgs

    add_to_panel : function (msg) {
        if (blacklist.on(msg)) {
            return;
        }
        if (this.received.hasOwnProperty(msg.id)) {
            return;
        }
        chat_history.add(msg);
        $('#messages').append($('<div></div>').attr($.extend({'class': 'message'}, msg)).html('<span class="name">'+msg.name +'<span>: <span class="content">'+msg.content+'</span>'));
    },
    
}

var chat_history = {

    _history : (function () {
        localStorage.history = localStorage.history || JSON.stringify([]);
        return $.parseJSON(localStorage.history);
    }()),
    
    _max : 200,

    add : function (msg) {
        var removed,
            messages = $('#messages').children('.message');
        this._history.unshift(msg);
        while (this._history.length > this._max) {
            removed = this._history.pop();
            messages.filter('[id="'+removed.id+'"]').remove();
        }
    },

    clear : function () {
        this._history = [];
    },

    save : function () {
        localStorage.history = JSON.stringify(this._history);
    },

    del : function () {
        delete localStorage.history;
    },
};

var blacklist = {

    _blacklist : (function () {
        localStorage.blacklist = localStorage.blacklist || JSON.stringify({});
        return $.parseJSON(localStorage.blacklist);
    }()),

    on : function (msg) {
        if (this._blacklist.hasOwnProperty(msg.player_id)) {
            return true;
        }
        return false;
    },

    save : function () {
        localStorage.blacklist = JSON.stringify(this._blacklist);
    },

    del : function () {
        this._blacklist = {};
        delete localStorage.blacklist;
    },

    add : function (player_id, info) {
        this._blacklist[player_id] = info || '';
        this.save();
    },

    remove : function (player_id) {
        delete this._blacklist(player_id);
        this.save();
    },

};

var route = {};
route.chat = function (msg) {
    console.log('received msg');
    chat.add_to_panel(msg);
};
