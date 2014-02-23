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
function getStatusMsg(statusID, name){
  if(statusID == 0) { // If is a good status
    return "<div style=\"color:blue\">"+statusMsg[0]+"</div>"
  } else { 
    return "<div style=\"color:red\">"+(name!=undefined?name+" : ":"")+statusMsg[parseInt(statusID)]+"</div>"
  }
}
  function interateAsyncData(index, graph, callBack, endFunction, result) {
    var loopid   = null
    var stopped  = false
    
    this.stop = function() {
      stopped = true
      clearTimeout(loopid)
    }

    function loop(key,data,times,stats, dataLabel) {
      clearTimeout(loopid)
      if (key == 0) {
        if (endFunction) {
          endFunction(dataLabel)
          return 
        } else {
          return
        }
      }
      if (!stopped) { 
        loopid = setTimeout(function() {
          callBack(key,data,times,stats, dataLabel)
          loop(--key,data,times,stats, dataLabel)
        },1);
      }
    }
    
    dataLabel = graph.data[index].label;
    loop(graph.data[index].data.length-1,graph.data[index].data,graph.times[dataLabel],graph.stats[dataLabel], dataLabel);
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
          (options.lastTemp ? " (<b id=\"lastTemp"+name+"\"></b>)</h2>" : "</h2>")+
          (options.closeBox ? "<a class=\"boxclose\" id=\"boxclose"+name+"\"></a>" : "") +

          (options.tabs ? "<div id=\"tabs"+name+"\">" +
                        "<ul>"+
                          "<li><a href=\"#tabs-1"+name+"\">Gráfico</a></li>"+
                          "<li><a href=\"#tabs-2"+name+"\">Dados plano</a></li>"+
                          "<li><a href=\"#tabs-3"+name+"\">Estatísticas</a></li>"+
                          "<li><a href=\"#tabs-4"+name+"\">Opções</a></li>"+
                        "</ul>" : "") +
            
          (options.tabs ? "<div id=\"tabs-1"+name+"\" style=\"width:90%;height:75%\">" : "")+
            "<div id=\""+name+"\"  style=\"width:100%;height:80%;\"></div>"+
            (options.showStatus ? "<h3>Status: <br> "+
            "<div id=\"status"+name+"\">"+
              "<div style=\"color:blue\">"+this.nodeStatus+"</div>"+
            "</div></h3>" : "") + 
            (options.miniGraph ? "<br><div id=\"smallgraph"+name+"\" style=\"width: 100%;height: 18%;\"></div>" : "")+ 
          (options.tabs ? "</div>" : "")+

          (options.tabs ? "<div id=\"tabs-2"+name+"\" style=\"width:90%;height:77%\">" : "")+
          (options.tabs ? "<select id=\"selectRawData"+name+"\" ></select>": "")+
          (options.tabs ? "<textarea id=\"rawData"+name+"\" readonly style=\"resize: none; width:100%;height:100%\"></textarea>" : "")+
          (options.tabs ? "</div>" : "")+

          (options.tabs ? "<div id=\"tabs-3"+name+"\" style=\"width:90%;height:77%\">" : "")+
          (options.tabs ? "<select id=\"selectStatistics"+name+"\" ></select>": "")+
          (options.tabs ? "<textarea id=\"statistics"+name+"\" readonly style=\"resize: none; width:100%;height:100%\"></textarea>" : "")+
          (options.tabs ? "</div>" : "")+

          (options.tabs ? "<div id=\"tabs-4"+name+"\" style=\"width:90%;height:77%\">" : "")+
          (options.tabs ? "Mostar os dados: <br>" : "") +
          (options.tabs ? "<p id=\"choices"+name+"\" style=\"float:left; width:90%;\"></p>" : "")+
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
                   mode    : "time",
                   minTickSize: [1, "minute"],
                   timezone: "browser",
                   timeformat: "%H:%M"
                 }
      });

  this.highlight = function() {
    this.plot.unhighlight();
    var keys = Object.keys(stats);
    keys.sort();
 
    for(i in keys) {
      for(index in stats[keys[i]]) {
        if (parseInt(stats[keys[i]][index]) != 0) {
          this.plot.highlight(parseInt(i),parseInt(index))
        }
      }
    }
  }
 
  this.fillSelectWithTypesOfData = function(selectName) {
    for(var i in this.data) {
      $("#"+selectName).append($("<option>", {
          value : i
        , text  : this.data[i].label
      }))
    }
  }

  var dataStatisticsInterator;
  this.fillDataStatistics = function() {
    
    this.fillSelectWithTypesOfData("selectStatistics"+name);
    var pai = this;
    $("#selectStatistics"+name).change(function() {
      if (dataStatisticsInterator) dataStatisticsInterator.stop()
      $("#"+divId+" #statistics"+name).empty(); 
      $("#"+divId+" #statistics"+name).append("Estatísticas dos dados "+pai.data[$(this).val()].label+"\n"+
                                             " Carregando... \n");
  
      var result = {mean:0,alerts:0}
      dataStatisticsInterator = new interateAsyncData($(this).val(), pai,
          function(key, data, times, stats, label) {
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
          function(label) {
            $("#"+divId+" #statistics"+name).empty()
            $("#"+divId+" #statistics"+name).append("Estatísticas dos dados \n")
            $("#"+divId+" #statistics"+name).append(""+label+" Média : "+Math.round(result.mean)+"\n")
            $("#"+divId+" #statistics"+name).append(""+label+" Máxima : "+result.highest+"\n")
            $("#"+divId+" #statistics"+name).append(""+label+" Mínima : "+result.lowest+" \n")
            $("#"+divId+" #statistics"+name).append("Quantidade de alertas : "+result.alerts+" \n")
          },
          result
      )
    })
    $("#selectStatistics"+name).change();
  }

  var dataTextInterator;
  this.fillDataTextArea = function(){
    this.fillSelectWithTypesOfData("selectRawData"+name);
    
    var pai = this;
    pai.stats = stats;
    pai.times = times
    $("#selectRawData"+name).change(function() {
      if(dataTextInterator) dataTextInterator.stop();
      $("#"+divId+" #rawData"+name).empty()
      $("#"+divId+" #rawData"+name).append("Chave tipo Tempo dado  status\n");
      dataTextInterator = new interateAsyncData($(this).val(), pai, 
        function(key, data, times, stats, dataLabel) {
          $("#"+divId+" #rawData"+name)
                  .append(key+" "+" "+dataLabel+" "+times[key]+" "+data[key][1]+" "+stats[key]+"\n")
        }
      );
    })

    $("#selectRawData"+name).change();
  }

  this.setData = function(data, time, stat){
    this.data = data
    times = time
    stats = stat
    if (options.miniGraph) {
      this.plotMiniGraph();
    }
  }

  this.setNodeStatus = function(nodeStatus, name){
    this.nodeStatus = nodeStatus
    $("#"+divId+" [id='status"+this.name+"']").html(getStatusMsg(nodeStatus,name)) 
  }

  this.update = function(){
    this.plot.setData(this.data)
    this.plot.setupGrid()
    this.plot.draw()

    for(var i in this.plot.getData()) 
      this.plot.getData()[i].highlightColor = "#D80000"

    this.highlight()

    $("#"+divId+" [id='g"+name+"']").show()

    lastDataByType = this.data[this.data.length-1]
    $("#"+divId+" #lastTemp"+name).html(lastDataByType.label+": "+lastDataByType.data[this.data.length-1][1]) //!TODO

    if(options.tabs) {
      this.fillDataTextArea()
      this.fillDataStatistics()
      this.fillOptions()
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
        var x = item.dataIndex,
        y = item.datapoint[1];
        showTooltip(item.pageX, item.pageY,
                    "Data: "+ times[item.series.label][x] + "<br> "+item.series.label+": " + y + " "
                    + "<br> Status: " + getStatusMsg(parseInt(stats[item.series.label][x])));
      }
    } else {
      $("#tooltip").remove();
      previousPoint = null;            
    }		
  });

  this.fillOptions = function() {
    // insert checkboxes 
    var i = 0;
    $.each(this.data, function(key, val) {
      val.color = i;
      ++i;
    });
    var choiceContainer = $("#choices"+name);
    $.each(this.data, function(key, val) {
      choiceContainer.append("<br/><input type='checkbox' name='" + key +
        "' checked='checked' id='id" + key + "'></input>" +
        "<label for='id" + key + "'>"
        + val.label + "</label>");
    });
    choiceContainer.find("input").click(plotAccordingToChoices);
    var pai = this
      function plotAccordingToChoices() {
        var data = [];
        choiceContainer.find("input:checked").each(function () {
          var key = $(this).attr("name");
          if (key && pai.data[key]) {
            data.push(pai.data[key]);
          }
        });
        if (data.length > 0) {
          pai.plot.setData(data)
            pai.plot.setupGrid()
            pai.plot.draw()
        }
      }

    plotAccordingToChoices();
  }

  this.plotMiniGraph = function()  {
    var plot = this.plot
    var rangeselectionCallback = function(o) {
      var xaxis = plot.getAxes().xaxis;
      xaxis.options.min = o.start;
      xaxis.options.max = o.end;
      plot.setupGrid();
      plot.draw();
    }

    var sData = $.extend(true,[],this.data);
    var miniGraph = $.plot("#"+divId+" [id='smallgraph"+name+"']",sData,{
        legend: {
              show: false
        },
        xaxis: {     
          mode    : "time",
          timezone: "browser",
          timeformat: "%H:%M"
        },
        yaxis: {
          show: false
        },
        grid:{
          color: "#666",
          downsample: { threshold: 50 },
          backgroundColor: { colors: ["#ddd", "#fff"]}
        },
        rangeselection:{
          start   : sData[0].data[sData[0].data.length-1][0],
          end     : sData[0].data[0][0],
          color   : "#feb",
          enabled : true,
          callback: rangeselectionCallback
        }
    });
  }
    
  $("#"+divId+" #tabs"+name ).tabs();

}

var graphList = {}

function hasData(graphData) {
  for(i in graphData.data) {
    if(graphData.data[i].data.length > 1) return true
  }
  return false;
}

function clearEmptyGraphs(divId) {
  for (var index in graphList) {
    if (!hasData(graphList[index])) {
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

    if(stats[temp[3]] == undefined)
      stats[temp[3]] = []

    stats[temp[3]].push(temp[2])

    if(time[temp[3]] == undefined)
      time[temp[3]] = []

    time[temp[3]].push(temp[1])

    if (res[temp[3]] == undefined) 
      res[temp[3]] = []

    res[temp[3]].push([Date.parse(temp[1]), temp[0]])
  }
  
  var keys = Object.keys(res);
  keys.sort();

    
  var finalData = []
  for(var i in keys) finalData.push({ label: keys[i], data: res[keys[i]] })

  return [finalData,time,stats]
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
          var type      = row[4]

          if(temperatures[nodeName] == undefined){
            temperatures[nodeName] = []
          }

          temperatures[nodeName].push([tempValue,time,stats,type])
          
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
    
    var keys = Object.keys(tempData[2])
    var highestKey = keys[0]
    for(var i in keys) {
      var lastStatus = tempData[2][keys[i]][tempData[2][keys[i]].length-1]
      if(lastStatus > tempData[2][highestKey][tempData[2][highestKey].length-1]) 
        highestKey = keys[i]
    }

    graphList[nodeKey].setNodeStatus(tempData[2][highestKey][tempData[2][highestKey].length-1], highestKey)
    
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
        plotData(res,"HistoricGrid", {closeBox : true, tabs: true, miniGraph: true})  
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

