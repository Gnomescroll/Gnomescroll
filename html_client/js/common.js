// Common helper methods

// converts arguments to an array
if (Array.prototype.convertArgs === undefined) {
    Array.prototype.convertArgs = function (args, start_index, slice_count) {
        return Array().slice.call(args, start_index, slice_count);
    };
}


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

// Same function as python dict's setDefault method
// obj.setDefault(key [, default]);
// If key exists, returns obj[key],
// Else sets obj[key] = default and returns default.
// Default defaults to null.
if (Object.prototype.setDefault === undefined) {
    Object.prototype.setDefault = function (key, value) {
        if (typeof key !== 'string') {
            throw { message: 'Object.setDefault() called with invalid args.' };
        }
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
if (Object.prototype.getDefault === undefined) {
    Object.prototype.getDefault = function (key, value) {
        if (typeof key !== 'string') {
            throw { message: 'Object.getDefault() called with invalid args.' };
        }
        value = value || undefined;
        if (this.hasOwnProperty(key)) {
            return this[key];
        } else {
            return value;
        }
    };
}

// returns an object's keys as an array
if (Object.prototype.keysArray === undefined) {
    Object.prototype.keysArray = function () {
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
if (Object.prototype.valuesArray === undefined) {
    Object.prototype.valuesArray = function () {
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
if (Object.prototype.keysArray === undefined) {
    Object.prototype.keysArray = function () {
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
