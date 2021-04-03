const http = require('http');
const querystring = require('querystring');

const server = http.createServer(
    (req,res) =>{
        if(req.method === 'POST'){
          let postData = '';
          req.on("data",chunk =>{
              postData += chunk.toString();
          });
          req.on("end",()=>{
            console.log('postData:',postData);
            res.end('DataReceivedDone');
          });
          console.log('post data type:',req.headers['content-type']);
        }
    }
);

server.listen(18701,()=>{
    console.log('server is running on port 18701.');
})