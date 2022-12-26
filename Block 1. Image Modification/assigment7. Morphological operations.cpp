
#include <opencv2/core/core.hpp> //core routines
#include <opencv2/highgui/highgui.hpp>//imread,imshow,namedWindow,waitKey
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
using namespace std;

void threshold(cv::Mat &in, int thresh_val){
	try{
	uchar *ptr;
	
	for( int r=0;r<in.rows;r++){
		uchar *ptr=in.ptr<uchar>(r);
		for( int c=0;c<in.cols;c++){
			if(ptr[c]>thresh_val){
				ptr[c]=255;
			}
			else{
				ptr[c]=0;
			}
		}
	}

	}catch(std::exception &ex){
	  cout<<ex.what()<<endl;
	}


}

void erode(cv::Mat &in,cv::Mat &out, int size){
	
	uchar *ptr;
	uchar *ptr2;
	int min=size/2*(-1);
	int max=size/2;
	
	for( int r=0;r<in.rows;r++){
		ptr2=out.ptr<uchar>(r);
		for( int c=0;c<in.cols;c++){
			
			for(int i=min; i<=max; i++){
				ptr=in.ptr<uchar>(r+i);
				for(int j=min; j<=max; j++){
						
					if(r+i>=0 && r+i<=in.rows && c+j>=0 && c+j<=in.cols ){
						
						if(ptr[c+j]==255){
							
							ptr2[c]=255;
						}
						
					}
				}
			}
			
				
		}
	}

}

void dilate(cv::Mat &in,cv::Mat &out, int size){
	
	uchar *ptr;
	uchar *ptr2;
	int min=size/2*(-1);
	int max=size/2;
	
	for( int r=0;r<in.rows;r++){
		ptr2=out.ptr<uchar>(r);
		for( int c=0;c<in.cols;c++){
			
			for(int i=min; i<=max && ptr2[c]!=0; i++){
				ptr=in.ptr<uchar>(r+i);
				for(int j=min; j<=max && ptr2[c]!=0 ; j++){
						
					if(r+i>=0 && r+i<=in.rows && c+j>=0 && c+j<=in.cols ){
						
						if(ptr[c+j]==0){
							//cout << "Entro if";
							ptr2[c]=0;
						}
						
					}
				}
			}
			
				
		}
	}

}


int main(int argc,char **argv){
	try{
	
		if(argc!=5) {cerr<<"Usage:in_image out_image thresh_val (erode|dilate|open|close) "<<endl;return 0;} 
		
		cv::Mat image, outImage, testImage;

		string operation;
		operation = argv[4]; 
		
		//cv::Mat kernel(3, 3, CV_32FC1,1.0f);
		int size=3;
	
		//Load the image 
		image=cv::imread(argv[1]);
		//Change image to grayscale 
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
		threshold(image,atoi(argv[3]));

		image.copyTo(outImage);
		image.copyTo(testImage);
		cout << endl<<operation<<endl;

		if(operation.compare("erode")==0){
			erode(image,outImage,size);	
		}

		else if(operation.compare("dilate")==0){
			dilate(image,outImage,size);
		}

		else if(operation.compare("open")==0){
			erode(image,testImage,size);
			dilate(testImage,outImage,size);

			//Test to see if it is working
			/*uchar *ptr;
			uchar *ptr2;
			int min=size/2*(-1);
			int max=size/2;
			int diff=0;
			int cont=0;

			for( int r=0;r<image.rows;r++){
				ptr=image.ptr<uchar>(r);
				ptr2=outImage.ptr<uchar>(r);
				for( int c=0;c<image.cols;c++){
					diff += abs(ptr2[c]-ptr[c]);
					if(ptr2[c]-ptr[c]!=0){
						cont++;
					}

				}
			}

			cout << "Diferencia Pixeles: " << diff;
			cout << endl <<"Pixeles Cambiados: " << cont;*/

		}

		else if(operation.compare("close")==0){
			cout << "close image";
			dilate(image,testImage,size);
			erode(testImage,outImage,size);
		}


		else{
			cout << "No valid operation";
		}
    	
		cv::imshow("Input Image",image);
		cv::imshow("Output Image",outImage);
		cv::imwrite(argv[2],outImage);
		char c=0;
		while(c!=27)  //waits until ESC pressed
			c=cv::waitKey(0);

	}catch(std::exception &ex){
	  cout<<ex.what()<<endl;
	}
}