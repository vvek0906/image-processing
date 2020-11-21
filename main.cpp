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
	vector< vector<Point> > contours;  // point class holds the x,y coordinates of a given pixel/location in image area
	vector<Vec4i> hierarchy; //storage structure for upto 4 dimensions
	findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE ); //Contours can be explained simply as a curve
	//joining all the continuous points (along the boundary), having same color or intensity. The contours are a useful tool 
	//for shape analysis and object detection and recognition. Contours here is collection of continuous points in image.

	if(contours.size()>0)objectDetected=true;
	else objectDetected = false;

	if(objectDetected){
		vector< vector<Point> > largestContourVec;
		largestContourVec.push_back(contours.at(contours.size()-1)); // largest contour is selected
		objectBoundingRectangle = boundingRect(largestContourVec.at(0)); // bounding rectangle or minnenclosing circle can be used to mark the object.
		int xpos = objectBoundingRectangle.x+objectBoundingRectangle.width/2; // center x coordinate
		int ypos = objectBoundingRectangle.y+objectBoundingRectangle.height/2; // center y coordinate

		theObject[0] = xpos , theObject[1] = ypos;
	}
	int x = theObject[0];
	int y = theObject[1];
	
	//draw some crosshairs around the object
	circle(cameraFeed,Point(x,y),20,Scalar(0,255,0),2); //appoint a circle for object
	line(cameraFeed,Point(x,y),Point(x,y-25),Scalar(0,255,0),2); // mark the center plus sign of circle containing object.
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
	Mat frame1,frame2;   // Mat class of OPENCV is used to store values of pixels of images
	Mat grayImage1,grayImage2;
	Mat differenceImage;
	Mat thresholdImage;
	VideoCapture capture;

	while(1){

		
		capture.open("bouncingBall.mp4"); // capture the target video

		if(!capture.isOpened()){ //error check for capturing
			cout<<"ERROR ACQUIRING VIDEO FEED\n";
			getchar();
			return -1;
		}

		while(capture.get(CV_CAP_PROP_POS_FRAMES)<capture.get(CV_CAP_PROP_FRAME_COUNT)-1){

			
			capture.read(frame1); //reading frames
			
			cvtColor(frame1,grayImage1,COLOR_BGR2GRAY); //converting to grayscale format
			
			capture.read(frame2);
			
			cvtColor(frame2,grayImage2,COLOR_BGR2GRAY);
			absdiff(grayImage1,grayImage2,differenceImage); // gets difference of images pixels wise
			threshold(differenceImage,thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY); //applying threshold to image; pixel values below threshold are reduced to 0 rest are maximized.
			if(debugMode==true){								// for developer's purpose to observe diff of images
				imshow("Difference Image",differenceImage);
				imshow("Threshold Image", thresholdImage);
			}else{
				destroyWindow("Difference Image");
				destroyWindow("Threshold Image");
			}
			blur(thresholdImage,thresholdImage,Size(BLUR_SIZE,BLUR_SIZE));    // add a constant blur to pixels
			threshold(thresholdImage,thresholdImage,SENSITIVITY_VALUE,255,THRESH_BINARY); // again threshold entire image
			if(debugMode==true){
				
				imshow("Final Threshold Image",thresholdImage);

			}
			else {
				destroyWindow("Final Threshold Image");
			}

			if(trackingEnabled){   //key board control to enable tracking of object in video

				searchForMovement(thresholdImage,frame1);
			}

			imshow("Frame1",frame1);
			switch(waitKey(10)){     //waits for something to be pressed on video

			case 27:       // press escape to exit
				return 0;
			case 116:                 //press T to enable tracking
				trackingEnabled = !trackingEnabled;
				if(trackingEnabled == false) cout<<"Tracking disabled."<<endl;
				else cout<<"Tracking enabled."<<endl;
				break;
			case 100: 		// press D to debug
				debugMode = !debugMode;
				if(debugMode == false) cout<<"Debug mode disabled."<<endl;
				else cout<<"Debug mode enabled."<<endl;
				break;
			case 112:             // press P to pause
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
