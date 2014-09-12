/*
 * Export data and save on a file
 */

exports.exportData = function(req, res) {
  function renderize(data){
    res.render('exportData', { layout : false, result : data })
  }
  if (req.params.mode === 'all') {
    dataDAO.getAllValues(renderize);
  } else {
    var request = req.params.mode.split('&');
    if (request.length == 0) return;
    var node_name = request.shift();
    dataDAO.getValuesFrom(node_name, request, renderize);
  }
};
