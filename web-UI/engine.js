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

function nodeGraphManager(name){
	this.name = name
	this.data = []

	$("#GraphsGrid").append("<div class=\"graph\">"+
        						"<h2>Node: "+name+"</h2>"+
                	"<div id=\""+name+"\"  style=\"width:600px;height:300px;float:left\"></div>"+
                        "<h3>Status: <br> <div id=\"status\" style=\"color:blue\">OK</div></h3>" + 
                	"</div>"+
        		"</div>")

	this.plot = $.plot("#"+name, [{data:[], label: name+" temp."}],
			              {series: {
					lines:{show:true}, 
					points:{show:true}},
				       grid: {
					hoverable: true,
					clickable: true}
				      })
	this.nodeStatus = "OK"

	this.setData = function(data){
		this.data = data 		
	}
	
	this.setNodeStatus = function(nodeStatus){
		this.nodeStatus = nodeStatus
		$("#status").html(nodeStatus)
	}
		
	this.update = function(){
		this.plot.setData([ this.data ])
		this.plot.setupGrid()
		this.plot.draw()
	}
	
	var previousPoint = null;
	$("#"+name).bind("plothover", function (event, pos, item) {
		if (item) {
			if (previousPoint != item.dataIndex) {
				previousPoint = item.dataIndex;
				$("#tooltip").remove();
				var x = item.datapoint[0].toFixed(2),
				y = item.datapoint[1].toFixed(2);
				showTooltip(item.pageX, item.pageY,
				     "As "+ x + " temperatura de  " + y+ " graus célcius");
			}
		} else {
			$("#tooltip").remove();
			previousPoint = null;            
		}		

	});
 
}

var graphList = {}

function insertIndex(stack){
	var res = []
	var i = 0
	for(var j=0; j < stack.length; j++)
		res.push([++i, stack.pop()])
	return res
}

function parseData(input) {
	var temperatures = {}

	var rows = input.split("<br>");

	for(var captura=1; captura < rows.length-1; ++captura){	
		var row = rows[captura].split("&")
		var nodeName = row[0].replace(/#/g,"").replace(" ","")
                              .replace("\n","node").replace("[","").replace("]","")
		var time = row[1]
		var tempValue = row[2]
		
		if(temperatures[nodeName] == undefined){
			temperatures[nodeName] = []
			i=0
		}
	
		temperatures[nodeName].push(tempValue)
	}
	
	return temperatures
}

function plotData(data){
	window.teste = data
	for(var node in data){
			if (graphList[node] == undefined)
				graphList[node] = new nodeGraphManager(node)
			graphList[node].setData(insertIndex(data[node]))
			graphList[node].update()
	}
}

function showTooltip(x, y, contents) {
	$("<div id='tooltip'>" + contents + "</div>").css({
		position: "absolute",
		display: "none",
		top: y + 5,
		left: x + 5,
		border: "1px solid #fdd",
		padding: "2px",
		"background-color": "#fee",
		opacity: 0.80
	}).appendTo("body").fadeIn(200);
}

var updateInterval = 2000;

function update() {
	var input
	$.get('cgi-bin/getDados').success(
			function(data){	
				var res = parseData(data);
				plotData(res);
			});

	setTimeout(update, updateInterval)
}

jQuery(document).ready(new function () {
		var tc = new timeControl()
		tc.startClockUpdate()
		update()
});
