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
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	void addCircleButtonPressed();
	void removeLastCircleButtonPressed();
	void removeRandomButtonPressed();
	void buildGui();
	void deserialize();
	void serialize();
	
	ofxPanel gui;
	ofParameterGroup mainGroup;
	ofParameter<float> radius;
	ofParameter<ofColor> color;
	ofxParameterCollection<glm::vec2> positionsCollection;
	ofxButton addCircleButtton;
	ofxButton removeLastCircleButton;
	ofxButton removeRandomButton;
	std::string settingsFilename;
	ofEventListeners el;
	ofxParameterServer paramServer;
	
};
