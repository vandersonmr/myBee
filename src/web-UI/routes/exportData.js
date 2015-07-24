/*
 * Export data and save on a file
 */

exports.exportData = function(req, res) {
  var query = req.params.mode.split('=');
  function renderize(data){
    if (query[1] === 'csv')
      res.render('exportData', { layout : false, result : data })
    else if (query[1] === 'pdf')
      res.render('exportDataPDF', { layout : false, result : data })
  }
  if (query[0] === 'all') {
    dataDAO.getAllValues(renderize);
  } else {
    var request = query[0].split('&');
    if (request.length == 0) return;
    var node_name = request.shift();
    dataDAO.getValuesFrom(node_name, request, renderize);
  }
};
