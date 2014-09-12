/*
 * Export data and save on a file
 */

exports.deleteData = function(req, res) {
  function renderize(data){
    res.render('layoutEmpty', { layout : false, result : data })
  }
  if (req.params.mode === 'all') {
    dataDAO.getAllNodes(renderize);
  } else {
    var request = req.params.mode.split('&');
    if (request.length == 0) return;
    var node_name = request.shift();
    dataDAO.deleteValuesFrom(node_name, request, renderize);
  }
};
