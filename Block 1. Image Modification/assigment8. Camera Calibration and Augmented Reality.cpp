#include <opencv2/core/core.hpp> //core routines
#include <opencv2/highgui/highgui.hpp>//imread,imshow,namedWindow,waitKey
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>

#include "opencv2/core.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/videoio.hpp"

#include <cctype>
#include <stdio.h>
#include <string.h>
#include <time.h>

using namespace std;
int main(int argc,char **argv){

   if(argc!=7) {cerr<<"Usage:rows cols size intrinsics.yml input_video image|video"<<endl;return 0;} 
   int rows=atoi(argv[1]);
   int cols=atoi(argv[2]);
   int size=atoi(argv[3]);

   cv::Mat camera_matrix, distorsion_coefficient, projectedImage;
   cv::VideoCapture projectedVideo;

   cv::FileStorage file(argv[4], cv::FileStorage::READ );
   file["camera_matrix"] >> camera_matrix;
   file["distortion_coefficients"] >> distorsion_coefficient;
   file.release();

   string pathToVideo=argv[5];
   cv::VideoCapture video(pathToVideo);

   string archivo= argv[6];
   string opcion= archivo.substr(archivo.find_last_of(".")); 
   if(opcion==".jpg" || opcion==".png"){
      projectedImage=cv::imread(archivo);
   }
   else if(opcion==".mp4" || opcion==".avi"){
      projectedVideo = cv::VideoCapture(archivo);
      //This is necessary for the aux corners (line 75)
      projectedVideo.read(projectedImage);
      projectedVideo.set(cv::CAP_PROP_POS_FRAMES, 0);
   }
   else{
      cout << "Formato del archivo a mostrar no valido";
      return 0;
   }
      
   cv::Mat image;
   cv::Size patternsize(rows,cols);
   cv::Mat rvec = cv::Mat::zeros(3, 1, CV_32FC1); 
   cv::Mat tvec = cv::Mat::zeros(3, 1, CV_32FC1); 
   vector<cv::Point2f> corners; 
   cv::Mat viewGray;

   vector<cv::Point3f> obj_points;

   for (int i = 0; i < rows; ++i){
      for (int j = 0; j < cols; ++j){
         obj_points.push_back(cv::Point3f(i*size, j*size, 0));
      }
   }
   
   vector<cv::Point2f> imagecorners = { cv::Point2f(0,0), cv::Point2f(projectedImage.cols,0), cv::Point2f(0,projectedImage.rows) , cv::Point2f(projectedImage.cols,projectedImage.rows-1)};

   while(video.grab()){
      
      video.read(image);
   
      cv::cvtColor(image, viewGray, cv::COLOR_BGR2GRAY);
      //If video is not correctly recorded and can't detect corners in some frames
      auto aux=corners;
      cv::findChessboardCorners(viewGray, patternsize, corners,cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);
      if(corners.size()==0){
         corners=aux;
      }
      cv::cornerSubPix(viewGray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
         
      cv::solvePnP(obj_points,corners,camera_matrix,distorsion_coefficient,rvec,tvec);

      if(opcion==".mp4" || opcion==".avi"){
         
         auto var=projectedVideo.read(projectedImage);
         //Video loop
         if(!var){
            projectedVideo.set(cv::CAP_PROP_POS_FRAMES, 0);
            continue;
         }
      
      }
      
      vector<cv::Point2f> boardcorners = { corners[0], corners[rows-1], corners[corners.size()-rows] , corners[corners.size()-1]};

      //With this matrix we know where is located the area inside of our corners in each iamge
      cv::Mat perspectiveTransform = cv::getPerspectiveTransform(imagecorners, boardcorners);

      cv::Mat mask(projectedImage.size(), projectedImage.type(),cv::Scalar(255,255,255));
      //A mask with the area where the image will be projected
      cv::warpPerspective(mask, mask, perspectiveTransform, image.size());

      cv::Mat warpImage(projectedImage.size(), projectedImage.type(),cv::Scalar(0,0,0));
      //Transform the projected image to fit inside the corners
      cv::warpPerspective(projectedImage, warpImage, perspectiveTransform, image.size());
      
      //Mask indicates wich pixels copy to image because we dont want to overwrite the rest of the image
      warpImage.copyTo(image,mask);

      cv::imshow("image",image);
            
      cv::waitKey(10);
   }
}

