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

int detectGreen(Mat x);
int cameraCut(Mat i1, Mat i2);
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
  for(;;)
  {
    if(!pause)
    {
      Mat frame;
      cap >> frame; // get a new frame from camera
      Mat original = frame.clone();

      stringstream ss;
      ss << "images/i";
      ss << frameNumber++;
      ss << ".png";
       
      //imwrite(ss.str(), original);
      imshow("original", original);

      bool enoughGreen = detectGreen(original) > 120000;

      cvtColor(frame, edges, CV_BGR2GRAY);
      GaussianBlur(edges, edges, Size(7,7), 2, 2);
      Canny(edges, edges, 0, 30, 3);

      int overlapping = cameraCut(edges, oldEdgeImage);
      int average = 0;
      
      int drawImage = false;
      if(!t(overlapping, oldOverlapping, 700)){
        printf("cut %d\n", frameNumber);
      }

      oldOverlapping = overlapping;

      isSteady(window);
 
      if(drawImage){
      }

      oldImage = frame;
      oldEdgeImage = edges;
    }
        
    int key = waitKey(1);
    if(key == 1048625) break;
      if(key == 1048608) pause = !pause;
  }

  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
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

int detectGreen(Mat img)
{
  if(img.size().width == 0)
  {
    return false;
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
  cv::erode(img, img, cv::Mat(), cv::Point(-1,-1));
  cv::erode(img, img, cv::Mat(), cv::Point(-1,-1));

  for(int i=0; i<img.cols; i++){
    for(int j=0; j<img.rows; j++){
      if(img.at<Vec3b>(Point(i,j))[GREEN]){
        greenTot++;
      }
    }
  }

  return greenTot;
}

bool t(int n, int m, int allowance) {
  return n < m * 1.1 && n > m * 0.9; 
}


int cameraCut(Mat i1, Mat i2)
{
  if(!i2.size().width || !i1.size().width)
  {
    return false;
  }
    
  const int R = 5;

  int confidence = 0;
  
  cv::dilate(i1, i1, cv::Mat(), cv::Point(-1,-1));
  cv::erode(i1, i1, cv::Mat(), cv::Point(-1,-1));
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
