exports.dataDAO = function(mysql) {

  this.getLastStatus = function(id, callBack) {  
    var queryText = "select Value from data where Prefix='" + id + "' order by Date desc limit 1;"

    mysql.query(queryText,
        function(err, result, fields) {
          if (err) throw err;
          else {
            callBack(result);
          }
        });
  }  
}
