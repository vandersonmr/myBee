
/**
 * Module dependencies.
 */

var express  =  require('express')
  , routes   =  require('./routes')
  , getDados =  require('./routes/getDados')
  , getNodes =  require('./routes/getNodes')
  , _mysql   =  require('mysql')
  , dataDB   =  require('./public/javascripts/dataDAO');

// DataBase - Mysql

var HOST       = 'localhost';
var PORT       = 3306;
var MYSQL_USER = 'root';
var MYSQL_PASS = '123456';
var DATABASE   = 'monitorAbelhas';

mysql = _mysql.createConnection({
  host: HOST,
  port: PORT,
  user: MYSQL_USER,
  password: MYSQL_PASS,
});

mysql.connect(function(err) {
  if (err) throw err;    // connected! (unless `err` is set)
});

mysql.query('use ' + DATABASE);

// Instaciation
var app     = module.exports = express.createServer();
dataDAO = new dataDB.dataDAO(mysql); 

// Configuration

app.configure(function(){
  app.set('views', __dirname + '/views');
  app.set('view engine', 'ejs');
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(app.router);
  app.use(express.static(__dirname + '/public'));
});

app.configure('development', function(){
  app.use(express.errorHandler({ dumpExceptions: true, showStack: true }));
});

app.configure('production', function(){
  app.use(express.errorHandler());
});

// Routes

app.get('/'        , routes.index);
app.get('/getDados', getDados.getDados);
app.get('/getNodes', getNodes.getNodes);

//Extra

app.listen(3000, function(){
  console.log("Express server listening on port %d in %s mode", app.address().port, app.settings.env);
});
