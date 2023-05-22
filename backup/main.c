#include "vdMeasure.h"
#include <stdio.h>
#define NLEN 20
#define THRESH 125
#define MAX_LV  2 //cvDrawContours()参数，表示画出的最大数据结构层数

CvSeq*

///为什么使用vdMeasure.c中定义的函数时会报错？？

int main (void)
{
    int num_contours;    //轮廓数量
    int i,j;
    i=1;        ///要放到循环“开头”
    char name[NLEN];
    IplImage * img, * img_black;
    //img_black为背景黑色,有时作为temp来用,确保原图像不被改变

    CvSeq* first_contour=NULL;
    CvMemStorage* storage=cvCreateMemStorage();

    puts("Enter the source image name: (with extension)");
    gets(name);
    img=cvLoadImage(name,CV_LOAD_IMAGE_GRAYSCALE);
    img_black=cvCreateImage(cvGetSize(img),8,1);
    cvThreshold(img,img_black,THRESH,255,CV_THRESH_BINARY_INV);
    cvNamedWindow(name,CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Contours",CV_WINDOW_AUTOSIZE);
    cvShowImage(name,img_black);
    num_contours=cvFindContours(
        img_black,
        storage,
        &first_contour,
        sizeof(CvContour),
        CV_RETR_TREE,
        CV_CHAIN_APPROX_SIMPLE
    );
    cvZero(img_black); //全黑
    cvDrawContours(img_black,first_contour,cvScalarAll(255),
                   cvScalarAll(255),MAX_LV);
    cvShowImage("Contours",img_black);
    printf("Total contours and holes dected: %d\n",num_contours);

    for(CvSeq* c=first_contour;c!=NULL;c=c->h_next,i++)
    {
        printf("Contour %d ",i);
        if(vdIsCircle(c))
            puts("is circle.");
        else
            puts("is not circle.");
        j=1;
        for(CvSeq* h=c->v_next;h!=NULL;h=h->h_next,j++)
        {
            printf("Hole %d ",j);
            if(vdIsCircle(h))
                puts("is circle.");
            else
                puts("is not circle.");
        }
    }
    cvWaitKey(0);
    cvReleaseImage(&img);
    cvReleaseImage(&img_black);
    cvDestroyAllWindows();

    //以后可能会需要cvClearMemStorage();
    return 0;
}


