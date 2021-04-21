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

//****************delete functions starter here****************
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
//****************compare functions starter here****************

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
//****************toString functions starter here****************
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
    sprintf(returner, "GPXData name is: %s and attribute value is: %s\n", obj->name, obj->value);

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
//****************Computing Summary Counts starter Here****************
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
//****************Returning Name Functions starter here****************
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
    Route *obj;
    ListIterator lister = createIterator(doc->routes);

    while ((obj = nextElement(&lister)) != NULL)
    {
        if (strcmp(obj->name, name) == 0)
            return obj;
    }

    return NULL;
}
// //A2 FUNCTIONS
GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile)
{
    int fileNameLen = strlen(fileName);
    if (fileName == NULL || fileName[fileNameLen-4] != '.' || fileName[fileNameLen-3] != 'g' || fileName[fileNameLen-2] != 'p' || fileName[fileNameLen-1] != 'x') {
        printf("Invalid file name\n");
        return NULL;
    }

    int gpxSchemaFileLen = strlen(gpxSchemaFile);
    if (gpxSchemaFile == NULL || gpxSchemaFile[gpxSchemaFileLen-4] != '.' || gpxSchemaFile[gpxSchemaFileLen-3] != 'x' || gpxSchemaFile[gpxSchemaFileLen-2] != 's' || gpxSchemaFile[gpxSchemaFileLen-1] != 'd') {
        printf("Invalid file name\n");
        return NULL;
    }

    LIBXML_TEST_VERSION

    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr parserCtxt;

    parserCtxt = xmlSchemaNewParserCtxt(gpxSchemaFile);
    xmlSchemaSetParserErrors(parserCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(parserCtxt);
    xmlSchemaFreeParserCtxt(parserCtxt);


    xmlDoc *doc = NULL;
    xmlNode *treeRoot = NULL;
    GPXdoc *obj = malloc(sizeof(GPXdoc));
    obj->creator = malloc(sizeof(char) * 125);
    obj->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    obj->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    obj->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);

    doc = xmlReadFile(fileName, NULL, 0);

    //when xml parser runs into failure
    if (doc == NULL)
    {
        printf("Error couldn't read file");
        xmlFreeDoc(doc);
        deleteGPXdoc(obj);
        return NULL;
    }

    xmlSchemaValidCtxtPtr validCtxt;
    validCtxt = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(validCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    int ret = xmlSchemaValidateDoc(validCtxt, doc);

    // https://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html

    if (ret == 0)
    {
      printf("%s validates\n", fileName);
    }
    else if (ret > 0)
    {
      printf("%s fails to validate\n", fileName);
    }
    else
    {
      printf("%s validation generated an internal error\n", fileName);
    }

    xmlSchemaFreeValidCtxt(validCtxt);

    treeRoot = xmlDocGetRootElement(doc);

    // xmlNsPtr nsPtr = xmlNewNs(treeRoot, treeRoot->ns->href, NULL);
    // xmlSetNs(treeRoot, nsPtr);

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

bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile)
{

    if(gpxSchemaFile == NULL || doc==NULL){
      return NULL;
    }

    int gpxSchemaFileLen = strlen(gpxSchemaFile);
    if (gpxSchemaFile == NULL || gpxSchemaFile[gpxSchemaFileLen-4] != '.' || gpxSchemaFile[gpxSchemaFileLen-3] != 'x' || gpxSchemaFile[gpxSchemaFileLen-2] != 's' || gpxSchemaFile[gpxSchemaFileLen-1] != 'd') {
        printf("Invalid file name\n");
        return NULL;
    }

    if (doc == NULL) {
      printf("Invalid GPXdoc\n");
      return NULL;
    }

    LIBXML_TEST_VERSION

    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr parserCtxt;

    parserCtxt = xmlSchemaNewParserCtxt(gpxSchemaFile);
    xmlSchemaSetParserErrors(parserCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(parserCtxt);
    xmlSchemaFreeParserCtxt(parserCtxt);

    
    xmlDocPtr tempDoc = NULL;
    xmlNodePtr root = NULL;
    bool checker = false;


    LIBXML_TEST_VERSION;

    //declare root and tempDoc
    root = xmlNewNode(NULL, BAD_CAST "gpx");
    tempDoc = xmlNewDoc(BAD_CAST "1.0");

    // set namespace
    xmlNsPtr nsPtr = xmlNewNs(root, schema->targetNamespace, NULL);
    xmlSetNs(root, nsPtr);

    //call tree function
    gpxTree(doc, &root);

    xmlDocSetRootElement(tempDoc, root);

    // xmlSchemaParserCtxtPtr ctxt;
    // xmlSchemaPtr schema = NULL;

    xmlLineNumbersDefault(1);

    // https://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
    //how to from this website

    // ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);
    // xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    // schema = xmlSchemaParse(ctxt);
    // xmlSchemaFreeParserCtxt(ctxt);

    // if (tempDoc == NULL)
    // {
    //   fprintf(stderr, "file is unable to parsed\n\n");
    // }

    //when the file is not null
    // else {
    //   xmlSchemaValidCtxtPtr ctxt;
    //   int ret;

    //   ctxt = xmlSchemaNewValidCtxt(schema);
    //   xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    //   ret = xmlSchemaValidateDoc(ctxt, tempDoc);

    //   if (ret == 0)
    //   {

    //     checker = true;
    //   }
    //   else if (ret > 0) {

    //     checker = false;
    //   }
    //   else {

    //     return NULL;
    //   }

    //   xmlFreeDoc(tempDoc);
    //   xmlSchemaFreeValidCtxt(ctxt);
    // }


      xmlSchemaValidCtxtPtr validCtxt;
      validCtxt = xmlSchemaNewValidCtxt(schema);
      xmlSchemaSetValidErrors(validCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
      int ret = xmlSchemaValidateDoc(validCtxt, tempDoc);

      // https://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html

      if (ret == 0)
      {
        checker = true;
      }
      else if (ret > 0)
      {
        checker = false;
      }
      else
      {
        checker = false;
      }

      xmlSchemaFreeValidCtxt(validCtxt);

    if(schema != NULL)
    //clean and free from mem leaks
    xmlSchemaFree(schema);
    xmlSchemaCleanupTypes();
    xmlMemoryDump();
    xmlCleanupParser();


    return checker;

}
bool writeGPXdoc(GPXdoc* doc, char* fileName)
{


  	if(fileName == NULL || doc == NULL){
       return NULL;
  	}

    bool writeCheck = true;
  	// writeCheck = validateGPXDoc(doc, "gpx.xsd");

  	if(writeCheck == false){
      fprintf(stderr, "unable to write file!\n\n");
    }
  	else{
      xmlNodePtr root = NULL;
  		xmlDocPtr tempDoc = NULL;


  		LIBXML_TEST_VERSION;

      root = xmlNewNode(NULL, BAD_CAST "gpx");
  		tempDoc = xmlNewDoc(BAD_CAST "1.0");
  		xmlDocSetRootElement(tempDoc, root);

      //tree from doc
  		gpxTree(doc,&root);

  		xmlSaveFormatFileEnc(fileName , tempDoc, "UTF-8", 1);
  	}

  	return writeCheck;
  }

//module 2 A2
float round10 (float len){
  int num =5;
  int num1= 10;

  len += num;
	len = (int)len / num1;
	len = num1* len;
	return len;
}
float getRouteLen(const Route *rt){
  if(rt == NULL || getLength(rt->waypoints) == 0){
    return 0;
  }

  Waypoint *pointTwo;
  Waypoint *pointOne;
  float tot = 0;
  //list iterator malloc space
  ListIterator *pointIter;
  pointIter = malloc(sizeof(ListIterator));
  *pointIter = createIterator(rt->waypoints);

  //error check retrun 0;

  //get the two points
	pointOne = (Waypoint*)pointIter->current->data;
	nextElement(pointIter);
	pointTwo = (Waypoint*)pointIter->current->data;

  tot = haver(pointOne->longitude, pointTwo->longitude, pointOne->latitude, pointTwo->latitude);
	pointOne = (Waypoint*)pointIter->current->data;

  //loop through way points
  int wayLen = getLength(rt->waypoints);
  for(int i = 1; i < wayLen; i++){
		pointTwo = (Waypoint*)pointIter->current->data;
		tot = tot + haver(pointOne->longitude, pointTwo->longitude,pointOne->latitude, pointTwo->latitude);
		pointOne = (Waypoint*)pointIter->current->data;
		nextElement(pointIter);
	}

  free(pointIter);
  return tot;

}
float getTrackLen(const Track *tr){
  if(tr == NULL||getLength(tr->segments) == 0){
		return 0;
	}
    float tot = 0;
    //list iterator malloc space
  	ListIterator *pointIter;
  	pointIter = malloc(sizeof(ListIterator));
  	*pointIter = createIterator(tr->segments);
  	TrackSegment *bufSeg;
     int segLen = getLength(tr->segments);

    for(int i = 0; i < segLen; i++){
      //malloc space
		    bufSeg = (TrackSegment*)pointIter;
		    ListIterator* pointIterOne;
		    pointIterOne = malloc(sizeof(ListIterator));
		    *pointIterOne = createIterator(bufSeg->waypoints);
		    Waypoint* pointTwo;
		    Waypoint *pointOne;
        //starting point
        if(i == 0){
          pointOne = (Waypoint*)pointIterOne->current->data;
        }

        int wayLen = getLength(bufSeg->waypoints);

          for(int x = 0; x < wayLen; x++){
          //total
    			pointTwo = (Waypoint*)pointIterOne->current->data;
    			tot = tot + haver(pointOne->longitude, pointTwo->longitude, pointOne->latitude, pointTwo->latitude);
    			pointOne = (Waypoint*)pointIterOne->current->data;
    			nextElement(pointIterOne);
        }
    //free info
    nextElement(pointIter);
		free(pointIterOne);
	}
	free(pointIter);
	return tot;
}
int numRoutesWithLength(const GPXdoc* doc, float len, float delta){

	if(doc == NULL || delta < 0 || len < 0){
		return 0;
	}

  float tot = 0;
  //list iterator malloc space
	ListIterator *pointIter;
	pointIter = malloc(sizeof(ListIterator));
	*pointIter = createIterator(doc->routes);
	Route *buf;
  int routeLen = getLength(doc->routes);

  for(int i = 0; i < routeLen; i++){
		buf = (Route*)pointIter->current->data;
		if(getRouteLen(buf) + delta <= len && getRouteLen(buf) >= len){
			tot++;
		}
		else if(getRouteLen(buf) + delta >= len && getRouteLen(buf) <= len){
			tot++;
		}
		nextElement(pointIter);
	}
	free(pointIter);
	return tot;
}

int numTracksWithLength(const GPXdoc* doc, float len, float delta){
  //error check
  if(doc == NULL || delta < 0 || len < 0){
		return 0;
	}
	float tot = 0;
  //list iterator malloc space
	ListIterator *pointIter;
	pointIter = malloc(sizeof(ListIterator));
	*pointIter = createIterator(doc->tracks);
	Track *buf;

  //get length
  int trackLen = getLength(doc->tracks);
	for(int i = 0; i < trackLen; i++){

		buf = (Track*)pointIter->current->data;
		if(getTrackLen(buf) + delta >= len && getTrackLen(buf) <= len){
			tot++;
		}
		else if(getTrackLen(buf) + delta <= len && getTrackLen(buf) >= len){
			tot++;
		}
		nextElement(pointIter);
	}
	free(pointIter);
	return tot;
}
bool isLoopRoute(const Route* route, float delta){
  //error check
  if(route == NULL || delta < 0||getLength(route->waypoints) < 4){
		return false;
	}

  //list iterator malloc space
	ListIterator *pointIter;
	pointIter = malloc(sizeof(ListIterator));
	*pointIter = createIterator(route->waypoints);

	Waypoint *temp;
	Waypoint *lastPoint;
	temp = (Waypoint*)pointIter->current->data;

  //get length
  int wayLen = getLength(route->waypoints);

  for(int i = 0; i < wayLen; i++){
		lastPoint = (Waypoint*)pointIter->current->data;
		nextElement(pointIter);
	}
  //use haver equation
	if(haver(temp->longitude,  lastPoint->longitude, temp->latitude,lastPoint->latitude) < delta){
		free(pointIter);
		return true;
	}

	free(pointIter);
	return false;
}

bool isLoopTrack(const Track *tr, float delta){
  if(tr == NULL || delta < 0){
		return false;
	}
	int num = 0;

  //list iterator malloc space
	ListIterator *pointIter;
	pointIter = malloc(sizeof(ListIterator));
	*pointIter = createIterator(tr->segments);

  ListIterator *pointIterTwo;
  pointIterTwo = malloc(sizeof(ListIterator));

	TrackSegment *buffer;
	Waypoint *temp;
	Waypoint *lastPoint;
	buffer = (TrackSegment*)pointIter->current->data;

  //get length
  int segLen = getLength(tr->segments);
  for(int k = 0; k < segLen; k++){
		*pointIterTwo = createIterator(buffer->waypoints);
		temp = (Waypoint*)pointIterTwo->current->data;

  //get length
  int wayLen = getLength(buffer->waypoints);
		for(int z = 0; z < wayLen; z++){
      //last point
			lastPoint = (Waypoint*)pointIterTwo->current->data;
			nextElement(pointIterTwo);
			num++;
		}
		nextElement(pointIter);
	}
  //use haver function
	if(num >= 2 && haver(temp->longitude, lastPoint->longitude,temp->latitude, lastPoint->latitude) < delta){
		free(pointIterTwo);
		free(pointIter);
		return true;
	}

	free(pointIterTwo);
	free(pointIter);
	return false;
}

List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
  if(doc == NULL){
		return NULL;
	}

  //initialize lists
  ListIterator *pointIter;
	pointIter = malloc(sizeof(ListIterator));
	*pointIter = createIterator(doc->routes);
	List * router = initializeList(routeToString, deleteRoute, compareRoutes);

  Route *temp;
  int startererPoint = 0;
	int enderPoint =  0;

  //get len
  int routeLen = getLength(doc->routes);

  for(int i = 0; i < routeLen; i++){

		temp = (Route*)pointIter->current->data;
    //malloc space
		ListIterator *pointIter2;
		pointIter2 = malloc(sizeof(ListIterator));
		*pointIter2 = createIterator(temp->waypoints);

		Waypoint *way;
    //get length
    int wayLen = getLength(temp->waypoints);
		for(int j = 0; j < wayLen; j++){
			way = (Waypoint*)pointIter2->current->data;

      //use haver function checking with source lat and long
			if(haver(sourceLong, way->longitude, sourceLat, way->latitude) <= delta){
				startererPoint = 1;
			}
      //use haver function checking with dest lat long
			if(haver(destLong,way->longitude, destLat, way->latitude) <= delta){
				enderPoint = 1;
			}
			nextElement(pointIter2);
		}
    //when they both ==1 insert back
		if(startererPoint == 1 && enderPoint == 1){
			insertBack(router, temp);
		}

    //resetpoints to 0
		startererPoint = 0;
		enderPoint = 0;
		nextElement(pointIter);
		free(pointIter2);
	}
	free(pointIter);

	if(getLength(router) <= 0){
		return NULL;
	}
	return router;

}
List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){

  //when doc == null return null
  if(doc == NULL){
		return NULL;
	}

  //define list iterator
	ListIterator *pointIter;
	pointIter = malloc(sizeof(ListIterator));
	*pointIter = createIterator(doc->tracks);

  //intialize list
  List * tracks = initializeList(trackToString, deleteTrack, compareTracks);

  int starter = 0;
  int ender =  0;
	Track *temp;

  //get length
  int trackLen = getLength(doc->tracks);

  for(int i = 0; i < trackLen; i++){
		temp = (Track*)pointIter->current->data;
    //list iterator malloc space
		ListIterator *pointIter2;
		pointIter2 = malloc(sizeof(ListIterator));
		*pointIter2 = createIterator(temp->segments);
		TrackSegment *seg;

    //get length
    int trackSegLen = getLength(temp->segments);

		for(int i = 0; i < trackSegLen; i++){
			seg = (TrackSegment*)pointIter->current->data;

      //list iterator malloc space
			ListIterator *pointIter3;
			pointIter3 = malloc(sizeof(ListIterator));
			*pointIter3 = createIterator(seg->waypoints);

			Waypoint *way;
      //get length
      int wayLen = getLength(seg->waypoints);
			for(int j = 0; j < wayLen; j++){
				way = (Waypoint*)pointIter3->current->data;


        //use haver for check source long and lat
				if(haver(sourceLong, way->longitude, sourceLat, way->latitude) <= delta){
					starter = 1;
				}
        //use dest for check dest long and lat
				if(haver(destLong, way->longitude,destLat, way->latitude) <= delta){
					ender = 1;
				}
				nextElement(pointIter3);
			}
			nextElement(pointIter2);
			free(pointIter3);
		}
    //if both starter and ender ==1 insert back
		if(starter == 1 && ender == 1){
			insertBack(tracks, temp);
		}
    //set starter and ender to 0
		starter = 0;
		ender = 0;
		nextElement(pointIter);
    //free lists
		free(pointIter2);
	}

	free(pointIter);

	if(getLength(tracks) <= 0){
		return NULL;
	}
	return tracks;
}
//module 3 A2
char* routeListToJSON(const List *list){
  //create buf malloc space
  char *buf;
	buf = malloc(sizeof(char*) * 1000);

	if(list == NULL){
		sprintf(buf, "[]");
		return buf;
	}

  //start of JSON List
	strcpy(buf, "[");

  //initulize list malloc space
	ListIterator *pointIter;
	pointIter = malloc(sizeof(ListIterator));
	pointIter->current = list->head;
	Route *temp;
	temp = (Route*)pointIter->current->data;
	int i = 0;
//loop through
	while((temp = nextElement(pointIter)) != NULL){
		i++;
	}
	pointIter->current = list->head;
	int k = 0;
	while((temp = nextElement(pointIter)) != NULL){
		strcat(buf, routeToJSON(temp));
		k++;
    //put commas
		if(k < i){
			strcat(buf, ",");
		}
	}
	strcat(buf, "]");

	free(pointIter);
	return buf;
}
char* trackListToJSON(const List *list){
  //create buf malloc space
  char *buf;
	buf = malloc(sizeof(char*) * 1000);

	if(list == NULL){
		sprintf(buf, "[]");
		return buf;
	}

	strcpy(buf, "[");

  //initulize list malloc space
	ListIterator *listpoint;
	listpoint = malloc(sizeof(ListIterator));
	listpoint->current = list->head;
	Track *temp;

	temp = (Track*)listpoint->current->data;
	int i = 0;
  //loop through
	while((temp = nextElement(listpoint)) != NULL){
		i++;
	}
	listpoint->current = list->head;
	int k = 0;
  //loop through then add comos
	while((temp = nextElement(listpoint)) != NULL){
		strcat(buf, trackToJSON(temp));
		k++;
		if(k < i){
			strcat(buf, ",");
		}
	}
  //end the json
	strcat(buf, "]");

	free(listpoint);
	return buf;
}

char* trackToJSON(const Track *tr){

	char *buf;
	buf = malloc(sizeof(char*) * 500);

  //when tr == null give empty JSON file
	if(tr == NULL){
		sprintf(buf, "{}");
		return buf;
	}
  //if name is empty
	if(strcmp(tr->name, "") == 0){
		if(isLoopTrack(tr, 15) == true){
			sprintf(buf, "{\"name\":\"None\",\"len\":%.1f,\"loop\":true}", round10(getTrackLen(tr)));
		}
		else{
			sprintf(buf, "{\"name\":\"None\",\"len\":%.1f,\"loop\":false}", round10(getTrackLen(tr)));
		}
	}
	else{
		if(isLoopTrack(tr, 15) == true){
			sprintf(buf, "{\"name\":\"%s\",\"len\":%.1f,\"loop\":true}", tr->name, round10(getTrackLen(tr)));
		}
		else{
			sprintf(buf, "{\"name\":\"%s\",\"len\":%.1f,\"loop\":false}", tr->name, round10(getTrackLen(tr)));
		}
	}
	return buf;
}


char* routeToJSON(const Route *rt){
  //malloc space
  char *buf;
	buf = malloc(sizeof(char*) * 500);
  //route obj == null return empty JSON
	if(rt == NULL){
		sprintf(buf, "{}");
		return buf;
	}

	if(strcmp(rt->name, "") == 0){
		if(isLoopRoute(rt, 15) == true){
			sprintf(buf, "{\"name\":\"None\",\"numPoints\":%d,\"len\":%.1f,\"loop\":true}", getLength(rt->waypoints), round10(getRouteLen(rt)));
		}
		else{
			sprintf(buf, "{\"name\":\"None\",\"numPoints\":%d,\"len\":%.1f,\"loop\":false}", getLength(rt->waypoints), round10(getRouteLen(rt)));
		}
	}
	else{
		if(isLoopRoute(rt, 15) == true){
			sprintf(buf, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":true}", rt->name, getLength(rt->waypoints), round10(getRouteLen(rt)));
		}
		else{
			sprintf(buf, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":false}", rt->name, getLength(rt->waypoints), round10(getRouteLen(rt)));
		}
	}
	return buf;
}
char* GPXtoJSON(const GPXdoc* gpx){
  //malloc space
  char *buf;
  buf = malloc(sizeof(char*)*1000);

  //return empty json when gpx obj == null
  if(gpx == NULL){
    sprintf(buf, "{}");
    return buf;
  }
  //create version char
  char *vers;
  vers = malloc(sizeof(char*)*10);
  sprintf(vers, "%.1f", gpx->version);

  sprintf(buf, "{\"version\":%s,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}", vers, gpx->creator, getLength(gpx->waypoints), getLength(gpx->routes), getLength(gpx->tracks));
  free(vers);
  return buf;
}

//Bonus
void addWaypoint(Route *rt, Waypoint *pt){
  if(rt == NULL || pt == NULL){
		return;
	}
	insertBack(rt->waypoints, pt);
	return;
}

void addRoute(GPXdoc* doc, Route* rt)
{
	if(rt == NULL || doc == NULL){
		return;
	}
	insertBack(doc->routes, rt);
	return;
}
GPXdoc* JSONtoGPX(const char* gpxString){

  if(gpxString == NULL){
		return NULL;
	}
  //gpx object malloc space
  GPXdoc *gpxPtr;
  gpxPtr = malloc(sizeof(GPXdoc));
  //copy in the namespace
  strcpy(gpxPtr->namespace, "http://www.topografix.com/GPX/1/1");

  //initialize lists
   gpxPtr->tracks = initializeList(trackToString, deleteTrack, compareTracks);
   gpxPtr->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
   gpxPtr->routes = initializeList(routeToString, deleteRoute, compareRoutes);

   //malloc space for creater
   gpxPtr->creator = malloc(sizeof(GPXdoc)*300);
	 char *bufstr;
   //malloc space for buf string
	 bufstr = malloc(sizeof(char*)*300);
   char *bufPtr;
   strcpy(bufstr, gpxString);
   //strtok the : , : }
   bufPtr = strtok(bufstr, ":");
   bufPtr = strtok(NULL, ",");
	 gpxPtr->version = atof(bufPtr);
	 bufPtr = strtok(NULL, ":");
	 bufPtr = strtok(bufPtr, "}");
	 strcpy(gpxPtr->creator, bufPtr);

	return gpxPtr;

}
Waypoint* JSONtoWaypoint(const char* gpxString){
  //when the string == null return null
  if(gpxString == NULL){
		return NULL;
	}
  //malloc space
  Waypoint *pointPtr;
	pointPtr = malloc(sizeof(Waypoint));
	pointPtr->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);

  char *bufstr;
  //malloc space for the buf string
	bufstr = malloc(sizeof(char*)*300);
  char *bufPtr;

  strcpy(bufstr, gpxString);
  //strtok for : ; : }
  bufPtr = strtok(bufstr, ":");
  bufPtr = strtok(NULL, ",");
  //use atof for lat
	pointPtr->latitude = atof(bufPtr);
	bufPtr = strtok(NULL, ":");
	bufPtr = strtok(NULL, "}");
  //use atof for long
	pointPtr->longitude = atof(bufPtr);
	return pointPtr;
}

Route* JSONtoRoute(const char* gpxString){
  //if gpx obj == null return null
  if(gpxString == NULL){
		return NULL;
	}
  //malloc space for ptr
  Route *routePtr;
	routePtr = malloc(sizeof(Route));

  //malloc space for name and intulize lists
  routePtr->name = malloc(sizeof(Route)*256);
  routePtr->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);
  routePtr->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);

  //create buf string and malloc space
  char *bufstr;
  bufstr = malloc(256);
  //buf ptr
  char *bufPtr;
  strcpy(bufstr, gpxString);
  bufPtr = strtok(bufstr, ":");
  bufPtr = strtok(NULL, "\"");
	bufPtr = strtok(bufPtr, "\"");
	strcpy(routePtr->name, bufPtr);

	return routePtr;
}
