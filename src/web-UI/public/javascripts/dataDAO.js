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
    if (types.length > 0) query += this.addFilter(types);
    console.log(query);
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
    if (types.length > 0) query += this.addFilter(types);
    console.log(query);
    mysql.query(query,
      function(err, result, fields) {
        if (err) throw err;
        else {
          callBack(result);
      }
    });
  }

  this.addFilter = function(types) {
    var filter = " and (Prefix = '" + types[0] + "'";
    for (var i = 1; i < types.length; i++) {
      filter += " or Prefix = '" + types[i] + "'";
    }
    filter += ");";
    return filter;
  }
}
