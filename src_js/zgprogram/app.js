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
const htmlWriteLeft = (req,res)=>{
    const leftStr = ''+
    '<tr><td width="20%">'+
    '<table >'+
      '<tr><td>2SearchResult:</td></tr>' +
      '<tr ><td></td></tr>' +
    '</table></td>';
  res.write(leftStr);
}
const htmlWriteMain = (req,res)=>{
    const mainStr = ''+
    '<td width="80%">' +
    '<table> <tr>' +
        '<td width="70%"><button id="save" class="btn btn-primary" onclick="save()" type="button">Save</button></td>' +
        '<td width="5%"><button id="preview" class="btn btn-primary" onclick="preview()" type="button">Preview</button></td>' +
        '<td width="5%"><button id="edit" class="btn btn-primary" onclick="edit()" type="button">Edit</button></td>' +
      '</tr><tr>'+
      
        '<td colspan=3><div class="zclick2edit"></div></td>' +
      '</tr>    </table>    </td></tr></table>';
    res.write(mainStr);
}
const htmlWriteTop = (req,res)=>{

    const inputValue = '';
    if(req.inputValue){
        inputValue = req.inputValue;
    }
    const topstr = '<body>'+
    '<table width="100%" >'+
    '  <tr><td colspan="2">        <form id="form_search" method="post" action="/zgprogram">'+
    '    <button  id="go" class="btn btn-primary" onclick="go()" type="button">GO!</button>' +
        '<input id="searchinput" type="text" size=120 on_click="si_click()" value="' +inputValue +
        '"> </input>' +
        '<input type="checkbox"id="cbtitle" onclick="check_title()">Title</input>' +
        '<input type="checkbox"id="cbtags" onclick="check_title()">Tags</input>' +
    '</form></td></tr>';
    res.write(topstr);
}
const htmlWriteHead = (res)=>{
    const headstr = ''+
    '<html lang="en">' +
    '<head>' +
      '<meta charset="UTF-8">'+
      '<title>My memory</title>'+
      '<script src="https://code.jquery.com/jquery-3.5.1.min.js" crossorigin="anonymous"></script>' +
      '<script src="https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js" integrity="sha384-Q6E9RHvbIyZFJoft+2mJbHaEWldlvI9IOYy5n3zV9zzTtmI3UksdQRVvoxMfooAo" crossorigin="anonymous"></script>' +
  
      '<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css" integrity="sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh" crossorigin="anonymous">' +
      '<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/js/bootstrap.min.js" integrity="sha384-wfSDF2E50Y2D1uUdj0O3uMBJnjuUD4Ih7YwaYd1iqfktj0Uod8GCExl3Og8ifwB6" crossorigin="anonymous"></script>' +
  
      '<link href="summernote.dist/summernote-bs4.min.css" rel="stylesheet">' +
      '<script src="summernote.dist/summernote-bs4.min.js"></script>' +
    '</head>';
    res.write(headstr);  
}
const serverHandler = (req,res)=>{
    const method = req.method;
    const url = req.url;
    req.path = url.split('?')[0];
    console.log("before zgprogram");
    if(method === 'GET' &&req.path === '/zgprogram'){
        res.setHeader('Content-Type', 'text/html');
        htmlWriteHead(res);
        htmlWriteTop(req,res);
        htmlWriteLeft(req,res);
        htmlWriteMain(req,res);
        // res.write("hello!!!");
        res.end();
    }
    if(method === 'GET' && req.path === '/zgprogram/home'){
        res.setHeader('Content-Type', 'text/html');
        res.write(' \
        <frameset id="allframes" rows="55,*"> \
        <frame id="ftop" src="/zgprogram/ftop"/> \
        <frameset id="allframes" cols="15%,*"> \
            <frame id="fleft" src="/zgprogram/list"/> \
            <frame id="fmain" src="/zgprogram/fmain"/> \
        </frameset> \
    </frameset> \
        ');
        res.end();
    }else if(method === 'GET' && req.path === '/zgprogram/ftop'){
        res.setHeader('Content-Type', 'text/html');
        res.write(' \
        <form id="form_search" method="post" action="/zgprogram/search"> \
        <button  id="go" class="btn btn-primary" onclick="go()" type="button">GO!</button> \
        <input id="searchinput" type="text" replacement="What do you want to search?" size=120 on_click="si_click()"> </input> \
        <input type="checkbox"id="cbtitle" onclick="check_title()">Title</input> \
        <input type="checkbox"id="cbtags" onclick="check_title()">Tags</input> \
    </form> \
    <script type="text/javascript"> \
    function si_click(){ \
        document.getElementById("searchinput").focus(); \
    } \
    function go(){ \
        // var inputstr = document.getElementById("searchinput").value; \
        // alert(inputstr); \
        var fm = document.getElementById("form_search"); \
        fm.submit(); \
    } \
    </script> \
        ');
        res.end();
        
    }else{   

        res.setHeader('Content-Type', 'application/json');
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
    }
}
// const serverHandler = (req,res)=>{
//     console.log("server is ON!");

// }

module.exports = serverHandler;