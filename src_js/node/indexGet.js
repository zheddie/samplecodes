const http = require('http');
const querystring = require('querystring');

const server = http.createServer(
    (req,res) =>{
        var mt = req.method;
        var url = req.url;
        req.query = querystring.parse(url.split('?')[1]);
        console.log('req.query',req.query,",method:",mt);
        res.end(JSON.stringify(req.query));
    }
);

server.listen(18701,()=>{
    console.log('server is running on port 18701.');
})