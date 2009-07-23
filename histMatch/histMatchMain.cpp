#include <ace/OS.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>


#include "histMatch.h"


using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char *argv[])
{
  Network net;
  HistMatchModule moduleID;
  moduleID.setName("/histMatch");
  return moduleID.runModule(argc,argv);
}
