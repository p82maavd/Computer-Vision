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
	
	for( int r=0;r<in.rows;r++){
		uchar *ptr3=out.ptr<uchar>(r);
		for( int c=0;c<in.cols;c++){
			
			for(int i=kernel.cols/2*(-1); i<=kernel.cols/2; i++){
				ptr=in.ptr<uchar>(r+i);
				ptr2=kernel.ptr<float>(i+kernel.cols/2);
				for(int j=kernel.cols/2*(-1); j<=kernel.cols/2; j++){
						
					if(r+i>=0 && r+i<=in.rows && c+j>=0 && c+j<=in.cols ){
						
						media+=(ptr[c+j]*ptr2[j+kernel.cols/2])/(kernel.cols*kernel.cols);
						
					}
				}
			}
			ptr3[c]=media;
			
			media=0.0;
			
				
		}
	}


}
int main(int argc,char **argv){
	try{
	
		if(argc!=3) {cerr<<"Usage:image kernel_size"<<endl;return 0;} 

		if(atoi(argv[2])%2==0) {cerr<<"kernel_size must be odd"<<endl;return 0;} 
		
		cv::Mat image, outImage;
		
		
		cv::Mat kernel(atoi(argv[2]), atoi(argv[2]), CV_32FC1,1.0f);

		//Load the image 
		image=cv::imread(argv[1]);
		//Change image to grayscale 
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
		image.copyTo(outImage);

		applyKernel(image,outImage,kernel);
		
		cv::imshow("Input Image",image);
		cv::imshow("Output Image",outImage);
		char c=0;
		while(c!=27)  //waits until ESC pressed
			c=cv::waitKey(0);

	}catch(std::exception &ex){
	  cout<<ex.what()<<endl;
	}
}
