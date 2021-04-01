#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxRemoteParameters/Client.h"

class ofApp : public ofBaseApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	void buildGui();

	void connectButtonPressed();
	void closeButtonPressed();

	ofParameterGroup params;

	ofxPanel gui;
	ofxButton connectButton;
	ofxButton closeButton;

	ofxRemoteParameters::Client paramClient;
	ofEventListener modelLoadedEventListener;
};
