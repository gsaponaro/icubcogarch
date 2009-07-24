#ifndef __ICUB_AAM_MODULE_H__
#define __ICUB_AAM_MODULE_H__

#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Module.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

using namespace yarp::os;
using namespace yarp::sig;

class AutoAssociativeMemoryModule : public yarp::os::Module
{
private:
	// port names
	ConstString							_namePortImageIn;
	ConstString							_namePortThresholdIn;
	ConstString							_namePortImageOut;
	ConstString							_namePortValueOut;
	// ports
	BufferedPort<ImageOf<PixelRgb> >	_portImageIn;
	BufferedPort<Vector>				_portThresholdIn;
	BufferedPort<ImageOf<PixelRgb> >	_portImageOut;
	BufferedPort<Vector>				_portValueOut;
	// inputs
	ImageOf<PixelRgb>					*_inputImg;
	double								_threshold;
	// outputs
	ImageOf<PixelRgb>					*_outputImg;
	double								_matchResult;
	
public:
	virtual bool open(yarp::os::Searchable &config);
	virtual bool updateModule();
	virtual bool interruptModule();
	virtual bool close();
};

#endif // __ICUB_AAM_MODULE_H__