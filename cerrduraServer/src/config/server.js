var express = require('express')
var path = require('path');
var bodyParser = require('body-parser');
var session = require('express-session');
var app = express();

// Configuración del puerto.
app.set('port', process.env.PORT || 8000);
app.set('view engine','ejs');
app.set('views',path.join(__dirname,'../app/views'));

//Middleware.
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended: false}));
app.use(session({
	secret: "123hdaszx323jj2ub",
	resave: false,
	saveUninitialized: false
}));


module.exports = app;
