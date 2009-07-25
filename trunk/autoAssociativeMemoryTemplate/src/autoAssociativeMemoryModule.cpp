// iCub
#include <iCub/autoAssociativeMemoryModule.h>

using namespace yarp::os;

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

	// create AAM ports
	_portImageIn.open(_namePortImageIn);
	_portThresholdIn.open(_namePortThresholdIn);
	_portImageOut.open(_namePortImageOut);
	_portValueOut.open(_namePortValueOut);
	
	return true;
}

bool AutoAssociativeMemoryModule::updateModule()
{
	// until this function returns false, it will be called forever by runModule()

	// blocking read -- proceed only when something arrives on the port
	Vector *thresholdVector = _portThresholdIn.read(true);
	// parse threshold
	if(thresholdVector!=NULL)
	{
		_threshold = (*thresholdVector)[0];
		printf("DEBUG: received threshold %f\n", _threshold);
	}
	
	
	// actual vision algorithm goes here
	
	// try to recall input image from AAM	
	// if there is a stored match, return it
	// otherwise, store input image into AAM for the future
	
	// outputs: _outputImg and _matchResult (write them to ports)
	
	
		
	return true;
}

bool AutoAssociativeMemoryModule::interruptModule()
{
	// interrupt ports gracefully when module communication is interrupted
	_portImageIn.interrupt();
	_portThresholdIn.interrupt();
	_portImageOut.interrupt();
	_portValueOut.interrupt();
	
	return true;	
}

bool AutoAssociativeMemoryModule::close()
{
	printf("Closing Auto-Associative Memory...\n\n");

	// close AAM ports
	_portImageIn.close();
	_portThresholdIn.close();
	_portImageOut.close();
	_portValueOut.close();

	// free data structures, delete dynamically-created variables
	
	// is this necessary?
	//Network::fini();

	return true;
}
