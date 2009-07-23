#include <cv.h>
#include "highgui.h"
#include <iostream.h>

int main( int argc, char *argv[] ) {

	//load in the image to be matched 
	IplImage *matchImg = cvLoadImage(argv[1]);
	int arr[2] = {16,16};
	CvHistogram *matchHist = cvCreateHist(2, arr, CV_HIST_ARRAY);

	//convert the reference image to HSV, split it, take the histogram
	IplImage *matchArr = cvCreateImage(cvGetSize(matchImg), IPL_DEPTH_8U, 3);
	IplImage *matchImgH = cvCreateImage(cvGetSize(matchImg), IPL_DEPTH_8U, 1);
 	IplImage *matchImgS = cvCreateImage(cvGetSize(matchImg), IPL_DEPTH_8U, 1);
 	IplImage *matchImgV = cvCreateImage(cvGetSize(matchImg), IPL_DEPTH_8U, 1);
 	cvCvtColor(matchImg, matchArr, CV_RGB2HSV);
	cvSplit(matchArr, matchImgH, matchImgS, matchImgV, NULL);
 	IplImage* imgArr[2] = { matchImgH, matchImgS };
  	cvCalcHist( imgArr, matchHist);

	//compare to each of our database images	
	for (int i = 2; i < argc; i++) {
		IplImage *testImg = cvLoadImage(argv[i]);	
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

      		double comp = cvCompareHist(testHist, matchHist, CV_COMP_BHATTACHARYYA);

      		cout << argv[i] << " " << comp << endl;
	}

	//return a table of the matching values to each of the other images

	//return also the winner


	return 0;
}
