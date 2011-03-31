//this is where drawing occurs
	
var board = {
	
	z_level: 1,
	x_offset: 0,
	y_offset: 0,

	board_tile_width: 16,
	board_tile_height: 16,
	
	board_canvas : null,
	board_manager : null,
	drawingCache : null,
	cursor_manager : null,
	board_event : null,
	
	init : function() {
		this.board_canvas = board_canvas;
		this.board_manager = board_manager;
		this.drawingCache = drawingCache;
		this.cursor_manager = cursor_manager;
		this.board_event = board_event;
//		this.drawing_manager = drawing_manager;
		
		this.board_event.init(this);
		this.board_canvas.init(this);
		this.drawingCache.init(this.board_canvas);
		this.cursor_manager.init(this);
		this.board_manager.init(this);
	},
	
	start : function() {
		this.board_manager.start();
	},
	
	resize : function resize() {
		///resize board
	},
	
	scroll : function(dx, dy) {
		this.x_offset += dx;
		this.y_offset += dy;
		this.reset();
	},

	scroll_z : function(zLevel) {
		this.z_level = zLevel;
		this.reset();
	},
	
	///redraw
	reset : function() {
		this.board_manager.reset();		
	},
}

var board_event = {
	
	board_manager : null,
	
	init : function(board) {
	this.board_manager = board.board_manager;	
	},
	
	agent_change : function agent_change(agent, type) {
		console.log("agent change start...")
			board_manager.agent_update(agent);
	},
	
	object_change : function (id, type) {
	
	},
	
	terrain_map_change : function(x, y, z, value) {
		console.log("board_event.terrain_map_change")
        if (typeof x === 'object') { // allow block object to be passed in
            value = x.value;
            z = x.z;
            y = x.y;
            x = x.x;
        }
		board_manager.update_tile(x, y, z, value);
	},
}

var board_manager = {
	
	board: null,
	//board_canvas: null,
	cursor_manager: null,

	x_min : null,
	x_max : null,
	y_min : null,
	y_max : null,
	z_level : null,
	
	//index : [], //index stores drawing cursor information for each board position

	agents : [],
	objects : [],

	
	init : function(board) {
		this.board = board;
		//this.board_canvas = board.board_canvas;
		this.cursor_manager = this.board.cursor_manager;

		this.x_min = this.board.x_offset;
		this.x_max = this.board.board_tile_width + this.x_min;
		this.y_min = this.board.y_offset;
		this.y_max = this.board.board_tile_height + this.y_min;
		this.z_level = this.board.z_level;

		//this.reset_index();
		//this.populate_index();
	},
	
	start : function() {
		this.populate_index();
	},
	
//	on_board : function(x_pos,y_pos,z_pos) {
//	return (this.x_min <= x_pos && x_pos < this.x_max && this.y_min <= y_pos && this.y_max > y_pos); 		
//	}

	resize : function () {
		//resizing
	},
	
	scroll : function (dx, dy) {
		var method = "slow";
			console.timeEnd('a')
		console.time('Scroll: ' + dx + " " + dy)
		
		if(method == slow) { this._easy_scroll(dx,dy); }
		if(method == fast) { this._fast_scroll(dx,dy); }
		console.timeEnd('Scroll: ' + dx + " " + dy)
	},
	
	_easy_scroll : function (dx, dy) {
		///implement the easy to implement way (drop everything and recompute from scratch)
		this.populate_index();
		this.blip();
	},
	
	_fast_scroll : function(dx, dy) {
		///incremental update of index
		
		this.board_cursor_manager.scroll(dx,dy); //incremental update, renames cursor

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
	
	populate_index: function() {
		console.log("populate_index")
		this.cursor_manager.reset_cursor_index();

		this.agents = []; //clear index
		this.objects = []; //clear index

		var x, y,xm, ym, zl, tile_value;
		xm = this.x_max;
		ym = this.y_max;
		zl = this.z_level;

		//could have quick method for grabbing a region of map in x-y plane to reduce function calls
		//region could be returned as an array?
		for(x = this.x_min; x<xm; x++) {
			for(y = this.y_min; y<xm; y++)
			{
				tile_value = state.levels[zl][x][y];
				//tile_vale = 1; ///FIX
				this.update_tile(x,y,zl, tile_value);
			}
		}

		///for each agent/ determine if agent is on board and if so, add it to the index
		var x_pos, y_pos, z_pos, pos, id; //x,y positions
		agents = state.agents;/// fill this in; get list of agents

		for(agent_id in agents) 
		{
            var agent = agents[agent_id];
			console.log('populate index, agent_update: ' + agent.id)
			this.agent_update(agent);
/*
			pos = agent.pos();
			x_pos = agent_pos[0];
			y_pos = agent_pos[1];
			z_pos = agent_pos[2];
			if(z_pos != this.z_level) {
				console.log("agent z level errr")
				continue;
			}
			if( this.x_min <= x_pos && x_pos < this.x_max && this.y_min <= y_pos && this.y_max > y_pos)
			{
				this.agents.push(agent.id);
				this.cursor_manager.add_agent_to_cursor(agent.id, x_pos - this.x_min ,y_pos - this.ymin);				

				//this.add_agent_to_index( agent.id, x_pos ,y_pos, z_pos); //agent_id and x,y,z position
			}
			else
			{
					console.log("Index Population: agent not on board, " + agent.id)
			}
*/
		}
		
		for(object_id in state.objects) 
		{
            var obj = state.objects[object_id];
			pos = obj.pos();
			x_pos = pos[0];
			y_pos = pos[1];
			z_pos = pos[2];
			if(z_pos != this.z_level) {
				console.log("object z level errr")
				continue;
			}
			if( this.x_min <= x_pos && x_pos < this.x_max && this.y_min <= y_pos && this.y_max > y_pos)
			{
				this.objects.push(obj.id);
				this.cursor_manager.add_object_to_cursor(obj.id, x_pos - this.x_min ,y_pos - this.ymin);
			}
		}
		
	},
	
	agent_update : function(agent) {
		var pos, x_pos, y_pos, z_pos;
		
		pos = agent.pos();
		x_pos = pos[0];
		y_pos = pos[1];
		z_pos = pos[2];

		var onBoard, inIndex;
		
		inIndex = $.inArray(agent.id, this.agents);
		onBoard = (z_pos == this.z_level && this.x_min <= x_pos && x_pos < this.x_max && this.y_min <= y_pos && this.y_max > y_pos);
		
		console.log("board_manager: agent_update")
		
		/*
		console.log(z_pos == this.z_level)
		console.log("(z_pos, z_level): " + z_pos + " " + this.z_level)
		console.log(this.x_min <= x_pos)
		console.log(x_pos < this.x_max)
		console.log(this.y_min <= y_pos )
		console.log(this.y_max > y_pos)

		console.log("Agent: " + x_pos + " " + y_pos + " " + z_pos) 
		console.log("x min-max: " + this.x_min + " " + this.x_max)
		console.log("y min-may: " + this.y_min + " " + this.y_max)
		console.log("z-level: " + this.z_level)
		console.log("agent_update: " + inIndex + ", " + onBoard)
		*/
		
		if(inIndex != -1 && onBoard) { ///agent moves around on the board
			this.cursor_manager.move_agent(id, x_pos - this.x_min, y_pos - this.y_min);
			console.log("1")
			return 0;
			 }
		if(inIndex == -1 && onBoard) { ///agent moves onto board
			if(!(agent.id in this.agents)) {
				this.agents.push(agent.id);
				this.cursor_manager.add_agent_to_cursor(id, x_pos - this.x_min, y_pos - this.y_min);
				console.log("2")
				return 0;
			}
			else
			{
				console.log("inIndex == -1 && onBoard : error, agent id is in list already")
			}
		}
		if(inIndex != -1 && !onBoard) { ///agent moves off board
			this.agents.splice(inIndex,1); 
			this.cursor_manager.remove_agent_from_cursor(agent.id);
			console.log("3")
			return 0;
		}
		if(inIndex == -1 && !onBoard) { //agent is off map
			console.log("4")
			return 0;
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
	
	update_tile: function(x_pos, y_pos, z_pos, tile_id) {
		if(z_pos == this.z_level && this.x_min <= x_pos && x_pos < this.x_max && this.y_min <= y_pos && this.y_max > y_pos) {
		//console.log("update tile: tile is on board")
		var bx, by;
		bx = x_pos - this.x_min;
		by = y_pos - this.y_min;
		this.cursor_manager.update_tile(bx, by, tile_id);
		} else {
			//console.log("update tile: tile is not on board ")
		}
	},
}

var cursor_manager = {
	
	board : null,
	board_manager: null,

	index : [],
	
	atc : {}, //agent to cursor
	otc : {}, //object to cursor
	
	init : function(board) {
		console.log('init')
		console.log(board)
		this.board = board;
		this.board_manager = board.board_manager;
		this.reset_cursor_index();
	},

	update_tile : function(bx, by, tile_id) {
		
		//console.log({ x : this.index, })
		
		var i, temp;
		i = bx + by*this.board.board_tile_width;
		temp = this.index[i]
		temp.tile_id = tile_id;
		temp.drawing_cursor = [0, -1, -1];
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
	
	reset_cursor_index: function() {
		console.log("reset_cursor_index")
		
		var i;
		for(var x=0; x < this.board.board_tile_width; x++) {
			for(var y=0; y < this.board.board_tile_height; y++) {
				i = x + y*this.board.board_tile_width;
				this.index[i] = {
					index: i,
					drawing_cursor: [0,-1,-1],
					//last_blip : 0, //needed?
					tile_id : 0,
					agent_num : 0,
					agent_list : [],
					object_num : 0,
					object_list: [],
					//debugging information
					bx : x,
					by : y,
					position : [x+this.board_manager.x_min,y+this.board_manager.y_min,this.board_manager.z_level],
				}
			}	
		}
	},

	// tile -> agents -> objects -> tile
	
	blipa : function() {
		this.advance_all_drawing_cursor();
		this.blip();
	},
	
	advance_all_drawing_cursor : function() {
		var len = this.index.length;
		
		//console.log(this.index)
		for(var x=0; x < len; x++) {
			this._advance_drawing_cursor(this.index[x]);
		}
	},

	advance_drawing_cursor : function(bx, by) {
		console.log("advance_drawing_cursor: ")
		console.log(this.index[bx + by*this.board.board_tile_width])
		
		this._advance_drawing_cursor(this.index[bx + by*this.board.board_tile_width]);
	},

	_advance_drawing_cursor : function(x) {
		
		//console.log(x)
		
		if(x.drawing_cursor[0] != -1) //if cursor is on tile/rendering tile
		{
			//console.log("1")
			if(x.agent_num > 0) //then if agents are on tile, render agent
			{
				console.log("1.1")
				x.drawing_cursor[0] = -1;
				x.drawing_cursor[1] = 0;
			} 
			else if(x.object_num > 0)  //if no agents, then render objects if they else
			{
				console.log("1.2")
				x.drawing_cursor[0] = -1;
				x.drawing_cursor[2] = 0;
			}
			else //else keep rendering the tile
			{
				//console.log("1.3")
				//do nothing, only the tile exists on this square
			}
		}
		else if(x.drawing_cursor[1] != -1) //if cursor is rendering a agent
		{
			//console.log("2")

			//console.log("board_manager.advance_drawing_cursor: WTF 0.0")
			x.drawing_cursor[1]++;
			if(x.drawing_cursor[1] < x.agent_num) //if more agents, switch to next agent
			{
				console.log("2.0")
				//do nothings
			}
			else if(x.drawing_cursor[1] == x.agent_num)
			{
				if(x.object_num > 0)
				{
					console.log("2.1")
					x.drawing_cursor = [-1, -1, 0];
				}
				else //if no objects on square, then render tile
				{
					console.log("2.2")
					x.drawing_cursor = [0, -1, -1]
				}
			}
			else if(x.drawing_cursor[1] > x.agent_num)
			{	
				console.log("2.3")
				console.log("board_manager.advance_drawing_cursor: WTF 1, absolute error, probably a race condition")
				console.log(x)
			}
		}
		else if(x.drawing_cursor[2] != -1)
		{			
			console.log("3")

			console.log("board_manager.advance_drawing_cursor: WTF 0.1")
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
				console.log("board_manager.advance_drawing_cursor: WTF 2, absolute error, probably a race condition")				
			}
		}
		
		//console.log(x)
	},
	
	agent_to_cursor : function(id) {
		
	},
	
	object_to_cursor : function(id) {
		
	},
	///
	add_agent_to_cursor : function(id, bx, by) {
		console.log("cursor_manager: add agent to cursor")
		
		var bx, by, i;
		i = bx + by*this.board.board_tile_width;
		
		var cursor = this.index[i];
		
		console.log(cursor)
		
		this.atc[id] = cursor;
		cursor.agent_list.push(id);
		cursor.drawing_cursor = [-1, cursor.agent_num, -1];
		cursor.agent_num++;
		console.log("agent num: " + cursor.agent_num)
		this._draw_board_tile(i);

		//console.log(cursor)		
		//console.log(this.index[i])
	},
	
	remove_agent_from_cursor : function(id) {
		console.log("remove_agent_from_cursor: " + id)
		
		if(id in this.atc) {
		var cursor = this.atc[id];
		this._remove_agent_from_cursor(cursor, id);
		} 
		else {
		console.log("remove_agent_from_cursor: agent is missing")
		}
	},
	
	_remove_agent_from_cursor : function(cursor, id) {
		console.log("cursor: " + cursor)

		var inIndex = $.inArray(id, cursor.agent_list);
		if(inIndex == -1) 
		{ 
			console.log("cursor_manager, _remove_agent_from_cursor: Agent id does not exist in cursor!") 
		}
		else 
		{
			cursor.agent_list.splice(inIndex,1);	
			delete this.atc[id];
			cursor.agent_num--;
			
			var drawing_cursor = cursor.drawing_cursor;	
			if(drawing_cursor[1] == -1 && cursor.agent_num >= 1) { cursor.drawing_cursor = [-1, 0, -1]; } 
			else if(cursor.agent_num > drawing_cursor[1]  && cursor.agent_num != 0) { cursor.drawing_cursor = [-1, 0, -1]; }
			else if (cursor.object_num > 0) { cursor.drawing_cursor = [-1, -1, 0]; }
			else { cursor.drawing_cursor = [0, -1, -1]; }
			
			this._draw_board_tile(cursor.index);
		}
	},
	
	move_agent: function(id, bx, by) {
		//var i, cursor1, cursor2;
		//i = bx + by*this.board.board_tile_width;		
		//cursor1 = this.index[i];
		
		this.remove_agent_from_cursor(id);
		//remove_agent_from_cursor : function(id)
		
		this.add_agent_to_cursor(id, bx, by);
	},
	// MOVE TO DRAWING FUNCTION INTERFACE CLASS	
	//does a full redraw
	blip : function() {
		//console.log(this.index)
		
		var x, y, cursor, w, h;
		w = this.board.board_tile_width;
		h = this.board.board_tile_height;
		console.log("Blip Start: wh= " + w + " " + h)
		for(var x=0; x < w; x++) {
			for(var y=0; y < h; y++) {
				//console.log(x + " " + y)
				this._draw_board_tile(x + y*w);
			}	
		}
	},

// MOVE TO DRAWING FUNCTION INTERFACE CLASS
	draw_board : function() {
		for(x in this.index) { this._draw_board_tile(x); }
	},

// MOVE TO DRAWING FUNCTION INTERFACE CLASS	
	draw_board_tile : function(bx,by) {
		this._draw_board_tile(bx + by*this.board.board_tile_width);	
	},

// MOVE TO DRAWING FUNCTION INTERFACE CLASS
	_draw_board_tile : function(x) {
		
		if(!(x in this.index)) {
			console.log('error')
		}
		
		x = this.index[x];
		if(x.drawing_cursor[0] != -1) {
			//draw tile
			// x.tile_id, x.bx, x.by
			///
			//console.log("Draw Tile")
			drawingCache.drawTile(x.bx, x.by, x.tile_id);
			///
		} else if(x.drawing_cursor[1] != -1) {
			//draw agent
			// x.agent_list[x.drawing_cursor[1]], x.bx, x.by
			///
			console.log("Draw Agent")
			drawingCache.drawSprite(x.bx, x.by, 1, 1);
			///
		} else if(x.drawing_cursor[2] != -1) {
		
			console.log("Draw Object")
			//draw object
			// x.object_list[x.drawing_cursor[2]], x.bx, x.by
		}
	},
	

	test_draw_board_0 : function() {
	var i, len, x;
	len = this.index.length;
	for(i=0; i<len; i++) {
		x = this.index[i];

		if(x.drawing_cursor[0] != -1) {
			drawingCache.drawTile(x.bx, x.by, x.tile_id);
		} else if(x.drawing_cursor[1] != -1) {
			console.log("Draw Agent")
			drawingCache.drawSprite(x.bx, x.by, 1, 1);
		} else if(x.drawing_cursor[2] != -1) {
			console.log("Draw Object")
		}		
		
		}	
	},

	test_draw_board_1 : function() {
	var i, len, x;
        index = this.index,
        len = this.index.length,
        dc = drawingCache,
        x;
	len = this.index.length;
	for(i=0; i<len; i++) {
		x = this.index[i];
		if(x.drawing_cursor[0] != -1) {
			drawingCache.drawTile(x.bx, x.by, x.tile_id);
			return 0;
		}
		if(x.drawing_cursor[1] != -1) {
			console.log("Draw Agent")
			drawingCache.drawSprite(x.bx, x.by, 1, 1);
			return 0;
		}
		if(x.drawing_cursor[2] != -1) {
			console.log("Draw Object")
			return 0;
		}		
		}	
	},

	test_draw_board_2 : function() {
	var i, len, drawing_cursor;
	len = this.index.length;
	//console("len: " + len)
	for(i=0; i<len; i++) {
		drawing_cursor = this.index[i].drawing_cursor;
		if(drawing_cursor[0] != -1) {
			drawingCache.drawTile(x.bx, x.by, x.tile_id);
			return 0;
		}
		if(drawing_cursor[1] != -1) {
			console.log("Draw Agent")
			drawingCache.drawSprite(x.bx, x.by, 1, 1);
			return 0;
		}
		if(drawing_cursor[2] != -1) {
			console.log("Draw Object")
			return 0;
		}		
		}	
	},

}

var drawing_manager = {
	

	
}

//this is where drawing occurs to
var board_canvas = {
	
	board : null,
	board_manager: null,
	
	//needs init code
	//will re-init the cache when board is re-sized
	
	canvas_tile_width: 16,  //width of canvas in number of tiles
	canvas_tile_height: 16, //width of canvas in number of tiles
	
	tile_pixel_width: 24,
	tile_pixel_height: 24,
    
	cache_canvas_dom: $('<canvas></canvas>').attr('id', 'board'), // the thing used for reading/writing to canvas
                       
	ctx: null, // drawing surface
    
    init: function (board) {
            this.cache_canvas_dom = $('canvas#board')[0];
            this.ctx = this.cache_canvas_dom.getContext("2d")
            this.board = board;
            this.board_manager = board.board_manager;
        },
        
    resize: function() {
		///Will be called to handle resizing of board and/or zooming
		
		///resize can increase/decrease number of map tile or can increase/decrease tilepixel size
	},
}

var tooltip_text = {
    
    text: function(dx, dy) {
        var bx, by, i;
        bx = Math.floor(dx / board_canvas.tile_pixel_width);
        by = Math.floor(dy / board_canvas.tile_pixel_height);
        i = bx + by * board.board_tile_width;
        console.log(cursor_manager.index[i]);
        return this.cursor_info_string(cursor_manager.index[i]);
    },
    
    to_display: [ 'agent_list',
                  'agent_num',
                  'object_list',
                  'object_num',
                  'tile_id',
                  'drawing_cursor',
                  //'bx',
                  //'by',
                  //'position',
                  //'index', 
                  ],
                  
    add_to_display: function(key) {
        if (!$.isArray(key)) key = [key];
        $.each(key, function(i, val) {
            if ($.inArray(val.toString(), this.to_display) === -1) {
                this.to_display.push(val);
            }
        });
    },
    
    remove_from_display: function(key) {
        if (!$.isArray(key)) key = [key];
        $.each(key, function(i, val) {
            var index = $.inArray(val.toString(), this.to_display);
            if (index > -1) {
                this.to_display.splice(index, 1);
            }
        });
    },
    
    cursor_info_string: function(obj) {
        var str='', len, i=0, key;
        len = this.to_display.length;
        for (i=0; i < len; i++) {
            key = this.to_display[i];
            if (key === undefined) continue;
            str += key;
            str += ' : ';
            str += obj[key].toString();
            if (i !== len-1) str += '<br>';
        }
        return str;
    },
}
