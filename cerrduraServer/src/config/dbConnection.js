// Conexión con la base de datos
const sql = require('mysql');

module.exports = () => {
	return sql.createConnection({
		host: 'localhost',
		user: 'root',
		password: 'root',
		database: 'cerraduraBD'		
		});
}
