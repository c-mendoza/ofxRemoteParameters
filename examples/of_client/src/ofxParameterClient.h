//
// Created by cmendoza on 3/9/21.
//

#ifndef OF_CLIENT_OFXPARAMETERCLIENT_H
#define OF_CLIENT_OFXPARAMETERCLIENT_H

namespace ofxParameterServer
{
	class ofxParameterClient
	{
	public:
		setup(std::string serverAddress, int serverInPort, int serverOutPort)
	private:
		ofxOscReceiver oscReceiver;
		ofxOscSender oscSender;
	};

}
#endif //OF_CLIENT_OFXPARAMETERCLIENT_H
