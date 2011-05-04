//this is where drawing occurs
    
var board = {
    
    z_level  : 1,
    x_offset : 0,
    y_offset : 0,

    tile_width  : 16,
    tile_height : 16,

    _init_interval   : 0,
    _retry_delay     : 100,
    _retry_attempts  : 30,  // 30*100 = 3000ms

    _init : (function () {
        var count = 0;
        return function () {
            var err;
            if (tileset_state.loaded) {
                clearInterval(this._init_interval);
                this._init_interval = 0;
                this.start();
                return true;
            } else if (++count === this._retry_attempts) { // give up after n tries
                count = 0;
                err  = 'tileset_state never loaded; board failed to init ';
                err += this._retry_attempts;
                err += ' (' + (this._retry_delay * this._retry_attempts) + 'ms total)';
                console.log('board init fail');
                dispatcher.trigger('board_init_fail', err);
            }
            return false;
        };
    }()),
    
    init : function() {
        if (!this._init()) {
            this._init_interval = setInterval('board._init();', this._retry_delay);
        }
    },
    
    start : function() {
        this.manager.start();
        dispatcher.trigger('board_start');
    },

    reset : function () {
        this.canvas.reset();
        this.cursor_manager.reset();
        this.manager.reset();
        clearInterval(this._init_interval);
        this._init_interval = 0;
    },
    
    resize : function resize() {
        //resize board
    },

    _bounded_adjust : function (prop, delta, upper, lower) {
        var tmp;
        lower = lower || 0;
        if (upper < lower) {
            tmp = upper;
            upper = lower;
            lower = upper;
        }
        this[prop] += delta;
        this[prop] = Math.max(this[prop], lower);
        this[prop] = Math.min(this[prop], upper);
    },
    
    scroll : function(dx, dy) {
        var old_x = this.x_offset,
            old_y = this.y_offset;
        
        this._bounded_adjust('x_offset', dx, state.map_width);
        this._bounded_adjust('y_offset', dy, state.map_height);
        if (old_x != this.x_offset || old_y != this.y_offset) { // check that the view actually scrolled
            this.manager.redraw();
        }
    },

    scroll_z : function(z, delta_mode) {
        var old_z = this.z_level;
        if (delta_mode) {
            this._bounded_adjust('z_level', z, state.max_z_level);
        } else {
            this.z_level = z;
        }
        if (old_z != this.z_level) {
            this.manager.redraw();
        }
    },
};

//var board_manager = {
board.manager = {
    
    agents  : [],
    objects : [],
    
    start : function() {
        this.populate_index();
    },

    resize : function () {
        //resizing
    },

    redraw : function () {
        this.populate_index(true);
    },

    reset : function () {
        this.agents = [];
        this.objects = [];
    },
    
    scroll : function (dx, dy, method) {
        if (method === undefined) {
            method = "easy";
        }

        console.timeEnd('a');
        console.time('Scroll: ' + dx + " " + dy);
        
        if(method === 'easy') { this._easy_scroll(dx,dy); }
        if(method === 'fast') { this._fast_scroll(dx,dy); }
        console.timeEnd('Scroll: ' + dx + " " + dy);
    },
    
    _easy_scroll : function (dx, dy) {
        //implement the easy to implement way (drop everything and recompute from scratch)
        //this.populate_index();
        //board.cursor_manager.blip();
        //board.init(board.x_offset + dx, board.y_offset+dy);
    },
    
    _fast_scroll : function(dx, dy) {
        //incremental update of index
        
        board.cursor_manager.scroll(dx,dy); //incremental update, renames cursor

        //remove from index objects on squares off screen
        //add objects on index that is now on screen
        
        /*
        calculate two regions; first region is region of objects off screen
        objects in this region can be removed by
            1> iterate over all objects in object list
            2> get objects in this region, from cursor data (can return these objects on scroll)
        purge objects from index that are in the off-screen region
        iterate over all objects to get those objects in the new region
        redraw methods
            full redraw of screen
            translation of existing and 
        */
        
        //this.add_object_to_index( object.id, x_pos ,y_pos, z_pos);
        //this.add_agent_to_index( agent.id, x_pos ,y_pos, z_pos);
        
    },

    on_board : function (x, y, z) {
        if (typeof x === 'object') {
            if ($.isArray(x)) {
                y = x[1];
                z = x[2];
                x = x[0];
            } else {
                y = x.y;
                z = x.z;
                x = x.x;
            }
        }
        var x_min = board.x_offset,
            y_min = board.y_offset,
            x_max = x_min + board.tile_width,
            y_max = y_min + board.tile_height,
            on = (x_min <= x &&
                  x_max >  x &&
                  y_min <= y &&
                  y_max >  y);
                  
        if (z !== undefined) {
            on = (on && board.z_level == z)
        }
        return on;
    },

    _populate_tiles : function (reset_index) {
        if (reset_index) {
            board.cursor_manager._reset_tile_cursors();
        }
        var x,
            y,
            x_min = board.x_offset,
            y_min = board.y_offset,
            x_max = x_min + board.tile_width,
            y_max = y_min + board.tile_height,
            zl    = board.z_level,
            lvl   = state.levels[zl];

        if (lvl === undefined) return false;
        
        //could have quick method for grabbing a region of map in x-y plane to reduce function calls
        //region could be returned as an array?
        for(x = x_min; x < x_max; x++) {
            for(y = y_min; y < y_max; y++) {
                this.update_tile(x, y, zl, lvl[x][y]);
            }
        }
        return true;
    },

    _populate_agents : function (reset_index) {
        if (reset_index) {
            board.cursor_manager._reset_agent_cursors();
        }
        this.agents = []; //clear index
        
        var id,
            agent,
            agents = state.agents;

        for (id in agents) { // DOESN'T CHECK AGENTS IN VIEW
            if (!agents.hasOwnProperty(id)) continue;
            agent = agents[id];
            if (this.on_board(agent.pos())) {
                this.agents.push(id);
                this.agent_update(agent);
            }
        }
    },

    _populate_objects : function (reset_index) {
        if (reset_index) {
            board.cursor_manager._reset_object_cursors();
        }
        this.objects = []; //clear index
        
        var id,
            obj,
            objects = state.objects;

        for (id in objects) {
            if (!objects.hasOwnProperty(id)) continue;
            obj = objects[id];
            if (this.on_board(obj.pos())) {
                    this.objects.push(obj.id);
                    // TODO
                    // implement cursor_manager.add_object_to_cursor
                    //this.cursor_manager.add_object_to_cursor(obj.id, x_pos, y_pos);
            }
        }
    },
    
    populate_index : function(reset_index) {
        if (reset_index) {
            board.cursor_manager.reset_cursor_index();
        }
        this._populate_tiles();
        this._populate_agents();
        this._populate_objects();
    },
    
    agent_update : function(agent) {
        var pos     = agent.pos(),
            x_pos   = pos[0] - board.x_offset,
            y_pos   = pos[1] - board.y_offset,
            id      = agent.id,
            inIndex = $.inArray(agent.id, this.agents),
            onBoard = this.on_board(pos);
        
        if(inIndex !== -1 && onBoard) { //agent moves around on the board
            board.cursor_manager.move_agent(id, x_pos, y_pos);
        }
        else if(inIndex === -1 && onBoard) { //agent moves onto board
            this.agents.push(agent.id);
            board.cursor_manager.add_agent_to_cursor(id, x_pos, y_pos);
        }
        else if(inIndex !== -1 && !onBoard) { //agent moves off board
            this.agents.splice(inIndex, 1); 
            board.cursor_manager.remove_agent_from_cursor(id);
        }
        else if(inIndex === -1 && !onBoard) { //agent is off map
        }
    },
/*  
    add_agent_to_index : function(id, x_pos, y_pos, z_pos) {
        //add id to the list
        /// add id or add reference?
    },
    
    add_object_to_index : function(id, x_pos, y_pos, z_pos) {
        //add 
    },
    
    remove_agent_from_index : function(id) {
        
    },
    
    remove_object_from_index: function(id) {
        
    },
*/
    
    update_tile : function(x, y, z, tile_id) {
        var bx,
            by;
            
        if (this.on_board(x, y, z)) {
            bx = x - board.x_offset;
            by = y - board.y_offset;
            board.cursor_manager.update_tile(bx, by, tile_id);
        }
    },
};

board.cursor_manager = {

    index : [],
    
    atc : {}, //agent to cursor
    otc : {}, //object to cursor
    
    init : function() {
        this.reset_cursor_index();
    },

    reset : function () {
        this.index = [];
        this.atc = {};
        this.otc = {};
    },

    update_tile : function(bx, by, tile_id) {
        var i = bx + by*board.tile_width,
            temp = this.index[i]; // cursor obj
            
        temp.tile_id = tile_id;   // set cursor obj properties
        //temp.drawing_cursor = [0, -1, -1];
        temp.drawing_cursor[0] = 0;
        this._draw_board_tile(i);
    },
    
    //naive scroll is accomplished by cursor index reset and index reconstruction
    //this is non-naive scroll that perserves cursor state for objects still on screen
    scroll : function(bx, by) {
        /*
        This shuffles the index around and nulls the cursor objects now off screen
        */
    },
    
    //internal method, not interface method
    // takes an this.index element

    _generic_cursor_index_properties_reset : function (cursor_type, props) {
        if (typeof props === 'string') {
            props = [props];
        }
        var cursor_type_indexes = {
                'tile'  : 0,
                'agent' : 1,
                'object': 2
            },
            cursor_type_index = cursor_type_indexes[cursor_type],
            i, x, y,
            prop_len = props.length,
            j,
            cursor,
            prop,
            default_val,
            max_x = board.tile_width,
            max_y = board.tile_height,
            default_type;
        for (x=0; x < max_x; x++) {
            for (y=0; y < max_y; y++) {
                i = x + y*max_x;
                cursor = this.index[i];
                if (cursor !== undefined) {
                    for (j=0; j < prop_len; j++) {
                        prop = props[j];
                        default_val = this._default_cursor[prop];
                        if (typeof cursor[prop] === 'object') {
                            if (prop === 'drawing_cursor') {
                                cursor[prop][cursor_type_index] = default_val[cursor_type_index];
                            } else {
                                default_type = ($.isArray(cursor[prop])) ? [] : {};
                                cursor[prop][cursor_type_index] = $.extend(true, default_type, default_val);
                            }
                        } else {
                            cursor[prop] = default_val;
                        }
                    }
                }
            }
        }
    },
    
    _reset_tile_cursors : function () {
        this._generic_cursor_index_properties_reset('tile', ['tile_id', 'drawing_cursor']);
    },

    _reset_agent_cursors : function () {
        this._generic_cursor_index_properties_reset('agent', ['agent_list', 'drawing_cursor', 'agent_num']);
    },

    _reset_object_cursors : function () {
        this._generic_cursor_index_properties_reset('object', ['object_list', 'drawing_cursor', 'object_num']);
    },

    _default_cursor : {
        drawing_cursor : [0, -1, -1],
        //last_blip      : 0, //needed?
        tile_id        : 0,
        agent_num      : 0,
        agent_list     : [],
        object_num     : 0,
        object_list    : []
    },

    _load_default_cursor : function (i) {
        return $.extend(true, { index: i }, this._default_cursor);
    },
    
    reset_cursor_index: function() {
        var i, x, y,
            max_x = board.tile_width,
            max_y = board.tile_height,
            x_off = board.x_offset,
            y_off = board.y_offset,
            z_lvl = board.z_level;
        for (x=0; x < max_x; x++) {
            for (y=0; y < max_y; y++) {
                i = x + y*max_x;
                this.index[i] = $.extend(true, {
                    bx: x,      //debugging information
                    by: y,
                    position : [x + x_off, y + y_off, z_lvl]
                }, this._load_default_cursor(i));
            }   
        }
    },

    // tile -> agents -> objects -> tile
    
    blipa : function() {
        this.advance_all_drawing_cursor();
        this.blip();
    },
    
    advance_all_drawing_cursor : function() {
        var adc = this._advance_drawing_cursor,
            index = this.index,
            len = index.length,
            x;
        for(x=0; x < len; x++) {
            adc(index[x]);
        }
    },

    advance_drawing_cursor : function(bx, by) {
        this._advance_drawing_cursor(this.index[bx + by*board.tile_width]);
    },

    _advance_drawing_cursor : function(x) {
        
        if(x.drawing_cursor[0] != -1) //if cursor is on tile/rendering tile
        {
            if(x.agent_num > 0) //then if agents are on tile, render agent
            {
                x.drawing_cursor[0] = -1;
                x.drawing_cursor[1] = 0;
            } 
            else if(x.object_num > 0)  //if no agents, then render objects if they else
            {
                x.drawing_cursor[0] = -1;
                x.drawing_cursor[2] = 0;
            }
            else //else keep rendering the tile
            {
                //do nothing, only the tile exists on this square
            }
        }
        else if(x.drawing_cursor[1] != -1) //if cursor is rendering a agent
        {

            //console.log("board_manager.advance_drawing_cursor: WTF 0.0");
            x.drawing_cursor[1]++;
            if(x.drawing_cursor[1] < x.agent_num) //if more agents, switch to next agent
            {
                //do nothings
            }
            else if(x.drawing_cursor[1] == x.agent_num)
            {
                if(x.object_num > 0)
                {
                    x.drawing_cursor = [-1, -1, 0];
                }
                else //if no objects on square, then render tile
                {
                    x.drawing_cursor = [0, -1, -1]
                }
            }
            else if(x.drawing_cursor[1] > x.agent_num)
            {   
                console.log("board_manager.advance_drawing_cursor: WTF 1, absolute error, probably a race condition");
                console.log(x);
            }
        }
        else if(x.drawing_cursor[2] != -1)
        {           
            console.log("board_manager.advance_drawing_cursor: WTF 0.1");
            x.drawing_cursor[2]++;
            if(x.drawing_cursor[2] < x.object_num)
            {
                //do nothing
            }
            else if(x.drawing_cursor[2] == x.object_num)
            {
                x.drawing_cursor[2] = -1;
                x.drawing_cursor[0] = 0;
            }
            else if(x.drawing_cursor[2] > x.object_num)
            {
                console.log("board_manager.advance_drawing_cursor: WTF 2, absolute error, probably a race condition");             
            }
        }
    },
    
    agent_to_cursor : function(id) {
        
    },
    
    object_to_cursor : function(id) {
        
    },

    add_agent_to_cursor : function(id, bx, by) {
        var i = bx + by*board.tile_width,
            cursor = this.index[i];
        
        this.atc[id] = cursor;
        cursor.agent_list.push(id);
        cursor.drawing_cursor = [-1, cursor.agent_num, -1];
        cursor.agent_num++;
        this._draw_board_tile(i);
    },
    
    remove_agent_from_cursor : function(id) {
        var cursor;
        if (id in this.atc) {
            cursor = this.atc[id];
            this._remove_agent_from_cursor(cursor, id);
        }
    },
    
    _remove_agent_from_cursor : function(cursor, id) {
        var inIndex = $.inArray(id, cursor.agent_list),
            drawing_cursor;
        if(inIndex === -1) { 
            return;
        } else {
            cursor.agent_list.splice(inIndex, 1);    
            delete this.atc[id];
            cursor.agent_num--;
            
            drawing_cursor = cursor.drawing_cursor; 
            if (drawing_cursor[1] == -1 && cursor.agent_num >= 1) {
                cursor.drawing_cursor = [-1, 0, -1];
            } else {
                cursor.drawing_cursor = [0, -1, -1];
            }
            
            //else if(cursor.agent_num > drawing_cursor[1]  && cursor.agent_num != 0) { cursor.drawing_cursor = [-1, 0, -1]; }
            //else if (cursor.object_num > 0) { cursor.drawing_cursor = [-1, -1, 0]; }
            //else { cursor.drawing_cursor = [0, -1, -1]; }
            
            this._draw_board_tile(cursor.index);
        }
    },
    
    move_agent: function(id, bx, by) {
        //var i, cursor1, cursor2;
        //i = bx + by*board.tile_width;      
        //cursor1 = this.index[i];
        
        this.remove_agent_from_cursor(id);
        //remove_agent_from_cursor : function(id)
        
        this.add_agent_to_cursor(id, bx, by);
    },
    // MOVE TO DRAWING FUNCTION INTERFACE CLASS 
    //does a full redraw
    blip : function() {
        var w = board.tile_width,
            h = board.tile_height,
            x = 0,
            y = 0;
        
        for(x=0; x < w; x++) {
            for(y=0; y < h; y++) {
                this._draw_board_tile(x + y*w);
            }   
        }
    },

// MOVE TO DRAWING FUNCTION INTERFACE CLASS
    draw_board : function() {
        var i = 0,
            len = this.index.length,
            x;
            
        for (i=0; i < len; i++) {
            x = this.index[i];
            this._draw_board_tile(x);
        }
    },

// MOVE TO DRAWING FUNCTION INTERFACE CLASS 
    draw_board_tile : function(bx, by) {
        this._draw_board_tile(bx + by*board.tile_width); 
    },

// MOVE TO DRAWING FUNCTION INTERFACE CLASS
    _draw_board_tile : function(x) {
        // x is an int or integer string
        if (isNaN(parseInt(x, 10)) || !(x in this.index)) {
            return false;
        }
        
        x = this.index[x];
        if (x.drawing_cursor[2] != -1) {
            //draw object
            // x.object_list[x.drawing_cursor[2]], x.bx, x.by
        } else if (x.drawing_cursor[1] != -1) {
            //draw agent
            // x.agent_list[x.drawing_cursor[1]], x.bx, x.by
            drawingCache.drawSprite(x.bx, x.by, 1, 1);
        } else if (x.drawing_cursor[0] != -1) {
            //draw tile
            // x.tile_id, x.bx, x.by
            drawingCache.drawTile(x.bx, x.by, x.tile_id);
        }
    },
    

    test_draw_board_0 : function() {
        var i = 0,
            len = this.index.length,
            x;
        for (i=0; i<len; i++) {
            x = this.index[i];

            if (x.drawing_cursor[0] != -1) {
                drawingCache.drawTile(x.bx, x.by, x.tile_id);
            } else if (x.drawing_cursor[1] != -1) {
                drawingCache.drawSprite(x.bx, x.by, 1, 1);
            } else if (x.drawing_cursor[2] != -1) {
            }       
            
        }   
    },

    test_draw_board_1 : function() {
        var i,
            index = this.index,
            len = this.index.length,
            dc = drawingCache,
            x;
        for (i=0; i<len; i++) {
            x = this.index[i];
            if (x.drawing_cursor[0] != -1) {
                dc.drawTile(x.bx, x.by, x.tile_id);
                return false;
            }
            if (x.drawing_cursor[1] != -1) {
                dc.drawSprite(x.bx, x.by, 1, 1);
                return false;
            }
            if (x.drawing_cursor[2] != -1) {
                return false;
            }       
        }   
    },

    test_draw_board_2 : function() {
        var i, len, drawing_cursor;
        len = this.index.length;
        for (i=0; i<len; i++) {
            drawing_cursor = this.index[i].drawing_cursor;
            if (drawing_cursor[0] != -1) {
                drawingCache.drawTile(x.bx, x.by, x.tile_id);
                return false;
            }
            if (drawing_cursor[1] != -1) {
                drawingCache.drawSprite(x.bx, x.by, 1, 1);
                return false;
            }
            if (drawing_cursor[2] != -1) {
                // implement
                return false;
            }       
        }   
    }
};

board.drawing_manager = {
    // EMPTY
};

//this is where drawing occurs to
board.canvas = new TileCanvas('board', '#game', {
    width       : 16,
    height      : 16,
    pixel_width : 24,
    pixel_height: 24
});

// tooltip methods
board.info = {

    // text displayed in tooltip
    tooltip_text: function(dx, dy) {
        var bx, by, i;
        bx = Math.floor(dx / board.canvas.pixel_width);
        by = Math.floor(dy / board.canvas.pixel_height);
        i = bx + by * board.tile_width;
        return this.cursor_info_string(board.cursor_manager.index[i]);
    },

    // which board properties to display
    to_display: [
        //'agent_list',
        'agent_num',
        //'object_list',
        'object_num',
        'tile_id',
        'drawing_cursor',
        //'bx',
        //'by',
        //'position',
        //'index', 
    ],

    // add a board property to display (in the command line, for example)
    add_to_display: function(keys) {
        if (!$.isArray(keys)) {
            keys = [keys];
        }
        var len = keys.length,
            i,
            key;
        for (i=0; i < len; i++) {
            key = keys[i];
            if ($.inArray(key.toString(), this.to_display) === -1) {
                this.to_display.push(key);
            }
        }
    },

    // remove a board property from to display (in the command line, for example)
    remove_from_display: function(keys) {
        if (!$.isArray(keys)) {
            keys = [keys];
        }
        var len = keys.length,
            i,
            index;
        for (i=0; i < len; i++) {
            index = $.inArray(keys[i].toString(), this.to_display);
            if (index > -1) {
                this.to_display.splice(index, 1);
            }
        }
    },

    // formats the data for display in the tooltip
    cursor_info_string: function(obj) {
        var str = '',
            i   = 0,
            len = this.to_display.length,
            key;

        for (i=0; i < len; i++) {
            key = this.to_display[i];
            if (key === undefined) continue;
            str += key;
            str += ' : ';
            str += obj[key].toString();
            if (i !== len-1) {
                str += '<br>';
            }
        }
        return str;
    },
    
    // called by tooltip extension on activate event
    tooltip_active_method: function(event) {
        var txt;
        $('body').trigger(event); // trigger body click events
        txt = board.info.tooltip_text(event.offsetX, event.offsetY);
        return txt;
    }
};

board.event = {

    info_terrain_map : function (name, msg) { // full terrain map
        if (msg.z_level == board.z_level) {
            board.manager._populate_tiles();
        }
    },

    set_terrain_map : function (name, msg) { // single terrain tile
        if (state.contains(msg)) {
            board.manager.update_tile(msg.x, msg.y, msg.z, msg.value);
        }
    },
    
    agent_position_change : function (name, msg) {
        var agent = state.gameObjectKnown(msg.id, 'agent');
        if (agent !== false) {
            board.manager.agent_update(agent);
        }
    },
    
    object_position_change : function (name, msg) {
        //implement
    },
    
    game_object_to_state : function (name, obj) {
        obj = state.gameObjectKnown(obj);
        if (obj !== false) {
            board.manager[obj.base_type + '_update'](obj);
        }
    },

    board_init_fail : function () {
        clearInterval(board._init_interval);
        board._init_interval = 0;
    },
};

dispatcher.listen('board_init_fail', board.event.board_init_fail);

dispatcher.listen('info_terrain_map', board.event.info_terrain_map);
dispatcher.listen('agent_position_change', board.event.agent_position_change);
dispatcher.listen('set_terrain_map', board.event.set_terrain_map);

dispatcher.listen('game_object_to_state', board.event.game_object_to_state);
