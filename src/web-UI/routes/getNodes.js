
/*
 * GET getDados
 */

exports.getNodes = function(req, res){
  function renderize(data){
    res.render('getNodes',{ layout : false, result : data })
  }
  dataDAO.getAllNodes(renderize);
};
