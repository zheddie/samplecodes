const {SuccessModel,ErrorModel} = require('../model/responseModel');
const {getList,getDetail,createNewBlog,updateBlog,deleteBlog} = require('../controllers/blog');

const handleBlogRoute = (req,res)=>{
    //处理路由逻辑
    const method = req.method;
    const id = req.query.id;
    const postData = req.body;
    if(method === 'POST' && req.path === '/zgprogram/search'){
        //api/blog/list?author=xxx&keyword=123
        const author = req.query.author || '';
        const keyword = req.query.keyword || '';
        const listDataPromise = getList(author,keyword);
        return listDataPromise.then((listData)=>{
            return new SuccessModel(listData);
        })
    }
    if(method === 'GET' && req.path === '/api/blog/list'){

        //api/blog/list?author=xxx&keyword=123
        const author = req.query.author || '';
        const keyword = req.query.keyword || '';
        const listDataPromise = getList(author,keyword);
        return listDataPromise.then((listData)=>{
            return new SuccessModel(listData);
        })
        //return new SuccessModel(listData);
        // return{ 
        //     message: "获取博客列表接口"
        // }
    }
    if(method === 'GET' && req.path === '/api/blog/details'){
        
        const detailData = getDetail(id);
        return new SuccessModel(detailData);

    }
    if(method === 'POST' && req.path === '/api/blog/new'){
        const newBlog =  createNewBlog(postData);
        return new SuccessModel(newBlog);
    }

    if(method === 'POST' && req.path === '/api/blog/update'){
        
        const updateData = updateBlog(id,postData);
        if(updateData){
            return new SuccessModel('Success');
        }else{
            return new ErrorModel('Success');
        }
        
    }
    if(method === 'POST' && req.path === '/api/blog/delete'){
        const deleteResut = deleteBlog(id);
        if(deleteResut){
            return new SuccessModel('Success');
        }else{
            return new ErrorModel('Success');
        }
        
    }
}

module.exports = handleBlogRoute;