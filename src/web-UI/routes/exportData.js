/*
 * Export data and save on a file
 */

exports.exportData = function(req, res) {
  var query = req.params.mode.split('=');
  function renderize(data){
    
    /* calculo da temperatura e humidade media */
    var temperatura_total = 0.0;
    var humidade_total = 0.0;
    var qtde_temp = 0;
    var qtde_hum = 0;
    var values = [];
    for (var i = 0; i < data.length; i++) {
      values.push({"tipo": data[i].Type, "valor": data[i].Value});
      if (data[i].Type === 'temperature') {
        temperatura_total += parseFloat(data[i].Value);
        qtde_temp += 1;
      } else if (data[i].Type === 'humidity') {
        humidade_total += parseFloat(data[i].Value);
        qtde_hum += 1;
      }
    }

    /* calculo da variancia da temperatura e humidade */
    var temperatura_variancia = 0.0;
    var humidade_variancia = 0.0;
    for (var i = 0; i < data.length; i++) {
      if (data[i].Type === 'temperature') {
        temperatura_variancia += Math.pow(parseFloat(data[i].Value)
            - (temperatura_total / qtde_temp), 2);
      } else if (data[i].Type === 'humidity') {
        humidade_variancia += Math.pow(parseFloat(data[i].Value)
          - (humidade_total / qtde_hum), 2);

      }
    }
    temperatura_variancia /= qtde_temp;
    humidade_variancia /= qtde_temp;

    var data_json = {
      data: data,
      temperatura_media: temperatura_total / qtde_temp,
      temperatura_variancia: temperatura_variancia,
      temperatura_desvio: Math.sqrt(temperatura_variancia),
      humidade_media: humidade_total / qtde_hum,
      humidade_variancia: humidade_variancia,
      humidade_desvio: Math.sqrt(humidade_variancia)
    };

    if (query[1] === 'csv')
      res.render('exportData', { layout : false, result : data_json })
    else if (query[1] === 'pdf') {
      var columns = [
        {title: "Tipo", key: "tipo"},
        {title: "Valor", key: "valor"}
      ];
      res.send({columns: columns, values: values});
    }
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
