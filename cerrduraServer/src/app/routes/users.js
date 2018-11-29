const dbConnection = require('../../config/dbConnection');
var sha512 = require('js-sha512');
var sha256 = require('js-sha256');

module.exports = app  => {
	const connection = dbConnection();
 
  app.get('/', (req, res) => {
  			console.log("Nuevo cliente conectado! \n");	    
  	      req.session.nombre = null;
  	      registrado = false;
  	      errorRegistro = false;
  	      entrar = true;
  	 		res.render('users/login');
  });
  
  app.get('/alta', (req, res) => {
  			console.log("Nuevo cliente conectado! \n");	    
  	      req.session.nombre = null;
  	      registrado = false;
  	      errorRegistro = false;
  	      entrar = true;
  	 		res.render('users/inicio');
  });  
  
  app.get('/login', (req, res) => {      	
			req.session.nombre = null;      	
      	entrar = true;
  	 		res.render('users/login');
  });
 
   app.post('/userlog',(req,res) => {
  	
   	connection.query('SELECT * FROM administradores WHERE user = ? and password = ?', 
		[req.body.user, req.body.password],     	
   	(err, result) => {
   		   if(result.length == 0){
   		   	req.session.nombre = null;
					entrar = false;
					res.render('users/login');   		   
   		   }else {
   		   	req.session.nombre = req.body.user;
   		   	entrar = true;
   		 		user = 0; 	
		       	mostrarU = false; mostrarD = false;
					res.render('users/users');   		   
   		   }
   		});    
   }); 
 
	app.post('/userSign', (req, res) => {		
   
    var pinCifrado = sha256.create();
    pinCifrado.update(req.body.pin);    
    var uuidCifrado = sha256.create();
    uuidCifrado.update(req.body.uuidTarjeta);
    var idCerraduraCifrado = sha256.create();
    idCerraduraCifrado.update(req.body.idCerradura);
    
    connection.query('INSERT INTO usuarios(user, nomCerradura, idCerradura, pin, uuidTarjeta, nombre, apellidos, email) values(?,?,?,?,?,?,?,?)', 
		[req.body.user, req.body.nomCerradura,idCerraduraCifrado.hex(), pinCifrado.hex(), uuidCifrado.hex(), req.body.nombre, req.body.apellidos, req.body.email] ,
		(err,result) => {

			if(err != null) {		
				errorRegistro = true;
				registrado = false;
				res.render('users/inicio');			
			} else {
	 			registrado = true;	
	 			errorRegistro = false;	
	   	   res.render('users/inicio');
			}
		});		
	});
 
    app.post('/listarUsers', (req, res) => {
    if(req.session.nombre) {  	 
  	 connection.query('SELECT * FROM usuarios', (err, result) => {
  	 	   if (result.length == 0) {
				mostrarU = false;
				mostrarD = false;  	 	
				res.render('users/users');
  	 	   } else {
		   	mostrarU = true;
		   	mostrarD = false;
  	 		   res.render('users/users', {
				user: result		 	 		
  	 		});
  	 	}
  	 });
  	} else {
  		res.render("users/login");  	
  	}
  }); 
  
  app.post('/bajaUser' , (req, res) => {
  if(req.session.nombre) {  	   		
  		const {userBaja} = req.body;
		connection.query('INSERT INTO usuariosEliminados(user) value(?)',   [req.body.userBaja] );  		
  		connection.query('DELETE FROM usuarios WHERE ?', {
		   user: userBaja		
  		}, (err, result) => { 			
   	   connection.query('SELECT * FROM usuarios', (err, result) => {
  	 	   if (result.length == 0) {
				result = 0;				
				mostrarU = false;
				mostrarD = false;  	 	
				res.render('users/users');
  	 	   } else {
		   	mostrarU = true;
		   	mostrarD = false;
  	 		   res.render('users/users', {
				user: result		 	 		
  	 	   	});
  	    	}
  	    });
  	  });  
  } else {
	res.render("users/login");  
  }
  });
  
   app.post('/bajaCerradura' , (req, res) => {
	if(req.session.nombre) {  	 		  		
  		const {user} = req.body;
  		connection.query('delete from usuarios WHERE nomCerradura = ? and user = ?',
  		   [req.body.nomCerradura, req.body.user]  
		   , (err, result) => { 		
			connection.query('SELECT * FROM usuarios WHERE ?', {
		   user: user		
  		}, (err, result) => { 			
   		if(result.length == 0){
				result = 0;
				mostrarU = false;
				mostrarD = false;
				res.render('users/users');   			
   		}else {
   				mostrarU = false;
   		   	mostrarD = true;
  	 		      res.render('users/users', {
			   	cerraduras: result		 	 		
  	 	   	});
   		}
  	  });  
    });
 } else {
	res.render("users/login");
 }
});
  
  app.post('/listarCerraduras' , (req, res) => {
  if(req.session.nombre) {  	 		
  		const {user} = req.body;
  		connection.query('SELECT * FROM usuarios WHERE ?', {
		   user: user		
  		}, (err, result) => { 			
   		if(result.length == 0){
				result = 0;
				mostrarU = false;
				mostrarD = false;
				res.render('users/users');   			
   		}else {
   				mostrarU = false;
   		   	mostrarD = true;
  	 		      res.render('users/users', {
			   	cerraduras: result		 	 		
  	 	   	});
   		}
  	  });  
  } else {
	res.render("users/login");  
  }	  
});

  app.post('/allUsersOn' , (req, res) => {
  if(req.session.nombre) {  	   
  		connection.query('UPDATE usuarios SET estado = "on"' ,(err, result) => { 
			mostrarU = false;
			mostrarD = false;  		
  		   res.render("users/users") });
  	} 	else {
	res.render("users/login");  
  }	  	
});

app.post('/allUsersOff' , (req, res) => {
   if(req.session.nombre) {  	   
  		connection.query('UPDATE usuarios SET estado = "off"' ,(err, result) => { 
			mostrarU = false;
			mostrarD = false;  		
  		   res.render("users/users") });
  	} 		else {
	res.render("users/login");  
  }	  
});
  
  app.post('/cambiarEstado' , (req, res) => {
  if(req.session.nombre) {  	   
  		const {user} = req.body;
  		connection.query('UPDATE usuarios SET estado = ? WHERE nomCerradura = ? and user = ?',
  		   [req.body.estado, req.body.nomCerradura, req.body.user]  
		   , (err, result) => { 		
			connection.query('SELECT * FROM usuarios WHERE ?', {
		   user: user		
  		}, (err, result) => { 			
   		if(result.length == 0){
				result = 0;
				mostrarU = false;
				mostrarD = false;
				res.render('users/users');   			
   		}else {
   				mostrarU = false;
   		   	mostrarD = true;
  	 		      res.render('users/users', {
			   	cerraduras: result		 	 		
  	 	   	});
   		}
  	  });  
    });
  } else {
	res.render("users/login");
}    
});

  app.post('/cambiarPreferencia' , (req, res) => {
  if(req.session.nombre) {  	   
  		   const {user} = req.body;
  	   	connection.query('UPDATE usuarios SET preferencia = ? WHERE nomCerradura = ? and user = ?',
  		   [req.body.estado, req.body.nomCerradura, req.body.user]  
		   , (err, result) => { 		
			connection.query('SELECT * FROM usuarios WHERE ?', {
		   user: user		
  		}, (err, result) => { 			
   		if(result.length == 0){
				result = 0;
				mostrarU = false;
				mostrarD = false;
				res.render('users/users');   			
   		}else {
   			for (var i = 0; i<result.length; i++) {
						var preferencia = result[i].preferencia;
						var uuidTarjeta = result[i].uuidTarjeta;
						var idCerradura = result[i].idCerradura;
						var pin = result[i].pin;					 	  
   		   }	
   				 connection.query('SELECT * FROM usuariosModificados WHERE idCerradura = ? AND uuidTarjeta = ?', 
			       [idCerradura, uuidTarjeta], (err, result) => { 	
			       		if(result.length != 0){
									 connection.query('UPDATE usuariosModificados SET preferencia = ? WHERE idCerradura = ? and uuidTarjeta = ?',
				                 [preferencia, idCerradura, uuidTarjeta], (err, result) =>{});  
							}  else {
   		                 	 connection.query('INSERT INTO usuariosModificados(user, idCerradura, uuidTarjeta, pin, preferencia) VALUE(?,?,?,?,?)',
				               [user, idCerradura, uuidTarjeta, pin, preferencia], (err, result) =>{});  							
							}
			       });
						   			 			   			
   				mostrarU = false;
   		   	mostrarD = true;
  	 		      res.render('users/users', {
			   	cerraduras: result		 	 		
  	 	   	});
   		}
  	  });  
    });
  } else {
	res.render("users/login");
}    
});
  
}
