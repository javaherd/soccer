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
int cameraCut(Mat i1, Mat i2);
int compareGreen(Mat i1, Mat i2);
bool t(int, int, int);
bool isSteady(int*);
const int W = 15;
int window[15] = {0};
int main(int, char** argv)
{
  VideoCapture cap("match1.mp4"); // open the default camera
  if(!cap.isOpened())  // check if we succeeded
    return -1;

  Mat edges;
  namedWindow("edges",1);
  bool pause = false;
  bool drawImage = true;
  Mat oldImage;
  Mat oldEdgeImage;
  bool frameOK = false;
  int combo = 0;
  int frameNumber = 0;
  int oldOverlapping;

  int framesThrough = 0;
  int scene = 0;
  int compare;

  int minLength = 10;
  Mat oldBinaryRGImg;
  int framesSinceLastChange = 0;

  for(;;)
  {
    if(!pause)
    {
      Mat frame;
      cap >> frame; // get a new frame from camera
      frameNumber++;
      Mat original = frame.clone();

      stringstream ss;
      ss << "images/";
      if (scene < 10)
        ss << "0";
      if (scene < 100)
        ss << "0";
      ss << scene;
      ss << "_";
      if (framesSinceLastChange < 10)
        ss << "0";
      if (framesSinceLastChange < 100)
        ss << "0";
      ss << framesSinceLastChange;
      ss << ".png";

      imwrite(ss.str(), original);
      cout << ss.str() << endl;
      imshow("original", original);

      Mat binaryRGImg = binaryRG(original);
      imshow("green", binaryRGImg);

      cvtColor(frame, edges, CV_BGR2GRAY);
      GaussianBlur(edges, edges, Size(7,7), 2, 2);
      Canny(edges, edges, 0, 60, 3);
      framesSinceLastChange++;
      greenFramesSinceLastChange += isGreen(binaryRGImg);

      if(frameNumber % 4 == 0){
        compare = compareGreen(binaryRGImg, oldBinaryRGImg);

        if(compare < 200000){
          float percentGreen = (float)greenFramesSinceLastChange / (float)framesSinceLastChange;
          percentGreen *= 100;
          //printf("framesSinceLastChange: %f\n", (float)framesSinceLastChange);
          //printf("greenSinceLastChange: %f\n", (float)greenFramesSinceLastChange);
          //printf("percent: %f\n", percentGreen);

          if(framesSinceLastChange >= 80 && percentGreen > 70){
            scene++;
          }
          framesSinceLastChange = 0;
          greenFramesSinceLastChange = 0;
        }

        oldBinaryRGImg = binaryRGImg;
        oldImage = frame;
        oldEdgeImage = edges;
      }
    }
        
    int key = waitKey(1);
    if(key == 27) break;
    if(key == 32) pause = !pause;
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

bool isSteady(int* window){
  int total = 0;
  for(int i = 1; i < W; i++){
    total += (window[i] - window[i-1]);
    //printf(" %d\n", window[i]);
  }
  //printf("%d\n", total);
  return total;
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

bool t(int n, int m, int allowance) {
  return n < m * 1.2 && n > m * 0.8; 
}


int cameraCut(Mat i1, Mat i2)
{
  if(!i2.size().width || !i1.size().width)
  {
    return false;
  }
    
  const int R = 5;

  int confidence = 0;
  
  imshow("edges", i1);

  for(int i=R; i<i1.cols-R; i++){
    for(int j=R; j<i1.rows-R; j++){
      Vec3b pixel1 = i1.at<Vec3b>(Point(i,j));
      Vec3b pixel2 = i2.at<Vec3b>(Point(i,j)); 
      
      if(pixel1[0] > 0 && pixel2[0] > 0)
      {
          confidence++;
      }
    }
  }

  //printf("%d\n", confidence);

  return confidence;
}
