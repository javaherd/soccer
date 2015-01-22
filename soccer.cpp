#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

#include "opencv2/opencv.hpp"

#define BLUE 0
#define GREEN 1
#define RED 2

using namespace cv;
using namespace std;

Mat binaryRG(Mat x);
int isGreen(Mat i1);
int greenFramesSinceLastChange = 0;
int compareGreen(Mat i1, Mat i2);

bool debug = false;

int main(int, char** argv)
{
  if(debug)
    namedWindow("original");
  
  if(strcmp(argv[1], "debug") == 0)
    debug = true;

  VideoCapture cap("match1.mp4"); // change to argv
  if(!cap.isOpened())
    return -1;

  Mat oldImage;
  Mat oldBinaryRGImg;
  
  int frameNumber = 0;
  int greenDifference;
  int framesSinceLastChange = 0;

  // 101 will remain constant until the next epoch, which will be in 2038
  while(101 == 101)
  {
    Mat frame;
    cap >> frame; // get a new frame from camera
    frameNumber++;
 
      if(debug)
        imshow("original", frame);

      Mat binaryRGImg = binaryRG(frame);
      //imshow("green", binaryRGImg);

      // Always presume that the scene will be good, until the test later
      // increment scene length counter
      framesSinceLastChange++;
      // increment scene green
      greenFramesSinceLastChange += isGreen(binaryRGImg);

    if(frameNumber > 4000)
      return 0;

    if(frameNumber % 10 == 0){
      // if the amount of shared green between two consecutive frames dips,
      // then we assume that the camera is now looking at a different scene
      bool cameraCut = compareGreen(binaryRGImg, oldBinaryRGImg) < 200000;

      if(cameraCut){
        float percentGreen = 
              ((float)greenFramesSinceLastChange / (float)framesSinceLastChange) * 100;
       
        if(debug){ 
          fprintf(stderr,"frames since last change: %f\n", (float)framesSinceLastChange);
          fprintf(stderr,"green since last change: %f\n", (float)greenFramesSinceLastChange);
          fprintf(stderr,"percent green: %f\n", percentGreen);
        }

        if(framesSinceLastChange >= 100 && percentGreen > 70){
          printf("%d\n%d\n", frameNumber - framesSinceLastChange + 5, frameNumber - 5);
        }
        framesSinceLastChange = 0;
        greenFramesSinceLastChange = 0;
      }

      oldBinaryRGImg = binaryRGImg;
      oldImage = frame;
    }
    waitKey(1);
  }

  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}

int isGreen(Mat i1){
  float points = 0;
  float total = 0;


  for(int i=0; i<i1.cols; i++){
    for(int j=0; j<i1.rows; j++){
      if(i1.at<Vec3b>(Point(i,j))[GREEN] > 0){
        points++;
      }
      total++;
    }
  }

  return points > (total / 2);
}

int compareGreen(Mat i1, Mat i2){
  int points = 0;

  for(int i=0; i<i1.cols; i++){
    for(int j=0; j<i2.rows; j++){
      if(i1.at<Vec3b>(Point(i,j))[GREEN] == i2.at<Vec3b>(Point(i,j))[GREEN]){
        points++;
      }
    }
  }

  return points;
}

Mat binaryRG(Mat img)
{
  if(img.size().width == 0)
  {
    return img.clone();
  }
  
  int greenTot = 0;

  for(int i=0; i<img.cols; i++){
    for(int j=0; j<img.rows; j++){
      Vec3b color = img.at<Vec3b>(Point(i,j));
      int red   = color[RED];
      int blue = color[BLUE];
      int green = color[GREEN];
      int r =  (int)(255*((float)red/(float)(1 + red + green + blue)));
      int g = (int)(255*((float)green/(float)(1 + red + green + blue)));
      color[BLUE] = 0;
      color[RED] = 0;
      color[GREEN] = 0;

      if(g > 100)
        color[GREEN] = 255;
      img.at<Vec3b>(Point(i,j)) = color;
    }
  }
  // cv::erode(img, img, cv::Mat(), cv::Point(-1,-1));
  cv::dilate(img, img, cv::Mat(), cv::Point(-1,-1));


  return img;
}
