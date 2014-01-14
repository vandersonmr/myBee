
/*
 * GET getDados
 */

exports.getDados = function(req, res){
  function renderize(data){
    res.render('getDados',{ layout : false, result : data })
  }
  dataDAO.getLastsDatasByMinute(2, renderize);
};
