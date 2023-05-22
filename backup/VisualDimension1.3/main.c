#include "vdMeasure.h"
#define NLEN 20
#define THRESH 125
#define MAX_LV  2 //cvDrawContours()参数，表示画出的最大数据结构层数

void find_show_contours(CvMemStorage* storage, CvSeq** first_contour);
//为什么传递CvSeq*的话first_contour依旧是NULL？
//答：应该是因为first_contour的值会改变，所以需要传递cvseq**
//这样就能通过指针直接修改调用函数的first_contour?
///为什么传递CvMemStorage*就可行，而不是CvMemStorage**？

//为什么使用vdMeasure.c中定义的函数时会报错（已经解决）
//error: undeclare reference to (some function) ?（类似这样）
//答：好像是因为IDE把我的代码当成了C++而不是C
//解决方法：创建一个opencv工程，拷贝或自己写纯C程序
//这样好像IDE就默认此工程为C？这时将原代码拷贝覆盖纯C代码
//注意：纯C的话，函数有默认参数也要传递此参数！！！

int main (void)
{
    strcpy(vdUnit,"pixels");
    int i,j;
    double radius;
    i=1;
    CvSeq* first_contour=NULL;
    CvMemStorage* storage=cvCreateMemStorage(0);
    find_show_contours(storage,&first_contour);

    ///提示输入的时候图片显示窗口灰色，可能需要多线程
    ///或者把find_show_contours拆分开来！！！
    puts("Enter the true radius of the smallest circle contour reference:");
    scanf("%lf",&radius);   //为了省事，和下面用同一个变量
    getchar();
    puts("Enter the unit name:");
    gets(vdUnit);
    vdFindReference(first_contour,radius);
    printf("Pixels Per %s: %f\n\n",vdUnit,vdPixelsPerUnit);

    for(CvSeq* c=first_contour;c!=NULL;c=c->h_next,i++)
    {
        printf("Contour: #%d Elements: %d ",i,c->total);
        if((radius=vdIsCircle(c))>0)
            printf("is circle. Radius: %g %s\n",radius,vdUnit);
        else
        {
            puts("is not circle.");
            if(vdIsPoly(c))
            {
                puts("\tIt is polygon.");
                vdPrintPolyLines(c);
            }
        }
        j=1;
        for(CvSeq* h=c->v_next;h!=NULL;h=h->h_next,j++)
        {
            printf("\tHole: #%d Elements: %d ",j,h->total);
            if((radius=vdIsCircle(h))>0)
                printf("is circle. Radius: %g %s\n",radius,vdUnit);
            else
                puts("is not circle.");
        }
    }
    cvWaitKey(0);
    //有没有和cvDestroyAllWindows()类似的release image函数？
    cvReleaseMemStorage(&storage);   //画蛇添足一下，养成“好习惯”
    cvDestroyAllWindows();
    return 0;
}

void find_show_contours(CvMemStorage* storage, CvSeq** first_contour)
{
    char name[NLEN];
    int num_contours;    //轮廓数量
    //img_black为背景黑色,有时作为temp来用,确保原图像不被改变

    IplImage * img, * img_black;
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
        storage,    ///为什么传递CvMemStorage*就行，而不是CvMemStorage**？
        first_contour,
        sizeof(CvContour),
        CV_RETR_TREE,
        CV_CHAIN_APPROX_SIMPLE,
        cvPoint(0,0)
    );
    cvZero(img_black); //全黑
    cvDrawContours(img_black,*first_contour,cvScalarAll(255),
                   cvScalarAll(255),MAX_LV,1,8,cvPoint(0,0));
    cvShowImage("Contours",img_black);
    printf("Total contours and holes detected: %d\n",num_contours);
    cvReleaseImage(&img);
    cvReleaseImage(&img_black);
}
