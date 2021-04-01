#include "ofApp.h"

static std::string moduleName = "of_client";

const static std::string SERVER_IP_ADDRESS = "127.0.0.1";

void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	connectButton.setup("Connect");
	connectButton.addListener(this, &ofApp::connectButtonPressed);

	// Note that this stops the server OSC receiver and prevents any further communications.
	// If you want to restart the server you must do so yourself in your server app.
	closeButton.setup("Close Server");
	closeButton.addListener(this, &ofApp::closeButtonPressed);

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
	gui.add(&closeButton);
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

void ofApp::closeButtonPressed() {
	// You can call server methods by name:
	paramClient.callServerMethod("close");
}