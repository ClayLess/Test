#include"If_Move.h"
#include<iostream>
using namespace std;
int main(void)
{
	system("pause");
	VideoCapture camera("test_3.mp4");
	Pointer_Movement_Setting A(camera,80,180);
	//A.ROISetting();
	A.ROISetting(160,150,260,230);
	A.ChangeRate(5);
	A.IfMove();
	/*Mat frame0;
	while(1)
	{
		camera0>>frame0;
		imshow("camera",frame0);
		if(waitKey(10)==27)
			break;
	}
	destroyWindow("camera");
	camera0.release();*/
	system("pause");
}