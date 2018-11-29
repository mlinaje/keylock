// Archivo que inicia la app
const app = require('./config/server');
var https = require('https');
var fs = require('fs');
var path = require('path');

require('./app/routes/users')(app);
require('./app/routes/cerraduras')(app);

// Iniciamos el servidor.
var privateKey = fs.readFileSync('/home/pi/Desktop/CerraduraServer/src/private.key');
var certificate = fs.readFileSync('/home/pi/Desktop/CerraduraServer/src/server.crt');
var credentials = {key: privateKey, cert: certificate};

var httpsServer = https.createServer(credentials, app);
httpsServer.listen(443);
var http = require('http');
http.createServer(app).listen(8000);
var forcessl = require('express-force-ssl');
app.use(forcessl);

console.log('Node express server started');

