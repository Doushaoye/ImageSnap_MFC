#pragma once

#include "stdafx.h"
#include "vector"
#include <map>
#include <opencv2\opencv.hpp>
#include "ImageScrollView.h"
using namespace std;

//typedef enum
//{
//	PERSON,
//	CAR,
//}OBJCALSS;


typedef enum
{
	//ren
	Lred,
	Lgreen,
	Mred,
	Mgreen,
	Rred,
	Rgreen,
	Lyellow,
	Myellow,
	Ryellow,
	Unkown
}PARTCALSS;




typedef enum
{
	//ren
	FRONTAL,
	LEFT,
	RIGHT,
	BACK,
	UNKNOWN
}POSE;

typedef enum
{
	//ren
	PRO_OBJ = 11,
	PRO_OBJ_INDEX,
	PRO_OBJ_CLASS,
	PRO_OBJ_POSE,
	PRO_OBJ_TRU,

	PRO_OBJ_OCC,
	PRO_OBJ_ENABLE,
	PRO_OBJ_LEFT,
	PRO_OBJ_TOP,
	PRO_OBJ_RIGHT,
	PRO_OBJ_BUTTOM,

	PRO_PART = 51,
	PRO_PART_INDEX,
	PRO_PART_OBJ,
	PRO_PART_NAME,
	PRO_PART_ENABLE,

	PRO_PART_LEFT,
	PRO_PART_TOP,
	PRO_PART_RIGHT,
	PRO_PART_BUTTOM

}PROITEM;


struct ss_property
{
	string property_name;
	string property_options;
	int property_value;
	int property_id;
};

typedef struct _Part
{
	int ipartclass;
	CRect partrect;
	vector <cv::Point> polygon;
	BOOL ablerect;
	BOOL enable;
	char color[100];
}_Part;

typedef struct _Object
{
	int ischange;
	int iobjclass;
	string classname;
	int pose;
	BOOL truncated;
	BOOL occluded;
	BOOL difficult;
	vector<ss_property> o_property;
	CRect objbox;
	vector <_Part> partlist;
	BOOL enable;
}_Object;

typedef struct _Curve
{
	string classname;
	int iobjclass;
	int pose;
	BOOL truncated;
	BOOL occluded;
	BOOL difficult;
	vector<CurvePoint> objcurve;
	vector<ss_property> o_property;
	vector <_Part> partlist;
	BOOL enable;
}_Curve;

typedef struct _Polygon
{
	string classname;
	int iobjclass;
	int pose;
	BOOL truncated;
	BOOL occluded;
	BOOL difficult;
	vector<cv::Point> objpolygon;
	vector<ss_property> o_property;
	vector <_Part> partlist;
	BOOL enable;
}_Polygon;

typedef struct _Image
{
	vector<ss_property> o_property;
	BOOL enable;
}_Image;