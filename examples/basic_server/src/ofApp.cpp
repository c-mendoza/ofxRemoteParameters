#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	// Always give the ofParameterGroup a name:
	mainGroup.setName("Parameter Server Simple Example");
	// Add the parameters to the group:
	mainGroup.add(circleRadius.set("Circle Radius", 10, 1, 500),
				  circleColor.set("Circle Color", ofColor::orange),
				  circlePosition.set("Circle Position",
									 {200, 200},
									 {0, 0},
									 {ofGetWidth(),
									  ofGetHeight()}));


	gui.setup();
	gui.clear();
	gui.setWidthElements(400);
	gui.add(mainGroup);

	paramServer.setup(mainGroup);
	ofSetCircleResolution(40);

	// Note that if you use the GUI the client does not update automatically. If you want the client to update
	// you will need to call paramServer.syncParameters() whenever a parameter does change. Check the advanced example
	// for more info.
}

void ofApp::draw()
{
	ofBackgroundGradient(ofColor::gray, ofColor::blueSteel);
	gui.draw();
	ofFill();
	ofSetColor(circleColor);
	ofDrawEllipse(circlePosition, circleRadius * 0.5, circleRadius * 0.5);
}

