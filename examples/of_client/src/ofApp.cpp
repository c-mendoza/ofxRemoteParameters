#include "ofApp.h"

static std::string moduleName = "of_client";

const static std::string SERVER_IP_ADDRESS = "192.168.1.13";

void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	connectButton.setup("Connect");
	connectButton.addListener(this, &ofApp::connectButtonPressed);

	paramClient.setup(params, SERVER_IP_ADDRESS, 12000, 12001);
	modelLoadedEventListener = paramClient.modelLoadedEvent.newListener([this]() {
		buildGui();
	});

	buildGui();
}

void ofApp::buildGui()
{
	gui.clear();
	gui.setup("OF Client");
	gui.add(&connectButton);
	gui.add(params);
}

void ofApp::update()
{

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
	paramClient.connect();
}