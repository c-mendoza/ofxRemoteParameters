#pragma once

#include "ofMain.h"
#include "ofxParameterCollection.h"
#include "ofxGui.h"
#include "ofxParameterServer.h"

class ofApp : public ofBaseApp {
	
public:
	void setup();
	void draw();

	ofxPanel gui;
	ofxParameterServer paramServer;
	ofParameterGroup mainGroup;
	ofParameter<glm::vec2> circlePosition;
	ofParameter<float> circleRadius;
	ofParameter<ofColor> circleColor;
};
