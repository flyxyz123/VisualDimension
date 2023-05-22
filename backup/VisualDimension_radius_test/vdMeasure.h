#ifndef VDMEASURE_H_
#define VDMEASURE_H_

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

///可能圆的半径需要是整数才精确？？？感觉是谬论

double vdPixelsPerUnit;     //不能在头文件中初始化变量？
//pixels per mm or inch or...

char vdUnit[20];
//单位名

double vdIsCircle(const CvSeq* c);  ///能不能检测弧形？

double vdCircleRadius(const CvSeq* c);
///比vdIsCircle()中的avg略好，但误差仍旧很大

double vdCircleRadius2(const CvSeq* c);
///算法极差，但却是暂时最佳圆半径函数，不抗噪音

float vdLine(const CvPoint* a, const CvPoint* b);
//传递两个整数坐标

float vdLineFloat(const CvPoint2D32f* a, const CvPoint2D32f* b);
//传递两个浮点数坐标，为使得vdIsCircle()函数更为精准而设定
//不知道可不可以用联合，效果如何？

CvSeq* vdFindReference(CvSeq* first_contour, const double radius);
//寻找最小圆形轮廓(最上层)为参考物
//radius为真实半径，可为任意单位
//返回参考物的轮廓

double vdConvert(double pixels);

#endif // VDMEASURE_H_
