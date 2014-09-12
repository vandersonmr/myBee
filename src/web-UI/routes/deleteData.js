/*
 * Export data and save on a file
 */

exports.deleteData = function(req, res) {
  function renderize(data){
    res.render('layoutEmpty', { layout : false, result : data })
  }
  dataDAO.runQuery(req.params.query, renderize);
};
