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
