//class
function Board(div_id, width, height)
{
	//functions
	this.set_tile = Board_set_tile
	//appearance
	this.div_id = div_id
	this.width = width; //board width in tiles
	this.height = height; //board height in tiles
	//tile appearance
	this.tpw = 50; //tile pixel height
	this.tph = 50; //tile pixel width
	
	//internal state
	this.x_offset = 0;
	this.y_offset = 0;
	this.z_offset = 0;

	//alert("new board");

	//board init stuff

	var div_id, content;
	
	//css for board
	$("#"+this.div_id).css({
	'width': (this.width -1 )*this.tpw,
	'height': (this.height*this -1).tph  
	});
	
	//create tiles, css tiles
	var div_id, content;
	for(var i=0; i < this.width; i++) {
		for(var j=0; j < this.height; j++) {
			//div for each map square
			div_id = this.div_id+"_"+i+"_"+j;
			content = i + "_" + j;
			$("#"+this.div_id).append("<div id="+div_id+" >" + content +"</div>");
			$("#"+div_id).css({
				'width': this.tpw,
				'height': this.tph,
				'position':'absolute',
				'left': j * this.tpw,
				'top': i * this.tph,
				'z-index':0
				});
			//canvas for each map square
			canvas_id = this.div_id+"_"+i+"_"+j+"_canvas";
			$("#"+this.div_id).append("<canvas id="+canvas_id+"> </canvas>");
			$("#"+canvas_id).css({
				'width': this.tpw,
				'height': this.tph,
				'position':'absolute',
				'left': j * this.tpw,
				'top': i * this.tph,
				'z-index':-1
				});

		}	
	}

	//load tileset
	this.tile_set = new Image()
	x = this;
	this.tile_set.onload = function() {
		
		x.set_tile(1,1,2);
};

	this.tile_set.src = "http://127.0.0.1:8056/static/tiles/Bisasam_20x20_ascii.png";
	}

Board_set_tile = function(x, y, value) {
	
	//create a temporary canvas object
	//do multiplication of pixels by rgb value
	//draw to another canvas

	//var canvas_temp = new Canvas();
	
	canvas_id = this.div_id+"_"+x+"_"+y+"_canvas";
	var canvas = document.getElementById(canvas_id);
	var context = canvas.getContext("2d");
	
	var tx = value % 16;
	if(value < 16) { var ty = 0; } else { var ty = (value - tx) / 16; }
	
	sx = 20*tx;
	sy = 20*ty;

	sh = 20;
	sw = 20;
	
	dw = 300;
	dh = 150;
	dx = 0;
	dy = 0;
	
	context.drawImage(this.tile_set, sx, sy, sw, sh, dx, dy, dw, dh);
	
	tile_pw = 20; //tile pixel width for tileset
	tile_ph = 20; //tile pixel height for tilset
	//context.drawImage(this.tile_set, xs, ys, tile_pw, tile_ph, 0, 0, this.tpw, this.tph);
/*
	$(selector).css({
				'background': 'url(http://127.0.0.1:8056/static/tiles/Bisasam_20x20_ascii.png)',
				'background-position': '-'+xs+'px -'+ys+'px',				
				//'clip': 'rect('+ys+'px,'+xe+'px,'+ye+'px,'+xs+'px)',
				'background-color':'#777777'
				});
*/
}
