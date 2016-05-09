#include<opencv2/opencv.hpp>
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
using namespace cv;
#include<iostream>
using namespace std;
bool select_flag=false;
Point origin;
Rect select;
void on_Mouse(int event,int x,int y,int,void*)
{
	//Point origin;//不能在这个地方进行定义，因为这是基于消息响应的函数，执行完后origin就释放了，所以达不到效果。
    if(select_flag)
    {
        select.x=MIN(origin.x,x);//不一定要等鼠标弹起才计算矩形框，而应该在鼠标按下开始到弹起这段时间实时计算所选矩形框
        select.y=MIN(origin.y,y);
        select.width=abs(x-origin.x);//算矩形宽度和高度
        select.height=abs(y-origin.y);
		//保证所选矩形框在视频显示区域之内
    }
    if(event==CV_EVENT_LBUTTONDOWN)
    {
        select_flag=true;//鼠标按下的标志赋真值
        origin=Point(x,y);//保存下来单击是捕捉到的点
        select=Rect(x,y,0,0);//这里一定要初始化，宽和高为(0,0)是因为在opencv中Rect矩形框类内的点是包含左上角那个点的，但是不含右下角那个点
    }
    else if(event==CV_EVENT_LBUTTONUP)
    {
        select_flag=false;
    }
}
class Pointer_Movement_Setting
{
private:
	int rate;//采样数
	Mat *frame;//帧组
	Mat current;//当前帧
	VideoCapture cam;//获取到的摄像机信息
	float GThreshold_min;//下限
	float GThreshold_max;//上限
	int ROI_x1;//ROI参数
	int ROI_y1;
	int ROI_x2;
	int ROI_y2;
	int per_of_ROI;
	//bool msg;
public:
	int row;//宽
	int col;//高
	Pointer_Movement_Setting(VideoCapture n,float th_min,float th_max,int rate);
	void IfMove();
	void ROISetting();//更改ROI
	void ROISetting(int x1,int y1, int x2,int y2);
	void ChangeRate(int);
};

Pointer_Movement_Setting::Pointer_Movement_Setting(VideoCapture n,float th_min=50,float th_max=100,int r=3)
{
	cam=n;
	GThreshold_min = th_min;
	GThreshold_max= th_max;
	Mat a;
	if(cam.isOpened())
	{
		cam>>a;
		row=a.rows;
		col=a.cols;
	}
	rate=r;
	frame=new Mat[rate];
	ROI_x1=0;
	ROI_y1=0;
	ROI_x2=row;
	ROI_y2=col;
	per_of_ROI=5;
}

void Pointer_Movement_Setting::IfMove()
{
	Mat a;
	int DiffCount;
	int MaxDiff;
	VideoWriter WR;
	namedWindow("current",CV_WINDOW_AUTOSIZE);
	WR.open("output.avi",CV_FOURCC('D', 'I', 'V', 'X'),25,Size(ROI_x2-ROI_x1,ROI_y2-ROI_y1),false);
	bool IsEmpty=false;
	while(waitKey(40)!=27&&!IsEmpty)
	{
		MaxDiff=0;
		for(int i=0;i<rate;i++)
		{
			DiffCount=0;
			cam>>a;
			if(a.empty())
			{
				IsEmpty=true;
				break;
			}
			//rectangle(a,Point(ROI_x1,ROI_y1),Point(ROI_x2,ROI_y2),Scalar(255,0,0),1,8,0);
			current=a.clone();
			a=a(Range(ROI_y1,ROI_y2),Range(ROI_x1,ROI_x2));//change
			cvtColor(a,frame[i],CV_BGR2GRAY);
			for(int j=0;j<a.rows;j++)
			{
				for(int k=0;k<a.cols;k++)
				{
					if(frame[i].at<uchar>(j,k)<=GThreshold_max&&frame[i].at<uchar>(j,k)>=GThreshold_min)
					{
						frame[i].at<uchar>(j,k)=0;
					}
					else 
						frame[i].at<uchar>(j,k)=255;
					if(i!=0&&(frame[i].at<uchar>(j,k)!=frame[0].at<uchar>(j,k)))
						DiffCount++;
				}
			}
			/*threshold(frame[i],frame[i],GThreshold_max,255,THRESH_TOZERO_INV);
			threshold(frame[i],frame[i],GThreshold_min,255,THRESH_BINARY);
			for(int j=0;j<a.rows;j++)
			{
				for(int k=0;k<a.cols;k++)
				{
					if(i!=0&&(frame[i].at<uchar>(j,k)!=frame[0].at<uchar>(j,k)))
						DiffCount++;
				}
			}*/
			imshow("current",current);
			//imshow("test",frame[i]);
			if(WR.isOpened())
				WR<<frame[i];
			else cout<<"error"<<endl;
			MaxDiff=(MaxDiff>DiffCount)?MaxDiff:DiffCount;
		}
		if(MaxDiff>frame[0].rows*frame[0].cols*per_of_ROI/100)cout<<1;
		else cout<<0;
	}
	cout<<endl;
	destroyWindow("current");
	destroyWindow("test");
}

void Pointer_Movement_Setting::ROISetting()
{
	Mat a;
	Mat b;
	cam>>a;
	namedWindow("press enter to confirm",1);
	setMouseCallback("press enter to confirm",on_Mouse,0);
	while(waitKey(20)!=13)
	{
		b=a.clone();
		ROI_x1=select.x;
		ROI_y1=select.y;
		ROI_x2=select.x+select.width;
		ROI_y2=select.y+select.height;
		rectangle(b,select,Scalar(255,0,0),1,8,0);
		imshow("press enter to confirm",b);
	}
	destroyWindow("press enter to confirm");
	cout<<"ROI_x1="<<ROI_x1<<endl;
	cout<<"ROI_y1="<<ROI_y1<<endl;
	cout<<"ROI_x2="<<ROI_x2<<endl;
	cout<<"ROI_y2="<<ROI_y2<<endl;
}

void Pointer_Movement_Setting::ChangeRate(int n)
{
	this->per_of_ROI=n;
}

void Pointer_Movement_Setting::	ROISetting(int x1,int y1, int x2,int y2)
{
	ROI_x1=x1;
	ROI_x2=x2;
	ROI_y1=y1;
	ROI_y2=y2;
}