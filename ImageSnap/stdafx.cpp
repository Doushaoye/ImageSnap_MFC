
// stdafx.cpp : 只包括标准包含文件的源文件
// ImageSnap.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

char objclass[NUM_OBJ][200] = {"person","car"};
//char objclass[NUM_OBJ][50];
//char g_pose[NUM_POSE][50] = {"Frontal","Left","Right","Back","Unspecified"};
char g_pose[NUM_POSE][50] = {"Frontal","Left","Right","Rear","Unspecified"};
char g_part[NUM_PART][80] = { "Lred",
"Lgreen",
"Mred",
"Mgreen",
"Rred",
"Rgreen",
"Lyellow",
"Myellow",
"Ryellow",
"Unkown"};
int tmp_class=0;
BOOL ischange=FALSE;

