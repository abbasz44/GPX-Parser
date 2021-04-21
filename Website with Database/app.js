'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

// get the mysql client
const mysql = require('mysql2');

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
  'getWaypointsFromRoute': ['string', ['string', 'string']],
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
    console.log(file);

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

// database controls
let connection;
app.get('/mySQLLogin/:user/:pass/:name', function(req , res){
	connection = mysql.createConnection({
    host: 'dursley.socs.uoguelph.ca',
    user: req.params.user,
    password: req.params.pass,
    database: req.params.name
  })

  connection.connect(() => {
    connection.query("CREATE TABLE FILE(gpx_id INT AUTO_INCREMENT, PRIMARY KEY(gpx_id), file_name VARCHAR(60) NOT NULL, ver DECIMAL(2,1) NOT NULL, creator VARCHAR(256) NOT NULL)", (err, results, fields) => {
      console.log(err.sqlMessage)
    });
    connection.query("CREATE TABLE ROUTE(route_id INT AUTO_INCREMENT, PRIMARY KEY(route_id), route_name VARCHAR(256) NOT NULL, route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL, CONSTRAINT `fk_gpx_id` FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE)", (err, results, fields) => {
      console.log(err.sqlMessage)
    });
    connection.query("CREATE TABLE POINT(point_id INT AUTO_INCREMENT, PRIMARY KEY(point_id), point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256), route_id INT NOT NULL, CONSTRAINT `fk_route_id` FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE)", (err, results, fields) => {
      console.log(err.sqlMessage)
    });
  })
  res.send("logged in")
});

app.get('/mySQLStoreAllFiles', function(req , res){
  storeData();
  res.send(200);
})

app.get('/mySQLClearAllDatas', function(req , res){
  clearData();
  res.send(200);
})

app.get('/mySQLGetCounts', function(req , res){
  if (connection) {
    connection.connect(() => {
      connection.query('SELECT (SELECT COUNT(*) FROM FILE) AS num_files, (SELECT COUNT(*) FROM ROUTE) AS num_routes, (SELECT COUNT(*) FROM POINT) AS num_points', (err, results, fields) => {
        if (!err) {
          console.log('results', err, results);
          res.send(results[0]);
        } else {
          console.log(err);
        }
      });
    })
  }
})


app.get('/mySQLGetRTESorted/:sortByLength', function(req , res){
  const sortByLength = req.params.sortByLength;
  console.log(sortByLength);
  if (connection) {
    connection.connect(() => {
      let sortColums = sortByLength ? 'route_len' : 'route_name';
      connection.query(`SELECT * FROM ROUTE ORDER BY ${sortColums} DESC`, (err, results, fields) => {
        if (!err) {
          console.log('results', err, results);
          res.send(results);
        } else {
          console.log(err);
        }
      });
    })
  }
})

app.get('/mySQLGetRTEFromFileSorted/:fileName/:sortByLength', function(req , res){
  const sortByLength = req.params.sortByLength;
  const fileName = req.params.fileName;
  if (connection) {
    connection.connect(() => {
      let sortColums = sortByLength ? 'route_len' : 'route_name';
      const query = `
      SELECT route_name, route_len
      FROM ROUTE rte
      LEFT JOIN FILE f ON rte.gpx_id = f.gpx_id
      WHERE f.file_name = '${fileName}'
      ORDER BY ${sortColums} DESC
      `;
      console.log(query);
      connection.query(query, (err, results, fields) => {
        if (!err) {
          console.log('results', err, results);
          res.send(results);
        } else {
          console.log(err);
        }
      });
    })
  }
})

app.get('/mySQLGetPointsFromRTE/:fileName/:routeName', function(req , res){
  const routeName = req.params.routeName;
  const fileName = req.params.fileName;
  if (connection) {
    connection.connect(() => {
      const query = `
      SELECT point_name, point_index, latitude, longitude
      FROM POINT pt
      LEFT JOIN ROUTE rte on pt.route_id = rte.route_id
      LEFT JOIN FILE f ON rte.gpx_id = f.gpx_id
      WHERE f.file_name = '${fileName}'
      AND rte.route_name = '${routeName}'
      `;
      console.log(query);
      connection.query(query, (err, results, fields) => {
        if (!err) {
          console.log('results', err, results);
          res.send(results);
        } else {
          console.log(err);
        }
      });
    })
  }
})

app.get('/mySQLGetPointsFromFile/:fileName', function(req , res){
  const fileName = req.params.fileName;
  if (connection) {
    connection.connect(() => {
      const query = `
      SELECT point_name, point_index, latitude, longitude
      FROM POINT pt
      LEFT JOIN ROUTE rte on pt.route_id = rte.route_id
      LEFT JOIN FILE f ON rte.gpx_id = f.gpx_id
      WHERE f.file_name = '${fileName}'
      `;
      console.log(query);
      connection.query(query, (err, results, fields) => {
        if (!err) {
          console.log('results', err, results);
          res.send(results);
        } else {
          console.log(err);
        }
      });
    })
  }
})

app.get('/mySQLRefreshData', function(req , res){
  clearData();
  storeData();
  res.send(200);
})

const clearData = () => {
  if (connection) {
    connection.connect(() => {

      connection.query('DELETE FROM FILE', (err, results, fields) => {
        console.log(results);
        if (!err) {
          console.log('results', err, results);
        } else {
          console.log("Failed to clear all data");
          console.log(err);
        }
      });
    })
  }
}

const storeData = () => {
  if (connection) {
    let files = fs.readdirSync('uploads');
    files.forEach(fileName => {
      if (fileName.split('.').pop() == "gpx") {
        connection.connect(() => {
          // check if file of name exists
          connection.query('SELECT * FROM FILE WHERE FILE.file_name = \'' + fileName + '\'', (err, results, fields) => {
            console.log(results);
            if (!err && results.length == 0) {
              let gpxJSON = JSON.parse(libraries.getGPXJSON('uploads/' + fileName));

              // insert into FILE
              const query = 'INSERT INTO FILE(file_name, ver, creator) VALUES (\'' + fileName + '\', \'' + gpxJSON.version + '\', \'' + gpxJSON.creator + '\')';
              connection.query(query, (err, results, fields) => {
                if (!err) {
                  let routes = JSON.parse(libraries.getAllRoutesJSON(fileName));
                  console.log('routes', routes);
                  for (const route of routes) {

                    // insert into ROUTE
                    const query = 'INSERT INTO ROUTE(route_name, route_len, gpx_id) VALUES (\'' + route.name + '\', \'' + route.length + '\', \'' + results.insertId + '\')'
                    connection.query(query, (err, results, fields) => {
                      if (!err) {
                        let waypoints = JSON.parse(libraries.getWaypointsFromRoute(fileName, route.name));
                        console.log('waypoints', waypoints);
                        for (let i = 0; i < waypoints.length; i++) {
                          let wpt = waypoints[i];
                          console.log(wpt);
                          // insert into POINT
                          const query = 'INSERT INTO POINT(point_index, latitude, longitude, point_name, route_id) VALUES (\'' + i + '\', \'' + wpt.latitude + '\', \'' + wpt.longitude + '\', \'' + wpt.name + '\', \'' + results.insertId + '\')'
                          connection.query(query, (err, results, fields) => {
                            if (err) {
                              console.log("Error adding waypoint", err);
                            }
                          })
                        }
                      } else {
                        console.log("Error adding routes", err);
                      }
                    })
                  }
                } else {
                  console.log("Error adding file", err);
                }

              })

            } else {
              console.log(fileName, "already exists in table")
              console.log(err)
            }
          });
        })
      }
    })
  }
}

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
