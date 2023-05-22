#ifndef VDMEASURE_H_
#define VDMEASURE_H_

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

#define VDCONVERT(PIXELS) ((PIXELS)/(vdPixelsPerUnit))

double vdPixelsPerUnit;
//pixels per mm or inch or...

char vdUnit[20];
//单位名

double vdIsCircle(const CvSeq* c);  ///能不能检测弧形？
double vdRadius1(const CvSeq* c);   //轮廓上“所有点的直径”相加后求平均值再除以2
double vdRadius2(const CvSeq* c);   //轮廓最高减最低点，最左减最右点，二直径相加/2
double vdRadius3(const CvSeq* c);   //求出圆形轮廓面积再求出半径
///检测周长误差极大，不采取
//三种检测半径方法

bool vdIsPoly(const CvSeq* c);
///偷懒检测多边形法

void vdPrintPolyLines(const CvSeq* c);

float vdLine(const CvPoint* a, const CvPoint* b);
//传递两个整数坐标

float vdLineFloat(const CvPoint2D32f* a, const CvPoint2D32f* b);
//传递两个浮点数坐标，为使得vdIsCircle()函数更为精准而设定
//不知道可不可以用联合，效果如何？

CvSeq* vdFindReference(CvSeq* first_contour, const double radius);
//寻找最小圆形轮廓(最上层)为参考物
//radius为真实半径，可为任意单位
//返回参考物的轮廓

#endif // VDMEASURE_H_
