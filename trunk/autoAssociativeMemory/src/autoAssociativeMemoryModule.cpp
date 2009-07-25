// iCub
#include <iCub/autoAssociativeMemoryModule.h>


///////////////////// HistMatchData definitions ///////////////////////////////

HistMatchData::HistMatchData()
{
  setThreshold(0.6);
  databaseName = "defaultDatabase";
  databaseContext = "";
}

HistMatchData::~HistMatchData()
{
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

vector<ImageOf<PixelRgb> >& HistMatchData::images()
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

void HistMatchData::setDatabaseContext(string s)
{
  databaseContext = s;
}

string HistMatchData::getDatabaseContext()
{
  return databaseContext;
}

void HistMatchData::setDatabaseName(string s)
{
  databaseName = s;

}

string HistMatchData::getDatabaseName()
{

  return databaseName;
  
}


void HistMatchData::loadDatabase()
{

  string file;
  string databaseFolder;
  if (databaseContext == "")
    databaseFolder = databaseName;
  else
    databaseFolder = databaseContext + "/" + databaseName;

  ifstream datafile((databaseFolder + "/" + databaseName).c_str());
  if (datafile.is_open()) {
    while (!datafile.eof()) {
      getline(datafile,file);
      if (file.size() <= 3) break;
      file = databaseFolder + "/" + file;
      IplImage *thisImg = cvLoadImage(file.c_str());
      ImageOf <PixelRgb> yarpImg;
      yarpImg.wrapIplImage(thisImg);
      imgs.push_back(yarpImg);
    }
  }
}


////////////////////// Receivers definitions //////////////////////////////////

ImageReceiver::ImageReceiver(HistMatchData* d, BufferedPort<ImageOf<PixelRgb> >* iPort, BufferedPort<Bottle>* mPort) : data(d), imgPort(iPort), matchPort(mPort)
{
  //cvNamedWindow("iplImage", CV_WINDOW_AUTOSIZE);
}


void ImageReceiver::onRead(ImageOf<PixelRgb>& img)
{
  data->imgMutex.wait();

  std::vector<ImageOf<PixelRgb> >& images = data->images();
  IplImage *currLP = cvCreateImage(cvSize(img.width(), img.height()), IPL_DEPTH_8U, 3);
  IplImage* currImg = cvCreateImage(cvSize(img.width(), img.height()), IPL_DEPTH_8U, 3);

  cvLogPolar((IplImage*)img.getIplImage(), currLP, cvPoint2D32f(img.width()/2,img.height()/2), 30);

  cvCvtColor(currLP, currImg, CV_RGB2HSV);

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

      IplImage *refLP = cvCreateImage(cvSize(it->width(), it->height()), IPL_DEPTH_8U, 3);
      IplImage* refImg = cvCreateImage(cvSize(it->width(), it->height()), IPL_DEPTH_8U, 3);

      cvLogPolar((IplImage*)it->getIplImage(), refLP, cvPoint2D32f(it->width()/2,it->height()/2), 30);
      cvCvtColor(refLP, refImg, CV_RGB2HSV);

      CvHistogram* refHist = cvCreateHist(2, arr, CV_HIST_ARRAY);

      IplImage *refImgH = cvCreateImage(cvGetSize(refImg), IPL_DEPTH_8U, 1);
      IplImage *refImgS = cvCreateImage(cvGetSize(refImg), IPL_DEPTH_8U, 1);
      IplImage *refImgV = cvCreateImage(cvGetSize(refImg), IPL_DEPTH_8U, 1);
      cvSplit(refImg, refImgH, refImgS, refImgV, NULL);
      imgArr[0] = refImgH;
      imgArr[1] = refImgS;
      cvCalcHist(imgArr, refHist);

      double comp = 1 - cvCompareHist(currHist, refHist, CV_COMP_BHATTACHARYYA);
      cout << comp << " ";
      if (comp > matchValue)
	{
	  matchValue = comp;
	  matchImage = *it;
	  found = true;
	}
      cvReleaseImage(&refImg); cvReleaseImage(&refImgH); cvReleaseImage(&refImgS); cvReleaseImage(&refImgV);
      cvReleaseImage(&refLP); cvReleaseHist(&refHist);
    }

  if (found)
    {
      imgPort->prepare() = matchImage;
      imgPort->write();
      
      Bottle& out = matchPort->prepare();
      out.clear();
      out.addDouble(matchValue);
      matchPort->write();
      cout << "found" << endl;
    }
  else
    {
      images.push_back(img);
      imgPort->prepare() = img;
      imgPort->write();

      Bottle& out = matchPort->prepare();
      out.clear();
      out.addDouble(0.0);
      matchPort->write();
      cout << "stored" << endl;
      string s;
      ostringstream oss(s);
      oss << "image" << images.size()-1 << ".jpg";
      cout << oss.str() << endl;

      string databasefolder = data->getDatabaseContext() + "/" + data->getDatabaseName();
      //cvShowImage("iplImage", img.getIplImage());
      cvCvtColor(img.getIplImage(), currImg, CV_RGB2BGR);
      cvSaveImage((databasefolder + "/" + oss.str()).c_str(), currImg);
      
      ofstream of;
      of.open((databasefolder + "/" + data->getDatabaseName()).c_str(),ios::app);
      of << oss.str() << endl;
      of.close();

    }

  cvReleaseImage(&currImg); cvReleaseImage(&currImgH); cvReleaseImage(&currImgS); cvReleaseImage(&currImgV);
  //cvReleaseImage(&imgArr[0]); cvReleaseImage(&imgArr[1]);
  cvReleaseImage(&currLP); cvReleaseHist(&currHist);

  data->imgMutex.post();
}

ThresholdReceiver::ThresholdReceiver(HistMatchData* d) : data(d) { }


void ThresholdReceiver::onRead(Bottle& threshold)
{
  double t = threshold.get(0).asDouble();
  std::cout << "the threshold is now: " << t << std::endl;
  data->setThreshold(t);
}


///////////////// Auto Associative Memory Module definitions //////////////////

bool AutoAssociativeMemoryModule::open(Searchable &config)
{
  // read options from command line
  Bottle initialBottle(config.toString().c_str());
    
  // if autoAssociativeMemory.ini exists, then load it
  ConstString initializationFile = initialBottle.check("from",
						       Value("autoAssociativeMemory.ini"),
						       "Initialization file (string)").asString();
    
  ConstString context = initialBottle.check("context",
					    Value("autoAssociativeMemory"),
					    "Context (string)").asString();

  // create and initialize resource finder
  ResourceFinder rf;
  rf.setDefaultContext(context.c_str());
  rf.setDefaultConfigFile(initializationFile.c_str());
  rf.configure("ICUB_ROOT", 0, NULL);

  // pass configuration over to bottle
  Bottle botConfig(rf.toString().c_str());

  // parse parameters or assign default values (append to getName=="/aam")
  _namePortImageIn = botConfig.check("portImageIn",
				     Value(getName("image:i")),
				     "Input image port (string)").asString();
  _namePortThresholdIn = botConfig.check("portThresholdIn",
					 Value(getName("threshold:i")),
					 "Input threshold port (string)").asString();
  _namePortImageOut = botConfig.check("portImageOut",
				      Value(getName("image:o")),
				      "Output image port (string)").asString();
  _namePortValueOut = botConfig.check("portValueOut",
				      Value(getName("value:o")),
				      "Output value port (string)").asString();

  string databaseName = botConfig.check("database",
					Value("defaultDatabase"),
					"Database name (string)").asString().c_str();
  string path = botConfig.check("path",
				Value("~/iCub/app/"),
				"complete path to context").asString().c_str();
				   

  string ctxt = path;
  ctxt += context.c_str();
  data.setDatabaseContext(ctxt);
  cout << "context: " << ctxt << endl;
  data.setDatabaseName(databaseName);
  cout << "databaseName: " << databaseName << endl;
  data.loadDatabase();

  // create AAM ports
  _portImageIn = new ImageReceiver(&data, &_portImageOut, &_portValueOut);
  _portThresholdIn = new ThresholdReceiver(&data);

  _portImageIn->useCallback();
  _portThresholdIn->useCallback();

  _portImageIn->open(_namePortImageIn);
  _portThresholdIn->open(_namePortThresholdIn);
  _portImageOut.open(_namePortImageOut);
  _portValueOut.open(_namePortValueOut);
	
  return true;
}

bool AutoAssociativeMemoryModule::updateModule()
{		
  return true;
}

bool AutoAssociativeMemoryModule::interruptModule()
{
  // interrupt ports gracefully
  _portImageIn->interrupt();
  _portThresholdIn->interrupt();
  _portImageOut.interrupt();
  _portValueOut.interrupt();
	
  return true;	
}

bool AutoAssociativeMemoryModule::close()
{
  cout << "Closing Auto-Associative Memory...\n\n";

  // close AAM ports
  _portImageIn->close();
  _portThresholdIn->close();
  _portImageOut.close();
  _portValueOut.close();
  
  // free data structures, delete dynamically-created variables
    
  // is this necessary?
  //Network::fini();
    
  return true;
}
