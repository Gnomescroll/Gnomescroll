
var screen = ( function () {

    var x_res,
        y_res,
        x_scale = 0.9, // 90%
        y_scale = 0.9,
        grid_cells = 40, // 40x40 grid, default
        cell_width = 16,
        cell_height = 16; // 16x16 cells
    
    x_res = window.innerWidth;
    y_res = window.innerHeight;
    
    var scale = y_scale,
        res = y_res; // assume y is shortest
        
    if (x_res < y_res) { //browser in portrait
        scale = x_scale;
        res = x_res;
    }
    
    res = parseInt( Math.round( (res * scale)/10. ) ); //scale
    res = res * 10;
    x_res = y_res = res; //make it a square
    
    grid_cells = Math.floor( res / cell_width );    
    res = grid_cells * cell_width; //readjust
    x_res = y_res = res;
    
    // cell coordinate generation
    // to be called on resize
    
    var generateCells, cells;
        
    generateCells = function () {
        
        // row, col are 0-indexed
        var cell_coordinates = [],
            i = 0,
            col,
            row,
            x,
            y,
            total_cells = Math.pow(grid_cells,2);
        
        for ( i=0; i < total_cells; i++ ) {
            var square = i;
            col = (square % grid_cells) - 1;
            row = (( square - row ) / grid_cells) - 1;
            x = col * cell_width;
            y = row * cell_height;
            cell_coordinates.push([x,y]);
            
        }
        
        return cell_coordinates;
        
    }
    
    cells = generateCells();
    
    var calculateGridSize, resizeScreen;
    
    var canvas, setCanvas;
    var init, test, public_;
    
    calculateGridSize = function (height, width) {
        
        var scale, res;
        
        x_res = width || window.innerWidth;
        y_res = height || window.innerHeight;
        
        scale = y_scale;
        res = y_res; // assume y is shortest
            
        if (x_res < y_res) { //browser in portrait
            scale = x_scale;
            res = x_res;
        }
        
        res = parseInt(Math.round((res * scale)/10.)); //scale
        res = res * 10;
        x_res = y_res = res; //make it a square
        
        grid_cells = parseInt(Math.floor(res / cell_width));
        res = grid_cells * cell_width; //readjust
        x_res = y_res = res;
        
    }
    
    resizeScreen = function () {
        
        calculateGridSize();
        setCanvas();
        
        //needs to scale all the shit down
        
    }
    
    setCanvas = function () {
        
        canvas = $('#screen');

        if (!canvas.length) {
            canvas = $('<canvas></canvas>').attr('id','screen');
            $('body').prepend(canvas);
        }
        
        canvas = canvas[0];
        
        canvas.width = x_res;
        canvas.height = y_res;
        
    }
    
    // init canvas
    init = function () {
    
        calculateGridSize();
        setCanvas();

        // bind resize event here
        // 2 resize events:
        // window resize
        // user adjust cell size

    }
    
    test = function () {
        
        var context;
        
        ctx = canvas.getContext("2d");
        
        // draw a circle
        
        ctx.beginPath();
        ctx.arc(x_res/2, y_res/2, x_res/2, 0, Math.PI*0.5, true); 
        ctx.closePath();
        ctx.fill();
        
    }
    
    public_ = {
                    init: init,
                    resizeScreen: resizeScreen,
                    test: test
                  }
    
    return public_;

}());
