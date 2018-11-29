const dbConnection = require('../../config/dbConnection');
var fs = require('fs');
var ursa = require('ursa');
var CryptoJS = require("crypto-js");
var base64 = require("js-base64").Base64;
var request = require('request');


var AESKey = '987E99D500AED2A6FAF7158809CF4FF9';
var bloqIV = "AAAAAAAAAAAAAA==";
var pkey = ursa.createPrivateKey(fs.readFileSync('/home/pi/Desktop/private.pem'));		  

function encrypt(msg) {
	  var msg64 = base64.encode(msg)
		var iv = CryptoJS.enc.Base64.parse(bloqIV);    
      var key= CryptoJS.enc.Hex.parse(AESKey);  	      
       
      var result = CryptoJS.AES.encrypt(msg64, key , { 	
      	iv: iv,
			padding: CryptoJS.pad.Pkcs7,
			mode: CryptoJS.mode.CBC      	
      });
  	  var pt = result.ciphertext.toString(CryptoJS.enc.Base64); 
  	  return pt;
}    	
    	  
function decrypt(msg, ivCif) {
      
      var plain_iv =  new Buffer(ivCif , 'base64').toString('hex');
      var iv = CryptoJS.enc.Hex.parse(plain_iv);
      var key= CryptoJS.enc.Hex.parse(AESKey);

      var bytesmsg  = CryptoJS.AES.decrypt(msg, key , { iv: iv});
	   var msgpt = bytesmsg.toString(CryptoJS.enc.Base64);
		var msgb64 =  new Buffer(msgpt , 'base64').toString('ascii');
		var resultado =     new Buffer(msgb64 , 'base64').toString('ascii');
    	   
    	 return resultado;  
}    	  
    	  
  module.exports = app  => {
  const connection = dbConnection();
  
   app.get('/getpublickey', (req, res) => {
  	    var pubkey =fs.readFileSync('/home/pi/Desktop/publick.pem');
	    res.send(pubkey.toString());
    });

    app.get('/prueba', (req, res) => {
		  console.log("KEYLOCK CONECTADO!\n");  
		  res.send("OK");  
    });
    
    app.get('/getestadoConexion', (req, res) => {
		 res.send("OK");
    });
  	    
  app.get('/getestado', (req, res) => {
  	 	
		var idcerradura =    decrypt(req.query.idCerradura, req.query.iv);
      var uuid =    decrypt(req.query.uuidTarjeta, req.query.iv); 
      var pin =    decrypt(req.query.pin, req.query.iv);
      
   	connection.query('SELECT estado FROM usuarios WHERE idCerradura = ? AND uuidTarjeta = ? AND pin = ?', 
		[idcerradura, uuid, pin],     	
   	(err, result) => {
   		   if(result.length == 0){
					res.send('Usuario no encontrado');   		   
   		   }else {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	for (var i = 0; i<result.length; i++) {
						  res.write(result[i].estado);
   		   	}
   		   	res.end();   		   
   		   }
   		});      	 			
  });
  
  app.get('/getusuario', (req, res) => {
     var idcerradura =    decrypt(req.query.idCerradura, req.query.iv);
      var uuid =    decrypt(req.query.uuidTarjeta, req.query.iv); 
      		  	 		
   	connection.query('SELECT user FROM usuarios WHERE idCerradura = ? AND uuidTarjeta = ?', 
		[idcerradura, uuid],     	
   	(err, result) => {
   		   if(result.length == 0){
					res.send('Usuario no encontrado');   		   
   		   }else {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	for (var i = 0; i<result.length; i++) {
						  res.write(result[i].user);
   		   	}
   		   	res.end();   		   
   		   }
   		});      	 			
  });

  app.get('/notificacion', (req, res) => {
      var idcerradura =    decrypt(req.query.idDispositivo, req.query.iv);
      var uuid =    decrypt(req.query.uuidTarjeta, req.query.iv); 
      var tipo = req.query.tipo;
      
      console.log("Enviando petición de notificación de acceso del tipo: ", tipo);
      		  	 		
   	connection.query('SELECT user FROM usuarios WHERE idCerradura = ? and uuidTarjeta = ?',
   	 [idcerradura, uuid],		     	
   	(err, result) => {
   		   if(result.length == 0){
					res.send('Usuario no encontrado');   		   
   		   }else {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   	
					var headers = {
						'Content-Type': 'text/plain'					
					}
					var options = {
						url : 'http://192.168.1.35:8080/KeyLockServer/api/keylock/notificacion?idDispositivo='+idcerradura+'&uuid='+uuid+'&tipo='+tipo,
						method : 'GET',
						jar : true,
						headers : headers
					}
					
					request(options, function (error, response, body) {
								if (!error && response.statusCode == 200) {
									console.log(body);		
								}						
						});
   					
   		   	res.end();   		   
   		   }
   		});      	 			
  });

  app.get('/getusuarioseliminados', (req, res) => {
  	 	console.log("prueba");
   	connection.query('SELECT * FROM usuariosEliminados',      	
   	(err, result) => {
   		   if(result.length == 0){
					res.send('Usuario no encontrado');   		   
   		   }else {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	for (var i = 0; i<result.length; i++) {
   		   		  usuario = result[i].user;
						  res.write(usuario + "\n");
						  
						  connection.query('DELETE FROM usuariosEliminados WHERE user=?',
						  [usuario], (err, result) =>{});
   		   	}
   		   	res.end();   		   
   		   }
   	});      	 			
  });

    app.get('/getnumusuarios', (req, res) => {
  	 	cont  = 0;
      var idcerradura =    decrypt(req.query.idCerradura, req.query.iv);
     	
   	connection.query('SELECT * FROM usuarios WHERE idCerradura = ?', 
		[idcerradura],     	
   	(err, result) => {
   		   if(result.length == 0){
					res.send('Usuario no encontrado');   		   
   		   }else {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	for (var i = 0; i<result.length; i++) {
						  cont = cont + 1;   		   		  					
					}
					res.write(cont.toString());
   		   	res.end();   	   
   		   }
   		});      	 	
  });  

    app.get('/getnumusuariosmod', (req, res) => {
  	 	cont  = 0;
      var idcerradura =    decrypt(req.query.idCerradura, req.query.iv);
      	
   	connection.query('SELECT * FROM usuariosModificados WHERE idCerradura = ?', 
		[idcerradura],     	
   	(err, result) => {
   		   if(result.length == 0){
					res.send('Usuario no encontrado');   		   
   		   }else {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	for (var i = 0; i<result.length; i++) {
						  cont = cont + 1;   		   		  					
					}
					res.write(cont.toString());
   		   	res.end();   	   
   		   }
   		});      	 	
  });  

app.get('/actualizacion', (req, res) => {
	   contador = req.query.cont;
  	 	cont = 1; 

       var idcerradura =    decrypt(req.query.idCerradura, req.query.iv);
      	
   	connection.query('SELECT * FROM usuariosModificados WHERE idCerradura = ?', 
		[idcerradura],     	
   	(err, result) => {
   		   if(result.length == 0){
					res.send('Usuario no encontrado');   		   
   		   }else {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	for (var i = 0; i<result.length; i++) {
						  if (cont == contador) {
								
								var usuario = encrypt(result[i].user);
								var idcerraduraCif = encrypt(result[i].idCerradura);
								var uuidCif = encrypt(result[i].uuidTarjeta);
								var pinCif = encrypt(result[i].pin);
								var preferencia = result[i].preferencia;
														  	
								res.write(usuario + "\n" +
								idcerraduraCif + "\n" +
								uuidCif + "\n" +
								pinCif + "\n" +
								preferencia + "\n");				
								
		                  connection.query('DELETE FROM usuariosModificados WHERE user=?',
						      [result[i].user], (err, result) =>{});		  
						  }
						  cont = cont + 1;
   		   	}
   		   	res.end();   		   
   		   }
   		});      	 			
  });

  app.get('/modificarnomdisp', (req, res) => {
  	 	idcerradura = req.query.idCerradura;
  	 	uuid = req.query.uuidTarjeta;
  	 	nomCerradura = req.query.nomDisp;
		pin = req.query.pin;

		var idcerradurad = pkey.decrypt(idcerradura,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
      var uuidd = pkey.decrypt(uuid,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
		var nomCerradurad = pkey.decrypt(nomCerradura,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
      var pind = pkey.decrypt(pin,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
   	
   	connection.query('UPDATE usuarios SET nomCerradura=? WHERE idCerradura=? AND uuidTarjeta=? AND pin = ?', 
		[nomCerradurad, idcerradurad, uuidd, pind],     	
   	(err, result) => {
   		   if(err){
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	res.write("Ya existe ese dispositivo con ese nombre!");
   		   	res.end();   					   		   
   		   }
   		   if (!err) {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	res.write("Datos del dispositivo modificado!");
   		   	res.end();		   		   
   		   }
   		});      	 			
  });
  
  app.get('/modificarpindisp', (req, res) => {
	   var user;  	 	
	   var preferencia;
	   
  	 	idcerradura = req.query.idCerradura;
  	 	uuid = req.query.uuidTarjeta;
  	 	pin = req.query.pin;

		var idcerradurad = pkey.decrypt(idcerradura,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
      var uuidd = pkey.decrypt(uuid,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
		var pind = pkey.decrypt(pin,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);

   	connection.query('UPDATE usuarios SET pin=? WHERE idCerradura=? AND uuidTarjeta=?', 
		[pind, idcerradurad, uuidd],     	
   	(err, result) => {
   		   if (!err) {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	res.write("Datos del dispositivo modificado!");
   		   	res.end();
   		   	  
   				connection.query('SELECT * FROM usuarios WHERE idCerradura = ? and uuidTarjeta = ?', 
					[idcerradurad, uuidd],     	
   				(err, result) => {
   		   		if(result.length == 0){
							res.send('Usuario no encontrado');   		   
   		   		}else {
   		   			for (var i = 0; i<result.length; i++) {
									user = result[i].user;
								   preferencia = result[i].preferencia;					 	  
   		   			}			 
   		   		}
   				});    
   		   	 connection.query('SELECT * FROM usuariosModificados WHERE idCerradura = ? AND uuidTarjeta = ?', 
			       [idcerradurad, uuidd], (err, result) => { 	
			       		if(result.length != 0){
									  connection.query('UPDATE usuariosModificados SET preferencia = ? WHERE idCerradura = ? and uuidTarjeta = ?',
				                 [preferencia, idcerradurad, uuidd], (err, result) =>{});  
							}  else {
   		               	connection.query('INSERT INTO usuariosModificados(user, idCerradura, uuidTarjeta, pin, preferencia) VALUE(?,?,?,?,?)',
				               [user, idcerradurad, uuidd, pind, preferencia], (err, result) =>{});  							
							}
			       });   
   		   }
   		});      	 			
  });  
  
  app.get('/setestado', (req, res) => {
  	 	idcerradura = req.query.idCerradura;
  	 	uuid = req.query.uuidTarjeta;
  	 	pin = req.query.pin;
  	 	estado = req.query.estado;

		var idcerradurad = pkey.decrypt(idcerradura,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
      var uuidd = pkey.decrypt(uuid,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
		var pind = pkey.decrypt(pin,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
		var estadod = pkey.decrypt(estado,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
		
		console.log("Cambiando estado del dispositivo a: ", estadod);		
		
   	connection.query('UPDATE usuarios SET estado = ? WHERE idCerradura = ? AND uuidTarjeta = ? AND pin =?', 
		[estadod, idcerradurad, uuidd, pind],     	
   	(err, result) => {
   		   if(result.length == 0){
					res.send('Usuario no encontrado');   		   
   		   }else {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	res.write("Ok");
   		   	res.end();   		   
   		   }
   		});      	 			
  });  
  
    app.get('/setpreferencia', (req, res) => {
		var user;  	 	
		var preferencia;
		
  	 	idcerradura = req.query.idCerradura;
  	 	uuid = req.query.uuidTarjeta;
  	 	pin = req.query.pin;
  	 	pref= req.query.estado;
  	 		
  	 	var idcerradurad = pkey.decrypt(idcerradura,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
      var uuidd = pkey.decrypt(uuid,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
		var pind = pkey.decrypt(pin,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
		var preferenciad = pkey.decrypt(pref,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
		console.log("ESTABLECIENDO PREFERENCIA: " , preferenciad);
			
   	connection.query('UPDATE usuarios SET preferencia = ? WHERE idCerradura = ? AND uuidTarjeta = ? AND pin =?', 
		[preferenciad, idcerradurad, uuidd, pind],     	
   	(err, result) => {
   		   if(result.length == 0){
					res.send('Usuario no encontrado');   		   
   		   }else {
   		   	res.writeHead(200, { 'Content-Type': 'text/plain' });
   		   	res.write("OK");
   		   	res.end();   		   
   		   	
   		   	connection.query('SELECT * FROM usuarios WHERE idCerradura = ? and uuidTarjeta = ?', 
					[idcerradurad, uuidd],     	
   				(err, result) => {
   		   		if(result.length == 0){
							res.send('Usuario no encontrado');   		   
   		   		}else {
   		   			for (var i = 0; i<result.length; i++) {
									user = result[i].user;
									preferencia = result[i].preferencia;					 	  
   		   			}			 
   		   		}
   				});    
   		   	  connection.query('SELECT * FROM usuariosModificados WHERE idCerradura = ? AND uuidTarjeta = ?', 
			       [idcerradurad, uuidd], (err, result) => { 	
			       		if(result.length != 0){
									 connection.query('UPDATE usuariosModificados SET preferencia = ? WHERE idCerradura = ? and uuidTarjeta = ?',
				                 [preferencia, idcerradurad, uuidd], (err, result) =>{});  
							}  else {
   		                 	 connection.query('INSERT INTO usuariosModificados(user, idCerradura, uuidTarjeta, pin, preferencia) VALUE(?,?,?,?,?)',
				               [user, idcerradurad, uuidd, pind, preferencia], (err, result) =>{});  						
				               console.log(preferencia);	
							}
			       });
   		   }
   		});      	 			
  });  
    
  app.get('/autenticacion', (req, res) => {
  	 	idcerradura = req.query.idCerradura;
  	 	uuid = req.query.uuidTarjeta;
  	 	pin = req.query.pin;
  	 	

		var idcerradurad = pkey.decrypt(idcerradura,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
      var uuidd = pkey.decrypt(uuid,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);
		var pind = pkey.decrypt(pin,'base64', 'utf8', ursa.RSA_PKCS1_PADDING);

		console.log("Autenticando dispositivo: ");
		console.log(idcerradurad);
		console.log(uuidd);
		console.log(pind);
		console.log("\n");

   	connection.query('SELECT * FROM usuarios where idCerradura = ? AND uuidTarjeta = ? AND pin  = ?', 
		[idcerradurad, uuidd, pind],     	
   	(err, result) => {
   		   if(result.length == 0){
					res.send('DISPOSITIVO NO AUTENTICADO');   		   
   		   }else {
   		   	res.send('DISPOSITIVO AUTENTICADO');   		   
   		   }
   		});      	 			
  });      
    
}