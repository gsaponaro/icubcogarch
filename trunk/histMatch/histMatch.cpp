#include "histMatch.h"

//global variables

HistMatchData::HistMatchData()
{
  setThreshold(0.6);

  imgPort.open("/associative-memory_image:o");
  matchPort.open("/associative-memory_match_value:o");
}

HistMatchData::~HistMatchData()
{
  imgPort.close();
  matchPort.close();
}

/*void HistMatchData::addImage(IplImage* img)
{
  imgMutex.wait();
  images.push_back(img);
  imgMutex.post();
}

void getImages(std::vector<IplImage*>* v)
{
  imgMutex.wait();
  v = &images;
  imgMutex.post();
}*/

std::vector<ImageOf<PixelRgb> >& HistMatchData::images()
{
  return imgs;
}

void HistMatchData::setThreshold(double t)
{
  thrMutex.wait();
  threshold = t;
  thrMutex.post();
}

void HistMatchData::getThreshold(double& t)
{
  thrMutex.wait();
  t = threshold;
  thrMutex.post();
}


void HistMatchData::setDatabase(string s) {

	databasefile = s;

}

string HistMatchData::getDatabase() {

	return databasefile;

}


void HistMatchData::loadImages(const char *database) {

        string file;
	databasefile = database;
        ifstream datafile(database);
        if (datafile.is_open()) {
                while (!datafile.eof()) {
                        getline(datafile,file);
                        IplImage *thisImg = cvLoadImage(file.c_str());
			ImageOf <PixelRgb> yarpImg;
			yarpImg.wrapIplImage(thisImg);
                        imgs.push_back(yarpImg);
                }
        }
}


ImageReceiver::ImageReceiver(HistMatchData* d) : data(d) { }


void ImageReceiver::onRead(ImageOf<PixelRgb>& img)
{
  data->imgMutex.wait();

  std::vector<ImageOf<PixelRgb> >& images = data->images();
  IplImage* currImg = cvCreateImage(cvSize(img.width(), img.height()), IPL_DEPTH_8U, 3);
  cvCvtColor((IplImage*)img.getIplImage(), currImg, CV_RGB2HSV);

  int arr[2] = { 16, 16 };
  CvHistogram* currHist = cvCreateHist(2, arr, CV_HIST_ARRAY);

  IplImage *currImgH = cvCreateImage(cvGetSize(currImg), IPL_DEPTH_8U, 1);
  IplImage *currImgS = cvCreateImage(cvGetSize(currImg), IPL_DEPTH_8U, 1);
  IplImage *currImgV = cvCreateImage(cvGetSize(currImg), IPL_DEPTH_8U, 1);
  cvSplit(currImg, currImgH, currImgS, currImgV, NULL);
  IplImage* imgArr[2] = { currImgH, currImgS };
  cvCalcHist(imgArr, currHist);

  double matchValue, threshold;
  data->getThreshold(threshold);
  matchValue = threshold;
  bool found = false;
  std::vector<ImageOf<PixelRgb> >::iterator it;
  ImageOf<PixelRgb> matchImage;
  std::cout << "threshold: " << threshold << " ";
  for (it = images.begin(); it != images.end(); ++it)
    {

      IplImage* refImg = cvCreateImage(cvSize(it->width(), it->height()), IPL_DEPTH_8U, 3);
      cvCvtColor((IplImage*)it->getIplImage(), refImg, CV_RGB2HSV);

      CvHistogram* refHist = cvCreateHist(2, arr, CV_HIST_ARRAY);

      IplImage *refImgH = cvCreateImage(cvGetSize(refImg), IPL_DEPTH_8U, 1);
      IplImage *refImgS = cvCreateImage(cvGetSize(refImg), IPL_DEPTH_8U, 1);
      IplImage *refImgV = cvCreateImage(cvGetSize(refImg), IPL_DEPTH_8U, 1);
      cvSplit(refImg, refImgH, refImgS, refImgV, NULL);
      imgArr[0] = refImgH;
      imgArr[1] = refImgS;
      cvCalcHist(imgArr, refHist);

      double comp = 1 - cvCompareHist(currHist, refHist, CV_COMP_BHATTACHARYYA);
      std::cout << comp << " ";
      if (comp > matchValue)
	{
	  matchValue = comp;
	  matchImage = *it;
	  found = true;
	}
      cvReleaseImage(&refImg); cvReleaseImage(&refImgH); cvReleaseImage(&refImgS); cvReleaseImage(&refImgV);
      cvReleaseHist(&refHist);
    }

  if (found)
    {
      data->imgPort.prepare() = matchImage;
      data->imgPort.write();
      
      Bottle& out = data->matchPort.prepare();
      out.clear();
      out.addDouble(matchValue);
      data->matchPort.write();
      std::cout << "found" << std::endl;
    }
  else
    {
      images.push_back(img);
      data->imgPort.prepare() = img;
      data->imgPort.write();

      Bottle& out = data->matchPort.prepare();
      out.clear();
      out.addDouble(0.0);
      data->matchPort.write();
      std::cout << "stored" << std::endl;
      string s = "image";
      s += images.size()-1;
      s += ".jpg";
      cvSaveImage(s.c_str(), img.getIplImage()); 
      
      ofstream of;
      of.open(data->getDatabase().c_str(),ios::app);
      of << s << endl;
      of.close();

    }

  cvReleaseImage(&currImg); cvReleaseImage(&currImgH); cvReleaseImage(&currImgS); cvReleaseImage(&currImgV);
  //cvReleaseImage(&imgArr[0]); cvReleaseImage(&imgArr[1]);
  cvReleaseHist(&currHist);

  data->imgMutex.post();
}

ThresholdReceiver::ThresholdReceiver(HistMatchData* d) : data(d) { }


void ThresholdReceiver::onRead(Bottle& threshold)
{
  double t = threshold.get(0).asDouble();
  std::cout << "the threshold is: " << t << std::endl;
  data->setThreshold(t);
}


bool HistMatchModule::open(Searchable &config)
{
  Property options(config.toString());
  imgReceiver = new ImageReceiver(&data);
  thrReceiver = new ThresholdReceiver(&data);

  if (options.check("database","Checking for a database...")) {
	cout << "Database found; Using..." << endl;
  	data.loadImages(options.find("database").asString());
  } else {
	cout << "No database found; starting from scratch..." << endl;
  }

  imgReceiver->useCallback();
  thrReceiver->useCallback();

  imgReceiver->open("/associative-memory_image:i");
  thrReceiver->open("/associative-memory_threshold:i");

  return true;
}

bool HistMatchModule::updateModule()
{

  return true;
}


bool HistMatchModule::close()
{
  imgReceiver->close();
  thrReceiver->close();
  return true;
}
