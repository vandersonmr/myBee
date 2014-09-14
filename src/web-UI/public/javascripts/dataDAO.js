exports.dataDAO = function(mysql) {

  this.getLastsDatasByMinute = function(minutes, callBack) {  
    //TODO! <--BUG HERE
    /*var queryText = "select * from (select *,str_to_date(Date,'%a %b %e %H:%i:%s %Y') "
      + "as Time from data) as t INNER JOIN nodesOnline ON t.nodeIP="
      + "nodesOnline.nodeID where t.Time > NOW() - INTERVAL "+minutes+" MINUTE ORDER BY Time DESC;";*/

    var queryText = "select * from (select *,str_to_date(Date,'%a %b %e %H:%i:%s %Y') "
      + "as Time from data) as t where t.Time > NOW() - INTERVAL "+minutes+" MINUTE ORDER BY Time DESC;";

    mysql.query(queryText,
        function(err, result, fields) {
          if (err) throw err;
          else {
            callBack(result);
          }
        });
  }  

  this.getAllNodes = function(callBack) {  
    var queryText = "select distinct Prefix from data;"
      mysql.query(queryText,
          function(err, result, fields) {
            if (err) throw err;
            else {
              callBack(result);
            }
      });
  }

  this.getAllValues = function(callBack) {
    var queryText = "select * from data order by Prefix;"
    mysql.query(queryText,
      function(err, result, fields) {
        if (err) throw err;
        else {
          callBack(result);
        }
    });
  }

  this.getAllFrom = function(nodeName, callBack) {
    var queryText = "select * from data where Prefix = '"+nodeName+"'";
    mysql.query(queryText,
        function(err, result, fields) {
          if (err) throw err;
          else {
            callBack(result);
          }
        });
  }

  this.getValuesFrom = function(nodeName, types, callBack) {
    var query  = "select * from data where Prefix = '" + nodeName + "'";
    if (types != null || types.length > 0) query += this.addFilter(types);
    mysql.query(query,
      function(err, result, fields) {
        if (err) throw err;
        else {
          callBack(result);
        }
    });
  }

  this.deleteAllValues = function(callBack) {
    var query = "delete from data";
    mysql.query(query,
      function(err, result, fields) {
        if (err) throw err;
        else {
          callBack(result);
      }
    });
  }

  this.deleteValuesFrom = function(nodeName, types, callBack) {
    var query  = "delete from data where Prefix = '" + nodeName + "'";
    if (types != null || types.length > 0) query += this.addFilter(types);
    mysql.query(query,
      function(err, result, fields) {
        if (err) throw err;
        else {
          callBack(result);
      }
    });
  }

  this.createBackup = function(callBack) {
    var d = new Date();
    var query = "create table data_bak_" + d.getTime() + " as select * from data";
    mysql.query(query, function(err, result, fields) {
      if (!err) {
        mysql.query("delete from data");
        callBack(result);
      }
    });
  }

  this.addFilter = function(types) {
    var filter = " and (Type = '" + types[0] + "'";
    for (var i = 1; i < types.length; i++) {
      filter += " or Type = '" + types[i] + "'";
    }
    filter += ");";
    return filter;
  }
}
