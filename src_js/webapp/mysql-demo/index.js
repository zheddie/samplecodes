const mysql = require('mysql');
//create connection
const conn = mysql.createConnection({host:'mysqlserver',user:'root',password:'passw0rd',port:3306,database:'myblog'});

conn.connect();
const sql = 'select * from blogs';
const sqlinsert = "insert into blogs (title,content,author,createat) values('title5','content5','wangwu',1454188812366);"
conn.query(sqlinsert,(err,result)=>{
    if(err){
        console.error('error:',err);
        return;
    }
    console.log('result:',result);
});
conn.end();