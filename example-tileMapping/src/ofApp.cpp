#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	mDiscoveryService = ofxPixelPusher::DiscoveryService::getInstance();
	mFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);

	mFbo.readToPixels(mPixels);
	mDiscoveryService->setPowerScale(0.01);
	mDiscoveryService->addRegistrationCallback([=](std::shared_ptr<ofxPixelPusher::PixelPusher> pusher) {
		for (auto strip : pusher->getStrips()) {
			strip->setTexture(mPixels.getData(), mFbo.getWidth(), mFbo.getHeight(), 3);
		}
	});
}

//--------------------------------------------------------------
void ofApp::update(){
	mFbo.begin();
	ofBackground(0);
	ofSetLineWidth(5.0);
	ofSetColor(0, 255, 0);
	//ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	ofDrawLine(0, ofGetFrameNum() % ofGetHeight(), ofGetWidth(), (ofGetFrameNum() + int(ofGetHeight() / 2.0)) % ofGetHeight());
	mFbo.end();

	mFbo.readToPixels(mPixels);

	for (auto pusher : mDiscoveryService->getPushers()) {
		for (auto strip : pusher->getStrips()) {
			strip->setTexture(mPixels.getData(), mFbo.getWidth(), mFbo.getHeight(), 3);
			strip->setTexCoords(0, 0.5, 1, 0.5);
			strip->setPixelsFromTex();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	mFbo.draw(0, 0);
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
