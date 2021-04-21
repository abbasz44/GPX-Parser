// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything
        url: '/getAllGPXData',   //The server endpoint we are connecting to
        // data: {
        //     stuff: "Value 1",
        //     junk: "Value 2"
        // },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string,
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            // $('#blah').html("On page load, received string '"+data.stuff+"' from server");
            //We write the object to the console to show that the request was successful
            console.log("data", data);
            data.forEach(gpxJSON => {
                console.log("Adding ", gpxJSON)
                // Add data to generic gpx info table
                $('#fileLogBody').append('<tr>' +
                '<td>' + '<a href=uploads/' + gpxJSON.file + '>' + gpxJSON.file + '</td>' +
                '<td>' + gpxJSON.version + '</td>' +
                '<td>' + gpxJSON.creator + '</td>' +
                '<td>' + gpxJSON.numRoutes + '</td>' +
                '<td>' + gpxJSON.numTracks + '</td>' +
                '<td>' + gpxJSON.numWaypoints + '</td>' +
                '</tr>');

                $("#dropper").append('<option value="' + gpxJSON.file + '">' + gpxJSON.file + '</option>');
            })

        },
        fail: function(error) {
            // Non-200 return, do something with error
            alert("Track error")
            console.log(error);
        }
    });

//rte and tarck info
    $('#dropper').change(function(){
     let fileName = $("#dropper option:selected").text();

     console.log(fileName);
    //connection to the html
     $('#routeTable tr').remove();
     $('#trackTable tr').remove();

     $.ajax({
         type: 'get',            //Request type
         dataType: 'json',       //Data type - we will use JSON for almost everything
         url: '/trk/' + fileName,   //The server endpoint we are connecting to

         success: function (data) {
       console.log(data);
       //loop through data
             for(var i = 0; i < data.length; i++)
             {
         console.log(data[i]);

         $("#trackTable").append("<tr><td> Track " + (i+1) + "</td><td>" + data[i]["name"] +
                                "</td><td>" + data[i]["len"] + "</td><td>"
                                + data[i]["loop"] + "</td></tr>");

         

          alert("Track successful")
       }
         },
         fail: function(error) {
             // Non-200 return, do something with error
             alert("Track error")
             console.log(error);
       }
     });
     $.ajax({
         type: 'get',            //Request type
         dataType: 'json',       //Data type - we will use JSON for almost everything
         url: '/rte/' + fileName,   //The server endpoint we are connecting to

         success: function (data) {
       console.log(data);
       //loop through data
             for(var i = 0; i < data.length; i++)
             {
         console.log(data[i]);


         $("#routeTable").append("<tr><td> Route " + (i+1) +
                                  "</td><td id=\"rte-"+data[i]["numPoints"]+"\">" +
                                  data[i]["name"] + "</td><td>" +
                                  data[i]["numPoints"] + "</td><td>" +
                                  data[i]["len"] + "</td><td>" +
                                  data[i]["loop"] + "</td></tr>");



         alert("Route successful")
       }
         },
         fail: function(error) {
             // Non-200 return, do something with error
              alert("error")
             console.log(error);

       }
     });
//other data go here need help with that
    })

    // //rename functions go here
        $('#renamer').submit(function(e) {
            let filename = $("#dropper option:selected").text();

    			e.preventDefault();

    			console.log("form recieved");


    			if($('#trackRename').is(':checked'))
    			{
    				console.log("form recieved");

            let newname = $("#nameNew").val();
            let index = parseInt($("#component").val());

            console.log(filename);
            console.log(index);


    				$.ajax({
      				type: 'get',            //Request type
      				dataType: 'json',       //Data type - we will use JSON for almost everything
      				url: '/renameTrack/',   //The server endpoint we are connecting to
      				data: {
    					filename: filename,
    					newname: newname,
    					index: index
    				},

      				success: function (data) {
    					alert("Track renamed (refresh page too see)");
    					},
    					fail: function(error) {
    						// Non-200 return, do something with error
    					alert("error");

    						console.log(error);
    					}
    				});
    			}
          if($('#routeRename').is(':checked'))
    			{
    				console.log("route form");

            let newname = $("#nameNew").val();
    				let index = parseInt($("#component").val());

    				console.log(filename);
    				console.log(index);

    				$.ajax({
      				type: 'get',            //Request type
      				dataType: 'json',       //Data type - we will use JSON for almost everything
      				url: '/renameRoute/',   //The server endpoint we are connecting to
      				data: {
    					filename: filename,
    					newname: newname,
    					index: index
    				},
      				success: function (data) {
                            alert("Route renamed (refresh page too see)");
    					},
    					fail: function(error) {
    						// Non-200 return, do something with error
                            alert("error");
    						console.log(error);
    					}
    				});
    			}
    		});


    });
