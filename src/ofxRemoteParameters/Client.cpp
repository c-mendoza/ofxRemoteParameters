
#include <ofAppRunner.h>
#include <ofXml.h>
#include "Client.h"

using namespace ofxRemoteParameters;

const std::string Client::LogModuleName = "ofxRemoteParameters::Client";

bool Client::setup(ofParameterGroup& parameterGroup, std::string serverAddress, int serverInPort, int serverOutPort)
{
	this->parameterGroup = std::dynamic_pointer_cast<ofParameterGroup>(parameterGroup.newReference());

	bool success = oscSender.setup(serverAddress, serverInPort);

	if (success)
	{
		success = oscReceiver.setup(serverOutPort);
	}
	else
	{
		ofLogError(LogModuleName) << "OSC Sender setup failed! Address: " << serverAddress << " Port: " << serverInPort;
		return success;
	}

	if (!success)
	{
		ofLogError(LogModuleName) << "OSC Receiver setup failed!" << "Port: " << serverOutPort;
		return success;
	}

	loopListener = ofGetMainLoop()->loopEvent.newListener([this](){

		ofxOscMessage message;
		while(oscReceiver.getNextMessage(message))
		{
			parseReceivedMessage(message);
		}
	});
	return success;
}

void Client::threadedFunction()
{
	ofThread::threadedFunction();
}

void Client::parseReceivedMessage(ofxOscMessage& m)
{

	ofLogVerbose(LogModuleName) << "address = " << m.getAddress();
	ofLogVerbose(LogModuleName) << "arg = " << m.getArgAsString(0);
	for (int i = 0; i < m.getNumArgs(); i++)
	{
		ofLogVerbose(LogModuleName) << "arg " << i << " = " << m.getArgAsString(i);
	}

	auto components = ofSplitString(m.getAddress(),
									ofToString('/'),
									false,
									true);
	for (int j = 0; j < components.size(); ++j)
	{
		ofLogVerbose(LogModuleName) << j << " " << components[j];
	}

	if (components.size() < 3) return;

	// Only pay attention to messages that are responses:
	if (components[1] == ApiResponse)
	{
		// Handle Connect:
		if (components[2] == MethodConnect)
		{
			// Call connect, set vars
		}
		// Handle getModel:
		else if (components[3] == MethodGetModel)
		{
			// Parse XML, populate group
			parseModel(m.getArgAsString(0));
		}
	}
}

void Client::connect()
{
	ofxOscMessage message;
	message.setAddress("/" + ApiRoot + "/" + MethodConnect);
	message.addStringArg("");
	oscSender.sendMessage(message, false);
}

void Client::parseModel(std::string xmlModel)
{
	ofXml xml;
	xml.parse(xmlModel);
	for(auto& ip : getLocalIPs())
	{
		ofLogVerbose(LogModuleName) << ip;
	}
}
