/*Zachery Abbas
Assignment 1
1019463
*/
#include "GPXHelper.h"
#include "GPXParser.h"
/*
Logic behind this function is that this function gets a list and a type
the list will the lists from the structs in GPXParser and the value specifies which list it is
these will be used in the toString functions

used: https://stackoverflow.com/questions/14948932/how-to-create-my-own-string-functions-in-c to help me with the
toString fucntion
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
			sprintf(temp, "\n** ");
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
				wObj->latitude = atof((char *)(attr->children->content));
				longFlag = longFlag + 1;
			}
			else if (strcmp((char *)(attr->name), "lat") == 0)
			{
				wObj->longitude = atof((char *)(attr->children->content));
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

	//needed to the errror checking before getting the name and other data
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
					GPXData *dObj = (GPXData *)calloc(1, sizeof(GPXData));

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
