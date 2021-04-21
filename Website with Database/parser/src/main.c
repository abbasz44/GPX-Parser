#include "GPXParser.h"
#include "LinkedListAPI.h"
#include "GPXHelper.h"

int main(int argc, char *argv[])
{
    // GPXdoc *newDoc = createValidGPXdoc("/home/undergrad/0/abbasz/2750/A2pre/testFiles/GHTC_James-Stone(2rt_0trk_0seg_4220m).gpx", "/home/undergrad/0/abbasz/2750/cis2750w21_project/bin/gpx.xsd");
    // char *toPrint = GPXdocToString(newDoc);
    // printf("%s\n", toPrint);

    // bool valid = validateGPXDoc(newDoc, "/home/undergrad/0/abbasz/2750/cis2750w21_project/bin/gpx.xsd");
    // bool valid = writeGPXdoc(newDoc, "/home/undergrad/0/abbasz/2750/cis2750w21_project/bin/MINE.gpx");

    // if (valid) {
    //     printf("Valid\n");
    // }
    // else { 
    //     printf("fuck\n");
    // }

    // char *trxBet = tracksBetween("simple1.gpx", 49, -124, 52, -125, 200);

    // printf("trx: %s\n", trxBet);

    renameRoute("simple1.gpx", "Route Starter", 0);



    // // deleteGPXdoc(newDoc);
    // // Track *testTrack = getTrack(newDoc, "Mount Steele Track");
    //
    //
    //


    // GPXdoc *newDoc = createGPXdoc("/home/undergrad/0/abbasz/2750/cis2750w21_project/bin/Reyn-Rozh_parking(0rt_1trk_1seg_400m).gpx");
    // char *string = GPXtoJSON(newDoc);
    // printf("xml\n\n %s", string);
    // free(string);

    // Route *testRoute = getRoute(newDoc, "Some route");
    // char *routeToJSONTest = routeToJSON(testRoute);
    // printf("%s\n", routeToJSONTest);

    // Track *testTrack = getTrack(newDoc, "Mount Steele Trail");
    // char *TrackToJSONTest = trackToJSON(testTrack);
    // printf("%s\n", TrackToJSONTest);

    return 0;
}
