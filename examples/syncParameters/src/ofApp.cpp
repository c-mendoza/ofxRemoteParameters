#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	
	// The mainGroup is where all of the ofParameters that will be serialized or deserialized need to be.
	mainGroup.setName("ofxPC Example");
	mainGroup.add(radius.set("Circle Radius", 10, 1, 500),
				  color.set("Circle Color", ofColor::orange));
	
	auto min = glm::vec2(0, 0);
	auto max = glm::vec2(ofGetWidth(), ofGetHeight());
	
	// Set up our collection. Notice the trailing space in "Position ". Purely cosmetic, but
	// the names will look nicer in the gui.
	positionsCollection.setup("Position ", "Circle Positions", mainGroup, min, max);
	
	// Register an event listener that will inform us when the number of items
	// in the collection changes:
	el.push(positionsCollection.collectionChangedEvent.
			newListener([this](ofxParameterCollection<glm::vec2>& collection) {  // <- note the lambda argument
		// Rebuild the gui when the collection changes:
		buildGui();
		paramServer.syncParameters();
	}));
	
	// Listen for button presses
	// This button adds a circle:
	addCircleButtton.addListener(this, &ofApp::addCircleButtonPressed);
	addCircleButtton.setup("Add Circle");
	
	// This one removes the last circle:
	removeLastCircleButton.addListener(this, &ofApp::removeLastCircleButtonPressed);
	removeLastCircleButton.setup("Remove Last");
	
	// And this one removes a randomly selected circle:
	removeRandomButton.addListener(this, &ofApp::removeRandomButtonPressed);
	removeRandomButton.setup("Remove Random");
	settingsFilename = "settings.xml";
	
	gui.setup();
	buildGui();

	paramServer.setup(mainGroup);

}

// We are setting up the gui in a separate method to rebuild it when we deserialize or
// when we add a circle.
// NOTE: There seems to be a bad memory leak in ofxGui that happens when we repeatedly clear and add gui elements
// the gui this way. ofxParameterCollection should not be leaking however.

void ofApp::buildGui() {
	gui.clear();
	gui.add(&addCircleButtton);
	gui.add(&removeLastCircleButton);
	gui.add(&removeRandomButton);
	gui.add(mainGroup);
}

//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackgroundGradient(ofColor::gray, ofColor::blueSteel);
	gui.draw();
	
	// We iterate over the collection to get all of the ofParameters
	for (auto& pos : positionsCollection) {
		ofFill();
		ofSetColor(color);
		// pos is a shared_ptr<ofParameter<glm::vec2>>, so we need to dereference it:
		ofDrawEllipse(*pos, radius, radius);
		
		// An alternative would be to use pos->get() (get() in this case is a method of
		// ofParameter that gets us the parameter's value:
		// ofDrawEllipse(pos->get(), radius, radius);
	}
	
	ofSetColor(255);
	ofDrawBitmapString("Press 'l' to load, 's' to save", 100, ofGetHeight() - 50);
}

void ofApp::exit() {
	// Cleanup:
	addCircleButtton.removeListener(this, &ofApp::addCircleButtonPressed);
	removeLastCircleButton.removeListener(this, &ofApp::removeLastCircleButtonPressed);
	removeRandomButton.removeListener(this, &ofApp::removeRandomButtonPressed);
}

void ofApp::addCircleButtonPressed() {
	// Add an item to the collection in a random location:
	positionsCollection.addItem(glm::vec2(ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight())));
}

void ofApp::removeLastCircleButtonPressed() {
	if (positionsCollection.size() == 0) return; // Don't do anything if we don't have items in the collection
	
	// Remove the last item in the collection:
	positionsCollection.removeItem(positionsCollection.back());
	
	// We could have also used:
	// positionsCollection.removeAt(positionsCollections.size()-1);
}

void ofApp::removeRandomButtonPressed() {
	if (positionsCollection.size() == 0) return; // Don't do anything if we don't have items in the collection
	
	int index = std::floor(ofRandom(positionsCollection.size()));
	positionsCollection.removeAt(index);
}

// Standard serialization (it is in fact copied from ofxGui!)
void ofApp::serialize() {
	ofXml xml;
	if(ofFile(settingsFilename, ofFile::Reference).exists()){
		xml.load(settingsFilename);
	}
	ofSerialize(xml, mainGroup);
	xml.save(settingsFilename);
}

// Equally standard deserialization, with the exception of the
// positionsCollection.preDeserialize(xml) call.
void ofApp::deserialize() {
	ofXml xml;
	xml.load(settingsFilename);
	
	// Pre-deserialize the collection so that ofDeserialize can find the collection's parameters:
	positionsCollection.preDeserialize(xml);
	ofDeserialize(xml, mainGroup);
	
	// Our data changed so we rebuild our gui:
	buildGui();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 's') {
		serialize();
	}
	if(key == 'l') {
		deserialize();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
	
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
	
}
