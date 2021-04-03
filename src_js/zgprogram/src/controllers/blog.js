const {execSQL} = require('../db/mysql');

const getList = (author,keyword) =>{
    let sql = 'select * from blogs where 1=1 ';
    if(author){
        sql+=`and author='${author}' `;
    }
    if(keyword){
        sql+=`and title like '%${keyword}%'`;
    }
    return execSQL(sql);   

    // //get data from db. 
    // return[
    //     {
    //         id:1,
    //         title:'title1',
    //         content:'content1',
    //         author:'zhangsan',
    //         createAt: 1610555518935
    //     },
    //     {
    //         id:2,
    //         title:'title2',
    //         content:'content2',
    //         author:'zhangsan',
    //         createAt: 1610555518935
    //     }
    // ]

}
const getDetail = (id)=>{
    return{
        id:1,
        title:'title1',
        content:'content1',
        author:'zhangsan',
        createAt: 1610555518935       
    };
}
const createNewBlog = (blogData={})=>{
    console.log('blogData',blogData);
    return {
        id:1
    }
}
const updateBlog = (id,blogData={})=>{
    console.log('id',id);
    console.log('blogData',blogData);
    return true;
}
const deleteBlog = (id)=>{
    console.log('id',id);
    return true;
}
module.exports = {
    getList,
    getDetail,
    createNewBlog,
    updateBlog,
    deleteBlog

};