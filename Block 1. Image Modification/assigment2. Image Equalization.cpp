#include <opencv2/core/core.hpp> //core routines
#include <opencv2/highgui/highgui.hpp>//imread,imshow,namedWindow,waitKey
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
using namespace std;



int main(int argc,char **argv){
	try{
	
		if(argc!=2) {cerr<<"Usage:image"<<endl;return 0;} 
		
		cv::Mat image, grayimage;

		//Load the image 
		image=cv::imread(argv[1]);
		
		cv::cvtColor(image, grayimage, cv::COLOR_BGR2GRAY);
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
		cv::equalizeHist(image,image);
		cv::imshow("equalizeHist function Image",image);
		vector<int> colors(255);
		vector<int> sumcolors(255);
		for( int x=0;x<grayimage.rows;x++){
			for( int y=0;y<grayimage.cols;y++){
				uchar *ptr=grayimage.ptr<uchar>(x)+y;
				colors[ptr[0]]=colors[ptr[0]]+1;
				
			}
		}
		int suma =0;
		for ( int i =0;i<256;i++){
			suma+=colors[i];
			sumcolors[i]=suma;
		}
		int division=sumcolors[255]/255;
		for ( int i =0;i<256;i++){
			
			sumcolors[i]/=division;
		}

		for( int x=0;x<grayimage.rows;x++){
			for( int y=0;y<grayimage.cols;y++){
				uchar *ptr=grayimage.ptr<uchar>(x)+y;
				ptr[0]=sumcolors[ptr[0]];
				
			}
		}
		
		
		cv::imshow("Equalized Image",grayimage);
		char c=0;
		while(c!=27)  //waits until ESC pressed
			c=cv::waitKey(0);

	}catch(std::exception &ex){
	  cout<<ex.what()<<endl;
	}
}