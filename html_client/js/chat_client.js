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
        var pm;
        if (typeof msg === 'object') {
            pm = msg.pm;
            msg = msg.msg;
        } else if (typeof msg !== 'string') {
            return;
        }
        if (msg.length > this.max_msg_length) {
            console.log('message too long');
            return;
        }
        data = {
            cmd: 'chat',
            msg: 'chat',
            content: msg,
            client_id: globals.client_id,
            player_id: globals.player_id,
            world_id: globals.world_id,
            name: $('#name').val(),
        };
        if (pm) {
            data.pm = pm;
        }
        $.post(globals.server_out + globals.api_path, { json: JSON.stringify(data) });
        return true;
    },

    pm : function (msg, client_id) {
        msg = { msg: msg };
        msg.pm = client_id;
        this.send(msg);
    },

    max_msg_length : 500,

    received : {}, // used to filter out mistaken duplicate msgs

    add_msg : function (msg, isNew, system) {
        if (!system) {
            if (blacklist.on(msg)) {
                return;
            }
            if (this.received.hasOwnProperty(msg.id)) {
                return;
            }
        }
        if (isNew) {
            chat_history.add(msg);
        }
        var name_class = 'name',
            name_container;
        if (msg.client_id === localStorage.client_id) {
            name_class = 'you';
        }
        if (msg.name === 'system') {
            name_container = '<span class="name system"><strong>'+ msg.name +'</strong></span>';
        } else if (name_class === 'you') {
            name_container = '<span class="name you">'+ msg.name +'</span>'
        } else {
            name_container = '<a class="'+name_class+'" client_id="'+msg.client_id+'" href="">'+msg.name +'</a>';
        }
        $('#messages').append($('<div></div>').attr($.extend({'class': 'message'}, msg)).html(name_container+': <span class="content">'+msg.content+'</span>'));
    },
}

var chat_history = {

    _history : (function () {
        localStorage.history = localStorage.history || JSON.stringify([]);
        return $.parseJSON(localStorage.history);
    }()),

    last_save : (function () {
        var ht = localStorage.history_timestamp;
        if (ht === undefined) return new Date();
        return ht;
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
        localStorage.history_timestamp = new Date().toString();
    },

    del : function () {
        delete localStorage.history;
    },

    load : function () {
        var hist = this._history,
            len = hist.length,
            i,
            item;

        for (i=len-1; i >= 0; i--) {
            item = hist[i];
            chat.add_msg(item, false);
        }
        if (localStorage.history_timestamp !== undefined) {
            chat.add_msg({
                'name'    : 'system',
                'content' : 'Chat history loaded from '+ localStorage.history_timestamp
            }, false, true);
        }
        $('#messages').scrollTop(100000);
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
    chat.add_msg(msg, true);
};

chat_history.load();

$('#submit').click(function (event) {
    event.preventDefault();
    var s = $(this),
        client_id = s.attr('client_id'),
        input = $('#input');
    if (input.val()) {
        if (client_id !== '') {
            chat.pm(input.val(), client_id);
        } else {
            chat.send(input.val());
        }
    }
    s.attr({
        value : 'Send',
        client_id : ''
    });
    input.val('');
    $('#cancel_pm').remove();
});

$('#input').attr({
    'maxlength': chat.max_msg_length
});

$('#name').attr({
    value : localStorage.name || ''
}).blur(function() {
    localStorage.name = $(this).val();
});

$('a.name').live('click', function (event) {
    event.preventDefault();
    var s = $(this),
        client_id = s.attr('client_id'),
        name = s.text();
    $('#submit').attr({
        value : 'Send PM to '+name,
        client_id : client_id
    });
    $('#controls').append($('<a id="cancel_pm"></a>').html('Cancel PM'));
    return false;
});

$('#cancel_pm').live('click', function (event) {
    event.preventDefault();
    $('#submit').triggerHandler('click');
    return false;
});

$(window).unload(function () {
    blacklist.save();
    chat_history.save();
});
