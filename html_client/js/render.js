
var render = ( function () {

    var x_res,
        y_res,
        x_scale = 0.9, // 90%
        y_scale = 0.9,
        grid_cells = 40, // 40x40 grid, default
        cell_width = 24,
        cell_height = 24; // 16x16 cells
    
    // cell coordinate generation
    // to be called on resize
    
    var generateCells, cells=[];
        
    var calculateCanvasResolution, resizeScreen;
    var calculateGridCells;
    var canvas, setCanvas, colorCanvas, colorMap, ctx;
    var staging_canvas, staging_ctx, setStagingCanvas;
    var controlMap, setControls;
    var init, test, public_;
    
    generateCells = function () {
        
        // row, col are 0-indexed
        var i = 0,
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
            cells.push([x,y]);
        }
        
        console.log(cells.length);
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
        setStagingCanvas();
        
        //needs to scale all the shit down
        
    }
    
    setCanvas = function () {
        
        canvas = $('#screen');

        if (!canvas.length) {
            canvas = $('<canvas></canvas>').attr('id','screen');
            $('#game').prepend(canvas);
        }
        
        canvas.css('margin-left', parseInt((window.innerWidth - x_res) / 3)); //shift it to the right
        
        canvas = canvas[0];
        
        canvas.width = cell_width*grid_cells;
        canvas.height = cell_height*grid_cells;
        
        colorCanvas(canvas, 'black');
        
        ctx = canvas.getContext('2d');
    }
    
    setStagingCanvas = function () {
        
        staging_canvas = $('#stage');
        
        if (!staging_canvas.length) {
            staging_canvas = $('<canvas></canvas>').attr('id', 'stage');
            staging_canvas.css('display','none');
            $('#game').prepend(staging_canvas);
        }
        
        staging_canvas = staging_canvas[0]
        
        staging_canvas.width = cell_width;
        staging_canvas.height = cell_height;
        
        colorCanvas(staging_canvas, "transparent");
        
        staging_ctx = staging_canvas.getContext('2d');
    };
    
    colorCanvas = function (canvas, color) {
        var ctx = canvas.getContext('2d');
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        ctx.fillStyle = colorMap[color];
        ctx.fillRect(0, 0, canvas.width, canvas.height);
    }
    
    colorMap = {
        
        red: "rgba(255,0,0,1)",
        green: "rgba(0,255,0,1)",
        blue: "rgba(0,0,255,1)",
        white: "rgba(255,255,255,1)",
        black: "rgba(0,0,0,1)",
        transparent: "rgba(255,255,255,0)"
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
        setStagingCanvas();

        setControls();
        // bind resize event here
        // 2 resize events:
        // window resize
        // user adjust cell size
        
        //$(window).resize( function (event) {
            //resizeScreen();
            //test();
        //});

    }
    
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
    
    var drawTile = function (ctx, tile_num, cell_num) {
        
        var cell_x = cells[cell_num][0],
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
    
    var canvasContext = function (c) {
        if (c === undefined) {
            c = canvas;
        }
        return c.getContext('2d');
    }
    
    var colorTile = function (ctx, cell_num, tile_num, color) {
        
        var imageData,
            staged_tile,
            x = cells[cell_num][0],
            y = cells[cell_num][1];
            
        var colors = {
            red: {r:255, g:0, b:0},
            green: {r:0, g:255, b:0},
            blue: {r:0, g:0, b:255},
            white: {r:255, g:255, b:255},
            black: {r:0, g:0, b:0}
        }
        
        // put tile on stage
        // get imageData from stage
        // manipulate all staged image data with alpha > 0
        // put the staged image data on the #screen
        
        drawTile(staging_ctx, tile_num, 0);
        
        imageData = staging_ctx.getImageData(0, 0, cell_width, cell_height);
        
        if (colors[color] !== undefined) {
            color = colors[color];
        }
        
        var _x,_y, k, grad;
        var offset, r, g, b, a, bg = [];
        for (_x = 0; _x < imageData.width; _x++) {
            for (_y = 0; _y < imageData.height; _y++) {
                offset = (_y * imageData.width + _x) * 4;
                
                for (k = 0; k < 4; k++) {
                    bg.push(imageData.data[offset+k]);
                }
                
                // normal coloring of a greyscale image
                if (imageData.data[offset+3] !== 0) {
                    grad = imageData.data[offset]/255; // (or max, but 255 is max)
                    imageData.data[offset] = Math.floor(color.r * grad);
                    imageData.data[offset+1] = Math.floor(color.g * grad);
                    imageData.data[offset+2] = Math.floor(color.b * grad);
                }
                
                bg = []; // reset pixel check
            }
        }
                
        ctx.putImageData(imageData, x, y);
        
        colorCanvas(staging_canvas, "transparent");
    }
    
    
    test = function () {
        
        var ctx;
        
        ctx = canvasContext("2d");
        
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
        
        var gaussian_lame = function (mean, sd) {
            var iters = 5,
                g = 0;
            
            for (var i =0; i < 5; i++) {
                g += Math.random()*2-1;
            }
            
            return Math.round(g*sd + mean);
            
        }
        
        tileCheckers = function (color) {
            var tile_num = 19; // the crate
            
            var draw_func;
            
            if (color !== undefined) {
                
                draw_func = function (ctx, tile_num, cell_num, color) {
                    
                    drawTile(ctx, tile_num, cell_num);
                    
                    switch (cell_num % 4) {
                        case 0:
                            color = 'red';
                            break;
                        case 1:
                            color = 'green';
                            break;
                        case 2:
                            color = "blue";
                            break;
                        default:
                            color = "white";
                    }
                    
                    colorTile(ctx, cell_num, tile_num, color);
                            
                }
            } else {
                draw_func = function (ctx, tile_num, cell_num) {
                    drawTile(ctx, tile_num, cell_num);
                }
            }
                    
            
            $.each(cells, function(i, cell) {
                draw_func(ctx, tile_num, i, color);
            });
        }
        
        
        var adjustable_random_colors = function(scale) {
            var tile_num = 19; // the crate
            
            if (scale === undefined) {
                scale = 1;
            }
            
            var draw_func,
                random_sort;
            
            random_sort = function (thing) {
                return (0.5 - Math.random());
            }
                
            var tick = -1;
            
            draw_func = function (ctx, tile_num, cell_num, color) {
                
                var tick_color;
                
                drawTile(ctx, tile_num, cell_num);
                
                tick += 1;

                //random color
                tick_color = [tick % 4, (tick-1) % 4, (tick-2) % 4, (tick-3) % 4];
                tick_color.sort(random_sort);
                
                switch (cell_num % 4) {
                    case tick_color[0]:
                        color = 'red';
                        break;
                    case tick_color[1]:
                        color = 'green';
                        break;
                    case tick_color[2]:
                        color = "blue";
                        break;
                    default:
                        color = "white";
                }
                var r, g, b;
                r = Math.min(Math.floor(Math.random()*256)*0.3, 255);
                g = Math.min(Math.floor(Math.random()*256)*1.5, 255);
                b = Math.min(Math.floor(Math.random()*256)*0.7, 255);
                color = { r: r, g: g, b: b};
                
                colorTile(ctx, cell_num, tile_num, color);
            };
            
            $.each(cells, function(i, cell) {
                draw_func(ctx, tile_num, i, scale);
            });
        };
        
        var orb = function() {

            var tile_num = 19; // the crate

            var draw_func;
            
            draw_func = function (ctx, tile_num, cell_num) {
                
                // distributed color
                var gauss, origin, dist, cell_coord = [];
                var max_dist, red, blue, green;
                
                cell_coord.push( cell_num % grid_cells );
                cell_coord.push( (cell_num - cell_coord[0]) / grid_cells);
                
                //gauss = gaussian_lame(0, grid_cells*100);
                origin = [grid_cells/2, grid_cells/2];
                
                dist = Math.sqrt(Math.pow((cell_coord[0] - origin[0]),2) + Math.pow((cell_coord[1] - origin[1]), 2));
                max_dist = Math.sqrt(Math.pow(grid_cells/2, 2));
                
                //dist = dist*gauss;
                //max_dist = max_dist*gauss;
                
                red = Math.min(Math.floor((255/max_dist) * dist), 255);
                blue = 255-red;
                //blue = Math.min(Math.floor(dist*gauss), 255);
                //red = 255-blue;
                green = 0;

                color = {r: red, g: green, b: blue};
                
                colorTile(ctx, cell_num, tile_num, color);
            }
            
            $.each(cells, function(i, cell) {
                draw_func(ctx, tile_num, i);
            });
        };
        
        //drawCircle();
        //checkerboard("red");
        //gridBoard("red");
        
        //tileCheckers("red");
        
        //adjustable_random_colors(0.7);
        
        orb();
    
    }
        
    public_ = {
                init: init,
                resizeScreen: resizeScreen,
                canvasContext: canvasContext,
                colorTile: colorTile,
                grid_cells: grid_cells,
                cells: cells,
                test: test,
              }
    
    return public_;

}());
