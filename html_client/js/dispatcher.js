// event dispatcher (not for DOM events, use jQuery for that)
var dispatcher = {

    listeners : new Dict(),

    // bind callback to event_name event.
    // Count is optional; defaults to Infinity
    // Context is optional; callback will be called with that context if provided.
    // If you wish to pass context but no count, this method will detect that, and count will default to Infinity
    listen : function (event_name, callback, count, context) {
        if (typeof event_name !== 'string' || typeof callback !== 'function') {
            console.log('Attempt to listen to listen to dispatcher, but bad arguments.');
            return false;
        }
        var listeners = this.listeners.setDefault(event_name, []);
        if (callback.LISTENING_TO === undefined) {
            callback.LISTENING_TO = new Dict();
        }
        callback.LISTENING_TO.setDefault(event_name, []).push(listeners.length);
        if (arguments.length === 3 && !(!isNaN(parseInt(count, 10)) || count === Infinity)) {
            context = count;
            count = undefined;
        }
        count = count || Infinity;
        listeners.push({
            callback: callback,
            count: count,
            context: context
        });
        console.log('added listener');
        console.log(listeners);
        return true;
    },

    // removes listeners bound to event_name
    // if callback is provided, it remove only that callback from the event_name
    // Note: callback must be an instance of the original function passed
    unlisten : function (event_name, callback) {
        var listener_id,
            i,
            len;
        if (callback !== undefined) {
            delete this.listeners[event_name];
        } else {
            len = callback.LISTENING_TO[event_name].length;
            for (i=0; i < len; i++) {
                listener_id = callback.LISTENING_TO[event_name][i];
                this.listeners[event_name][listener_id] = null;
            }
        }
    },

    // trigger events listening to event_name, and removes them
    // additional arguments will be passed on to the callbacks
    // the event_name is always passed as the first argument to a callback
    trigger : function (event_name) {
        console.log(event_name + ' triggered');
        var callbacks = this.listeners.getDefault(event_name, []),
            len = callbacks.length,
            i,
            callback,
            args = Arguments().toArray(arguments);

        for (i=0; i < len; i++) {
            callback = callbacks[i];
            try {
                if (callback.count-- <= 0) {
                    callbacks[i] = callback = null;
                }
            } catch (e) {
                callbacks[i] = callback = null;
            }
            if (callback === null) continue;
            context = callback.context || this;
            callback = callback.callback;
            callback.apply(context, args);
            console.log('triggered event');
        }
    },
};
