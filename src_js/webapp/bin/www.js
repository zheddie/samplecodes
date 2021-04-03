const http = require('http');

const serverHandler = require('../app');
const PORT = 18701;

const server = http.createServer(serverHandler);
server.listen(PORT,()=>{
    console.log('server is running on port '+PORT+' ...');
});