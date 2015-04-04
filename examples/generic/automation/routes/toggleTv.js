
/*
 * GET home page.
 */

exports.toggleTv = function(req, res){
  function renderize(data) {
    res.render('layoutEmpty', {layout: false, result: data})
  }
  client.write('client-room1&toggle-tv1');
  renderize(null);
};
