const mysql = require('mysql');
const {MYSQL_CONFIG} = require('../config/db');
//create connection
const conn = mysql.createConnection(MYSQL_CONFIG);

conn.connect();

// function execSQL(sql,callback){
//     conn.query(sql,callback);
// }

function execSQL(sql){
    const promise = new Promise((resolve,reject)=>{
        conn.query(sql,(err,result)=>{
            if(err){
                reject(err);
                return;
            }
            resolve(result);
        })
    });
    return promise;
}
module.exports = {execSQL};