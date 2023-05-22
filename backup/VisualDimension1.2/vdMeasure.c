#include "vdMeasure.h"

double vdIsCircle(const CvSeq* c) ///需要测试！！返回的半径可能并不准确！
{
    double avg;      //轮廓各店到原点的平均距离，相当于半径
    double sum=0.0;
    int total=c->total;     ///我存储了total，因为多次调用，可能并没有优化
    float dist[total];
    CvBox2D rect=cvFitEllipse2(c);
    CvPoint2D32f center={rect.center.x, rect.center.y};
    //rect.center中存储的是float类型xy坐标
    //cvPoint中存储的是int类型，二者不同

    for(int i=0;i<total;i++)
    {
        CvPoint* tempPointInt=(CvPoint*)cvGetSeqElem(c,i);
        CvPoint2D32f tempPointFloat;
        tempPointFloat.x=(float)(tempPointInt->x);
        tempPointFloat.y=(float)(tempPointInt->y);
        dist[i]=vdLineFloat(&center,&tempPointFloat);
        sum+=dist[i];
    }
    avg=sum/(double)total;
    sum=0.0;
    for(int i=0;i<total;i++)
    {
        sum+=(dist[i]-avg)*(dist[i]-avg);
        //printf("$%g$",dist[i]);
    }
    sum=sqrt(sum/(double)total);         //参考网址中这儿为啥是su/=total-1???
    //标准差=sqrt(方差)
    if(sum<1)
        return avg;     //若是圆，返回半径
    else
        return -1.0;       //否则返回-1.0表示非圆
}

float vdLine(const CvPoint* a, const CvPoint* b)
{
    return sqrt((a->x-b->x)*(a->x-b->x)+(a->y-b->y)*(a->y-b->y));
}

float vdLineFloat(const CvPoint2D32f* a, const CvPoint2D32f* b)
{
    return sqrt((a->x-b->x)*(a->x-b->x)+(a->y-b->y)*(a->y-b->y));
}

CvSeq* vdFindReference(CvSeq* first_contour, const double radius)
{
    int i=0;
    double min_radius=0;
    CvSeq *reference_circle;

    for(CvSeq* c=first_contour;c!=NULL;c=c->h_next,i++)
    {
        double radius;
        if((radius=vdIsCircle(c))>0 && (min_radius==0 || radius<min_radius))
        {
            min_radius=radius;
            reference_circle=c;
        }
    }
    vdPixelsPerUnit=min_radius/radius;
    return reference_circle;
}

