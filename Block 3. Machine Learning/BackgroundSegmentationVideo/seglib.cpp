// ----------------------------------------
// seglib.cpp
// (c) FSIV, University of Cordoba
// ----------------------------------------

#include "seglib.hpp"

void erode(cv::Mat &in,cv::Mat &out, int size){
	
	uchar *ptr;
	uchar *ptr2;
	int min=size/2*(-1);
	int max=size/2;
	
	for( int r=0;r<in.rows;r++){
		ptr2=out.ptr<uchar>(r);
		for( int c=0;c<in.cols;c++){
			
			for(int i=min; i<=max && ptr2[c]!=255; i++){
				ptr=in.ptr<uchar>(r+i);
				for(int j=min; j<=max && ptr2[c]!=255; j++){
						
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

void fsiv_segm_by_dif(const cv::Mat & prevFrame, const cv::Mat & curFrame, cv::Mat & difimg, int thr, int r)
{
   // WRITE ME

	cv::Mat prevFrameGray, curFrameGray;
	cv::cvtColor(prevFrame, prevFrameGray, cv::COLOR_BGR2GRAY);
	cv::cvtColor(curFrame, curFrameGray, cv::COLOR_BGR2GRAY);

	//cv::absdiff(prevFrameGray,curFrameGray,difimg);

    for( int r=0;r<prevFrame.rows;r++){
		uchar *ptr=prevFrameGray.ptr<uchar>(r);
		uchar *ptr2=curFrameGray.ptr<uchar>(r);
		uchar *ptr3=difimg.ptr<uchar>(r);
		for( int c=0;c<prevFrame.cols;c++){
			//cout << "ABS: "<< abs(ptr2[c]-ptr[c]) << " thr: "<< thr<<endl;
			if(abs(ptr2[c]-ptr[c])>thr){
				ptr3[c]=255;
			}
			else{
				ptr3[c] =0;
			}
				
		}
			
			
	}


	cv::Mat copy, output;
	difimg.copyTo(copy);
	difimg.copyTo(output);
	

	erode(copy,difimg,r);
	dilate(difimg,output,r);

	difimg.copyTo(copy);
	difimg.copyTo(output);

	dilate(copy,difimg,r);
	erode(difimg,output,r);

	

	
}

void fsiv_apply_mask(const cv::Mat & frame, const cv::Mat & mask, cv::Mat & outframe)
{
   // WRITE ME
	//frame.copyTo(outframe,mask);
	cv::bitwise_and(frame,frame,outframe,mask);
}

// ================= OPTIONAL PART STARTS HERE =======================

void fsiv_learn_model(cv::VideoCapture & input, int maxframes, cv::Mat & MeanI, cv::Mat & I2, cv::Mat & VarI, cv::Mat & StdI)
{
   // WRITE ME

	int frameNumber=0;
	int key=0; 

	//Start video
	cv::Mat inFrame;

	
	cv::Mat image_f;
	while(input.grab() && frameNumber<maxframes)
	{

		frameNumber++;
		
		input.retrieve(inFrame);
		
		inFrame.convertTo(image_f, CV_32FC3);
		fsiv_acc_model(image_f, MeanI,I2);

	}
  
	MeanI/=maxframes;
	I2/=maxframes;
  	
	cv::Mat media_uchar;
	//MeanI.convertTo(media_uchar, CV_8UC3);
    
    //cv::imshow ("media", media_uchar);

  
  
  	// Restart Video 

	input.set(cv::CAP_PROP_POS_FRAMES, 0);
	
	frameNumber=0;

	while(input.grab() && frameNumber<maxframes)
	{

		frameNumber++;

		for( int r=0;r<I2.rows;r++){
			const float *ptr=I2.ptr<float>(r);
			const float *ptr1=MeanI.ptr<float>(r);
			float *ptr2=VarI.ptr<float>(r);
			float *ptr3=StdI.ptr<float>(r);
			for( int c=0;c<I2.cols*3;c=c+3){
				
				ptr2[c]+=abs(ptr[c]-ptr1[c]);
				ptr2[c+1]+=abs(ptr[c+1]-ptr1[c+1]);
				ptr2[c+2]+=abs(ptr[c+2]-ptr1[c+2]);
				ptr3[c]+=sqrt(ptr2[c]);
				ptr3[c+1]+=sqrt(ptr2[c+1]);
				ptr3[c+2]+=sqrt(ptr2[c+2]);
			}
		}

	} 

	StdI/=maxframes;

	cv::Mat std_uchar;
	MeanI.convertTo(MeanI, CV_8UC3);
    
	StdI.convertTo(StdI, CV_8UC3);
    
    //cv::imshow ("Std", std_uchar);

  

}
  

void fsiv_acc_model(const cv::Mat & frame, cv::Mat & MeanI, cv::Mat & I2)
{
   // WRITE ME

	cv::accumulate(frame,MeanI);
	cv::accumulateSquare(frame,I2);
	
}

void fsiv_segm_by_model(const cv::Mat & frame, cv::Mat & theMask, const cv::Mat & mean, const cv::Mat & std, float t, int r)
{
   // WRITE ME
	//cout << "frame size: "<< frame.rows << " " << frame.cols << "themask: "<<theMask.rows << " " << theMask.cols << endl;
	//cv::Mat frame2;
	//frame.convertTo(frame2, CV_32FC3);
	
	for( int r=0;r<frame.rows;r++){
		const uchar *ptr=frame.ptr<uchar>(r);
		const uchar *ptr2=mean.ptr<uchar>(r);
		const uchar *ptr3=std.ptr<uchar>(r);
		uchar *ptr4=theMask.ptr<uchar>(r);
		for( int c=0;c<frame.cols*3;c=c+3){
				
			if(abs(ptr2[c]-ptr[c])>=t*ptr3[c] || abs(ptr[c+1]-ptr2[c+1])>=t*ptr3[c+1] || abs(ptr[c+2]-ptr2[c+2])>=t*ptr3[c+2]){
				
				ptr4[c/3]=255;
						
			}
			
		}
	} 

	cv::Mat copy, output;
	theMask.copyTo(copy);
	theMask.copyTo(output);
	

	erode(copy,theMask,r);
	dilate(theMask,output,r);

	theMask.copyTo(copy);
	theMask.copyTo(output);

	dilate(copy,theMask,r);
	erode(theMask,output,r);

	output.copyTo(theMask);


}

void fsiv_update_model(const cv::Mat & frame, cv::Mat & mean, cv::Mat & I2, cv::Mat &std,  float alpha, const cv::Mat & theMask)
{
   // WRITE ME

}
