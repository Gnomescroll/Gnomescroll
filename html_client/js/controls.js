// initialization list:
// load:
//  tileset
//  map state
//  agents
//  objects

// manages controls for test/init/start/reset button
var controls = new function Controls () {

    if (!Function().calledOnce.call(arguments.callee)) { // enforce single instance
        throw { message: 'Attempted to create more than one instance of Controls.' };
    }

    /* public */
    
    this.init = function () { // bind callbacks to button events
        this._init_defaults();
        $('#init').click(this._init_event);
        $('#start').click(this._start_event);
        $('#update').click(this._update_event);
        $('#reset').click(this._reset_event);
        $('#test').click(this._test_event);
    };
    
    this.reset = function () {  // reset all the buttons
        for (var name in this._defaults) {   // set default classes on controls
            if (!this._defaults.hasOwnProperty(name)) continue;
            $('#'+name).attr('class', this._defaults[name]);
        }
    };

    this.trigger_load = function () { // starts chain of init/start
        this._init_event();
    };

    /* private */

    this._defaults = {}, // control default classes

    this._init_defaults = function () { // initializes default classes (only once)
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
                this._defaults[name] = d[name].attr('class');
            }
            arguments.callee.loaded = true;
        }
    };

    /* DOM event callbacks */

    function _get_selector () {
        var selector = $(this).attr('id');
        if (selector !== undefined && $(this).parent().attr('class') === 'main_button') {
            selector = this;
        } else {
            selector = false;
        }
        return selector;
    }

    function _event_wrapper (alt_selector, fn, fn_args) {
        fn = fn || function () {};
        return function (event) {
            var selector = _get_selector.call(this) || alt_selector,
                elem_cls,
                args;
            selector = $(selector);
            elem_cls = selector.attr('class');
            if (elem_cls.match(/ready/) !== null) {
                selector.attr('class', elem_cls.replace(/ready/, 'processing'));
                fn.apply(this, fn_args);
            }
            if (typeof event === 'function') { // callback
                args = Arguments().toArray(arguments, 1);
                event.apply(this, args);
            }
            return false;
        };
    }

    this._init_event = _event_wrapper('#init', function () {
        dispatcher.listen('game_init', function () {
            controls._initialized();
        }, 1);
        if (!_get_selector.call(this)) {
            dispatcher.listen('state_loaded', function () {
                controls._start_event();
            }, 1);
        }
        game.init();
    });

    this._start_event = _event_wrapper('#start', function () {
        dispatcher.listen('game_start', function () {
            controls._started();
        }, 1);
        game.start();
    });

    this._update_event = _event_wrapper('#update',  function () {  // not tested
        dispatcher.listen('state_loaded', function () {
            controls._updated();
        }, 1);
        state.init();
    });

    this._reset_event = _event_wrapper('#reset', function () {
        dispatcher.listen('game_reset', function () {
            controls._reset_complete();
        }, 1);
        game.reset();
    });

    this._test_event = _event_wrapper('#test', function () {
        //dispatcher.listen('test', function () {
            // not implemented
        //}, 1);
        //tests.run();
    });

    /* DOM event callback callbacks
     * i.e. callback triggered after dom event callback is 100% complete */

    function complete_ready(selcompl, selready) { // for buttons that go from processing -> complete and cause other buttons to become ready
        return function (callback) {
            var btn = $(selcompl),
                cls = btn.attr('class'),
                cls_match = cls.match(/processing/),
                i,
                len = selready.length;

            if (cls_match !== null) {
                cls = cls.replace(cls_match[0], 'complete');
            } else {
                cls += ' complete';
            }
            btn.attr('class', cls);

            if (typeof selready === 'string') {
                _is_ready(selready);
            } else {
                for(i=0; i < len; i++) {
                    _is_ready(selready[i]);
                }
            }

            if (typeof callback === 'function') { // callback
                args = Arguments().toArray(arguments, 1);
                callback.apply(this, args);
            }
        };
    }

    function _is_ready(sel) {   // adds ' ready' to a class
        var btn = $(sel),
            cls = btn.attr('class') + ' ready';
        btn.attr('class', cls);
    }

    function processing2ready(selector) { // for buttons that just go from processing -> ready
        return function () {
            var btn = $(selector),
                cls = btn.attr('class');
            btn.attr('class', cls.replace(/processing/, 'ready'));
        };
    }

    this._initialized = complete_ready('#init', '#start');

    this._started = complete_ready('#start', ['#update', '#reset']);

    this._updated = processing2ready('#update');

    this._reset_complete = processing2ready('#reset');

    this._tests_complete = processing2ready('#test');
};
