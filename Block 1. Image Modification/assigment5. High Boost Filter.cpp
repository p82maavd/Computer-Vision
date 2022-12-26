#include <opencv2/core/core.hpp> //core routines
#include <opencv2/highgui/highgui.hpp>//imread,imshow,namedWindow,waitKey
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
using namespace std;


void applyKernel(cv::Mat &in, cv::Mat &out, cv::Mat &kernel){
	float media=0.0;
	uchar *ptr;
	float *ptr2;
	int min=kernel.cols/2*(-1);
	int max=kernel.cols/2;
	int size=kernel.cols*kernel.cols;
	
	for( int r=0;r<in.rows;r++){
		float *ptr3=out.ptr<float>(r);
		for( int c=0;c<in.cols;c++){
			
			for(int i=min; i<=max; i++){
				ptr=in.ptr<uchar>(r+i);
				ptr2=kernel.ptr<float>(i+max);
				for(int j=min; j<=max; j++){
						
					if(r+i>=0 && r+i<=in.rows && c+j>=0 && c+j<=in.cols ){
						
						media+=(ptr[c+j]*ptr2[j+max]);
						
					}
				}
			}
			ptr3[c]=media/size;
			
			media=0.0;		
		}
	}

}
int main(int argc,char **argv){
	try{
	
		if(argc!=4) {cerr<<"Usage:image blur_level boost_factor"<<endl;return 0;} 
		
		cv::Mat kernel(atoi(argv[2]), atoi(argv[2]), CV_32FC1,1.0f);
		//Load the image .
		cv::Mat image;
		image=cv::imread(argv[1]);
		//Change image to grayscale 
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);


		cv::Mat outImage;
		image.convertTo(outImage, CV_32FC1);

		applyKernel(image,outImage,kernel);
		
		
		
		for( int r=0;r<image.rows;r++){
		
			uchar *ptr=image.ptr<uchar>(r);
			float *ptr2=outImage.ptr<float>(r);
			for( int c=0;c<image.cols;c++){
				
				ptr2[c]=ptr[c]+(ptr[c]-ptr2[c])*atoi(argv[3]);
				
					
			}
		}

		cv::Mat finalImage;
		double min, max;
		cv::minMaxLoc(outImage,&min, &max);
		cv::normalize(outImage,finalImage,max,min,cv::NORM_MINMAX,CV_8UC1 );

		
		cv::imshow("Input Image",image);
		
		cv::imshow("Output Image",finalImage);
		cv::imwrite("hbf.jpg", finalImage);
		char c=0;
		while(c!=27)  //waits until ESC pressed
			c=cv::waitKey(0);

	}catch(std::exception &ex){
	  cout<<ex.what()<<endl;
	}
}

//https://iq.opengenus.org/sharpening-filters/