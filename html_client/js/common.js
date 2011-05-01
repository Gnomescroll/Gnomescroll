// Common helper methods

// enforces things that should only be called once
if (Function.prototype.calledOnce === undefined) {
    Function.prototype.calledOnce = function () {
        if (this.called) {
            console.log('This is a singleton, and has already been contructed.');
            return false;
        }
        this.called = true;
        return true;
    };
}


function Arguments () {
    if (!(this instanceof arguments.callee)) {
        return new Arguments();
    }
}
Arguments.prototype = new Array();
// converts arguments to an array
if (Arguments.prototype.toArray === undefined) {
    Arguments.prototype.toArray = function (args, start_index, slice_count) {
        return Array().slice.call(args, start_index, slice_count);
    };
}


/* Extended Object: the Dict */
/* Has methods similar to python dictionaries */

function Dict () {
    if (!(this instanceof arguments.callee)) {
        return new Dict();
    }
}
Dict.prototype = new Object();
// Same function as python dict's setDefault method
// obj.setDefault(key [, default]);
// If key exists, returns obj[key],
// Else sets obj[key] = default and returns default.
// Default defaults to null.
if (Dict.prototype.setDefault === undefined) {
    Dict.prototype.setDefault = function (key, value) {
        value = value || null;
        if (this.hasOwnProperty(key)) {
            return this[key];
        } else {
            this[key] = value;
            return value;
        }
    };
}

// returns the value if it exists, else default
// Default defaults to undefined
if (Dict.prototype.getDefault === undefined) {
    Dict.prototype.getDefault = function (key, value) {
        value = value || undefined;
        if (this.hasOwnProperty(key)) {
            return this[key];
        } else {
            return value;
        }
    };
}

// returns an object's keys as an array
if (Dict.prototype.keysArray === undefined) {
    Dict.prototype.keysArray = function () {
        var arr = [],
            n;
        for (n in this) {
            if (this.hasOwnProperty(n)) {
                arr.push(n);
            }
        }
        return arr;
    };
}

// returns an object's values as an array
if (Dict.prototype.valuesArray === undefined) {
    Dict.prototype.valuesArray = function () {
        var arr = [],
            n;
        for (n in this) {
            if (this.hasOwnProperty(n)) {
                arr.push(this[n]);
            }
        }
        return arr;
    };
}

// returns an objects key:value pairs as an array [key, value]
if (Dict.prototype.keysArray === undefined) {
    Dict.prototype.keysArray = function () {
        var arr = [],
            n;
        for (n in this) {
            if (this.hasOwnProperty(n)) {
                arr.push([n, this[n]]);
            }
        }
        return arr;
    };
}
