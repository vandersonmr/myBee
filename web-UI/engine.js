/* Clock code */
function timeControl(){
	this.getTime = function(){
		var today=new Date()
		var h=today.getHours()
		var m=today.getMinutes()
		var s=today.getSeconds()

		m=this.checkTime(m)
		s=this.checkTime(s)
		return h+":"+m+":"+s
	}

	this.updateClock = function(){	
		document.getElementById('horario').innerHTML=this.getTime()
	}

	this.checkTime = function(i){
		if (i<10)
		{
			i="0" + i
		}
		return i
	}
	
	this.startClockUpdate = function(){
		var func = this
		setInterval(function(){ func.updateClock() },500)
	}
}
/* ---------------------------*/

/* Plot Graph code */
function nodeGraphManager(name){
	this.name = name;
	this.data = []
	this.plot = $.plot("#graph", [{data:[],lines:{show:true}}])
	this.nodeStatus = "OK";	

	this.setData = function(data){
		this.data = data; 		
	}
	
	this.setNodeStatus = function(nodeStatus){
		this.nodeStatus = nodeStatus;
		$("#status").html(nodeStatus);
	}
		
	this.update = function(){
		this.plot.setData([ this.data ]);
		this.plot.setupGrid();
		this.plot.draw();
	}
 
}

var temperatures = [], totalPoints = 100;
function parseData(input) {
	if(temperatures.length > 0)
		temperatures = temperatures.splice(1);

	var rows = input.split("<br>");
	for(var captura=1; captura < rows.length; ++captura){
		var temp = rows[captura].split("&")[2];

		temperatures.push(temp);

	}

	var res = [];
	for (var i = 0; i < temperatures.length; ++i)
		res.push([i, temperatures.pop()])
			return res;
}

var updateInterval = 2000;

function update() {
	var input;
	$.get('cgi-bin/getDados').success(
			function(data){	
			var node = window.node
			node.setNodeStatus(data.split("<br>")[0])
			node.setData(parseData(data));
			node.update();
			});

	setTimeout(update, updateInterval);
}
/* ---------------------------------------*/

/* Main */
jQuery(document).ready(new function () {
		var tc = new timeControl();
		tc.startClockUpdate();
		var node1 = new nodeGraphManager();
		window.node = node1;
		update();
		});
