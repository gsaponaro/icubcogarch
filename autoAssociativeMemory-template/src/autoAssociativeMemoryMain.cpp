// yarp
#include <yarp/os/Module.h>
#include <yarp/os/Network.h>

// iCub
#include <iCub/autoAssociativeMemoryModule.h>

using namespace yarp::os;

int main(int argc, char *argv[])
{
	// create a specific Network object, rather than using Network::init().
	// this way, automatic cleanup by the means of Network::fini() will be smoother
	Network network;

	AutoAssociativeMemoryModule moduleID;	// instantiate module
	moduleID.setName("/aam");				// set default name (port prefix) of module
	return moduleID.runModule(argc,argv);	// execute module
}