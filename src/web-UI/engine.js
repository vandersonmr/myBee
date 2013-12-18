/* timeControl is responsable for the clock */
function timeControl() {
	this.getTime = function() {
		var today = new Date()
		var h = today.getHours()
		var m = today.getMinutes()
		var s = today.getSeconds()

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

/* Get an statusID (Number) and return a html tag with a text that rigth 
 * represent the status. */
function getStatusMsg(statusID){
	if(statusID == 0) { // If is a good status
		return "<div style=\"color:blue\">"+statusMsg[statusID]+"</div>"
	} else { 
		return "<div style=\"color:red\">"+statusMsg[statusID]+"</div>"
	}
}

function nodeGraphManager(name){
	this.name = name
	this.data = []
	var times = []
	var stats = []
	this.nodeStatus = "OK"

	$("#GraphsGrid").append(
            "<div id=\"g"+name+"\" class=\"graph\">"+
        		  "<h2>Node: "+name+"</h2>"+
              "<div id=\""+name+"\"  style=\"width:600px;height:300px;float:left\"></div>"+
              "<h3>Status: <br> "+
              "<div id=\"status"+name+"\">"+
                  "<div style=\"color:blue\">"+this.nodeStatus+"</div>"+
              "</div></h3>" + 
            "</div>")

	this.plot = $.plot("#"+name, [{data:[], label: name+" temp."}],
			              {series: {
					lines:{show:true}, 
					points:{show:true}},
				       grid: {
					hoverable: true,
					clickable: true},
				       xaxis:{
					tickSize: 100}
				      })

	window.teste = this.plot
	this.setData = function(data, time, stat){
		this.data = data
		times = time
		stats = stat
	}
	
	this.setNodeStatus = function(nodeStatus){
		this.nodeStatus = nodeStatus
		$("#status"+this.name).html(getStatusMsg(nodeStatus))
	}
		
	this.update = function(){
		this.plot.setData([this.data])
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
				     "Data: "+ times[x] + "<br> Temperatura: " + y + "º célcius"
					 + "<br> Status: " + stats[x]);
			}
		} else {
			$("#tooltip").remove();
			previousPoint = null;            
		}		

	});
 
}

var graphList = {}

function clearEmptyGraphs() {
  for (var index in graphList) {
    if (graphList[index].data.length <= 1) {
      $("#g"+index).hide();
    } else {
      $("#g"+index).show();
    }
  }
}

window.setInterval(clearEmptyGraphs,1500);

function insertIndex(stack){
	var res = []
	var time = []
	var stats = []
	var i = 0
	var size = stack.length
	for(var j=0; j < size; j++){
		var temp = stack.pop();
		stats.push(temp[2])
		time.push(temp[1]);
		res.push([i++, temp[0]])
	}
	return [res,time,stats]
}

function parseData(input) {
	var temperatures = {}
	
	if (input.length == 0)
		return temperatures

	var rows = input.split("<br>")
	for(var captura = 0; captura < rows.length-1; captura++){
		var row = rows[captura].replace(/\n/g,"").split("&")
		var nodeName = row[0].replace(/#/g,"").replace(" ","")
                              .replace("\n","node").replace("[","").replace("]","")
		var time = row[1]
		var tempValue = row[2]
		var stats = row[3]	
		if(temperatures[nodeName] == undefined){
			temperatures[nodeName] = []
		}

		temperatures[nodeName].push([tempValue,time,stats])
	}
	return temperatures
}

function plotData(data){
	for(var node in data){
			if(data[node].length == 0)
				continue
			if (graphList[node] == undefined)
				graphList[node] = new nodeGraphManager(node);
		
			var tempData = insertIndex(data[node])
			graphList[node].setNodeStatus(tempData[2][tempData[2].length-1].replace(" ",""))
			graphList[node].setData(tempData[0],tempData[1],tempData[2])
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
	}).appendTo("body").fadeIn(200)
}

var updateInterval = 1000

function update() {
	var input
	$.get('cgi-bin/getDados').success(
			function(data){	
				var res = parseData(data)
				plotData(res)
			});

	setTimeout(update, updateInterval)
}

jQuery(document).ready(new function () {
		var tc = new timeControl()
		tc.startClockUpdate()
		update()
});
