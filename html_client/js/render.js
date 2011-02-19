
var screen = ( function () {

    var x_res,
        y_res,
        x_scale = 0.9, // 90%
        y_scale = 0.9,
        grid_cells = 40, // 40x40 grid, default
        cell_width = 16,
        cell_height = 16; // 16x16 cells
    
    // cell coordinate generation
    // to be called on resize
    
    var generateCells, cells=[];
        
    var calculateCanvasResolution, resizeScreen;
    var calculateGridCells;
    var canvas, setCanvas, colorCanvas;
    var controlMap, setControls;
    var init, test, public_;
    
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
            col = i % grid_cells;
            row = (( i - col ) / grid_cells);
            x = col * cell_width;
            y = row * cell_height;
            cell_coordinates.push([x,y]);
        }
        
        cells = cell_coordinates;
        
    }
    
    calculateGridCells = function () {
        
        grid_cells = parseInt(Math.floor( x_res / cell_width));
        //res = grid_cells * cell_width; //readjust
        //x_res = y_res = res;
    }
    
    calculateCanvasResolution = function (height, width) {
        
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
        
        calculateGridCells();

        
    }
    
    resizeScreen = function () {
        
        calculateGridCells();
        generateCells();
        setCanvas();
        
        //needs to scale all the shit down
        
    }
    
    setCanvas = function () {
        
        $('h1').remove();
        
        canvas = $('#screen');

        if (!canvas.length) {
            canvas = $('<canvas></canvas>').attr('id','screen');
            $('body').prepend(canvas);
        }
        
        canvas.css('margin-left', parseInt((window.innerWidth - x_res) / 3)); //shift it to the right
        
        canvas = canvas[0];
        
        canvas.width = x_res;
        canvas.height = y_res;
        
        colorCanvas('black');
    }
    
    colorCanvas = function (color) {
        var ctx = canvas.getContext('2d');
        ctx.fillStyle = color;
        ctx.fillRect(0, 0, canvas.width, canvas.height);
    }
    
    controlMap = ( function () {
        
        var increment = 2,
            increase_cell_size,
            decrease_cell_size;
        
        increment = 2;
        
        increase_cell_size = function (event) {
            
            if (cell_width >= Math.floor(x_res/2) || cell_height >= Math.floor(y_res/2)) { //don't go bigger than would fit 2x2
                return false;
            }
            
            cell_width += increment;
            cell_height += increment;
            resizeScreen();
        }
        
        decrease_cell_size = function (event) {
            
            if (cell_width <= 4 || cell_height <= 4) { // don't get smaller
                return false;
            }
            
            cell_width += -increment;
            cell_height += -increment;
            resizeScreen();
        }
        
        return {
                increase_cell_size: increase_cell_size,
                decrease_cell_size: decrease_cell_size
               }  
        
    }());
    
    setControls = function () {
        
        $('a[id="button"]').click( function (event) {
            event.preventDefault;
            controlMap[$(this).attr('class')](event);
            
            test(); //redraw tests to check
            
            return false;
        });
        
    }
    
    // init canvas
    init = function () {
    
        calculateCanvasResolution();
        generateCells();
        setCanvas();

        setControls();
        // bind resize event here
        // 2 resize events:
        // window resize
        // user adjust cell size
        
        $(window).resize( function (event) {
            resizeScreen();
            test();
        });

    }
    
        /*
    var tilemap = $('img#tilemap')[0];
    var tilemap_image = new Image();
    tilemap_image.src = "static/tiles/Bisasam_24x24.png";
    */
    
    var tilemap = {
        
        elem: $('img#tilemap')[0],
        
        image: ( function() {
                            var img = new Image();
                            img.src = "static/tiles/Bisasam_24x24.png";
                            return img;
                        }()),
                        
        tiles_wide: 16,
        tiles_high: 16,
        tile_width: 24,
        tile_height: 24
    }
    
    var drawTile = function (tile_num, cell_num) {
        
        var ctx = canvas.getContext('2d'),
            cell_x = cells[cell_num][0],
            cell_y = cells[cell_num][1],
            x_offset,
            y_offset,
            tile_x_pos;
            
        tile_x_pos = tile_num % tilemap.tiles_wide;
        x_offset = tile_x_pos * tilemap.tile_width; 
        y_offset = ((tile_num - tile_x_pos)/tilemap.tiles_wide) * tilemap.tile_height;
        
        ctx.drawImage(tilemap.image, 
                      x_offset, y_offset, tilemap.tile_width, tilemap.tile_height,
                      cell_x, cell_y, cell_width, cell_height);
        
    }
    
    
    test = function () {
        
        var context;
        
        ctx = canvas.getContext("2d");
        
        var drawCircle,
            drawSquare,
            drawOutlinedSquare,
            checkerboard,
            gridBoard;
        
        drawCircle = function () {
            
            ctx.beginPath();
            ctx.arc(x_res/2, y_res/2, x_res/2, 0, Math.PI*0.5, true); 
            ctx.closePath();
            ctx.fill();
        }

        drawSquare = function(x, y, width, height, color) {
            ctx.fillStyle = color || "black";
            ctx.fillRect(x, y, width, height);
        }
        
        drawOutlinedSquare = function(x, y, width, height, color) {
            ctx.strokeStyle = color || "black";
            ctx.strokeRect(x, y, width, height);
        }

        checkerboard = function(color)  {
            
            var draw_func = drawSquare;
            
            var even_width = function(i, x, y) {
                var row, col;
                col = i % grid_cells;
                row = (i-col)/grid_cells;
                if (row%2 === 0 && i%2 === 0) {
                    draw_func(x, y, cell_width, cell_height, color);
                } else if (row%2 === 1 && i%2 === 1) {
                    draw_func(x, y, cell_width, cell_height, color);
                }
            }
            
            var odd_width = function(i, x, y) {
                if (i%2 === 0) {
                    draw_func(x, y, cell_width, cell_height, color);
                }
            }
            
            var pattern;
            
            if (grid_cells % 2 === 0) {
                pattern = even_width;
            } else {
                pattern = odd_width;
            }
            
            $.each(cells, function(i, cell) {
            
                var x = cell[0],
                    y = cell[1];
                
                pattern(i, x, y);
            });
        }
        
        gridBoard = function(color) {
            
            $.each(cells, function (i, cell) {
                
                var x = cell[0],
                    y = cell[1];
                    
                drawOutlinedSquare(x, y, cell_width, cell_height, color);
            
            });
        }
        
        tileCheckers = function () {
            
            $.each(cells, function(i, cell) {
                drawTile(19, i);
            });
        }
        
        //drawCircle();
        //checkerboard("red");
        //gridBoard("red");
        
        tileCheckers();
    
    }
    

    
    public_ = {
                init: init,
                resizeScreen: resizeScreen,
                test: test
              }
    
    return public_;

}());
