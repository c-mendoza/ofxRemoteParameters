#pragma once

#include "ofMain.h"
#include "ofxParameterCollection.h"
#include "ofxGui.h"
#include "ofxParameterServer.h"

class ofApp : public ofBaseApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();

	void connectButtonPressed();
	
	ofxOscReceiver oscReceiver;
	ofxOscSender oscSender;

	ofParameterGroup params;

	ofxPanel gui;
	ofxButton connectButton;

};
