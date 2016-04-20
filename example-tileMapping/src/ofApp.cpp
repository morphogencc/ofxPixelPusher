#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	mDiscoveryService = ofxPixelPusher::DiscoveryService::getInstance();
	mFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);

	mFbo.readToPixels(mPixels);
	mDiscoveryService->setPowerScale(0.01);
	mDiscoveryService->addRegistrationCallback([=](std::shared_ptr<ofxPixelPusher::PixelPusher> pusher) {
		// whenever a strip is registered, set it to be a TwentySquared tile, set the tile to get its data from the data in mPixels, and set it to only use the middle chunk of the image
		for (auto strip : pusher->getStrips()) {
			strip->setStripType(ofxPixelPusher::StripType::TWENTYSQUARED);
			strip->setTexture(mPixels.getData(), mFbo.getWidth(), mFbo.getHeight(), 3);
			strip->setTexCoords(0.25, 0.25, 0.75, 0.75);
		}
	});
}

//--------------------------------------------------------------
void ofApp::update() {
	// draw some random stuff to an FBO
	mFbo.begin();
	ofBackground(0);
	ofSetLineWidth(5.0);
	ofSetColor(0, 255, 0);
	ofDrawLine(0, 2 * ofGetFrameNum() % ofGetHeight(), ofGetWidth(), 2 * ofGetFrameNum() % ofGetHeight());
	ofDrawLine(ofGetFrameNum() % ofGetWidth(), 0, ofGetFrameNum() % ofGetWidth(), ofGetHeight());
	ofDrawLine(0, ofGetFrameNum() % ofGetHeight(), ofGetWidth(), (ofGetFrameNum() + int(0.25*ofGetHeight())) % ofGetHeight());
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 15, 15);
	mFbo.end();

	// Read the FBO to our pixel array; when registered, the strip has a pointer to this array holding the image data.
	mFbo.readToPixels(mPixels);

	// for each strip, set pixels from the texture it is pointed to.
	for (auto pusher : mDiscoveryService->getPushers()) {
		for (auto strip : pusher->getStrips()) {
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
