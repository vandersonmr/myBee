
/*
 * GET getDados
 */

exports.getHistoric = function(req, res){
  function renderize(data){
    res.render('getHistoric',{ layout : false, result : data })
  }
  dataDAO.getAllFrom(req.params.nodeName,renderize);
};
