var http = require('http');

var app = http.createServer(function(req,res){
    res.setHeader('Content-Type', 'application/json');
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.write(JSON.stringify({ leftPressed: true, rightPressed: false, upPressed : false, downPressed: true}));
    res.end();
});
app.listen(8432);