#ifndef GPX_HELPER_H
#define GPX_HELPER_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>

#include "LinkedListAPI.h"
#include "GPXParser.h"

char *toStringFunc(int type, List *value);
int getDataWayPoint(List *data);
GPXdoc *details(xmlDoc *doc, xmlNode *root, GPXdoc *obj);
GPXdoc *routeGetter(xmlNode *node, xmlDoc *doc, GPXdoc *obj,Route *rObj);
GPXdoc *trackDetGetter(xmlNode *node, xmlDoc *doc,GPXdoc *obj,Track *tObj);
GPXdoc *trackSegGetter(xmlDoc *doc,xmlNode *node,GPXdoc *obj,Track *tObj, TrackSegment *tsObj);
GPXdoc *waypointDeatsGetter( TrackSegment *tsObj,GPXdoc *obj, Waypoint *wObj,Route *rObj, xmlNode *node, xmlDoc *doc);
GPXdoc *parser(xmlNode *node, xmlDoc *doc, GPXdoc *obj);
float haver(float lon1, float lon2, float lat1, float lat2);
void gpxTree(GPXdoc * doc, xmlNodePtr * root);



#endif
