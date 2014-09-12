/*
 * Export data and save on a file
 */

exports.exportData = function(req, res) {
  function renderize(data){
    res.render('exportData', { layout : false, result : data })
  }
  dataDAO.runQuery(req.params.query, renderize);
};
