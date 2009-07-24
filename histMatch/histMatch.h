#ifndef _HISTMATCH_H_
#define _HISTMATCH_H_

#include <ace/OS.h>
#include <yarp/os/all.h>
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
  string databasefolder;

public:
  Semaphore thrMutex;
  Semaphore imgMutex;

  BufferedPort<ImageOf<PixelRgb> > imgPort;
  BufferedPort<Bottle> matchPort;

  HistMatchData();
  ~HistMatchData();

//   void addImage(IplImage*);
//   void getImages(std::vector<IplImage*>*);

  std::vector<ImageOf<PixelRgb> >& images();
  void setThreshold(double);
  void getThreshold(double&);

  void setDatabase(string);
  string getDatabase();

  void loadImages(const char *);

};


class ImageReceiver : public BufferedPort<ImageOf<PixelRgb> >
{
private:
  HistMatchData* data;
  virtual void onRead(ImageOf<PixelRgb>&);

public:
  ImageReceiver(HistMatchData* d);

};

class ThresholdReceiver : public BufferedPort<Bottle>
{
private:
  HistMatchData* data;
  virtual void onRead(Bottle&);

public:
  ThresholdReceiver(HistMatchData* d);

};

class HistMatchModule : public Module
{
 private:
  HistMatchData data;
  ImageReceiver* imgReceiver;
  ThresholdReceiver* thrReceiver;

 public:
  virtual bool open(Searchable &config);
  virtual bool updateModule();
  virtual bool close();

};


#endif // _HISTMATCH_H_
