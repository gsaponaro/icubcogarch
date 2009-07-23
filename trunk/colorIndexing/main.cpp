#include <iostream>
#include <cv.h>
#include "highgui.h"

using namespace std;

int main(int argc, char *argv[])
{
  CvCapture* capture = cvCreateCameraCapture(0);

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

  cvNamedWindow("webcam", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("hist", CV_WINDOW_AUTOSIZE);
  //cvShowImage("hist", refHist->bins);

  while (true)
    {
      IplImage* testImg = cvQueryFrame(capture);
      cvShowImage("webcam", testImg);
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
  
      IplImage* patchImg = cvCreateImage(cvGetSize(testImg), IPL_DEPTH_32F, 1);
      //cvCalcBackProjectPatch(imgArr, patchImg, cvSize(1, 1), refHist, CV_COMP_INTERSECT, 5);
      cvCalcBackProject(imgArr, patchImg, refHist);

      cvShowImage("hist", patchImg);

      CvPoint max, min;
      cvMinMaxLoc(patchImg, NULL, NULL, &min, &max);

      cout << "min: " << min.x << " " << min.y << endl;
      cout << "max: " << max.x << " " << max.y << endl;

      if (cvWaitKey(33) != -1)
	break;
    }

  cvWaitKey(0);
  cvReleaseCapture(&capture);
  cvDestroyWindow("ImgH");
  cvDestroyWindow("ImgS");
  cvDestroyWindow("ImgV");
  cvDestroyWindow("hist");
  cvDestroyWindow("webcam");
  return 0;
}
