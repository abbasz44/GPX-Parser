#include "GPXHelper.h"
#include "GPXParser.h"
/*
Logic behind this function is that this function gets a list and a type
the list will the lists from the structs in GPXParser and the value specifies which list it is
these will be used in the toString functions
*/
char *toStringFunc(int type, List *value)
{
	void *obj = NULL;
	char *returner = (char *)calloc(5500, sizeof(char));
	char *temp = (char *)calloc(65, sizeof(char));
	int i = 0;

	ListIterator lister = createIterator(value);
	while ((obj = nextElement(&lister)) != NULL)
	{
		char *tempStr = NULL;
		i = i + 1;

		if (type == 1)
		{
			//routes
			sprintf(temp, "%d:\n", i);
			strcat(returner, temp);
			tempStr = routeToString(obj);
		}
		else if (type == 2)
		{
			//tracks
			sprintf(temp, "%d:\n", i);
			strcat(returner, temp);
			tempStr = trackToString(obj);
		}
		else if (type == 3)
		{
			//otherData of a waypont
			sprintf(temp, "\t- ");
			strcat(returner, temp);
			tempStr = gpxDataToString(obj);
		}
		else if (type == 4)
		{
			//waypoints
			sprintf(temp, "\n%d:\n", i);
			strcat(returner, temp);
			tempStr = waypointToString(obj);
		}
		else if (type == 5)
		{
			//track seg
			sprintf(temp, "\n%d :\n\n", i);
			strcat(returner, temp);
			tempStr = trackSegmentToString(obj);
		}
		else
		{
			//all other data
			sprintf(temp, " ");
			strcat(returner, temp);
			tempStr = gpxDataToString(obj);
		}

		strcat(returner, tempStr);
		free(tempStr);
	}

	free(temp);
	return returner;
}
/*
This function gets the details in the gpx doc before we get to the nitty gritty stuff
*/
GPXdoc *details(xmlDoc *doc, xmlNode *root, GPXdoc *obj)
{
	xmlAttr *element;
	int vFlag = 0;
	int cFlag = 0;

	// gets all the information
	for (element = root->properties; element != NULL; element = element->next)
	{
		if (strcmp((char *)(element->name), "creator") == 0)
		{
			char *creatorName = (char *)(element->children->content);
			obj->creator = malloc(sizeof(char) * strlen(creatorName) + 1);
			strcpy(obj->creator, creatorName);
			cFlag = cFlag + 1;
		}
		if (strcmp((char *)(element->name), "version") == 0)
		{
			obj->version = atof((char *)(element->children->content));
			vFlag = vFlag + 1;
		}
	}
	//use these flags when they hit it means it is time to free the infortation
	if ((cFlag != 1) || (vFlag != 1))
	{
		xmlFreeDoc(doc);
		deleteGPXdoc(obj);
		return NULL;
	}

	return obj;
}
/*
this function gets the waypoint data
*/
int getDataWayPoint(List *data)
{
	int num = 0;
	Waypoint *obj;
	ListIterator lister = createIterator(data);

	if (data == NULL)
	{
		return 0;
	}

	while ((obj = nextElement(&lister)) != NULL)
	{
		if (strlen(obj->name) != 0)
			num = num + 1;

		num = num + getLength(obj->otherData);
	}

	return num;
}
/*
this is the route getter where i will be getting all the information from the GPX file
it gets all the routes information and is the parser for the routes info to put in the struct
*/
GPXdoc *routeGetter(xmlNode *node, xmlDoc *doc, GPXdoc *obj,Route *rObj)
{
//error handling make sure freeing stuff so no memory leeks
	if (node->properties != NULL)
	{
		xmlFreeDoc(doc);
		insertBack(obj->routes, (void *)rObj);
		deleteGPXdoc(obj);
		return NULL;
	}

//error handling
	if ((node->children == NULL) || (node->children->next == NULL))
	{
		insertBack(obj->routes, (void *)rObj);
		return obj;
	}

//once passes all checks then it gets the name other data
	else
	{
		xmlNode *current = NULL;

		for (current = node->children->next; current != NULL; current = current->next->next)
		{
			if (current->type != XML_COMMENT_NODE)
			{
				if (strcmp((char *)(current->name), "name") == 0)
				{
					strcpy(rObj->name, (char *)(current->children->content));
				}
				else if (strcmp((char *)(current->name), "rtept") == 0)
				{
					//had to calloc here was getting memory error with the malloc (which is what i prefer)
					Waypoint *wObj = (Waypoint *)calloc(1, sizeof(Waypoint));
					wObj->name = (char *)calloc(250, sizeof(char));
					wObj->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
					obj = waypointDeatsGetter(NULL,obj, wObj, rObj, current, doc);

					if (obj == NULL)
					{
						return NULL;
					}
				}
				else
				{
					GPXData *dObj = malloc(sizeof(GPXData) + sizeof(char) * (strlen((char*)current->children->content) + strlen((char*)current->name)) + 1);
					strcpy(dObj->name, (char *)(current->name));
					strcpy(dObj->value, (char *)(current->children->content));

					insertBack(rObj->otherData, (void *)dObj);
				}
			}
		}

		// Inserting into linked list
		insertBack(obj->routes, (void *)rObj);
	}

	return obj;
}
/*
this is the track getter where i will be getting all the information from the GPX file
it gets all the tracks information and is the parser for the routes info to put in the struct
*/
GPXdoc *trackDetGetter(xmlNode *node, xmlDoc *doc,GPXdoc *obj,Track *tObj)
{
	//when found name it changes to 1
	int foundName = 0;

//error handling make sure free everything
	if (node->properties != NULL)
	{
		xmlFreeDoc(doc);
		insertBack(obj->tracks, (void *)tObj);
		deleteGPXdoc(obj);
		return NULL;
	}
//error handling
	if ((node->children == NULL) || (node->children->next == NULL))
	{
		insertBack(obj->tracks, (void *)tObj);
		return obj;
	}
	// getting the track inforation now
	else
	{
		xmlNode *current = NULL;

		for (current = node->children->next; current != NULL; current = current->next->next)
		{
			if (current->type != XML_COMMENT_NODE)
			{
				if (strcmp((char *)(current->name), "name") == 0)
				{
					tObj->name = malloc(sizeof(char) * strlen((char *)current->children->content) + 1);
					strcpy(tObj->name, (char *)(current->children->content));
					foundName = 1;
				}
				else if (strcmp((char *)(current->name), "trkseg") == 0)
				{
					// I actually need to initialize a list to go in *segments. Create a function that returns a list of tracksegments
					// In that function, initialize a list of waypoints, and use similar logic to wptdetails to parse that stuff and return a the list
					TrackSegment *tsObj = malloc(sizeof(TrackSegment));
					tsObj->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
					obj = trackSegGetter(doc,current,obj,tObj,tsObj);

					if (obj == NULL)
					{
						return NULL;
					}
				}
				else
				{
					GPXData *dObj = malloc(sizeof(GPXData) + sizeof(char) * (strlen((char*)current->children->content) + strlen((char*)current->name)) + 1);
					strcpy(dObj->name, (char *)(current->name));
					strcpy(dObj->value, (char *)(current->children->content));
					insertBack(tObj->otherData, (void *)dObj);
				}
			}
		}

		//making sure name is empty string
		if (foundName == 0) {
			tObj->name = malloc(sizeof(char));
			strcpy(tObj->name, "");
		}
		//insert into linked list
		insertBack(obj->tracks, (void *)tObj);
	}

	return obj;
}
GPXdoc *trackSegGetter(xmlDoc *doc,xmlNode *node,GPXdoc *obj,Track *tObj, TrackSegment *tsObj)
{
	//error handling free everything
	if (node->properties != NULL)
	{
		xmlFreeDoc(doc);
		//for segements and tracks
		insertBack(tObj->segments, (void *)tsObj);
		insertBack(obj->tracks, (void *)tObj);
		deleteGPXdoc(obj);
		return NULL;
	}

	//error handing
	if ((node->children == NULL) || (node->children->next == NULL))
	{
		insertBack(tObj->segments, (void *)tsObj);
		return obj;
	}
	//getting information
	else
	{
		xmlNode *current = NULL;

		for (current = node->children->next; current != NULL; current = current->next->next)
		{
			if (current->type != XML_COMMENT_NODE)
			{
				if (strcmp((char *)(current->name), "trkpt") == 0)
				{
					Waypoint *wObj = (Waypoint *)calloc(1, sizeof(Waypoint));
					wObj->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
					obj = waypointDeatsGetter(tsObj,obj, wObj, NULL, current, doc);

					if (obj == NULL)
					{
						return NULL;
					}
				}
				//freeing information make sure no memory leaks
				else
				{
					xmlFreeDoc(doc);
					insertBack(tObj->segments, (void *)tsObj);
					insertBack(obj->tracks, (void *)tObj);
					deleteGPXdoc(obj);
					return NULL;
				}
			}
		}

		// Inserting in the linked list
		insertBack(tObj->segments, (void *)tsObj);
	}

	return obj;
}
/*
the way point getter
the is used in all the functions above to get the way points for each of the waypoints trying to get
*/
GPXdoc *waypointDeatsGetter( TrackSegment *tsObj,GPXdoc *obj, Waypoint *wObj,Route *rObj, xmlNode *node, xmlDoc *doc)
{
	int flag = 0;
	int foundName = 0;


	xmlAttr *attr = node->properties;
	//if there is no lat or long set flag to 1
	if (attr == NULL)
	{
		flag = 1;
	}
	//if there is then make lat and long to 0
	else
	{
		int longFlag = 0;
		int latFlag = 0;


		for (attr = node->properties; attr != NULL; attr = attr->next)
		{
			if (strcmp((char *)(attr->name), "lon") == 0)
			{
				wObj->longitude = atof((char *)(attr->children->content));
				longFlag = longFlag + 1;
			}
			else if (strcmp((char *)(attr->name), "lat") == 0)
			{
				wObj->latitude = atof((char *)(attr->children->content));
				latFlag = latFlag + 1;
			}

			else
			{
			// printf("error found ");

				flag = 1;
			}
		}


		if ((latFlag != 1) || (longFlag != 1))
		{
			// printf("error found");
			flag = 1;
		}
	}

//when there us an error free doc and inster back
	if (flag == 1)
	{
		xmlFreeDoc(doc);
		if ((rObj == NULL) && (tsObj == NULL))
		{
			insertBack(obj->waypoints, (void *)wObj);
		}
		else if (tsObj == NULL)
		{
			insertBack(rObj->waypoints, (void *)wObj);
		}
		else
		{
			insertBack(tsObj->waypoints, (void *)wObj);
		}
		deleteGPXdoc(obj);
		return NULL;
	}

	//error handling
	if ((node->children == NULL) || (node->children->next == NULL))
	{
		//make sure name is empty string when not found
		if (foundName == 0) {
			wObj->name = malloc(sizeof(char));
			strcpy(wObj->name, "");
		}
//more error handling
		if ((rObj == NULL) && (tsObj == NULL))
		{
			insertBack(obj->waypoints, (void *)wObj);
		}
		else if (tsObj == NULL)
		{
			insertBack(rObj->waypoints, (void *)wObj);
		}
		else
		{
			insertBack(tsObj->waypoints, (void *)wObj);
		}
		return obj;
	}
	//now getting name and other data
	else
	{
		xmlNode *current = NULL;

		for (current = node->children->next; current != NULL; current = current->next->next)
		{
			if (current->type != XML_COMMENT_NODE)
			{
				if (strcmp((char *)(current->name), "name") == 0)
				{
					wObj->name = malloc(sizeof(char) * strlen((char *)(current->children->content)) + 1);
					strcpy(wObj->name, (char *)(current->children->content));
					foundName = 1;
				}
				else
				{
					GPXData *dObj = malloc(sizeof(GPXData) + sizeof(char) * (strlen((char*)current->children->content) + strlen((char*)current->name)) + 1);

					strcpy(dObj->name, (char *)(current->name));
					strcpy(dObj->value, (char *)(current->children->content));

					insertBack(wObj->otherData, (void *)dObj);
				}
			}
		}
		//make sure name is set to empty string
		if (foundName == 0) {
			wObj->name = malloc(sizeof(char));
			strcpy(wObj->name, "");
		}

		//put stuff into list
		if ((rObj == NULL) && (tsObj == NULL))
		{
			insertBack(obj->waypoints, (void *)wObj);
		}
		else if (tsObj == NULL)
		{
			insertBack(rObj->waypoints, (void *)wObj);
		}
		else
		{
			insertBack(tsObj->waypoints, (void *)wObj);
		}
	}

	return obj;
}
/*
this is where all the fucntions come together and parse the tree
*/
GPXdoc *parser(xmlNode *node, xmlDoc *doc, GPXdoc *obj)
{
	xmlNode *current = NULL;


	for (current = node; current != NULL; current = current->next->next)
	{
		if (current->type != XML_COMMENT_NODE)
		{
			if (strcmp((char *)(current->name), "wpt") == 0)
			{
				Waypoint *wObj = (Waypoint *)malloc(sizeof(Waypoint) * 1);
				wObj->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
				obj = waypointDeatsGetter(NULL,obj, wObj, NULL,  current, doc);

				if (obj == NULL)
				{
					return NULL;
				}
			}
			else if (strcmp((char *)(current->name), "rte") == 0)
			{
				Route *rObj = (Route *)malloc(sizeof(Route) * 1);
				rObj->name = (char *)malloc(sizeof(char) * 100);
				rObj->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
				rObj->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
				obj = routeGetter(current, doc, obj,rObj);

				if (obj == NULL)
				{
					return NULL;
				}
			}
			else if (strcmp((char *)(current->name), "trk") == 0)
			{
				Track *tObj = (Track *)malloc(sizeof(Track) * 1);
				tObj->name = (char *)malloc(sizeof(char) * 100);
				tObj->segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
				tObj->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
				obj = trackDetGetter(current, doc,obj,tObj);

				if (obj == NULL)
				{
					return NULL;
				}
			}
			//free everything make sure no memory leaks
			else
			{
				xmlFreeDoc(doc);
				deleteGPXdoc(obj);
				return NULL;
			}
		}
	}

	return obj;
}
//haversine fromaula to get distance between 2 points
//equation from https://www.movable-type.co.uk/scripts/latlong.html
float haver(float lon1, float lon2, float lat1, float lat2){
	float x = (lat2 - lat1) * (M_PI / 180);
	float y = (lon2 - lon1) * (M_PI / 180);
	float a = 0;
	float c = 0;
	float z = 0;
	float num = 6371000;

	a = sin(x/2) * sin(x/2) + sin(y/2) * sin(y/2) *cos(lat1 * (M_PI / 180)) * cos(lat2 * (M_PI / 180));
	c = 2 * atan2(sqrt(a), sqrt(1-a));
	z = num *c ;

	return z;
}

void gpxTree(GPXdoc * doc, xmlNodePtr * root)
{
	char tempStr[30];
	xmlNodePtr nodeRo = *root;
	xmlNodePtr child = NULL;
	xmlNodePtr inn = NULL;
	xmlNodePtr node = NULL;


	//create new prop and snprintf the version new prop for version creator and name space
	snprintf(tempStr, sizeof(tempStr), "%.1f", doc->version);
	xmlNewProp(nodeRo, BAD_CAST "version", BAD_CAST tempStr);
	xmlNewProp(nodeRo, BAD_CAST "creator", BAD_CAST doc->creator);


	//for way point and get length when it isn't zero
	int len = getLength(doc->waypoints);
	if(len != 0)
	{
		ListIterator iterr = createIterator(doc->waypoints);

		Waypoint * wp  = nextElement(&iterr);

		while(wp != NULL)
		{
			node = xmlNewChild(nodeRo, NULL, BAD_CAST "wpt", NULL);

			if(getLength(wp->otherData) != 0)
			{
				ListIterator iterr2 = createIterator(wp->otherData);

				GPXData * otherdataVar = nextElement(&iterr2);

				//when gpx data obj doesnt = null
				while(otherdataVar != NULL)
				{
					if (strcmp(otherdataVar->name, "ele") == 0){
					xmlNewChild(node, NULL, BAD_CAST "ele", BAD_CAST otherdataVar->value);
				}
					else{
						xmlNewProp(node, BAD_CAST otherdataVar->name, BAD_CAST otherdataVar->value);
					}
					//new element
					otherdataVar = nextElement(&iterr2);
				}

			}

			if(strcmp(wp->name, "\0") != 0){
				xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST wp->name);
			}

			//lat and long
			snprintf(tempStr, sizeof(tempStr), "%.6f", wp->latitude);
			xmlNewProp(node, BAD_CAST "lat", BAD_CAST tempStr);

			snprintf(tempStr, sizeof(tempStr), "%.6f", wp->longitude);
			xmlNewProp(node, BAD_CAST "lon", BAD_CAST tempStr);

			wp = nextElement(&iterr);
		}
	}

	//now for route
	if(getLength(doc->routes) != 0)
	{
		node = xmlNewChild(nodeRo, NULL, BAD_CAST "rte", NULL);
		ListIterator iterr1 = createIterator(doc->routes);

		Route * routeObj  = nextElement(&iterr1);

		while(routeObj != NULL)
		{
			if(strcmp(routeObj->name, "\0") != 0){
				xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST routeObj->name);
		}
			int oDatLen= getLength(routeObj->otherData);
			if( oDatLen != 0)
			{
				//list iterr
				ListIterator iterr2 = createIterator(routeObj->otherData);
				GPXData * otherDta = nextElement(&iterr2);

				while(otherDta != NULL)
				{
					if(strcmp(otherDta->name,"desc") == 0){
						child = xmlNewChild(node, NULL, BAD_CAST otherDta->name, BAD_CAST otherDta->value);
				}
				else{
					xmlNewProp(node, BAD_CAST otherDta->name, BAD_CAST otherDta->value);
				}

					otherDta = nextElement(&iterr2);
				}

			}
			int wayLen = getLength(routeObj->waypoints);
			if( wayLen != 0)
			{
				ListIterator iterr3 = createIterator(routeObj->waypoints);

				Waypoint * objEle = nextElement(&iterr3);

				while(objEle != NULL)
				{
					child = xmlNewChild(node, NULL, BAD_CAST "rtept", NULL);

					if(strcmp(objEle->name, "\0") != 0){
						xmlNewChild(child, NULL, BAD_CAST "name", BAD_CAST objEle->name);
					}
					//for lat and long
					snprintf(tempStr, sizeof(tempStr), "%.6f", objEle->latitude);
					xmlNewProp(child, BAD_CAST "lat", BAD_CAST tempStr);

					snprintf(tempStr, sizeof(tempStr), "%.6f", objEle->longitude);
					xmlNewProp(child, BAD_CAST "lon", BAD_CAST tempStr);

					//go through other data
					ListIterator iterr4 = createIterator(objEle->otherData);
					GPXData * wayObjEle = nextElement(&iterr4);

					//when the other data obj != NULL
					while(wayObjEle != NULL)
					{
						xmlNewProp(child, BAD_CAST wayObjEle->name, BAD_CAST wayObjEle->value);
						//next ele
						wayObjEle = nextElement(&iterr4);
					}
					objEle = nextElement(&iterr3);
				}
			}
			routeObj = nextElement(&iterr1);
		}
	}
	int trackLen = getLength(doc->tracks);
	if(trackLen != 0)
	{

		node = xmlNewChild(nodeRo, NULL, BAD_CAST "trk", NULL);
		ListIterator iterr1 = createIterator(doc->tracks);

		Track * trkObj  = nextElement(&iterr1);

		while(trkObj != NULL)
		{
			child = xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST trkObj->name);

			if(getLength(trkObj->otherData) != 0)
			{

				ListIterator iterr2 = createIterator(trkObj->otherData);
				GPXData * dataObj = nextElement(&iterr2);

				while(dataObj != NULL)
				{
					if(strcmp(dataObj->name,"desc") == 0){
					child = xmlNewChild(node, NULL, BAD_CAST dataObj->name, BAD_CAST dataObj->value);
				}
				else{
					xmlNewProp(node, BAD_CAST dataObj->name, BAD_CAST dataObj->value);
				}
					dataObj = nextElement(&iterr2);
				}
			}
			int segLen = getLength(trkObj->segments);
			if(segLen != 0)
			{
				//go through list
				ListIterator iterr5 = createIterator(trkObj->segments);
				TrackSegment * segObj = nextElement(&iterr5);

				while(segObj != NULL)
				{
					child = xmlNewChild(node, NULL, BAD_CAST "trkseg", NULL);
					ListIterator iterr6 = createIterator(segObj->waypoints);

					Waypoint * segWpObj = nextElement(&iterr6);

					while(segWpObj != NULL)
					{

						inn = xmlNewChild(child, NULL, BAD_CAST "trkpt", NULL);

						if(strcmp(segWpObj->name, "\0") != 0){
							xmlNewChild(inn, NULL, BAD_CAST "name", BAD_CAST segWpObj->name);
					}
						//get lan and long
						snprintf(tempStr, sizeof(tempStr), "%.6f", segWpObj->latitude);
						xmlNewProp(inn, BAD_CAST "lat", BAD_CAST tempStr);

						snprintf(tempStr, sizeof(tempStr), "%.6f", segWpObj->longitude);
						xmlNewProp(inn, BAD_CAST "lon", BAD_CAST tempStr);

						ListIterator iterr7 = createIterator(segWpObj->otherData);

						GPXData * trkDataObj = nextElement(&iterr7);

						//till NULL
						while(trkDataObj != NULL)
						{
							if(strcmp(trkDataObj->name,"ele") == 0)
							xmlNewChild(inn, NULL, BAD_CAST trkDataObj->name, BAD_CAST trkDataObj->value);

							else
							xmlNewProp(inn, BAD_CAST trkDataObj->name, BAD_CAST trkDataObj->value);

							trkDataObj = nextElement(&iterr7);
						}

						segWpObj = nextElement(&iterr6);
					}

					segObj = nextElement(&iterr5);
				}
			}

				trkObj = nextElement(&iterr1);
		}
	}

	*root = nodeRo;
}
