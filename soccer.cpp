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
bool cameraCut(Mat i1, Mat i2);
bool t(int, int, int);

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
  bool frameOK = false;
  int combo = 0;
  int frameNumber = 0;
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
       
      imwrite(ss.str(), original);
      imshow("original", original);
      drawImage = false;

      bool enoughGreen = detectGreen(original) > 120000;
      if(enoughGreen){
        drawImage = true;
      }

      if(cameraCut(original, oldImage)){
        frameOK = false;
      }

      if(!frameOK){
        combo++;
      }

      if(combo == 100){
        frameOK = true;
        combo = 0;
      }

      cvtColor(frame, edges, CV_BGR2GRAY);
      GaussianBlur(edges, edges, Size(7,7), 2, 2);
      Canny(edges, edges, 0, 30, 3);
      if(drawImage)
      {
        imshow("edges", edges);
      }
      oldImage = frame;
    }
        
    int key = waitKey(1);
    if(key == 1048625) break;
      if(key == 1048608) pause = !pause;
  }

  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
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
  cv::erode(img, img, cv::Mat(), cv::Point(-1,-1));

  for(int i=0; i<img.cols; i++){
    for(int j=0; j<img.rows; j++){
      if(img.at<Vec3b>(Point(i,j))[GREEN]){
        greenTot++;
      }
    }
  }

  imshow("binary", img);
  printf("green: %d\n", greenTot);

  return greenTot;
}

bool t(int n, int m, int allowance) {
  return n < m + allowance && n > m - allowance; 
}


bool cameraCut(Mat i1, Mat i2)
{
  if(!i2.size().width || !i1.size().width)
  {
    return false;
  }
    
  //imshow("old", i2);
  //imshow("main", i1);
    
  Mat hsv_i1;
  Mat hsv_i2;

  cvtColor(i1, hsv_i1, CV_BGR2HSV);
  cvtColor(i2, hsv_i2, CV_BGR2HSV);
    
  // Using 50 bins for hue and 60 for saturation
  int h_bins = 50; int s_bins = 60;
  int histSize[] = { h_bins, s_bins };

  // hue varies from 0 to 179, saturation from 0 to 255
  float h_ranges[] = { 0, 180 };
  float s_ranges[] = { 0, 256 };

  const float* ranges[] = { h_ranges, s_ranges };

  // Use the o-th and 1-st channels
  int channels[] = { 0, 1 };

  /// Histograms
  MatND hist_i1;
  MatND hist_i2;
    
  /// Calculate the histograms for the HSV images
  calcHist( &hsv_i1, 1, channels, Mat(), hist_i1, 2, histSize, ranges, true, false );
  normalize( hist_i1, hist_i1, 0, 1, NORM_MINMAX, -1, Mat() );

  calcHist( &hsv_i2, 1, channels, Mat(), hist_i2, 2, histSize, ranges, true, false );
  normalize( hist_i2, hist_i2, 0, 1, NORM_MINMAX, -1, Mat() );

  /// Apply the histogram comparison methods
  int compare_method = 0;
  double i1_i2 = compareHist(hist_i1, hist_i2, compare_method);

  return i1_i2 < 0.94;
}
