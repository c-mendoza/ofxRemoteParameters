#include "ofApp.h"

static std::string moduleName = "of_client";

const static std::string CLIENT_IP_ADDRESS = "192.168.1.13";
const static std::string SERVER_IP_ADDRESS = "192.168.1.13";

void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	oscReceiver.setup(12001);
	ofxOscReceiverSettings settings;
	oscSender.setup(SERVER_IP_ADDRESS, 12000);

	connectButton.setup("Connect");
	connectButton.addListener(this, &ofApp::connectButtonPressed);

	gui.setup("OF Client");
	gui.add(&connectButton);

}

void ofApp::update()
{
	ofxOscMessage m;

	while (oscReceiver.getNextMessage(m))
	{
		if (m.getAddress().compare("/ofxpsResponse/connect") == 0)
		{
			oscReceiver.getParameter()
			ofxOscMessage message;
			message.setAddress("/ofxpsMethod/getModel");
			message.addStringArg(CLIENT_IP_ADDRESS);
			oscSender.sendMessage(message, false);
		}
		else if (m.getAddress().compare("/ofxpsResponse/getModel") == 0)
		{
			ofLogVerbose(moduleName) << m.getArgAsString(0);
		}
	}
}

void ofApp::draw()
{
	gui.draw();
}

void ofApp::exit()
{
	ofBaseApp::exit();
}

void ofApp::connectButtonPressed()
{
	ofxOscMessage message;
	message.setAddress("/ofxpsMethod/connect");
	message.addStringArg(CLIENT_IP_ADDRESS);
	oscSender.sendMessage(message, false);

}