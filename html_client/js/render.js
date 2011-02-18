
function Screen () {

    // make sure Screen is bound to this
    if ( !this instanceof Screen) {
        return new Screen();
    }

    var x_res,
        y_res,
        x_scale = 0.9, // 90%
        y_scale = 0.9,
        grid_dim = 20,
        grid_width,
        grid_height; //20 squares wxh
    
    x_res = window.innerWidth;
    y_res = window.innerHeight;
    
    var scale = y_scale,
        res = y_res,
        other_res = x_res;
        
    if (x_res < y_res) { //browser in portrait
        scale = x_scale;
        res = x_res;
        other_res = y_res;
    }
    
    res = parseInt( Math.round( (res * scale)/10. ) ); //scale
    if (res % 2 != 0 ) { //make it divisible by 20
        res += -1;
    }
    res = res * 10;
    other_res = res; //make it a square
    
    
    // init canvas
    var canvas = $('<canvas></canvas>').attr('id','screen');
    
    canvas.width = x_res;
    canvas.height = y_res;
    
    $('body').append(canvas);
    
    

};
