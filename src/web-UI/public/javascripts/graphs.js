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
    setInterval(function(){ func.updateClock() },300)
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

function nodeGraphManager(name, divId, options){
    this.name = name
    this.data = []
    var times = []
    var stats = []
    this.nodeStatus = "OK"

    $("#"+divId).append(
        "<div id=\"g"+name+"\" class=\"graph\">"+
        
          "<h2>Node: "+name+
          (options.lastTemp ? " (Temperatura: <b id=\"lastTemp"+name+"\"></b> C)</h2>" : "</h2>")+
          (options.closeBox ? "<a class=\"boxclose\" id=\"boxclose"+name+"\"></a>" : "") +

          (options.tabs ? "<div id=\"tabs"+name+"\">" +
                        "<ul>"+
                          "<li><a href=\"#tabs-1"+name+"\">Gráfico</a></li>"+
                          "<li><a href=\"#tabs-2"+name+"\">Dados plano</a></li>"+
                          "<li><a href=\"#tabs-3"+name+"\">Estatísticas</a></li>"+
                        "</ul>" : "") +
            
          (options.tabs ? "<div id=\"tabs-1"+name+"\" style=\"width:90%;height:75%\">" : "")+
            "<div id=\""+name+"\"  style=\"width:100%;height:80%;\"></div>"+
            (options.showStatus ? "<h3>Status: <br> "+
            "<div id=\"status"+name+"\">"+
              "<div style=\"color:blue\">"+this.nodeStatus+"</div>"+
            "</div></h3>" : "") + 
          (options.tabs ? "</div>" : "")+

          (options.tabs ? "<div id=\"tabs-2"+name+"\" style=\"width:90%;height:77%\">" : "")+
          (options.tabs ? "<textarea id=\"rawData"+name+"\" readonly style=\"resize: none; width:100%;height:100%\"></textarea>" : "")+
          (options.tabs ? "</div>" : "")+

          (options.tabs ? "<div id=\"tabs-3"+name+"\" style=\"width:90%;height:77%\">" : "")+
          (options.tabs ? "</div>" : "")+
          "</div>");

  this.plot = $.plot("#"+divId+" [id='"+name+"']", [{data:[], label: name+" temp."}],
      {series: {
                 lines:{show:true}, 
                 points:{show:true}},
                 grid: {
                   hoverable: true,
                   downsample: { threshold: 500 },
                   clickable: true
                 },
                 xaxis:{
                   tickSize: 100
                 }
      });

  this.highlight = function() {
    this.plot.unhighlight();
    for(key in stats) {
      if (stats[key] != 0) {
        this.plot.highlight(0,parseInt(key));
      }
    }
  }
  
  this.interateAsyncData = function(callBack, endFunction, result) {
    function loop(key,data,times,stats) {
      if (key == 0) {
        if (endFunction)
          endFunction()
        else
          return
      }
      setTimeout(function() {
        callBack(key,data,times,stats)
        loop(--key,data,times,stats)
      },1);
    }

    loop(this.data.length-1,this.data,times,stats);
  }

  this.fillDataStatistics = function(){
    $("#"+divId+" #tabs-3"+name).append("<h3> Estatísticas dos dados </h3>"+
                                        " <br><h2> Carregando... </h2>");
  
    var result = {mean:0,alerts:0}
    this.interateAsyncData(
        function(key, data, times, stats) {
            var dataInt = parseInt(data[key][1])
            result.mean += dataInt/data.length;
            
            if (!result.highest) {
              result.highest = dataInt
            } else {
              if (dataInt > result.highest)
                result.highest = dataInt
            }

            if (!result.lowest) {
              result.lowest = dataInt
            } else {
              if (dataInt < result.lowest)
                result.lowest = dataInt
            }

            if (stats[key] != 0)
              result.alerts += 1
        },
        function() {
          $("#"+divId+" #tabs-3"+name).empty()
          $("#"+divId+" #tabs-3"+name).append("<h3> Estatísticas dos dados </h3>")
          $("#"+divId+" #tabs-3"+name).append("<li> Temperatura Média : "+Math.round(result.mean)+" </li>")
          $("#"+divId+" #tabs-3"+name).append("<li> Temperatura Máxima : "+result.highest+" </li>")
          $("#"+divId+" #tabs-3"+name).append("<li> Temperatura Mínima : "+result.lowest+" </li>")
          $("#"+divId+" #tabs-3"+name).append("<li> Quantidade de alertas : "+result.alerts+" </li>")
        },
        result
    )

  }

  this.fillDataTextArea = function(){
    $("#"+divId+" #rawData"+name).empty()
    $("#"+divId+" #rawData"+name).append("Chave  Tempo dado  status\n");

    this.interateAsyncData(function(key, data, times, stats) {
      $("#"+divId+" #rawData"+name).append(key+" "+times[key]+" "+data[key][1]+" "+stats[key]+"\n")
    })
  }

  this.setData = function(data, time, stat){
    this.data = data
    times = time
    stats = stat
  }

  this.setNodeStatus = function(nodeStatus){
    this.nodeStatus = nodeStatus
    $("#"+divId+" [id='status"+this.name+"']").html(getStatusMsg(nodeStatus)) 
  }

  this.update = function(){
    this.plot.setData([this.data])
    this.plot.setupGrid()
    this.plot.draw()
    this.plot.getData()[0].highlightColor = "#D80000"
    this.highlight()
    $("#"+divId+" [id='g"+name+"']").show()
    $("#"+divId+" #lastTemp"+name).html(this.data[this.data.length-1][1])
    if(options.tabs) {
      this.fillDataTextArea()
      this.fillDataStatistics()
    }
  }

  if (options.closeBox)
    $("#"+divId+" [id='boxclose"+name+"']").click(function() {
      $("#"+divId+" [id='g"+name+"']").hide();
    });

  var previousPoint = null;
  $("#"+divId+" [id='"+name+"']").bind("plothover", function (event, pos, item) { 
    if (item) {
      if (previousPoint != item.dataIndex) {
        previousPoint = item.dataIndex;
        $("#tooltip").remove();
        var x = item.datapoint[0],
        y = item.datapoint[1];
        showTooltip(item.pageX, item.pageY,
                    "Data: "+ times[x] + "<br> Temperatura: " + y + "º célcius"
                    + "<br> Status: " + getStatusMsg(parseInt(stats[x])));
      }
    } else {
      $("#tooltip").remove();
      previousPoint = null;            
    }		
  });

  $("#"+divId+" #tabs"+name ).tabs();

}

var graphList = {}

function clearEmptyGraphs(divId) {
  for (var index in graphList) {
    if (graphList[index].data.length <= 2) {
      $("#"+divId+" [id='g"+index.replace(divId,"")+"']").hide();
    } else {
      $("#"+divId+" [id='g"+index.replace(divId,"")+"']").show();
    }
  }
}


function insertIndex(stack){
  var res   = []
  var time  = []
  var stats = []
  var i = 0
  var size = stack.length
  for(var j=0; j < size; j++){
    var temp = stack.pop();
    stats.push(temp[2])
    time.push (temp[1]);
    res.push([i++, temp[0]])
  }
  return [res,time,stats]
}

function parseData(input) {
  var temperatures = {}

  if (input.length == 0)
    return temperatures

      var rows = input.split("<br>")

      for(var captura = 1; captura < rows.length-1; captura++){
          var row       = rows[captura].replace(/\n/g,"").split("&")
          var nodeName  = row[0].replace(/#/g,"").replace(" ","")
                            .replace("\n","node").replace("[","").replace("]","").replace(/ /g,'')

          var time      = row[1]
          var tempValue = row[2]
          var stats     = row[3]	

          if(temperatures[nodeName] == undefined){
            temperatures[nodeName] = []
          }

          temperatures[nodeName].push([tempValue,time,stats])
      }
  return temperatures
}

function plotData(data, divId, options) {
  for(var node in data){
    nodeKey = node + divId

    if(data[node].length < 2)
       continue

    if (graphList[nodeKey] == undefined) {
      graphList[nodeKey] = [];
      graphList[nodeKey] = new nodeGraphManager(node,divId,options);
    }

    var tempData = insertIndex(data[node])
    graphList[nodeKey].setNodeStatus(tempData[2][tempData[2].length-1].replace(" ",""))
    graphList[nodeKey].setData(tempData[0],tempData[1],tempData[2])
    graphList[nodeKey].update()

  }
}

function showTooltip(x, y, contents) {
  $("<div id='tooltip'>" + contents + "</div>").css({
    position : "absolute",
    display  : "none",
    top      : y + 5,
    left     : x + 5,
    border   : "1px solid #fdd",
    padding  : "2px",
    "background-color": "#fee",
    opacity  : 0.80
  }).appendTo("body").fadeIn(200)
}

var updateInterval = 1000

function update() {
  var input
    $.get('getDados').success(
        function(data){	
          var res = parseData(data)
          plotData(res,"GraphsGrid", {lastTemp: true , showStatus : true})
        });

  setTimeout(update, updateInterval)
}

function fillNodesOptions() {
  $.get('getNodes').success(
      function(data) {
        $("#nodesBox").empty();
        var rows = data.split('\n');
        $.each(rows, function(index, value) {
          
          if (value.replace(/\s/g,"") != "") // Not empty
            $("#nodesBox").append($("<option>", { 
                value : value
              , text  : value
            }));

        });
      }
      );
}

function addHistoric(nodeName) {
  $.get('getHistoric/'+nodeName).success(
      function(data) {
        var res = parseData(data)
        plotData(res,"HistoricGrid", {closeBox : true, tabs: true})  
      }
  ); 
}

$(document).ready(function (e) {
  var tc = new timeControl()
  tc.startClockUpdate()
  update()
  window.setInterval(function() { clearEmptyGraphs("GraphsGrid") } ,1500);
});


$("#ltempoReal").click(function() {
  $("#historico").hide();
  $("#GraphsGrid").show();
});

$("#lhistorico").click(function() {
  $("#GraphsGrid").hide();
  $("#historico").show();
  fillNodesOptions();
});

$("#historico").hide();

$("#addHist").click(function() { 
  addHistoric($("#nodesBox").val()); 
});

