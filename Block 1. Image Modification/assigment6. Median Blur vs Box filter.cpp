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
		uchar *ptr3=out.ptr<uchar>(r);
		for( int c=0;c<in.cols;c++){
			
			for(int i=min; i<=max; i++){
				ptr=in.ptr<uchar>(r+i);
				ptr2=kernel.ptr<float>(i+kernel.cols/2);
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

void medianFilter(cv::Mat &in,cv::Mat &out, int size){


	float media=0.0;
	uchar *ptr;
	float *ptr2;
	int min=size/2*(-1);
	int max=size/2;
	
	vector<int> median;
	
	for( int r=0;r<in.rows;r++){
		uchar *ptr2=out.ptr<uchar>(r);
		for( int c=0;c<in.cols;c++){
			
			for(int i=min; i<=max; i++){
				ptr=in.ptr<uchar>(r+i);
				for(int j=min; j<=max; j++){
						
					if(r+i>=0 && r+i<=in.rows && c+j>=0 && c+j<=in.cols ){
						
						median.push_back(ptr[c+j]);
						
					}
				}
			}
			sort (median.begin(), median.end());
			ptr2[c]=median[median.size()/2];
			//ptr2[c]=medianC(median);
			
			median.clear();
			
				
		}
	}


}
int main(int argc,char **argv){
	try{
	
		if(argc!=3) {cerr<<"Usage:image kernel_size"<<endl;return 0;} 

		if(atoi(argv[2])%2==0) {cerr<<"kernel_size must be odd"<<endl;return 0;} 
		
		cv::Mat image, boxFilterImage, medianFilterImage;
		
		
		cv::Mat kernel(atoi(argv[2]), atoi(argv[2]), CV_32FC1,1.0f);



		//Load the image 
		image=cv::imread(argv[1]);
		//Change image to grayscale 
		//cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

		vector<cv::Mat> rgbchannel;
    	cv::split(image, rgbchannel);

		image.copyTo(boxFilterImage);

		vector<cv::Mat> rgbchannelb;
    	cv::split(boxFilterImage, rgbchannelb);

		image.copyTo(medianFilterImage);

		vector<cv::Mat> rgbchannelm;
    	cv::split(medianFilterImage, rgbchannelm);

    	for(int i=0; i<3; i++){
    		applyKernel(rgbchannel[i],rgbchannelb[i],kernel);
    	}
		
		for(int i=0; i<3; i++){
			medianFilter(rgbchannel[i],rgbchannelm[i],atoi(argv[2]));
		}
		//cv::merge(rgbchannelb, boxFilterImage);
		cv::merge(rgbchannelm, medianFilterImage);
		
		cv::imshow("Input Image",image);
		cv::imshow("Box Filter Image",boxFilterImage);
		cv::imshow("Median Filter Image",medianFilterImage);
		char c=0;
		while(c!=27)  //waits until ESC pressed
			c=cv::waitKey(0);

	}catch(std::exception &ex){
	  cout<<ex.what()<<endl;
	}
}