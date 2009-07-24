#ifndef __ICUB_AAM_MODULE_H__
#define __ICUB_AAM_MODULE_H__

#include <ace/OS.h>
#include <yarp/os/all.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/sig/all.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cv.h>
#include <highgui.h>

using namespace std;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;


class HistMatchData
{
private:
  std::vector<ImageOf<PixelRgb> > imgs;
  double threshold;
  double matchValue;
  string databaseName;
  string databaseContext;

public:
  Semaphore thrMutex;
  Semaphore imgMutex;

  HistMatchData();
  ~HistMatchData();

//   void addImage(IplImage*);
//   void getImages(std::vector<IplImage*>*);

  vector<ImageOf<PixelRgb> >& images();
  void setThreshold(double);
  void getThreshold(double&);

  void setDatabaseContext(string);
  string getDatabaseContext();
  void setDatabaseName(string);
  string getDatabaseName();

  void loadDatabase();

};


class ImageReceiver : public BufferedPort<ImageOf<PixelRgb> >
{
private:
  HistMatchData* data;

  BufferedPort<ImageOf<PixelRgb> >* imgPort;
  BufferedPort<Bottle>* matchPort;

  virtual void onRead(ImageOf<PixelRgb>&);

public:
  ImageReceiver(HistMatchData* d, BufferedPort<ImageOf<PixelRgb> >* iPort, BufferedPort<Bottle>* mPort);

};

class ThresholdReceiver : public BufferedPort<Bottle>
{
private:
  HistMatchData* data;
  virtual void onRead(Bottle&);

public:
  ThresholdReceiver(HistMatchData* d);

};

class AutoAssociativeMemoryModule : public Module
{
private:
  // port names
  ConstString _namePortImageIn;
  ConstString _namePortThresholdIn;
  ConstString _namePortImageOut;
  ConstString _namePortValueOut;

  HistMatchData data;

  // ports
  ImageReceiver* _portImageIn;
  ThresholdReceiver* _portThresholdIn;
  BufferedPort<ImageOf<PixelRgb> > _portImageOut;
  BufferedPort<Bottle> _portValueOut;
  // inputs
  ImageOf<PixelRgb>* _inputImg;
  double _threshold;
  // outputs
  ImageOf<PixelRgb>* _outputImg;
  double _matchResult;
  
public:
  virtual bool open(Searchable &config);
  virtual bool updateModule();
  virtual bool interruptModule();
  virtual bool close();
};

#endif // __ICUB_AAM_MODULE_H__
