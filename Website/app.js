'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;

  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ********************
let libraries = ffi.Library('./libgpxparser', {
  'createGPXFileFromJSON': ['bool', ['string', 'string']],
  'getGPXJSON': ['string', ['string']],
  'getAllRoutesJSON': ['string', ['string']],
  'getAllTracksJSON': ['string', ['string']],
  'renameRoute': ['bool', ['string', 'string', 'int']],
  'renameTrack': ['bool', ['string', 'string', 'int']],

});

app.get('/getGPXJSON/:filename', function (req, res) {
  fs.stat('uploads/' + req.params.filename, function (err, stat) {
    if (err == null) {
      let gpxJSON = JSON.parse(libraries.getGPXJSON('uploads/' + req.params.filename));
      res.send(gpxJSON);
    }
  })
});

//Sample endpoint
app.get('/getAllGPXData', function(req , res){
  let retStr = req.query.stuff + " " + req.query.junk;
  let files = fs.readdirSync('uploads');
  console.log(files);

  let gpxJSONList = []

  files.forEach(fileName => {
    if (fileName.split('.').pop() == "gpx") {
      let json = JSON.parse(libraries.getGPXJSON('uploads/' + fileName));
      json = {...json, "file": fileName}
      console.log(json);
      gpxJSONList.push(json);
    }
  })

  console.log(gpxJSONList);
  res.send(gpxJSONList);
});

app.get('/rte/:filename', function(req , res){


    let file = req.params.filename;


    let returner = libraries.getAllRoutesJSON(file);

    if (returner) {
    res.status(200).send(returner);
  } else {
    res.status(500).send();
  }
});

app.get('/trk/:filename', function(req , res){


    let file = req.params.filename;

    let returner = libraries.getAllTracksJSON(file);

    if (returner) {
      res.status(200).send(returner);
    } else {
      res.status(500).send();
    }
});

//rename functions
app.get('/renameRoute', function(req , res){
	let file = req.query.filename;
	console.log(file);
  let returner = libraries.renameRoute(file, req.query.newname, req.query.index);
  if (returner) {
    res.status(200).send(returner);
  } else {
    res.status(500).send();
  }
});

app.get('/renameTrack', function(req , res){
	let file = req.query.filename;
	console.log(file);
  let returner = libraries.renameTrack(file, req.query.newname, req.query.index);
  if (returner) {
    console.log("changed track name")
    res.status(200).send(returner);
  } else {
    res.status(500).send();
  }
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
