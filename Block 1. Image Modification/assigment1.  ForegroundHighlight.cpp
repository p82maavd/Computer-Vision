#include <opencv2/core/core.hpp> //core routines
#include <opencv2/highgui/highgui.hpp>//imread,imshow,namedWindow,waitKey
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
using namespace std;

cv::Mat image;
cv::Mat copyimage;
struct MouseParams
{
    vector<cv::Point> rectangle;
    
};

void on_mouse( int event, int x, int y, int flags, void* param )
{
	MouseParams* mp = (MouseParams*)param;
	//Replace the image with the original one, this is necesary because in another iterations we have the image with a rectangle drawed
	copyimage.copyTo(image);
	
	if ( event == cv::EVENT_MOUSEMOVE )
    {
    	
    	if(mp->rectangle.size()==1 ){
			//Draw the rectangle
    		cv::rectangle(image,mp->rectangle[0],cv::Point(x,y),cv::Scalar::all(0));
    	}
    }



	if (event==cv::EVENT_RBUTTONDOWN) 
	{

	  	mp->rectangle.push_back(cv::Point(x,y));	 	

		if(mp->rectangle.size()==2){
	  
			// Calculation of the colored rectangle area

		  	int xleft;
		  	int xright;
		  	int yright;
		  	int yleft;


		  	if(mp->rectangle[0].x < mp->rectangle[1].x){
		  		xleft=mp->rectangle[0].x;
		  		xright=mp->rectangle[1].x;
		  	}

		  	else{
		  		xleft=mp->rectangle[1].x;
		  		xright=mp->rectangle[0].x;
		  	}

	  		
	  		if(mp->rectangle[0].y < mp->rectangle[1].y){
		  		yleft=mp->rectangle[0].y;
		  		yright=mp->rectangle[1].y;
		  	}

		  	else{
		  		yleft=mp->rectangle[1].y;
		  		yright=mp->rectangle[0].y;
		  	}

			auto area=cv::Rect(xleft,yleft,xright-xleft,yright-yleft);

			//Load the image in grayscale

			cv::Mat grayimage;
	  		cv::cvtColor(image, grayimage, cv::COLOR_BGR2GRAY);

	  		//Copy of the colored area in the grayscale image

			cv::Mat rectangle= image(area).clone();
			//We need the image in bgr
			cv::cvtColor(grayimage, grayimage, cv::COLOR_GRAY2BGR);
			rectangle.copyTo(grayimage(area));


	  		cv::imshow("Result Image",grayimage);
	  		mp->rectangle.pop_back();
	  		mp->rectangle.pop_back();

	  	}


	}
  
  //refresh the image window
  cv::imshow("Original Image",image);

}

int main(int argc,char **argv){
	try{
	
		if(argc!=2) {cerr<<"Usage:image"<<endl;return 0;} 
		//Load the image 
		image=cv::imread(argv[1]);
		//Create a copy of the original image
		image.copyTo(copyimage);
		
		cv::namedWindow("Original Image");

		MouseParams mp;
		
		
		cv::setMouseCallback( "Original Image", on_mouse, (void*)&mp );
		
		cv::imshow("Original Image",image);
		char c=0;
		while(c!=27)  //waits until ESC pressed
			c=cv::waitKey(0);

	}catch(std::exception &ex){
	  cout<<ex.what()<<endl;
	}
}