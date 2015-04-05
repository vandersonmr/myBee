
/*
 * GET getDados
 */
exports.getStatus = function(req, res){
  function renderize(data){
    res.render('getStatus',{ layout : false, result : data })
  }
  dataDAO.getLastStatus(req.params.id, renderize);
};
