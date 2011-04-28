// initialization list:
// load:
//  tileset
//  map state
//  agents
//  objects

// manages controls for test/init/start/reset button
var controls = {

    defaults : {}, // control default classes

    _init_defaults : function () {
        if (!arguments.callee.loaded) {
            var d = {
                    init  : $('#init'),
                    start : $('#start'),
                    update: $('#update'),
                    reset : $('#reset')
                },
                name;
            
            for (name in d) {
                if (!d.hasOwnProperty(name)) continue;
                this.defaults[name] = d[name].attr('class');
            }
            arguments.callee.loaded = true;
        }
    },

    _init_event : function (event) {
        var selector = $(this).attr('id');
        if (selector !== undefined) {
            selector = this;
        } else {
            selector = '#init';
        }
        if ($(selector).attr('class').match(/ready/) !== null) {
            game.init(controls._initialized);
        }
        if (typeof event === 'function') { // callback
            var args = [],
                len = arguments.length,
                i = 1;
            for (i=1; i < len; i++) {
                args[i-1] = arguments[i];
            }
            event.apply(this, args);
        }
        return false;
    },

    _start_event : function (event) {
        var selector = $(this).attr('id');
        if (selector !== undefined) {
            selector = this;
        } else {
            selector = '#start';
        }
        if ($(selector).attr('class').match(/ready/) !== null) {
            board.start();
            $('canvas#board').attr({'class': 'tip click', 'title': 'Game board'});        
            map_editor.init();
            options.init();
            //benchmark();
            game.start(controls._started);
        }
        if (typeof event === 'function') { // callback
            var args = [],
                len = arguments.length,
                i = 1;
            for (i=1; i < len; i++) {
                args[i-1] = arguments[i];
            }
            event.apply(this, args);
        }
        return false;
    },

    _update_event : function (event) {   // not tested
        var selector = $(this).attr('id');
        if (selector !== undefined) {
            selector = this;
        } else {
            selector = '#update';
        }
        if ($(selector).attr('class').match(/ready/) !== null) {
            game.update(controls._updated);
        }
        if (typeof event === 'function') { // callback
            var args = [],
                len = arguments.length,
                i = 1;
            for (i=1; i < len; i++) {
                args[i-1] = arguments[i];
            }
            event.apply(this, args);
        }
        return false;
    },

    _reset_event : function (event) {    // not implemented
        // reset map state
        var selector = $(this).attr('id');
        if (selector !== undefined) {
            selector = this;
        } else {
            selector = '#reset';
        }
        if ($(selector).attr('class').match(/ready/) !== null) {
            game.reset(controls._reset_complete);
        }
        if (typeof event === 'function') { // callback
            var args = [],
                len = arguments.length,
                i = 1;
            for (i=1; i < len; i++) {
                args[i-1] = arguments[i];
            }
            event.apply(this, args);
        }
        return false;
    },

    _test_event : function (event) {     // not implemented
        var selector = $(this).attr('id');
        if (selector !== undefined) {
            selector = this;
        } else {
            selector = '#test';
        }
        // run tests
        //tests.run(controls._tests_complete);
        if (typeof event === 'function') { // callback
            var args = [],
                len = arguments.length,
                i = 1;
            for (i=1; i < len; i++) {
                args[i-1] = arguments[i];
            }
            event.apply(this, args);
        }
        return false;
    },

    trigger_load : function () {
        this._init_event(this._start_event);
    },
    
    init$ : function () {
        this._init_defaults();
        $('#init').click(this._init_event);
        $('#start').click(this._start_event);
        $('#update').click(this._update_event);
        $('#reset').click(this._reset_event);
        $('#test').click(this._test_event);
    },

    _initialized : function () {
        var init_btn = $('#init'),
            start_btn = $('#start'),
            init_btn_cls,
            start_btn_cls;

        init_btn_cls = init_btn.attr('class').match(/ready/);
        if (init_btn_cls !== null) {
            init_btn_cls = init_btn.attr('class').replace(init_btn_cls[0], 'complete');
        } else {
            init_btn_cls = init_btn.attr('class') + ' complete';
        }
        init_btn.attr('class', init_btn_cls);

        start_btn_cls = start_btn.attr('class') + ' ready';
        start_btn.attr('class', start_btn_cls);
    },

    _started : function () {
        var start_btn = $('#start'),
            update_btn = $('#update'),
            reset_btn = $('#reset'),
            start_btn_cls,
            update_btn_cls,
            reset_btn_cls;

        start_btn_cls = start_btn.attr('class').match(/ready/);
        if (start_btn_cls !== null) {
            start_btn_cls = start_btn.attr('class').replace(start_btn_cls[0], 'complete');
        } else {
            start_btn_cls = start_btn.attr('class') + ' complete';
        }
        start_btn.attr('class', start_btn_cls);

        update_btn_cls = update_btn.attr('class') + ' ready';
        update_btn.attr('class', update_btn_cls);

        reset_btn_cls = reset_btn.attr('class') + ' ready';
        reset_btn.attr('class', reset_btn_cls);
    },

    _updated : function () {
    },

    _reset_complete : function () {
    },

    _tests_complete : function () {
    },

    reset : function () {
        var name;
        for (name in this.defaults) {
            if (!this.defaults.hasOwnProperty(name)) continue;
            $('#'+name).attr('class', this.defaults[name]);
        }
    },
};
