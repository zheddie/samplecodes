use myblog;

insert into blogs (title,content,author,createat) values('title1','content1','zhangsan',1234123412343);

insert into blogs (title,content,author,createat) values('title2','content2','zhangsan',1234123412366);

insert into blogs (title,content,author,createat) values('title3','content3','lisi',1454123412366);

insert into blogs (title,content,author,createat) values('title4','content4','wangwu',1454128812366);


select id,author from blogs;


use mysql;
select * from user;

ALTER USER 'root' IDENTIFIED WITH mysql_native_password BY 'passw0rd';

flush privileges;