
/**
 * Module dependencies.
 */

var express     =  require('express')
  , routes      =  require('./routes')
  , toggleTv    =  require('./routes/toggleTv')
  , net         =  require('net')
  , sleep	=  require('sleep');

// Instaciation
var app = module.exports = express.createServer();

var HOST = '192.168.1.189';
var PORT = '9000';

client = new net.Socket();

client.on('error', function(err) {
  console.log(err);
  sleep.sleep(10);
  client.connect(PORT, HOST);
});

client.connect(PORT, HOST);

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

app.get('/'                     , routes.index);
app.get('/toggleTv/:id'         , toggleTv.toggleTv);

//Extra

app.listen(3000, function(){
  console.log("Express server listening on port %d in %s mode", app.address().port, app.settings.env);
});
