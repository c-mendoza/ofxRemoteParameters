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

	ofParameterGroup params;

	ofxPanel gui;
	ofxButton connectButton;

	ofxRemoteParameters::Client paramClient;
	ofEventListener modelLoadedEventListener;
};
