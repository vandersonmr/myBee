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
	this.name = name
	this.data = []
	var times = []
	this.nodeStatus = "OK"

	$("#GraphsGrid").append("<div class=\"graph\">"+
        						"<h2>Node: "+name+"</h2>"+
                	"<div id=\""+name+"\"  style=\"width:600px;height:300px;float:left\"></div>"+
                        "<h3>Status: <br> <div id=\"status\" style=\"color:blue\">"+this.nodeStatus+"</div></h3>" + 
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

	this.setData = function(data, time)	{
		this.data = data
		times = time
	}
	
	this.setNodeStatus = function(nodeStatus){
		this.nodeStatus = nodeStatus
		$("#status").html(nodeStatus)
	}
		
	this.update = function(){
		this.plot.setData( [this.data] )
		this.plot.setupGrid()
		this.plot.draw()
	}
	
	var previousPoint = null;
	$("#"+name).bind("plothover", function (event, pos, item) {
		if (item) {
			if (previousPoint != item.dataIndex) {
				previousPoint = item.dataIndex;
				$("#tooltip").remove();
				var x = item.datapoint[0],
				y = item.datapoint[1];
				showTooltip(item.pageX, item.pageY,
				     "Data: "+ times[x] + "<br> Temperatura: " + y + "º célcius");
			}
		} else {
			$("#tooltip").remove();
			previousPoint = null;            
		}		

	});
 
}

var temperatures = [], totalPoints = 100;
function parseData(input) {
	if(temperatures.length > 0)
		temperatures = temperatures.splice(1)

	var rows = input.split("<br>");
	var time = []
	for(var captura=0; captura < rows.length; ++captura){
		var temp = rows[captura].split("&")
		var tempT = temp[2]
		var tempTime = temp[1]

		temperatures.push(tempT)
		time.push(tempTime)
	}

	var res = [];
	for (var i = 0; i < temperatures.length; ++i){
		res.push([i,temperatures.pop()])
	}
	return [res,time.reverse()]
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

var updateInterval = 500;

function update() {
	var input
	$.get('cgi-bin/getDados').success(
			function(data){	
				var node = window.node
				node.setNodeStatus(data.split("<br>")[0])
				var datas = parseData(data)
				node.setData(datas[0],datas[1])
				node.update()
			});

	setTimeout(update, updateInterval)
}
/* ---------------------------------------*/

/* Main */
jQuery(document).ready(new function () {
		var tc = new timeControl()
		tc.startClockUpdate()
		//var node1 = new nodeGraphManager("graph")
		var node1 = new nodeGraphManager("graph2")
		window.node = node1
		update()
});
