/*
 * Export data and save on a file
 */

exports.exportData = function(req, res) {
  var query = req.params.mode.split('&');
  var node = query[0].split('=')[1];
  var format = query[1].split('=')[1];
  var period = parseInt(query[2].split('=')[1]) - 1;
  function renderize(data){
    
    /* calculo da temperatura e umidade media */
    var temperatura_total = 0.0;
    var umidade_total = 0.0;
    var qtde_temp = 0;
    var qtde_um = 0;
    var values = [];
    for (var i = 0; i < data.length; i++) {
      values.push({"data": data[i].Date,"tipo": data[i].Type, "valor": data[i].Value});
      if (data[i].Type === 'temperature') {
        temperatura_total += parseFloat(data[i].Value);
        qtde_temp += 1;
      } else if (data[i].Type === 'humidity') {
        umidade_total += parseFloat(data[i].Value);
        qtde_um += 1;
      }
    }

    /* calculo da variancia da temperatura e umidade */
    var temperatura_variancia = 0.0;
    var umidade_variancia = 0.0;
    for (var i = 0; i < data.length; i++) {
      if (data[i].Type === 'temperature') {
        temperatura_variancia += Math.pow(parseFloat(data[i].Value)
            - (temperatura_total / qtde_temp), 2);
      } else if (data[i].Type === 'humidity') {
        umidade_variancia += Math.pow(parseFloat(data[i].Value)
          - (umidade_total / qtde_um), 2);

      }
    }
    temperatura_variancia /= qtde_temp;
    umidade_variancia /= qtde_temp;

    var values2 = [];    
    values2.push({"tipo": 'temperature', "media": temperatura_total / qtde_temp, "variancia": temperatura_variancia, "dp": Math.sqrt(temperatura_variancia)});
    values2.push({"tipo": 'humidity', "media": umidade_total / qtde_um, "variancia": umidade_variancia, "dp": Math.sqrt(umidade_variancia)}); 

    var data_json = {
      data: data,
      temperatura_media: temperatura_total / qtde_temp,
      temperatura_variancia: temperatura_variancia,
      temperatura_desvio: Math.sqrt(temperatura_variancia),
      umidade_media: umidade_total / qtde_um,
      umidade_variancia: umidade_variancia,
      umidade_desvio: Math.sqrt(umidade_variancia)
    };

    if (format === 'csv')
      res.render('exportData', { layout : false, result : data_json })
    else if (format === 'pdf') {
      var columns = [
	{title: "Data", key: "data"},
        {title: "Tipo", key: "tipo"},
        {title: "Valor", key: "valor"}
      ];
      var columns2 = [
	{title: "Tipo", key: "tipo"},
	{title: "Media", key: "media"},
	{title: "Variancia", key: "variancia"},
	{title: "Desvio Padrao", key: "dp"}
      ];
      res.send({columns: columns, columns2: columns2, values: values, values2: values2});
    }
    else if (format === 'txt')
	res.render('exportDataTXT', { layout : false, result : data_json })
  }
  if (node === 'all') {
    dataDAO.getAllValuesByPeriod(period, renderize);
  } else {
    var request = node.split(',');
    if (request.length == 0) return;
    var node_name = request.shift();
    dataDAO.getValuesFromByPeriod(node_name, request, period, renderize);
  }
};
