const express = require("express");
var jsdom = require('jsdom');
var bodyParser = require('body-parser');
const { JSDOM } = jsdom;
const { window } = new JSDOM();
const { document } = (new JSDOM('')).window;
global.document = document;

var $ = jQuery = require('jquery')(window);
var fs = require('fs');
var path = require('path');
var filePath = "/usr/webcam/config.txt"

const PORT = 8000;
const app = express();

app.use(express.static(__dirname));
app.use(bodyParser.urlencoded({ extended: false }))
app.use(bodyParser.json());
app.get('/', (req, res, next) => {
    res.redirect("home.html");
    next();
})
app.get('/refresh', (req, res) => {
    fs.readFile(filePath, { encoding: 'utf-8' }, function (err, data) {
        if (!err) {
            console.log('Received get config file request. Sent:  ' + data);
            res.send(data);
        } else {
            console.log(err);
        }
    });
})
app.post('/update', (req, res) => {
    var statusReq = req.body.status;
    var timeReq = req.body.time;
    fs.writeFile(filePath, statusReq + ' ' + timeReq, function(err) {
        if(err) {
            return console.log(err);
        }
        
        console.log("Config file updated. Status: " + statusReq + ", Time interval: " + timeReq);
    }); 
})

app.listen(PORT, () => console.log('Timelapse webapp listening on port ' + PORT + "!"))