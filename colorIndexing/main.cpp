#include <iostream>
#include <cv.h>
#include "highgui.h"

using namespace std;

int main(int argc, char *argv[])
{
  
  char *image1, *image2;
  
  if (argc < 3 || argc > 3) {
  	cerr << "Usage: " << argv[0] << " template target" << endl;
  	return 0;
  }

  IplImage* refImg = cvLoadImage(argv[1]);
  int arr[2] = {16, 16};
  CvHistogram* refHist = cvCreateHist(2, arr, CV_HIST_ARRAY);
  cout << refImg->colorModel << " "  << endl;
  
  IplImage *refArr = cvCreateImage(cvGetSize(refImg), IPL_DEPTH_8U, 3);
  IplImage *refImgH = cvCreateImage(cvGetSize(refImg), IPL_DEPTH_8U, 1);
  IplImage *refImgS = cvCreateImage(cvGetSize(refImg), IPL_DEPTH_8U, 1);
  IplImage *refImgV = cvCreateImage(cvGetSize(refImg), IPL_DEPTH_8U, 1);
  //cvCvtColor(refImg, refArr, CV_RGBA2RGB);
  cvCvtColor(refImg, refArr, CV_RGB2HSV);
  cvSplit(refArr, refImgH, refImgS, refImgV, NULL);

  cvNamedWindow("ImgH", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("ImgS", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("ImgV", CV_WINDOW_AUTOSIZE);
  cvShowImage("ImgH", refImgH);
  cvShowImage("ImgS", refImgS);
  cvShowImage("ImgV", refImgV);
 

  
  IplImage* imgArr[2] = { refImgH, refImgS };
  cvCalcHist( imgArr, refHist);

  cvNamedWindow("hist", CV_WINDOW_AUTOSIZE);
  //cvShowImage("hist", refHist->bins);

  IplImage* testImg = cvLoadImage(argv[2]);
  CvHistogram* testHist = cvCreateHist(2, arr, CV_HIST_ARRAY);

  IplImage *testArr = cvCreateImage(cvGetSize(testImg), IPL_DEPTH_8U, 3);
  IplImage *testImgH = cvCreateImage(cvGetSize(testImg), IPL_DEPTH_8U, 1);
  IplImage *testImgS = cvCreateImage(cvGetSize(testImg), IPL_DEPTH_8U, 1);
  IplImage *testImgV = cvCreateImage(cvGetSize(testImg), IPL_DEPTH_8U, 1);

  //cvCvtColor(testImg, refArr, CV_RGBA2RGB);
  cvCvtColor(testImg, testArr, CV_RGB2HSV);
  cvSplit(testArr, testImgH, testImgS, testImgV, NULL);

  imgArr[0] = testImgH;
  imgArr[1] =  testImgS;
  cvCalcHist( imgArr, testHist);

  double comp = cvCompareHist(testHist, refHist, CV_COMP_INTERSECT);

  cout << comp << endl;
  
  IplImage* patchImg = cvCreateImage(cvSize(testImg->width - 19, testImg->height - 19), IPL_DEPTH_32F, 1);
  cvCalcBackProjectPatch(imgArr, patchImg, cvSize(20, 20), refHist, CV_COMP_INTERSECT, 5);

  cvShowImage("hist", patchImg);

  CvPoint max, min;
  cvMinMaxLoc(patchImg, NULL, NULL, &min, &max);

  cout << min.x << " " << min.y << endl;
  cout << max.x << " " << max.y << endl;

  cvWaitKey(0);
  cvDestroyWindow("ImgH");
  cvDestroyWindow("ImgS");
  cvDestroyWindow("ImgV");
  cvDestroyWindow("hist");
  return 0;
}
