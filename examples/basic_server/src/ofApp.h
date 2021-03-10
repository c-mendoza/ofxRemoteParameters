#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxRemoteParameters/Server.h"
class ofApp : public ofBaseApp {
	
public:
	void setup();
	void draw();

	ofxPanel gui;
	ofxRemoteParameters::Server paramServer;
	ofParameterGroup mainGroup;
	ofParameter<glm::vec2> circlePosition;
	ofParameter<float> circleRadius;
	ofParameter<ofColor> circleColor;
};
