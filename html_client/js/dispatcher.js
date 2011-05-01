// event dispatcher (not for DOM events, use jQuery for that)
var dispatcher = {

    listeners : new Dict(),

    // bind callback to event_name event.
    // Context is optional; callback will be called with that context if provided.
    listen : function (event_name, callback, context) {
        if (typeof event_name !== 'string' || typeof callback !== 'function') {
            console.log('Attempt to listen to listen to dispatcher, but bad arguments.');
            return false;
        }
        var listeners = this.listeners.setDefault(event_name, []);
        if (callback.LISTENING_TO === undefined) {
            callback.LISTENING_TO = {};
        }
        callback.LISTENING_TO[event_name] = listeners.length;
        if (context !== undefined) {
            listeners.push({ callback: callback, context: context });
        } else {
            listeners.push(callback);
        }
        return true;
    },

    // removes listeners bound to event_name
    // if callback is provided, it remove only that callback from the event_name
    // Note: callback must be an instance of the original function passed
    unlisten : function (event_name, callback) {
        if (callback !== undefined) {
            delete this.listeners[event_name];
        } else {
            this.listeners[event_name].splice(callback.LISTENING_TO[event_name], 1);
        }
    },

    // trigger events listening to event_name, and removes them
    // additional arguments may be passed on to the callback
    // the event_name is always passed as the first argument to a callback
    trigger : function (event_name) {
        var callbacks = this.listeners.getDefault(event_name, []),
            len = callbacks.length,
            i,
            callback,
            args = Arguments().toArray(arguments);

        for (i=0; i < len; i++) {
            callback = callbacks[i];
            if (typeof callback === 'function') {
                context = this;
            } else if (typeof callback === 'object') {
                callback = callback.callback;
                context = callback.context;
            } else {
                continue;
            }
            callback.apply(context, args);
        }
        delete this.listeners[event_name];
    },
};
