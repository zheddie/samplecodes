const fs = require('fs');
const path = require('path');
// function getFileContent(filename,callback){
//     const fullFileName = path.resolve(__dirname,'data',filename);
//     fs.readFile(fullFileName,(err,data)=>{
//         if(err){
//             console.error(err);
//             return;
//         }
        
//         //console.log(data.toString());
//         callback(
//             JSON.parse(data.toString()));
//     });
// }
// getFileContent('a.json',(aData)=>{
//     console.log('aData',aData);
//     getFileContent(aData.next,(bData)=>{
//         console.log('bData',bData);
//         getFileContent(bData.next,(cData)=>{
//             console.log('cData',cData);
//         });
//     })
// });
//promise 实现。避免刚才的回调地狱
function getFileContent(filename){
    const promise = new Promise((resolvezg,reject)=>{
        const fullFileName = path.resolve(__dirname,'data',filename);
        fs.readFile(fullFileName,(err,data)=>{
            if(err){
                reject(err);
                return;
            }
            //console.log(data.toString());
            console.log(typeof resolvezg,resolvezg);
            resolvezg(
                JSON.parse(data.toString()));
        });
    });
    return promise;
}
getFileContent('a.json').then((aData)=>{
    console.log('aData',aData);
    return getFileContent(aData.next);
}).then((bData)=>{
    console.log('bData',bData);
    return getFileContent(bData.next);
}).then((cData)=>{
    console.log('cData',cData);
});