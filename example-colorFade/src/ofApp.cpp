#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowTitle("ofxPixelPusher -- example-colorFade");
	mDiscoveryService = ofxPixelPusher::DiscoveryListener::getInstance();
	mDiscoveryService->addRegistrationCallback([=](std::shared_ptr<ofxPixelPusher::PixelPusher> pusher) {
		pusher->setPowerScale(0.5);
	});
}

//--------------------------------------------------------------
void ofApp::update(){
	for (auto pusher : mDiscoveryService->getPushers()) {
		for (auto strip : pusher->getStrips()) {
			strip->setPixelsHSB(hueCounter, 1.0, 1.0);
		}
	}
	hueCounter++;
	hueCounter = hueCounter % 360;
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
