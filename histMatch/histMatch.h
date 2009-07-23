#ifndef _HISTMATCH_H_
#define _HISTMATCH_H_

#include <ace/OS.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>


class HistMatchModule : public yarp::os::Module
{
 private:
  


 public:
  virtual bool open(yarp::os::Searchable &config);
  virtual bool updateModule();
  virtual bool close();

};


#endif
