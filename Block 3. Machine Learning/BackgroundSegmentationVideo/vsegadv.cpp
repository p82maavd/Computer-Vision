/* 
   (c) Fundamentos de Sistemas Inteligenties en Vision
   University of Cordoba, Spain  
*/

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctype.h>
#include <cmath>


#include <opencv2/core/utility.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#include "seglib.hpp"

using namespace std;
using namespace cv;

#ifndef NDEBUG
int __Debug_Level = 0;
#endif

/*
  Use CMake to compile
*/

const cv::String keys =
    "{help h usage ? |      | print this message   }"        
#ifndef NDEBUG
    "{verbose        |0     | Set the verbose level.}"
#endif    
    "{t threshold    |0.2    | Segmentation threshold.}"
    "{s              |0   | Radius of structural element.}"    
    "{c              |  | Use the camera?}"
    "{g              |0 | Size for the gaussian Filter}"
    "{u              |10 | Frames for an update}"
    "{b              |100 | Number of frames for training}"
    "{@input         |<none>| Path to video/Camera index.}"
    "{@output        |<none>| Path to output video.}"
    ;

int main (int argc, char * const argv[])
{
  /* Default values */
  bool cameraInput=false;
  int threshold;
  const char * filein = 0;
  const char * fileout = 0;
  char opt;
  int radius = 0;
  
  cv::CommandLineParser parser(argc, argv, keys);
  parser.about("Background segmentation in video.");
  if (parser.has("help"))
  {
      parser.printMessage();
      return 0;
  }

#ifndef NDEBUG
  __Debug_Level = parser.get<int>("verbose");
#endif
    
  std::string input_path = parser.get<std::string>("@input");
  std::string output_path = parser.get<std::string>("@output");
  threshold = parser.get<int>("threshold"); 
  //cout << "threshold: "<<threshold<<endl; 
  radius = parser.get<int>("s");
  //cout << "radius: "<<radius<<endl;

  filein = input_path.c_str();
  fileout = output_path.c_str();

  std::cout << "Input stream: " << filein << endl;
  std::cout << "Output: " << fileout << endl;

  VideoCapture input;
    
  if (parser.has("c"))
  {
    std::cout << "Using camera index" << std::endl;
    input.open(atoi(filein));
  }
  else  
    input.open(filein);
    
  if (!input.isOpened())
  {
    cerr << "Error: the input stream is not opened.\n";
    abort();
  }

  Mat inFrame;
  bool wasOk = input.read(inFrame);
  
  if (!wasOk)
  {
    cerr << "Error: could not read any image from stream.\n";
    abort();
  }
  
  double fps=25.0;
  if (!cameraInput)
    //fps=input.get(CV_CAP_PROP_FPS);
  std::cout << fps << std::endl;
  
  Mat outFrame = Mat::zeros(inFrame.size(), CV_8UC3);
  //Mat theMask = Mat::zeros(inFrame.size(), CV_8UC1);
  
  std::cout << inFrame.size() << std::endl;
  VideoWriter output;
  
  output.open(fileout, VideoWriter::fourcc('M','J','P','G'), fps, inFrame.size());
  if (!output.isOpened())
  {
    cerr << "Error: the ouput stream is not opened.\n";
    exit(-1);
  }  

  int frameNumber=parser.get<int>("b");
  int key=0;

// WRITE ME 

  cv::namedWindow("Output");

  cv::Mat media(inFrame.rows, inFrame.cols, CV_32FC3, cv::Scalar(0.0, 0.0, 0.0)); 
  cv::Mat i2(inFrame.rows, inFrame.cols, CV_32FC3, cv::Scalar(0.0, 0.0, 0.0));
  cv::Mat vari(inFrame.rows, inFrame.cols, CV_32FC3, cv::Scalar(0.0, 0.0, 0.0));
  cv::Mat stdi(inFrame.rows, inFrame.cols, CV_32FC3, cv::Scalar(0.0, 0.0, 0.0));

  fsiv_learn_model(input,parser.get<int>("b"),media,i2,vari,stdi);
  
  while(input.grab() && key!=27)
  {

    frameNumber++;

    cv::Mat mask(inFrame.rows, inFrame.cols, CV_8UC1, cv::Scalar(0, 0, 0));
    cv::Mat outFrame(inFrame.rows, inFrame.cols, CV_8UC3, cv::Scalar(0, 0, 0));

	 // Do your processing
	 // TODO
    
    input.retrieve(inFrame);

    cv::imshow ("Input", inFrame);

    if(frameNumber%parser.get<int>("u")==0){
      
      fsiv_update_model(inFrame, media, i2, stdi, 0.8, mask);
      
    }
    
    fsiv_segm_by_model(inFrame, mask, media, stdi, parser.get<float>("t"), parser.get<int>("s"));
    if (parser.has("g"))
    {
      cv::GaussianBlur(mask, mask, Size(parser.get<int>("g"),parser.get<int>("g")), 1);
    }
    fsiv_apply_mask(inFrame,mask,outFrame);

    // Preparing the next iteration
    // TODO Add frame to output video
    
    cv::imshow ("Output", outFrame);
    
    output << outFrame;
    
    key=cv::waitKey(10);

    if(key==32){
      string name="out_"+to_string(frameNumber)+".png";
      cv::imwrite(name,outFrame);
    }

  } 
  return 0;
}
