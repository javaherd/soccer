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


int main(int, char** argv)
{
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
    Mat original = frame.clone();
  
    imshow("original", original);

    Mat binaryRGImg = binaryRG(original);
    imshow("green", binaryRGImg);

    // increment scene length counter
    framesSinceLastChange++;
    // increment scene green
    greenFramesSinceLastChange += isGreen(binaryRGImg);

    if(frameNumber % 4 == 0){
      greenDifference = compareGreen(binaryRGImg, oldBinaryRGImg);

      if(greenDifference < 200000){
        float percentGreen = (float)greenFramesSinceLastChange / (float)framesSinceLastChange;
        percentGreen *= 100;
        //printf("framesSinceLastChange: %f\n", (float)framesSinceLastChange);
        //printf("greenSinceLastChange: %f\n", (float)greenFramesSinceLastChange);
        //printf("percent: %f\n", percentGreen);

        if(framesSinceLastChange >= 80 && percentGreen > 70){
          printf("%d\n%d\n", frameNumber - framesSinceLastChange, frameNumber);
        }
        framesSinceLastChange = 0;
        greenFramesSinceLastChange = 0;
      }

      oldBinaryRGImg = binaryRGImg;
      oldImage = frame;
    }
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
