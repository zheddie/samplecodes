const http = require('http');
const querystring = require('querystring');

const server = http.createServer(
    (req,res) =>{
      const method = req.method;
      const url = req.url;
      const path = url.split('?')[0];
      const querys = querystring.parse(url.split('?')[1]);
      const responseData = {
        method,url,path,querys
      };
      console.log(responseData);
      res.setHeader('Content-Type', 'application/json');
      if(method === 'GET'){
          res.end(JSON.stringify(responseData));
      }
      if(method === 'POST'){
        let postdata = '';
        req.on('data',chunk=>{
          postdata+=chunk.toString();
        })
        req.on('end',()=>{
          responseData.postdata = postdata;
          res.end(JSON.stringify(responseData));
        })
      }
      
    }
);

server.listen(18701,()=>{
    console.log('server is running on port 18701.');
})