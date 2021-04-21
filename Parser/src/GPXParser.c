/*Zachery Abbas
Assignment 1
1019463
*/
#include "GPXParser.h"
#include "LinkedListAPI.h"
#include "GPXHelper.h"

GPXdoc *createGPXdoc(char *fileName)
{
    int fileNameLen = strlen(fileName);
    if (fileName == NULL || fileName[fileNameLen-4] != '.' || fileName[fileNameLen-3] != 'g' || fileName[fileNameLen-2] != 'p' || fileName[fileNameLen-1] != 'x') {
        printf("Invalid file name\n");
        return NULL;
    }

    xmlDoc *doc = NULL;
    xmlNode *treeRoot = NULL;
    GPXdoc *obj = malloc(sizeof(GPXdoc));
    obj->creator = malloc(sizeof(char) * 125);
    obj->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    obj->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    obj->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);


    LIBXML_TEST_VERSION

    doc = xmlReadFile(fileName, NULL, 0);

    //when xml parser runs into failure
    if (doc == NULL)
    {
        printf("Error couldn't read file");
        xmlFreeDoc(doc);
        deleteGPXdoc(obj);
        return NULL;
    }

    treeRoot = xmlDocGetRootElement(doc);

    //this comares the root name to gpx this is where I get the details
    //of the gpx doc
    if (strcmp((char *)(treeRoot->name), "gpx") == 0)
    {

        obj = details(doc, treeRoot, obj);

        if (obj == NULL)
        {
            return NULL;
        }
    }
    //free for memory errors
    else
    {
        xmlFreeDoc(doc);
        deleteGPXdoc(obj);
        return NULL;
    }
    //this gets all the other information out of the gpx file
    if (treeRoot->children != NULL)
    {
        obj = parser( treeRoot->children->next, doc, obj);
    }

    if (obj == NULL)
    {
        return NULL;
    }

    //free the document
    xmlFreeDoc(doc);

    return obj;
}

//****************delete functions start here****************
void deleteGPXdoc(GPXdoc *doc)
{

    if (doc == NULL)
    {
        return;
    }
    xmlCleanupParser();
    if(doc->creator == NULL){
  	free(doc -> creator);
  }
    if(doc->waypoints == NULL){
  	freeList(doc -> waypoints);
  }
    if(doc->routes == NULL){
  	freeList(doc -> routes);
  }
    if(doc->tracks == NULL){
  	freeList(doc -> tracks);
  }
  	free(doc);
}
void deleteGpxData(void *data)
{
    GPXData *tempData;

    if (data == NULL)
    {
        return;
    }
    tempData = (GPXData *)data;
    if(tempData->name == NULL){
    free(tempData->name);
    }
    if(tempData->value == NULL){
    free(tempData->value);
    }
    free(tempData);
}
void deleteWaypoint(void *data)
{
    Waypoint *tempPoint;

    if (data == NULL)
    {
        return;
    }

    tempPoint = (Waypoint *)data;
    if(tempPoint->otherData == NULL){
    freeList(tempPoint->otherData);
    }
    if(tempPoint->name == NULL){
    free(tempPoint->name);
    }
    free(tempPoint);
}
void deleteRoute(void *data)
{
    Route *tempRoute;

    if (data == NULL)
    {
        return;
    }

    tempRoute = (Route *)data;
    if(tempRoute->waypoints == NULL){
    freeList(tempRoute->waypoints);
    }
    if(tempRoute->otherData == NULL){
    freeList(tempRoute->otherData);
    }
    if(tempRoute->name == NULL){
    free(tempRoute->name);
    }
    free(tempRoute);
}
void deleteTrackSegment(void *data)
{
    TrackSegment *tempSeg;

    if (data == NULL)
    {
        return;
    }

    tempSeg = (TrackSegment *)data;

    if(tempSeg->waypoints == NULL){
    freeList(tempSeg->waypoints);
    }
    free(tempSeg);
}
void deleteTrack(void *data)
{
    Track *tempTrack;

    if (data == NULL)
    {
        return;
    }

    tempTrack = (Track *)data;
    if(tempTrack->segments == NULL){
    freeList(tempTrack->segments);
    }
    if(tempTrack->otherData == NULL){
    freeList(tempTrack->otherData);
    }
    if(tempTrack->name == NULL){
    free(tempTrack->name);
    }
    free(tempTrack);
}
//****************compare functions start here****************

int compareGpxData(const void *first, const void *second)
{
    GPXData *buf;
    GPXData *buf2;

    if (first == NULL || second == NULL)
    {
        return 0;
    }

    buf = (GPXData *)first;
    buf2 = (GPXData *)second;

    return strcmp((char *)buf->name, (char *)buf2->name);
}

int compareWaypoints(const void *first, const void *second)
{
    Waypoint *buf;
    Waypoint *buf2;

    if (first == NULL || second == NULL)
    {
        return 0;
    }

    buf = (Waypoint *)first;
    buf2 = (Waypoint *)second;

    return strcmp((char *)buf->name, (char *)buf2->name);
}
int compareRoutes(const void *first, const void *second)
{
    Route *buf;
    Route *buf2;

    if (first == NULL || second == NULL)
    {
        return 0;
    }

    buf = (Route *)first;
    buf2 = (Route *)second;

    return strcmp((char *)buf->name, (char *)buf2->name);
}
int compareTrackSegments(const void *first, const void *second)
{
    TrackSegment *buf;
    TrackSegment *buf2;

    if (first == NULL || second == NULL)
    {
        return 0;
    }

    buf = (TrackSegment *)first;
    buf2 = (TrackSegment *)second;

    return strcmp((char *)buf->waypoints, (char *)buf2->waypoints);
}
int compareTracks(const void *first, const void *second)
{
    Track *buf;
    Track *buf2;

    if (first == NULL || second == NULL)
    {
        return 0;
    }

    buf = (Track *)first;
    buf2 = (Track *)second;

    return strcmp((char *)buf->name, (char *)buf2->name);
}
//****************toString functions start here****************
char *GPXdocToString(GPXdoc *doc)
{
    if (doc == NULL)
    {
        char *errStr = malloc(sizeof(char) * 13);
        sprintf(errStr, "Doc is Null\n");
        return errStr;
    }

    char *returner = malloc(sizeof(char) * 10500);
    char *getter = malloc(sizeof(char) * 650);

    //version, creator
    sprintf(returner, "GPX File\n\n");
    if (doc->version)
    {
        sprintf(getter, "The version is: %0.2f\n", doc->version);
        strcat(returner, getter);
    }

    else if (doc->version)
    {
        sprintf(getter, "The creator is: %s\n", doc->creator);
        strcat(returner, getter);
    }

    else if (doc->version)
    {
        sprintf(getter, "The namespace is: %s\n", doc->namespace);
        strcat(returner, getter);
    }
    //getting all the gpx info is done
    free(getter);

    //Routes
    if (getLength(doc->routes) == 0)
    {
        strcat(returner, "\n\nRoutes are not Present\n\n");
    }
    else
    {
        strcat(returner, "\n\nAvalaible Routes:\n\n");
        //to string function that converts routes to string
        char *tempStr = toStringFunc(1, doc->routes);
        strcat(returner, tempStr);
        free(tempStr);
    }

    //waypoints
    if (getLength(doc->waypoints) == 0)
    {
        strcat(returner, "\n\nWaypoints are not Present\n\n");
    }
    else
    {
        strcat(returner, "\n\nAvalaible Waypoints:\n\n");
        //to string fucntion thats converts the way points to string
        char *tempStr = toStringFunc(4, doc->waypoints);
        strcat(returner, tempStr);
        free(tempStr);
    }

    //tracks
    if (getLength(doc->tracks) == 0)
    {
        strcat(returner, "\n\nTracks are not present\n\n");
    }
    else
    {

        strcat(returner, "\n\nAvalaible tracks:\n\n");
        //to string function to convert tracks to string
        char *tempStr = toStringFunc(2, doc->tracks);
        strcat(returner, tempStr);
        free(tempStr);
    }

    return returner;

}
char *gpxDataToString(void *data)
{
    if (data == NULL)
    {
        return NULL;
    }
    GPXData *obj = (GPXData *)data;
    char *returner = malloc(sizeof(char) * 1150);
    sprintf(returner, "GPXData name is: %s and Other Data is: %s\n", obj->name, obj->value);

    return returner;

}
char *waypointToString(void *data)
{
    Waypoint *obj = (Waypoint *)data;
    char *returner =malloc(sizeof(char) * 5500);
    char *tempStr = malloc(sizeof(char) * 125);

    //waypoint
    if (strlen(obj->name) == 0)
    {
        sprintf(returner, "\nThere is no waypoint name\n");
    }
    else
    {
        sprintf(returner, "\nName: %s\n", obj->name);
    }

    sprintf(tempStr, "\n\nThe longitude is: %.4f and the latitude is: %.4f\n", obj->longitude, obj->latitude);
    strcat(returner, tempStr);
    free(tempStr);

    //otherData
    if (getLength(obj->otherData) == 0)
    {
        strcat(returner, "\n\notherData unavaliable\n\n");
    }
    else
    {
        //use tostring function that works with waypoints other data
        char *tempStr = toStringFunc(3, obj->otherData);
        strcat(returner, tempStr);
        free(tempStr);
        strcat(returner, "\n\n");
    }

    return returner;

}
char *routeToString(void *data)
{
    Route *obj = (Route *)data;
    char *returner = malloc(sizeof(char) * 5500);

    // Copying over the Route's name
    if (strlen(obj->name) == 0)
    {
        sprintf(returner, "Name Unavaliable\n");
    }
    else
    {
        sprintf(returner, "Name: %s\n", obj->name);
    }

    //otherData
    if (getLength(obj->otherData) == 0)
    {
        strcat(returner, "Otherdata Unavaliable \n");
    }
    else
    {
        //to string function used to convert other data
        char *tempStr = toStringFunc(6, obj->otherData);
        strcat(returner, tempStr);
        free(tempStr);
    }

    // Copying over the Route's waypoints
    if (getLength(obj->waypoints) == 0)
    {
        strcat(returner, "\n\nPoints Unavaliable\n\n");
    }
    else
    {
        strcat(returner, "\n\nPoints are:\n\n");
        //use to string function to convert the points to string
        char *tempStr = toStringFunc(4, obj->waypoints);
        strcat(returner, tempStr);
        free(tempStr);
    }

    return returner;

}
char *trackSegmentToString(void *data)
{
    TrackSegment *obj = (TrackSegment *)data;
    char *returner = malloc(sizeof(char) * 5500);

    // Copying over the Track Segment's waypoints
    if (getLength(obj->waypoints) == 0)
    {
        strcpy(returner, "\n\nPoints are unavaliable\n\n");
    }
    else
    {
        strcpy(returner, "\n\nPoints are: \n\n");

        //use to string function to points to string
        char *tempStr = toStringFunc(4, obj->waypoints);
        strcat(returner, tempStr);
        free(tempStr);
    }

    return returner;
}

char *trackToString(void *data)
{
    Track *obj = (Track *)data;
    char *returner = malloc(sizeof(char) * 5500);

    //tracks name
    if (strlen(obj->name) == 0)
    {
        sprintf(returner, "Name Unavaliable\n");
    }
    else
    {
        sprintf(returner, "Track Name: %s\n", obj->name);
    }

    //otherData
    if (getLength(obj->otherData) == 0)
    {
        strcat(returner, "otherData unavaliable\n");
    }
    else
    {
        //use to string fucntion to convert the otherData
        char *tempStr = toStringFunc(6, obj->otherData);
        strcat(returner, tempStr);
        free(tempStr);
    }

    //Track segments
    if (getLength(obj->segments) == 0)
    {
        strcat(returner, "\n\nTrack segements unavaliable\n\n");
    }
    else
    {
        strcat(returner, "\n\nTrack Segments:\n\n");
        //use to string function to convert track segements to string
        char *tempStr = toStringFunc(5, obj->segments);
        strcat(returner, tempStr);
        free(tempStr);
    }

    return returner;

}

//Module 2
//****************Computing Summary Counts Start Here****************
int getNumWaypoints(const GPXdoc *doc)
{
    int num = 0;

    if (doc == NULL)
    {
        return 0;
    }

    num = getLength(doc->waypoints);

    return num;
}
int getNumRoutes(const GPXdoc *doc)
{
    int num = 0;

    if (doc == NULL)
    {
        return 0;
    }

    num = getLength(doc->routes);

    return num;
}
int getNumTracks(const GPXdoc *doc)
{
    int num = 0;

    if (doc == NULL)
    {
        return 0;
    }

    num = getLength(doc->tracks);

    return num;
}
int getNumSegments(const GPXdoc *doc)
{
    int num = 0;
    Track *obj;
    ListIterator lister = createIterator(doc->tracks);

    if (doc == NULL)
    {
        return 0;
    }

    while ((obj = nextElement(&lister)) != NULL)
    {
        num = num + getLength(obj->segments);
    }

    return num;
}
int getNumGPXData(const GPXdoc *doc)
{
    int num = 0;

    if (doc == NULL)
    {
        return 0;
    }

    //waypoints
    num = num + getDataWayPoint(doc->waypoints);

    //tracks
    if (getNumTracks(doc) > 0)
    {
        Track *obj1;
        ListIterator lister1 = createIterator(doc->tracks);

        //name
        while ((obj1 = nextElement(&lister1)) != NULL)
        {
            if (strlen(obj1->name) != 0)

                num = num + 1;

            num = num + getLength(obj1->otherData);

            //segments
            if (getLength(obj1->segments) != 0)
            {
                TrackSegment *obj2;
                ListIterator lister2 = createIterator(obj1->segments);

                while ((obj2 = nextElement(&lister2)) != NULL)
                {
                    num = num + getDataWayPoint(obj2->waypoints);
                }
            }
        }
    }

    //routes
    if (getNumRoutes(doc) > 0)
    {
        Route *obj;
        ListIterator lister = createIterator(doc->routes);

        while ((obj = nextElement(&lister)) != NULL)
        {
            if (strlen(obj->name) != 0)
                num = num + 1;

            num = num + getDataWayPoint(obj->waypoints);
            num = num + getLength(obj->otherData);
        }
    }

    return num;
}
//****************Returning Name Functions Start here****************
Waypoint *getWaypoint(const GPXdoc *doc, char *name)
{
    Waypoint *obj;
    ListIterator lister = createIterator(doc->waypoints);

    if ((doc == NULL) || (name == NULL))
    {
        return NULL;
    }

    while ((obj = nextElement(&lister)) != NULL)
    {
        if (strcmp(obj->name, name) == 0)
            return obj;
    }

    return NULL;
}

Track *getTrack(const GPXdoc *doc, char *name)
{
    if ((doc == NULL) || (name == NULL))
    {
        return NULL;
    }

    Track *obj;
    ListIterator lister = createIterator(doc->tracks);

    while ((obj = nextElement(&lister)) != NULL)
    {
        if (strcmp(obj->name, name) == 0)
        {
            return obj;
        }
    }


    return NULL;
}
Route *getRoute(const GPXdoc *doc, char *name)
{
    if ((doc == NULL) || (name == NULL))
    {
        return NULL;
    }

    Route *obj;
    ListIterator lister = createIterator(doc->routes);

    while ((obj = nextElement(&lister)) != NULL)
    {
        if (strcmp(obj->name, name) == 0)
            return obj;
    }

    return NULL;
}
