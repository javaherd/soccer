#include <iostream>
#include <stdio.h>

#include "opencv2/opencv.hpp"

#include <cstdlib>
#include <fstream>

using namespace cv;
using namespace std;

int main(int, char** argv)
{
  VideoCapture cap(argv[1]);

  if(!cap.isOpened())
    return -1;

  int frameW = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  int frameH = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
  VideoWriter video(argv[2], CV_FOURCC('M','J','P','G'), 24, Size(frameW,frameH), true);
  
  int frameNumber = 0;

  int frameLow = 0;
  int frameHigh = 0;

  int frames[100] = {0};
  
  int i = 0;

  string line;
  ifstream ifs("frames");

  while(std::getline(ifs, line)){
    frames[i] = std::stoi(line);
    getline(ifs, line);
    frames[i+1] = std::stoi(line);
    i += 2;
  }

  i = 0;
  while(true){
    if(frames[i] == 0){
      printf("quitting %d\n", i);
      return 0;
    }

    Mat frame;
    Mat edges;
    cap >> frame; // get a new frame from camera

    frameNumber++;

    if(frameNumber >= frames[i] && frameNumber < frames[i+1]){
      printf("l%d\n", frames[i]);
      printf("h%d\n", frames[i+1]);
      // Make edge image
      cvtColor(frame, edges, CV_BGR2GRAY);
      GaussianBlur(edges, edges, Size(7,7), 2, 2);
      Canny(edges, edges, 0, 60, 3);
    
      cvtColor(edges, edges, CV_GRAY2BGR);
      video.write(edges); 
    }else if(frames[i+1] == frameNumber){
      i += 2;
    }
  }

  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}
