#ifndef VDMEASURE_H_
#define VDMEASURE_H_

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

float pixels_per_xx; //pixels per mm or inch or...

float vdIsCircle(const CvSeq* c);
float vdLine(const CvPoint* a, const CvPoint* b);
//传递两个整数坐标

float vdLineFloat(const CvPoint2D32f* a, const CvPoint2D32f* b);
//传递两个浮点数坐标，为使得vdIsCircle()函数更为精准而设定

#endif // VDMEASURE_H_
