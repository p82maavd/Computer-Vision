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
    "{t threshold    |13     | Segmentation threshold.}"
    "{s              |0   | Radius of structural element.}"    
    "{c              |  | Use the camera?}"
    "{g              |0 | Size for the gaussian Filter}"
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
  Mat theMask = Mat::zeros(inFrame.size(), CV_8UC1);
  
  std::cout << inFrame.size() << std::endl;
  VideoWriter output;
  
  output.open(fileout, VideoWriter::fourcc('M','J','P','G'), fps, inFrame.size());
  if (!output.isOpened())
  {
    cerr << "Error: the ouput stream is not opened.\n";
    exit(-1);
  }  

  int frameNumber=0;
  int key=0;

// WRITE ME 

  cv::namedWindow("Output");
  
  while(input.grab() && key!=27)
  {

    frameNumber++;
        
    cv::imshow ("Input", inFrame);

     
	 // Do your processing
	 // TODO
    cv::Mat prev, diff;
    inFrame.copyTo(prev);
    inFrame.copyTo(diff);
    cv::cvtColor(diff,diff,cv::COLOR_BGR2GRAY);
    input.retrieve(inFrame);
    fsiv_segm_by_dif(prev,inFrame,diff,threshold,radius);

    if (parser.has("g"))
    {
      
      cv::GaussianBlur(diff, diff, Size(parser.get<int>("g"),parser.get<int>("g")), 1);
    }

    cv::imshow ("Diff", diff);
    cv::Mat outFrame;
    


    // TODO Apply the mask to the original frame and show

    fsiv_apply_mask(inFrame,diff,outFrame);
    cv::imshow ("Output", outFrame);

    // Preparing the next iteration

    // TODO Add frame to output video

    output << outFrame;

    
    key=cv::waitKey(10);

    if(key==32){
      string name="out_"+to_string(frameNumber)+".png";
      cv::imwrite(name,outFrame);
    }

  }        
  return 0;
}
