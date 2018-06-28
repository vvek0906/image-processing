#include <opencv\cv.h>
#include <opencv\highgui.h>

using namespace std;
using namespace cv;

const static int SENSITIVITY_VALUE = 20;
const static int BLUR_SIZE = 10;
int theObject[2] = {0,0};
Rect objectBoundingRectangle = Rect(0,0,0,0);


string intToString(int number){

	std::stringstream ss;
	ss << number;
	return ss.str();
}

void searchForMovement(Mat thresholdImage, Mat &cameraFeed){
	bool objectDetected = false;
	Mat temp;
	thresholdImage.copyTo(temp);
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE );

	if(contours.size()>0)objectDetected=true;
	else objectDetected = false;

	if(objectDetected){
		vector< vector<Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size()-1));
		objectBoundingRectangle = boundingRect(largestContourVec.at(0));
		int xpos = objectBoundingRectangle.x+objectBoundingRectangle.width/2;
		int ypos = objectBoundingRectangle.y+objectBoundingRectangle.height/2;

		theObject[0] = xpos , theObject[1] = ypos;
	}
	int x = theObject[0];
	int y = theObject[1];
	
	//draw some crosshairs around the object
	circle(cameraFeed,Point(x,y),20,Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
	line(cameraFeed,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);

	//write the position of the object to the screen
	putText(cameraFeed,"Tracking object at (" + intToString(x)+","+intToString(y)+")",Point(x,y),1,1,Scalar(255,0,0),2);

	

}
int main(){

		bool objectDetected = false;
		bool debugMode = false;
	bool trackingEnabled = false;
		bool pause = false;
	Mat frame1,frame2;
	Mat grayImage1,grayImage2;
	Mat differenceImage;
	Mat thresholdImage;
	VideoCapture capture;

	while(1){

		
		capture.open("bouncingBall.mp4");

		if(!capture.isOpened()){
			cout<<"ERROR ACQUIRING VIDEO FEED\n";
			getchar();
			return -1;
		}

		while(capture.get(CV_CAP_PROP_POS_FRAMES)<capture.get(CV_CAP_PROP_FRAME_COUNT)-1){

			
			capture.read(frame1);
			
			cvtColor(frame1,grayImage1,COLOR_BGR2GRAY);
			
			capture.read(frame2);
			
			cvtColor(frame2,grayImage2,COLOR_BGR2GRAY);
			absdiff(grayImage1,grayImage2,differenceImage);
			threshold(differenceImage,thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
			if(debugMode==true){
				imshow("Difference Image",differenceImage);
				imshow("Threshold Image", thresholdImage);
			}else{
				destroyWindow("Difference Image");
				destroyWindow("Threshold Image");
			}
			blur(thresholdImage,thresholdImage,Size(BLUR_SIZE,BLUR_SIZE));
			threshold(thresholdImage,thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY);
			if(debugMode==true){
				
				imshow("Final Threshold Image",thresholdImage);

			}
			else {
				destroyWindow("Final Threshold Image");
			}

			if(trackingEnabled){

				searchForMovement(thresholdImage,frame1);
			}

			imshow("Frame1",frame1);
			switch(waitKey(10)){

			case 27: 
				return 0;
			case 116: 
				trackingEnabled = !trackingEnabled;
				if(trackingEnabled == false) cout<<"Tracking disabled."<<endl;
				else cout<<"Tracking enabled."<<endl;
				break;
			case 100: 
				debugMode = !debugMode;
				if(debugMode == false) cout<<"Debug mode disabled."<<endl;
				else cout<<"Debug mode enabled."<<endl;
				break;
			case 112:
				pause = !pause;
				if(pause == true){ cout<<"Code paused, press 'p' again to resume"<<endl;
				while (pause == true){
					
					switch (waitKey()){
						
					case 112: 
						
						pause = false;
						cout<<"Code Resumed"<<endl;
						break;
					}
				}
				}



			}
		}
		//release the capture before re-opening and looping again.
		capture.release();
	}

	return 0;

}
