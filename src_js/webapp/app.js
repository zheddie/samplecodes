const querystring = require('querystring');
const handleBlogRoute = require('./src/routes/blog');

const getPostData = (req)=>{
    const promise = new Promise((resolve, reject)=>{
        if(req.method !== 'POST'){
            resolve({});
            return;
        }
        if(req.headers['content-type'] !== 'application/json'){
            resolve({});
            return;
        }
        let postData = '';
        req.on('data',(chunk)=>{
            postData+=chunk.toString();
        });
        req.on('end',()=>{
            //console.log('postData',postData);
            if(!postData){
                resolve({});
                return;
            }
            resolve(JSON.parse(postData));
            //res.end('Data receive DONE!');
        });
    });
    return promise;
}
const serverHandler = (req,res)=>{
    res.setHeader('Content-Type', 'application/json');
    const url = req.url;
    req.path = url.split('?')[0];
    req.query = querystring.parse(url.split('?')[1]);
    //处理POST数据，这个是异步数据处理
    getPostData(req).then((postData)=>{
        req.body = postData;
        //博客相关的路由
        const blogDataPromise = handleBlogRoute(req,res);
        if(blogDataPromise){
            blogDataPromise.then((blogData)=>{
                res.end(
                    JSON.stringify(blogData)
                    );
            });
            return;
        }
        res.writeHead(404,{"Content-Type":'text/plain'});
        res.write('404 not found');
        res.end();
    });

    

    // const responseData = {
    //     name: 'zg',
    //     zge: 20
    // };
    // res.end(
    //     JSON.stringify(responseData)
    // );
}


module.exports = serverHandler;