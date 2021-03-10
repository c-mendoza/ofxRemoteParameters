//
// Created by cmendoza on 3/9/21.
//

#ifndef OFXREMOTEPARAMETERS_CLIENT_H
#define OFXREMOTEPARAMETERS_CLIENT_H

#include <ofThread.h>
#include "ofxOsc.h"
#include "ofxRemoteParameters.h"

namespace ofxRemoteParameters
{
	class Client
	{
	public:
		bool setup(ofParameterGroup& parameterGroup, std::string serverAddress, int serverInPort = DefaultServerInPort, int serverOutPort = DefaultServerOutPort);
		void connect();

		const static std::string LogModuleName;

	private:
		ofxOscReceiver oscReceiver;
		ofxOscSender oscSender;
		ofEventListener loopListener;
		std::shared_ptr<ofParameterGroup> parameterGroup;
	protected:
		void parseReceivedMessage(ofxOscMessage& message);
		void parseModel(std::string basicString);
	};

}
#endif //OFXREMOTEPARAMETERS_CLIENT_H
