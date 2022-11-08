#include "vdMeasure.h"
#include <limits.h> //这个是用来干啥的？

static int is_equal(const void* a, const void* b, void* userdata);

double vdRadius1(const CvSeq* c)
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
    return sumDiameter/(double)total/2.0;
}

double vdRadius2(const CvSeq* c)
///算法极差，但却是暂时最佳圆半径函数，不抗噪音
///返回的实际上是int
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

double vdRadius3(const CvSeq* c)   //求出圆形轮廓面积再求出半径
{
    return sqrt(cvContourArea(c,CV_WHOLE_SEQ,0)/M_PI);
}

///contour circle 貌似比hole circle略大?
double vdIsCircle(const CvSeq* c)
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
    if(sum<1)   //若是圆，返回半径
    {
        if(vdPixelsPerUnit==0)
            return vdRadius3(c);
        else
            return VDCONVERT(vdRadius3(c));
    }
    else
        return -1.0;       //否则返回-1.0表示非圆
}

bool vdIsPoly(const CvSeq* c)///偷懒
{
    return (c->total<10)?true:false;
}

bool vdIsPolyCurve(const CvSeq* c)///偷懒检测是否为开口多边形+弧线
{
    return (c->total>10)?true:false;
}

void vdPrintPolyLines(const CvSeq* c)
{
    for(int i=0;i<c->total;i++)
    {
        float length=vdLine((CvPoint *)cvGetSeqElem(c,i),
        (CvPoint *)cvGetSeqElem(c,i!=c->total-1?i+1:0));
        if(vdPixelsPerUnit!=0)
            length=VDCONVERT(length);
        printf("\tLine: #%d Length: %g %s\n",i+1,length,vdUnit);
    }
}

void vdPrintPolyCurve(const CvSeq* c)   ///很乱，需改进
{
    CvMemStorage* temp=cvCreateMemStorage(0);
    ///存储空间使用默认大小可能过大，浪费空间，需要优化！！！
    CvSeq* labels=NULL;     //输出序列，指向整数的指针序列
    int userdata=20;
    int classCount=cvSeqPartition(c,temp,&labels,is_equal,&userdata);
    ///输出找到几个类,判断两点距离<20来找到弧线，不精确！需要优化！！！
    ///小于一个常量会导致像素变大或变小时找不到弧线！
    int * classElmCount=(int *)calloc(classCount,sizeof(int));
    //int"数组"存储每个相同类中有几个元素
    int curveCount,lineCount; //弧线数量和直线数量
    curveCount=lineCount=1;
    CvSlice curveSlice;
    CvSeq* curveSeq=NULL;
    VDCURVE curve;
    int i,j;
    float length;

    for(int i=0;i<labels->total;i++)
        classElmCount[*(int*)cvGetSeqElem(labels,i)]++;
    //遍历labels序列找出每个相同类中有几个元素

    printf("\tClassCount: %d\n",classCount);
    for(i=j=0;i<classCount && j<c->total;i++)
    {
        //忽略顶点以及转角的2点现象，剩下的是弧线，
        //找到弧线的起始结束点并切片传递给测量弧线函数
        if(classElmCount[i]>2)
        {
            curveSlice.start_index=j;
            j=j+classElmCount[i]-1;
            curveSlice.end_index=j;
            curveSeq=cvSeqSlice(c,curveSlice,NULL,0);
            ///copy_data 标志是否要复制元素,如果不为0,则需要复制;否则,则不需要复制?不懂
            ///上面是网上的，书中说copy_data表示是否深度复制，若深度复制就要复制每个元素
            curve=vdGetCurve(curveSeq);
            printf("\tCurve: #%d Radius: %g %s Centre: (%.0f,%.0f)\n",
                   curveCount++,curve.radius,vdUnit,curve.centre.x,curve.centre.y);
        }
        ///可以稍微改动vdPrintPolyLines()来简化下面两个if
        else if(classElmCount[i]==2)    ///忽略非转角二点而是斜线的情况，需改进
        {
            length=vdLine((CvPoint*)cvGetSeqElem(c,j+1),
                          (CvPoint*)cvGetSeqElem(c,(j+1)!=c->total-1?j+2:0));
            if(vdPixelsPerUnit!=0)
                length=VDCONVERT(length);
            printf("\tLine: #%d Length: %g %s\n",lineCount++,length,vdUnit);
            j=j+2;
        }

        if(classElmCount[i]==1 || classElmCount[i]>2)//圆弧后也可能要打印直线
        {
            length=vdLine((CvPoint*)cvGetSeqElem(c,j),
                          (CvPoint*)cvGetSeqElem(c,j!=c->total-1?j+1:0));
            if(vdPixelsPerUnit!=0)
                length=VDCONVERT(length);
            printf("\tLine: #%d Length: %g %s\n",lineCount++,length,vdUnit);
            j++;
        }
    }

    cvReleaseMemStorage(&temp);
    free(classElmCount);
}

VDCURVE vdGetCurve(const CvSeq* c)  //与vdIsCircle()基本一样，用avg算法
{
    double avg=0.0;      //轮廓各店到原点的平均距离，相当于半径
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
    if(sum<1)   //若是圆，返回半径
    {
        if(vdPixelsPerUnit==0)
            return (VDCURVE){rect.center,avg};
        else
            return (VDCURVE){rect.center,VDCONVERT(avg)};   //复合文字
    }
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

///------------------------------------

static int is_equal(const void* a, const void* b, void* userdata)
{
    return vdLine((CvPoint*)a,(CvPoint*)b)<*(int*)(userdata)?true:false;
    //若两点距离小于*userdata，返回true表示是同一类，反之不是同一类
}
