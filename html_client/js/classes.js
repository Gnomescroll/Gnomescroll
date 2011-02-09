//class
function Board(div_id, width, height)
{
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
			div_id = this.div_id+"_"+i+"_"+j;
			content = i + "_" + j;
			$("#"+this.div_id).append("<div id="+div_id+" >" + content +"</div>");
			$("#"+div_id).css({
				'width': this.tpw,
				'height': this.tph,
				'position':'absolute',
				'left': i * this.tpw,
				'top': j * this.tph
				});
		}		
	}
	
}

Board.set_tile = function(x, y, value) {
	var selector = "#"+this.div_id+"_"+x+"_"+y;
	$("#"+this.div_id).append("<div id=>hello world " + i + "_" + j +" </div>");
}
