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


var plot = $.plot("#graph", [{data:[],lines:{show:true}}]);

function setStatusFrom(data){
	var statusNode = data.split("<br>")[0];
	$("#status").html(statusNode);
}

function update() {
	var input;
	$.get('cgi-bin/getDados').success(
			function(data){	
			setStatusFrom(data);
			plot.setData([ parseData(data) ]);
			plot.setupGrid();
			plot.draw();
			});

	setTimeout(update, updateInterval);
}
/* ---------------------------------------*/

/* Main */
jQuery(document).ready(new function () {
		var tc = new timeControl();
		tc.startClockUpdate();
		update();
		});
