#include "vdMeasure.h"
#include <limits.h>

double vdCircleRadius(const CvSeq* c)
///比vdIsCircle()中的avg略好，但误差仍旧很大
{
    int total=c->total;
    float maxDiameter=0.0;
    float tempDiameter;
    double sumDiameter=0.0;
    CvPoint *point1, *point2;

    for(int i=0;i<total;i++)
    {
        point1=(CvPoint*)cvGetSeqElem(c,i);
        for(int j=0;j<total;j++)
        {
            point2=(CvPoint*)cvGetSeqElem(c,j);
            if((tempDiameter=vdLine(point1,point2))>maxDiameter)
                maxDiameter=tempDiameter;
        }
        sumDiameter+=maxDiameter;
    }
    printf(" $%.10f$ ",sumDiameter);
    return sumDiameter/(double)total/2.0;
}

double vdCircleRadius2(const CvSeq* c)
///算法极差，但却是暂时最佳圆半径函数，不抗噪音
{
    CvPoint* tempPoint;
    int max_x,min_x,max_y,min_y;
    max_x=max_y=0;
    min_x=min_y=INT_MAX;

    for(int i=0;i<c->total;i++)
    {
        tempPoint=(CvPoint*)cvGetSeqElem(c,i);
        if(tempPoint->x>max_x)
            max_x=tempPoint->x;
        if(tempPoint->x<min_x)
            min_x=tempPoint->x;
        if(tempPoint->y>max_y)
            max_y=tempPoint->y;
        if(tempPoint->y<min_y)
            min_y=tempPoint->y;
    }
    if(c->v_prev!=NULL)
        return ((double)max_x-(double)min_x+(double)max_y-(double)min_y)/4.0;
    else
        return round(((double)max_x-(double)min_x+(double)max_y-(double)min_y)/4.0-1);
}

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
    avg=sum/total;
    sum=0.0;
    for(int i=0;i<total;i++)
    {
        sum+=(dist[i]-avg)*(dist[i]-avg);
        //printf("$%g$",dist[i]);
    }
    sum=sqrt(sum/(double)total);         //参考网址中这儿为啥是sum/=total-1???
    //标准差=sqrt(方差)
    if(sum<1)   //若是圆，返回半径
    {
        if(vdPixelsPerUnit==0)
            return avg;
        else
            return vdConvert(avg);
    }
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
    double min_radius=0;
    CvSeq *reference_circle;

    for(CvSeq* c=first_contour;c!=NULL;c=c->h_next)
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

double vdConvert(double pixels)
{
    return pixels/vdPixelsPerUnit;
}
